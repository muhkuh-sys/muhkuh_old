/***************************************************************************
 *   Copyright (C) 2007 by Christoph Thelen                                *
 *   doc_bacardi@users.sourceforge.net                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "romloader_openocd_main.h"
#include "_luaif/romloader_openocd_wxlua_bindings.h"
#include <wx/dynarray.h>

#define OPENOCD_READ_IMAGE_CHUNKSIZE (8*1024)
#define OPENOCD_WRITE_IMAGE_CHUNKSIZE (32*1024)
#define OPENOCD_CALL_TIMESLICE_MS (10)           // sleep

#ifdef _WIN32
#define vsnprintf _vsnprintf
#endif

/*-------------------------------------*/
/* openocd includes */

extern "C"
{
	#include "config.h"
	#include "log.h"
	#include "types.h"
	#include "jtag.h"
	#include "configuration.h"
	#include "interpreter.h"
	#include "xsvf.h"
	#include "target.h"
	#include "command.h"
	#include "replacements.h"

	/* DLL loading functions in ft2232.c */
	int ft2232_load();       /* called in fn_init */
	void ft2232_unload();    /* called in fn_leave */
	int ft2232_isloaded();   /* called in fn_detect_interfaces */

}

/*-------------------------------------*/
/* some local prototypes */

bool fn_connect(void *pvHandle);
void fn_disconnect(void *pvHandle);
bool fn_is_connected(void *pvHandle);
int fn_read_data08(void *pvHandle, unsigned long ulNetxAddress, unsigned char *pucData);
int fn_read_data16(void *pvHandle, unsigned long ulNetxAddress, unsigned short *pusData);
int fn_read_data32(void *pvHandle, unsigned long ulNetxAddress, unsigned long *pulData);
int fn_read_image(void *pvHandle, unsigned long ulNetxAddress, char *pcData, unsigned long ulSize, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData);
int fn_write_data08(void *pvHandle, unsigned long ulNetxAddress, unsigned char ucData);
int fn_write_data16(void *pvHandle, unsigned long ulNetxAddress, unsigned short usData);
int fn_write_data32(void *pvHandle, unsigned long ulNetxAddress, unsigned long ulData);
int fn_write_image(void *pvHandle, unsigned long ulNetxAddress, const char *pcData, unsigned long ulSize, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData);
int fn_call(void *pvHandle, unsigned long ulNetxAddress, unsigned long ulParameterR0, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData);

/*-------------------------------------*/

static muhkuh_plugin_desc plugin_desc =
{
	wxT("OpenOCD Bootloader"),
	wxT(""),
	{ 0, 0, 1 }
};

const romloader_functioninterface tFunctionInterface =
{
	fn_connect,
	fn_disconnect,
	fn_is_connected,
	fn_read_data08,
	fn_read_data16,
	fn_read_data32,
	fn_read_image,
	fn_write_data08,
	fn_write_data16,
	fn_write_data32,
	fn_write_image,
	fn_call
};

static wxLuaState *m_ptLuaState;

// A single configuration
class romloader_openocd_config
{
public:
	wxString strCfgName;		// value of the "id" property of the Cfg tag
	wxArrayString astrInitCfg;	// content between <Init>...</Init>
	wxArrayString astrRunCfg;	// content between <Run>...</Run>
};

// All configurations read from the XML file
WX_DECLARE_OBJARRAY(romloader_openocd_config, romloader_openocd_configs);
#include <wx/arrimpl.cpp> // this is a magic incantation which must be done!
WX_DEFINE_OBJARRAY(romloader_openocd_configs);

romloader_openocd_configs m_atCfgs;


// configuration of the output handler.
class output_handler_priv
{
public:
	unsigned long iInstanceNo;	// Instance number to print in log messages. Not used yet
	bool fDoLog;				// if true, print to log.
	wxString strLogOutput;		// collect log output char by char (i.e. currently not used)
	bool fAppendOutput;			// if true, the output handler appends output to strOutput
	wxString strOutput;			// output to pass to callback
};

/*-------------------------------------*/

/* This version ignores the char messages (not used by our put/flush 
   routines on netx side */

int romloader_openocd_default_output_handler(struct command_context_s *context, char* line)
{
	output_handler_priv *ptPriv = (output_handler_priv*) context->output_handler_priv;
	if (ptPriv->fDoLog) 
	{
		unsigned long iInstanceNo = ptPriv->iInstanceNo;
		wxLogMessage(wxT("romloader_openocd(%x) %s"), iInstanceNo, line);
	}
	if (ptPriv->fAppendOutput) 
	{
		ptPriv->strOutput.Append(line);
		ptPriv->strOutput.Append(wxT("\n"));
	}
	return ERROR_OK;
}

/* There are two routes through which messages can arrive here:
target.c:target_call_timer_callbacks()
--> target_request.c:target_request()
--> target_request.c:target_asciimsg()
--> command.c:command_print()
--> romloader_openocd_default_output_handler
In this case, a string of arbitrary length without CR is passed.

target.c:target_call_timer_callbacks()
--> target_request.c:target_request()
--> target_request.c:target_asciichar()
--> romloader_openocd_default_output_handler
In this case, the string contains exactly 1 char, a CR terminating a message passed to uprintf is passed through.

- always append line to strLogOutput
- if length != 1, add \n
- if lenth != 1 or line = \n, pass on message
*/
#if 0
int romloader_openocd_default_output_handler(struct command_context_s *context, char* line)
{
	output_handler_priv *ptPriv = (output_handler_priv*) context->output_handler_priv;
	unsigned long iInstanceNo = ptPriv->iInstanceNo;
	size_t len = strlen(line);
	ptPriv->strLogOutput.Append(line);
	if (len!=1) ptPriv->strLogOutput.Append(wxT("\n"));
	if (len!=1 || strcmp(line, "\n")==0 /* || strcmp(line, "\r")==0 */)
	{
		wxLogMessage(wxT("romloader_openocd(%x) %s"), iInstanceNo, ptPriv->strLogOutput);
		if (ptPriv->fAppendOutput) ptPriv->strOutput.Append(ptPriv->strLogOutput);
		ptPriv->strLogOutput.Empty();
	}

	return ERROR_OK;
}
#endif

void romloader_openocd_log_printf(enum log_levels level, const char *format, ...)
{
	wxString strMsg;
	va_list args;
	char buffer[512];


	if (level > debug_level)
		return;

	va_start(args, format);
	vsnprintf(buffer, 512, format, args);

	strMsg = wxString::FromAscii(buffer);
	switch(level)
	{
	case LOG_ERROR:
		wxLogError(strMsg);
		break;
	case LOG_WARNING:
		wxLogWarning(strMsg);
		break;
	case LOG_INFO:
		wxLogMessage(strMsg);
		break;
	case LOG_DEBUG:
		wxLogDebug(strMsg);
		break;
	default:
		wxLogError(wxT("unknown errorlevel, ") + strMsg);
		break;
	}

	va_end(args);
}


void romloader_openocd_short_log_printf(enum log_levels level, const char *format, ...)
{
	wxString strMsg;
	va_list args;
	char buffer[512];


	if (level > debug_level)
		return;

	va_start(args, format);
	vsnprintf(buffer, 512, format, args);

	strMsg = wxString::FromAscii(buffer);
	switch(level)
	{
	case LOG_ERROR:
		wxLogError(strMsg);
		break;
	case LOG_WARNING:
		wxLogWarning(strMsg);
		break;
	case LOG_INFO:
		wxLogMessage(strMsg);
		break;
	case LOG_DEBUG:
		wxLogDebug(strMsg);
		break;
	default:
		wxLogError(wxT("unknown errorlevel, ") + strMsg);
		break;
	}

	va_end(args);
}


void splitStringToArray(wxString &strLines, wxArrayString *ptArray)
{
	wxString strLine;
	size_t sizLineStart;
	size_t sizPos;
	size_t sizLen;
	wxChar c;


	sizPos = sizLineStart = 0;
	sizLen = strLines.Len();
	while( sizPos<sizLen )
	{
		// find next break
		c = strLines.GetChar(sizPos);
		++sizPos;
		if( c==wxT('\n') || c== wxT('\r') )
		{
			// found line end
			strLine = strLines.SubString(sizLineStart, sizPos);
			// trim string from both sides
			strLine.Trim(true);
			strLine.Trim(false);
			// add string to the array
			if( strLine.IsEmpty()==false )
			{
				ptArray->Add(strLine);
				wxLogMessage(wxT("romloader_openocd(") + plugin_desc.strPluginId + wxT(") : ") + strLine);
			}
			// new line starts after eol
			sizLineStart = sizPos;
		}
	}

	// chars left without newline?
	if( sizLineStart<sizPos )
	{
		strLine = strLines.SubString(sizLineStart, sizPos);
		// trim string from both sides
		strLine.Trim(true);
		strLine.Trim(false);
		if( strLine.IsEmpty()==false )
		{
			ptArray->Add(strLine);
			wxLogMessage(wxT("romloader_openocd(") + plugin_desc.strPluginId + wxT(") : ") + strLine);
		}
	}
}


int readXmlTextArray(wxXmlNode *ptCfgNode, wxString strNodeName, wxArrayString *ptArray)
{
	wxXmlNode *ptNode;
	wxString strCfg;
	int iResult;
	wxString strMsg;


	/* expect failure */
	iResult = 1;

	wxLogMessage(wxT("romloader_openocd(") + plugin_desc.strPluginId + wxT(") : reading section '") + strNodeName + wxT("'"));

	/* get the init node */
	if( ptCfgNode==NULL )
	{
		wxLogMessage(wxT("romloader_openocd(") + plugin_desc.strPluginId + wxT(") : missing config node!"));
	}
	else
	{
		ptNode = ptCfgNode->GetChildren();
		while( ptNode!=NULL )
		{
			if( ptNode->GetType()==wxXML_ELEMENT_NODE && ptNode->GetName()==strNodeName )
			{
				break;
			}
			ptNode = ptNode->GetNext();
		}
		// not found (node is NULL) ?
		if( ptNode==NULL )
		{
			wxLogError(wxT("romloader_openocd(") + plugin_desc.strPluginId + wxT(") : failed to find '") + strNodeName + wxT("' section in config!"));
		}
		else
		{
			strCfg = ptNode->GetNodeContent();
			if( strCfg.IsEmpty()!=false )
			{
				wxLogError(wxT("romloader_openocd(") + plugin_desc.strPluginId + wxT(") : empty '") + strNodeName + wxT("' section in config!"));
			}
			else
			{
				/* split text into lines */
				splitStringToArray(strCfg, ptArray);
				iResult = 0;
			}
		}
	}

	return iResult;
}


/*-------------------------------------*/


int fn_init(wxLog *ptLogTarget, wxXmlNode *ptCfgNode, wxString &strPluginId)
{
	int iResult = 0;
	bool fOk;
	wxLog *pOldLogTarget;
	romloader_openocd_config *ptCfg;


	/* set main app's log target */
	pOldLogTarget = wxLog::GetActiveTarget();
	if( pOldLogTarget!=ptLogTarget )
	{
		wxLog::SetActiveTarget(ptLogTarget);
		if( pOldLogTarget!=NULL )
		{
			delete pOldLogTarget;
		}
	}

	/* say hi */
	wxLogMessage(wxT("bootloader openocd plugin init: ") + strPluginId);

	/* remember id */
	plugin_desc.strPluginId = strPluginId;

	/* init the lua state */
	m_ptLuaState = NULL;

	/* clear any stored configs */
	m_atCfgs.Empty();

	if (!ft2232_load())
	{
		wxLogMessage(wxT("romloader_openocd(%s) : ftd2xx.dll not loaded. JTAG will not be available."), plugin_desc.strPluginId);
	}
	else
	{
		wxLogMessage(wxT("romloader_openocd(%s) : ftd2xx.dll loaded"), plugin_desc.strPluginId);

		while (ptCfgNode != NULL)
		{
			ptCfg = new romloader_openocd_config();
			fOk = ptCfgNode->GetPropVal(wxT("id"), &ptCfg->strCfgName);
			if( fOk!=true || ptCfg->strCfgName.IsEmpty())
			{
				iResult = 1;
			}

			if( iResult==0 )
			{
				wxLogMessage(wxT("romloader_openocd(%s) : reading config '%s'"), plugin_desc.strPluginId, ptCfg->strCfgName);
				iResult = readXmlTextArray(ptCfgNode, wxT("Init"), &ptCfg->astrInitCfg);
			}
			if( iResult==0 )
			{
				iResult = readXmlTextArray(ptCfgNode, wxT("Run"), &ptCfg->astrRunCfg);
			}

			if (iResult == 0)
			{
				m_atCfgs.Add(ptCfg);
				ptCfgNode = ptCfgNode->GetNext();
			}
			else
			{
				delete ptCfg;
				break;
			}

		}
	}
	return iResult;
}


/*-------------------------------------*/

int fn_init_lua(wxLuaState *ptLuaState)
{
	bool fSuccess;


	/* remember the lua state for instance creation */
	m_ptLuaState = ptLuaState;

	/* init the lua bindings */
	fSuccess = wxLuaBinding_romloader_openocd_lua_init();
	if( fSuccess!=true )
	{
		wxLogMessage(wxT("failed to init romloader_openocd lua bindings"));
		return -1;
	}

	return 0;
}


/*-------------------------------------*/

int fn_leave(void)
{
	wxLogMessage(wxT("bootloader openocd plugin leave"));
	ft2232_unload();
	wxLogMessage(wxT("bootloader openocd ftd2xx.dll unloaded."));
	return 0;
}

/*-------------------------------------*/

const muhkuh_plugin_desc *fn_get_desc(void)
{
	return &plugin_desc;
}

/*-------------------------------------*/


static void romloader_openocd_close_instance(void *pvHandle)
{
	command_context_t *cmd_ctx;
	output_handler_priv *ptPriv;
	int iResult;
	wxString strMsg;


	/* cast the handle to the command context */
	cmd_ctx = (command_context_t*)pvHandle;

	strMsg.Printf(wxT("closing romloader openocd at %p"), cmd_ctx);
	wxLogMessage(strMsg);

	// close jtag after closing the target, as target_close may still
	// access the target to restore working areas
	iResult = target_close(cmd_ctx);
	if( iResult!=ERROR_OK )
	{
		strMsg.Printf(wxT("failed to close target interface: %d"), iResult);
		wxLogWarning(strMsg);
	}

	/* NOTE: this seems to work with ftd2xx, but not with libftdi */
	if( jtag!=NULL && jtag->quit!=NULL )
	{
		jtag->quit();
	}

	/* close all subsystems */
	iResult = jtag_close(cmd_ctx);
	if( iResult!=ERROR_OK )
	{
		strMsg.Printf(wxT("failed to close jtag interface: %d"), iResult);
		wxLogWarning(strMsg);
	}

	ptPriv = (output_handler_priv*) cmd_ctx->output_handler_priv;
	if (ptPriv != NULL) delete ptPriv;

	// free commandline interface
	command_done(cmd_ctx);

}


/*-----------------------------------*/

static int romloader_openocd_connect(romloader_openocd_config &tCfg, command_context_t **pptCmdCtx, bool fDetect)
{
	int iResult;
	command_context_t *cmd_ctx;
	output_handler_priv *ptPriv;
	target_t *target;
	wxString strCmd;
	wxString strMsg;
	size_t sizCfgCnt;
	size_t sizCfgMax;
	int iInitCnt;


	cmd_ctx = command_init();

	/* register subsystem commands */
	log_register_commands(cmd_ctx);
	jtag_register_commands(cmd_ctx);
	interpreter_register_commands(cmd_ctx);
	xsvf_register_commands(cmd_ctx);
	target_register_commands(cmd_ctx);

	/* init the log functions */
	iResult = log_init(cmd_ctx);
	if( iResult!=ERROR_OK )
	{
		strMsg.Printf(wxT("failed to init log level: %d"), iResult);
		wxLogError(strMsg);
	}
	else
	{
		ptPriv = new output_handler_priv();
		ptPriv->iInstanceNo = 0;
		ptPriv->fAppendOutput = false;
		ptPriv->fDoLog = true;
		command_set_output_handler(cmd_ctx, romloader_openocd_default_output_handler, ptPriv);
		log_set_output_handler(romloader_openocd_log_printf, romloader_openocd_short_log_printf);
		cmd_ctx->mode = COMMAND_CONFIG;

		// set config
		sizCfgCnt = 0;
		sizCfgMax = tCfg.astrInitCfg.GetCount();
		while( sizCfgCnt<sizCfgMax )
		{
			strCmd = tCfg.astrInitCfg.Item(sizCfgCnt);
			wxLogMessage(wxT("command: ") + strCmd);
			iResult = command_run_line(cmd_ctx, strCmd.ToAscii());
			if( iResult!=ERROR_OK )
			{
				strMsg.Printf(wxT("failed to set config: %d"), iResult);
				wxLogError(strMsg);
				strMsg = wxT("error line was: '") + strCmd + wxT("'");
				wxLogError(strMsg);
				break;
			}
			++sizCfgCnt;
		}
		if( iResult==ERROR_OK )
		{
			cmd_ctx->mode = COMMAND_EXEC;

			iResult = jtag_init(cmd_ctx);
			if( iResult!=ERROR_OK )
			{
				strMsg.Printf(wxT("failed to init jtag: %d"), iResult);
				wxLogError(strMsg);
			}
			else
			{
				iResult = target_init(cmd_ctx);
				if( iResult!=ERROR_OK )
				{
					strMsg.Printf(wxT("failed to init jtag: %d"), iResult);
					wxLogError(strMsg);
				}
				else
				{
					/* wait for target reset */
					wxMilliSleep(500);
					target = get_current_target(cmd_ctx);
					iInitCnt = 15;
					do
					{
						target_call_timer_callbacks();
						wxMilliSleep(100);
					} while( --iInitCnt>0 && target->state!=TARGET_HALTED);

					if( target->state!=TARGET_HALTED )
					{
						wxLogError(wxT("failed to halt the target"));
						iResult = ERROR_TARGET_NOT_HALTED;
					}
					else if (fDetect==false)
					{
						// set config
						sizCfgCnt = 0;
						sizCfgMax = tCfg.astrRunCfg.GetCount();
						while( sizCfgCnt<sizCfgMax )
						{
							strCmd = tCfg.astrRunCfg.Item(sizCfgCnt);
							wxLogMessage(wxT("command: ") + strCmd);
							iResult = command_run_line(cmd_ctx, strCmd.ToAscii());
							if( iResult!=ERROR_OK )
							{
								strMsg.Printf(wxT("failed to run command: %d"), iResult);
								wxLogError(strMsg);
								strMsg = wxT("error line was: '") + strCmd + wxT("'");
								wxLogError(strMsg);
								break;
							}
							++sizCfgCnt;
						}
						if( iResult!=ERROR_OK )
						{
							wxLogError(wxT("config failed!"));
						}
					}
				}
			}
		}
	}

	if( iResult!=ERROR_OK )
	{
		// close connection
		romloader_openocd_close_instance(cmd_ctx);
		*pptCmdCtx = NULL;
	}
	else
	{
		// connection open, ok!
		*pptCmdCtx = cmd_ctx;
	}

	return iResult;
}

/*-----------------------------------*/

int fn_detect_interfaces(std::vector<muhkuh_plugin_instance*> *pvInterfaceList)
{
	int iInterfaces;
	size_t iCfgCount;
	size_t iCfgNo;
	
	int iResult;
	muhkuh_plugin_instance *ptInst;
	command_context_t *cmd_ctx;
	wxString strName;
	wxString strTyp;
	wxString strLuaCreateFn;


	strTyp = plugin_desc.strPluginId;
	strLuaCreateFn = wxT("muhkuh.romloader_openocd_create");

	iCfgCount = m_atCfgs.Count();
	iInterfaces = 0;

	if (ft2232_isloaded())
	{
		for (iCfgNo = 0; iCfgNo<iCfgCount; ++iCfgNo)
		{
			romloader_openocd_config &ptCfg = m_atCfgs.Item(iCfgNo);

			// detect interface by trying to open it
			iResult = romloader_openocd_connect(ptCfg, &cmd_ctx, true);
			if( iResult==ERROR_OK )
			{
				// close the instance
				romloader_openocd_close_instance(cmd_ctx);

				// construct the name
				//strName.Printf(wxT("romloader_openocd  %s (#%d)"), ptCfg.strCfgName, iCfgNo);
				strName.Printf(wxT("romloader_openocd_%s"), ptCfg.strCfgName);
				ptInst = new muhkuh_plugin_instance(strName, strTyp, false, strLuaCreateFn, (void*) iCfgNo);
				++iInterfaces;

				// add the new instance to the list
				pvInterfaceList->push_back(ptInst);
			}
		}
	}
	return iInterfaces;
}


/*-------------------------------------*/

romloader *romloader_openocd_create(void *pvHandle)
{
	unsigned long iCfgNo;
	size_t iCfgCount;
	int iResult;
	command_context_t *cmd_ctx;
	wxString strTyp;
	wxString strName;
	romloader *ptInstance = NULL;

	iCfgNo = (unsigned long) pvHandle;
	iCfgCount = m_atCfgs.Count();

	if (iCfgNo >= iCfgCount)
	{
		wxLogError(wxT("romloader_openocd_create: handle %d is no valid plugin handle (max: %d)"), iCfgNo, iCfgCount);
	}
	else
	{
		wxLogMessage(wxT("romloader_openocd_create: trying to connect with config %d of %d"), iCfgNo+1, iCfgCount);
		romloader_openocd_config &ptCfg = m_atCfgs.Item(iCfgNo);

		iResult = romloader_openocd_connect(ptCfg, &cmd_ctx, false);
		if( iResult==ERROR_OK )
		{
			// create the new instance
			strTyp = plugin_desc.strPluginId;
			//strName.Printf(wxT("romloader_openocd %s (#%d)"), ptCfg.strCfgName, iCfgNo);
			strName.Printf(wxT("romloader_openocd_%s"), ptCfg.strCfgName);
			ptInstance = new romloader_openocd(strName, strTyp, 
				&tFunctionInterface, cmd_ctx, (muhkuh_plugin_fn_close_instance)romloader_openocd_close_instance, 
				m_ptLuaState);
		}
		else
		{
			wxLogError(wxT("romloader_openocd_create: connection attempt failed"));
		}

	}


	return ptInstance;
}

/*
romloader(wxString strName, wxString strTyp, 
	const romloader_functioninterface *ptFn, void *pvHandle, muhkuh_plugin_fn_close_instance fn_close, 
	wxLuaState *ptLuaState);
*/

romloader_openocd::romloader_openocd(wxString strName, wxString strTyp, 
									 const romloader_functioninterface *ptFn, 
									 void *pvHandle, 
									 muhkuh_plugin_fn_close_instance fn_close, 
									 wxLuaState *ptLuaState)
 : romloader(strName, strTyp, ptFn, pvHandle, fn_close,ptLuaState)
{
	// unfortunately, this is inaccessible by the C routines
	m_strMe.Printf(wxT("romloader_openocd(%p): "), this);
	wxLogMessage(wxT("%s constructor"), m_strMe);
}

romloader_openocd::~romloader_openocd(void){
	wxLogMessage(wxT("%s destructor"), m_strMe);
}

void romloader_openocd::connect(void){
	romloader::connect();
	detect_chiptyp();
}

/*-------------------------------------*/


/* open the connection to the device */
bool fn_connect(void *pvHandle)
{
	return true;
}


/* close the connection to the device */
void fn_disconnect(void *pvHandle)
{
}


/* returns the connection state of the device */
// TODO: Why false?
bool fn_is_connected(void *pvHandle)
{
	return false;
}


/* read a byte (8bit) from the netx to the pc */
int fn_read_data08(void *pvHandle, unsigned long ulNetxAddress, unsigned char *pucData)
{
	command_context_t *cmd_ctx;
	target_t *target;
	int iResult;


	/* cast the handle to the command context */
	cmd_ctx = (command_context_t*)pvHandle;

	/* get the target from the command context */
	target = get_current_target(cmd_ctx);

	/* read the data from the netX */
	iResult = target_read_u8(target, ulNetxAddress, pucData);
	if( iResult==ERROR_OK )
	{
		iResult = 0;
	}
	else
	{
		iResult = 1;
	}

	wxMilliSleep(1);

	return iResult;
}


/* read a word (16bit) from the netx to the pc */
int fn_read_data16(void *pvHandle, unsigned long ulNetxAddress, unsigned short *pusData)
{
	command_context_t *cmd_ctx;
	target_t *target;
	int iResult;


	/* cast the handle to the command context */
	cmd_ctx = (command_context_t*)pvHandle;

	/* get the target from the command context */
	target = get_current_target(cmd_ctx);

	/* read the data from the netX */
	iResult = target_read_u16(target, ulNetxAddress, pusData);
	if( iResult==ERROR_OK )
	{
		iResult = 0;
	}
	else
	{
		iResult = 1;
	}

	wxMilliSleep(1);

	return iResult;
}


/* read a long (32bit) from the netx to the pc */
int fn_read_data32(void *pvHandle, unsigned long ulNetxAddress, unsigned long *pulData)
{
	command_context_t *cmd_ctx;
	target_t *target;
	int iResult;


	/* cast the handle to the command context */
	cmd_ctx = (command_context_t*)pvHandle;

	/* get the target from the command context */
	target = get_current_target(cmd_ctx);

	/* read the data from the netX */
	iResult = target_read_u32(target, ulNetxAddress, (u32*)pulData);
	if( iResult==ERROR_OK )
	{
		iResult = 0;
	}
	else
	{
		iResult = 1;
	}

	wxMilliSleep(1);

	return iResult;
}

/* read a byte array from the netx to the pc */
int fn_read_image(void *pvHandle, unsigned long ulNetxAddress, char *pcData, unsigned long ulSize, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData)
{
	int iResult = 0;
	unsigned long ulChunkSize;
	unsigned long ulBytesLeft;
	bool fIsRunning;
	command_context_t *cmd_ctx;
	target_t *target;
	wxString strErrorMsg;


	/* cast the handle to the command context */
	cmd_ctx = (command_context_t*)pvHandle;

	/* get the target from the command context */
	target = get_current_target(cmd_ctx);


	/* Set chunk size: if a callback is defined, transmit several small chunks,
		if no callabck is defined, transmit all at once. */
	if (iLuaCallbackTag!=0)
	{
		ulChunkSize = OPENOCD_READ_IMAGE_CHUNKSIZE;
	}
	else
	{
		ulChunkSize = ulSize;
	}

	/* read the data from the netX */
	ulBytesLeft = ulSize;
	while (ulBytesLeft > 0)
	{
		/* last chunk may be smaller */
		if (ulChunkSize > ulBytesLeft)
		{
			ulChunkSize = ulBytesLeft;
		}

		/* callback */
		fIsRunning = romloader::callback_long(L, iLuaCallbackTag, ulSize-ulBytesLeft, pvCallbackUserData);
		if( fIsRunning!=true )
		{
			wxLogMessage(wxT("romloader_openocd: read_image canceled"));
			iResult = -2;
			break;
		}

		/* read a chunk */
		wxLogDebug(wxT("target_read_buffer: ->%x size %x"), ulNetxAddress, ulChunkSize);
		iResult = target_read_buffer(target, ulNetxAddress, ulChunkSize, (u8*)pcData);
		if( iResult!=ERROR_OK )
		{
			wxLogError(wxT("romloader_openocd: Error reading memory: target_read_buffer returned %d"), iResult);
			iResult = 1;
			break;
		}

		ulNetxAddress += ulChunkSize;
		pcData += ulChunkSize;
		ulBytesLeft -= ulChunkSize;

		wxMilliSleep(1);
	}

	return iResult;
}

/* write a byte (8bit) from the pc to the netx */
int fn_write_data08(void *pvHandle, unsigned long ulNetxAddress, unsigned char ucData)
{
	command_context_t *cmd_ctx;
	target_t *target;
	int iResult;


	/* cast the handle to the command context */
	cmd_ctx = (command_context_t*)pvHandle;

	/* get the target from the command context */
	target = get_current_target(cmd_ctx);

	/* read the data from the netX */
	iResult = target_write_u8(target, ulNetxAddress, ucData);
	if( iResult==ERROR_OK )
	{
		iResult = 0;
	}
	else
	{
		iResult = 1;
	}

	wxMilliSleep(1);

	return iResult;
}


/* write a word (16bit) from the pc to the netx */
int fn_write_data16(void *pvHandle, unsigned long ulNetxAddress, unsigned short usData)
{
	command_context_t *cmd_ctx;
	target_t *target;
	int iResult;


	/* cast the handle to the command context */
	cmd_ctx = (command_context_t*)pvHandle;

	/* get the target from the command context */
	target = get_current_target(cmd_ctx);

	/* read the data from the netX */
	iResult = target_write_u16(target, ulNetxAddress, usData);
	if( iResult==ERROR_OK )
	{
		iResult = 0;
	}
	else
	{
		iResult = 1;
	}

	wxMilliSleep(1);

	return iResult;
}


/* write a long (32bit) from the pc to the netx */
int fn_write_data32(void *pvHandle, unsigned long ulNetxAddress, unsigned long ulData)
{
	command_context_t *cmd_ctx;
	target_t *target;
	int iResult;


	/* cast the handle to the command context */
	cmd_ctx = (command_context_t*)pvHandle;

	/* get the target from the command context */
	target = get_current_target(cmd_ctx);

	/* read the data from the netX */
	iResult = target_write_u32(target, ulNetxAddress, ulData);
	if( iResult==ERROR_OK )
	{
		iResult = 0;
	}
	else
	{
		iResult = 1;
	}

	wxMilliSleep(1);

	return iResult;
}


/* write a byte array from the pc to the netx */
int fn_write_image(void *pvHandle, unsigned long ulNetxAddress, const char *pcData, unsigned long ulSize, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData)
{
	int iResult = 0;
	unsigned long ulChunkSize;
	unsigned long ulBytesLeft;
	bool fIsRunning;
	command_context_t *cmd_ctx;
	target_t *target;
	wxString strErrorMsg;


	/* cast the handle to the command context */
	cmd_ctx = (command_context_t*)pvHandle;

	/* get the target from the command context */
	target = get_current_target(cmd_ctx);


	/* Set chunk size: if a callback is defined, transmit several small chunks,
		if no callabck is defined, transmit all at once. */
	if (iLuaCallbackTag!=0)
	{
		ulChunkSize = OPENOCD_WRITE_IMAGE_CHUNKSIZE;
	}
	else
	{
		ulChunkSize = ulSize;
	}

	/* read the data from the netX */
	ulBytesLeft = ulSize;
	while (ulBytesLeft > 0)
	{
		/* last chunk may be smaller */
		if (ulChunkSize > ulBytesLeft)
		{
			ulChunkSize = ulBytesLeft;
		}

		/* callback */
		fIsRunning = romloader::callback_long(L, iLuaCallbackTag, ulSize-ulBytesLeft, pvCallbackUserData);
		if( fIsRunning!=true )
		{
			wxLogMessage(wxT("romloader_openocd: write_image canceled!"));
			iResult = -2;
			break;
		}

		/* read a chunk */
		wxLogDebug(wxT("target_write_buffer: ->%x size %x"), ulNetxAddress, ulChunkSize);
		iResult = target_write_buffer(target, ulNetxAddress, ulChunkSize, (u8*)pcData);
		if( iResult!=ERROR_OK )
		{
			wxLogError(wxT("romloader_openocd: Error writing memory: target_write_buffer returned %d"), iResult);
			iResult = 1;
			break;
		}

		ulNetxAddress += ulChunkSize;
		pcData += ulChunkSize;
		ulBytesLeft -= ulChunkSize;

		wxMilliSleep(1);
	}

	return iResult;
}


/* call routine */
int fn_call(void *pvHandle, unsigned long ulNetxAddress, unsigned long ulParameterR0, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData)
{
	command_context_t *cmd_ctx;
	int iOocdResult;
	int iResult;
	wxString strCmd;
	target_t *target;
	bool fIsRunning;
	enum target_state state;


	/* cast the handle to the command context */
	cmd_ctx = (command_context_t*)pvHandle;

	// expect failure
	iResult = 1;

	// set R0 parameter
	strCmd.Printf(wxT("reg r0 0x%08X"), ulParameterR0);
	iOocdResult = command_run_line(cmd_ctx, strCmd.ToAscii());
	if( iOocdResult!=ERROR_OK )
	{
		wxLogError(wxT("config failed!"));
	}
	else
	{
		// resume <ulNetxAddress>
		strCmd.Printf(wxT("resume 0x%08X"), ulNetxAddress);
		iOocdResult = command_run_line(cmd_ctx, strCmd.ToAscii());
		if( iOocdResult!=ERROR_OK )
		{
			wxLogError(wxT("config failed!"));
		}
		else
		{
			// wait for halt
			target = get_current_target(cmd_ctx);
			output_handler_priv *ptPriv = (output_handler_priv*) cmd_ctx->output_handler_priv;

			// redirect output if a callback is given
			if (iLuaCallbackTag!=0)
			{
				ptPriv->strOutput.Empty();
				ptPriv->fAppendOutput = true;
				ptPriv->fDoLog = false;
			}

			do
			{
				wxMilliSleep(OPENOCD_CALL_TIMESLICE_MS);

				// execute callback in the Lua script
				fIsRunning = romloader::callback_string(L, iLuaCallbackTag, ptPriv->strOutput, pvCallbackUserData);
				ptPriv->strOutput.Empty();

				// check if the target is still running
				target->type->poll(target);
				state = target->state;

				if( state==TARGET_HALTED )
				{
					wxLogMessage(wxT("call finished!"));
					iResult = 0;
				}
				else
				{
					if( fIsRunning!=true )
					{
						// operation was canceled, halt the target
						wxLogMessage(wxT("Call canceled, stopping target..."));
						target = get_current_target(cmd_ctx);
						iOocdResult = target->type->halt(target);
						if( iOocdResult!=ERROR_OK && iOocdResult!=ERROR_TARGET_ALREADY_HALTED )
						{
							wxLogError(wxT("Failed to halt target!"));
						}
						break;
					}
					else
					{
						target_call_timer_callbacks();
					}
				}
			} while( state!=TARGET_HALTED );

			if (iLuaCallbackTag!=0)
			{
				ptPriv->fAppendOutput = false;
				ptPriv->fDoLog = true;
			}

			// usb cmd delay
			wxMilliSleep(1);
		}
	}

	return iResult;
}




/*-------------------------------------*/

