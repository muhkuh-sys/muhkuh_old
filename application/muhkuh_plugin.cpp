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


#include "muhkuh_plugin.h"

#include <wx/file.h>
#include <wx/wfstream.h>


muhkuh_plugin::muhkuh_plugin(void)
 : m_fPluginIsOk(false)
 , m_fPluginIsEnabled(false)
 , m_ptLuaState(NULL)
 , m_ptCfgNode(NULL)
 , m_strInitError(wxEmptyString)
{
	// set prefix for messages
	setMe();

	memset(&m_tPluginIf, 0, sizeof(muhkuh_plugin_interface));
}


muhkuh_plugin::muhkuh_plugin(wxString strPluginCfgPath)
 : m_fPluginIsOk(false)
 , m_fPluginIsEnabled(false)
 , m_ptLuaState(NULL)
 , m_ptCfgNode(NULL)
 , m_strInitError(wxEmptyString)
{
	// set prefix for messages
	setMe();

	memset(&m_tPluginIf, 0, sizeof(muhkuh_plugin_interface));

	// try to open the plugin
	Load(strPluginCfgPath);
}


muhkuh_plugin::muhkuh_plugin(const muhkuh_plugin *ptCloneMe)
 : m_fPluginIsOk(false)
 , m_fPluginIsEnabled(false)
 , m_ptLuaState(NULL)
 , m_ptCfgNode(NULL)
 , m_strInitError(wxEmptyString)
{
	// set prefix for messages
	setMe();

	memset(&m_tPluginIf, 0, sizeof(muhkuh_plugin_interface));

	// copy the enable flag
	m_fPluginIsEnabled = ptCloneMe->m_fPluginIsEnabled;

	// try to open the plugin
	Load(ptCloneMe->m_strPluginCfgPath);
}


muhkuh_plugin::muhkuh_plugin(wxConfigBase *pConfig)
 : m_fPluginIsOk(false)
 , m_fPluginIsEnabled(false)
 , m_ptLuaState(NULL)
 , m_ptCfgNode(NULL)
 , m_strInitError(wxEmptyString)
{
	bool fReadOk;
	wxString strPath;


	// set prefix for messages
	setMe();

	memset(&m_tPluginIf, 0, sizeof(muhkuh_plugin_interface));

	// try to get the settings
	fReadOk = pConfig->Read(wxT("path"), &strPath);
	if( fReadOk==false )
	{
		setInitError(_("missing path information in the application config file"), wxEmptyString);
	}
	else
	{
		fReadOk = pConfig->Read(wxT("enable"), &m_fPluginIsEnabled, false);
		if( fReadOk==false )
		{
			wxLogWarning(m_strMe + _("missing 'enable' entry in application config file, defaulting to 'disabled'"));
		}
		Load(strPath);
	}
}


muhkuh_plugin::~muhkuh_plugin(void)
{
	fn_leave();
	close();
}


void muhkuh_plugin::setMe(void)
{
	m_strMe.Printf(wxT("muhkuh_plugin(%p) :"), this);
}


void muhkuh_plugin::setInitError(wxString strMessage, wxString strPath)
{
	wxString strMsg;


	// build errormessage
	m_strInitError.Printf(_("The plugin '%s' failed to load: %s"), strPath.c_str(), strMessage.c_str());
	// show message in logfile
	wxLogError(m_strMe + m_strInitError);

	// the plugin state is "not ok"
	m_fPluginIsOk = false;
}


bool muhkuh_plugin::openXml(wxString strXmlPath)
{
	bool fResult;
	wxFile cFile;
	wxFileInputStream *ptFileInputStream;
	wxXmlNode *ptNode;
	wxXmlNode *ptCfgNode;
	wxString strCfgName;
	wxString strSoName;
	wxString strId;
	wxFileName cFileName;
	wxString strXmlCfgPath;


	// expect failure
	fResult = false;

	// clear config node
	m_ptCfgNode = NULL;

	// open the xml document
	fResult = cFile.Open(strXmlPath, wxFile::read);
	if( fResult!=true )
	{
		setInitError(_("failed to open the xml file"), strXmlPath);
	}
	else
	{
		ptFileInputStream = new wxFileInputStream(cFile);
		fResult = m_xmldoc.Load(*ptFileInputStream);
		if( fResult!=true )
		{
			// failed to load the xml file
			setInitError(_("failed to load the plugin config from the input stream"), strXmlPath);
		}
		else
		{
			// look for root node
			ptNode = m_xmldoc.GetRoot();
			while( ptNode!=NULL && ptNode->GetType()!=wxXML_ELEMENT_NODE && ptNode->GetName()!=wxT("MuhkuhPluginConfig") )
			{
				ptNode = ptNode->GetNext();
			}
			// found node?
			if( ptNode==NULL )
			{
				setInitError(_("Root node 'MuhkuhPluginConfig' not found. Is this really a Muhkuh plugin config?"), strXmlPath);
				fResult = false;
			}
			else
			{
				// expect failure
				fResult = false;
				// get name attribute
				if( ptNode->GetPropVal(wxT("name"), &strCfgName)!=true )
				{
					setInitError(_("Root node 'MuhkuhPluginConfig' has no 'name' attribute!"), strXmlPath);
				}
				else if( strCfgName.IsEmpty()!=false )
				{
					setInitError(_("The 'name' attribute is empty!"), strXmlPath);
				}
				else if( ptNode->GetPropVal(wxT("so"), &strSoName)!=true )
				{
					setInitError(_("Root node 'MuhkuhPluginConfig' has no 'so' attribute!"), strXmlPath);
				}
				else if( strSoName.IsEmpty()!=false )
				{
					setInitError(_("The 'so' attribute is empty!"), strXmlPath);
				}
				else if( ptNode->GetPropVal(wxT("id"), &strId)!=true )
				{
					setInitError(_("Root node 'MuhkuhPluginConfig' has no 'id' attribute!"), strXmlPath);
				}
				else if( strId.IsEmpty()!=false )
				{
					setInitError(_("The 'id' attribute is empty!"), strXmlPath);
				}
				else
				{
					// get the config node
					ptCfgNode = ptNode->GetChildren();
					while( ptCfgNode!=NULL && ptCfgNode->GetType()!=wxXML_ELEMENT_NODE && ptCfgNode->GetName()!=wxT("Cfg") )
					{
						ptCfgNode = ptCfgNode->GetNext();
					}
					// found node?
					if( ptCfgNode==NULL )
					{
						setInitError(_("Node 'Cfg' not found."), strXmlPath);
					}
					else
					{
						// all data found
						m_strCfgName = strCfgName;
						m_ptCfgNode = ptCfgNode;
						// get the directory of the xml file
						cFileName.Assign(strXmlPath);
						// save path to xml config to build a relative path to the dll later
						strXmlCfgPath = cFileName.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
						// assign the so name
						cFileName.Assign(strSoName);
						// if the so has no extension, set the standard one for the platform
						if( cFileName.HasExt()==false )
						{
							cFileName.Assign( wxDynamicLibrary::CanonicalizeName(strSoName, wxDL_MODULE) );
						}
						if( cFileName.IsRelative()==true )
						{
							cFileName.Normalize(wxPATH_NORM_ALL, strXmlCfgPath, wxPATH_NATIVE);
						}
						m_strSoName = cFileName.GetFullPath(wxPATH_NATIVE);
						m_strPluginId = strId;
						fResult = true;
					}
				}
			}
		}

		// delete xml input stream
		delete ptFileInputStream;
	}

	return fResult;
}


wxString muhkuh_plugin::GetConfigName(void) const
{
	return m_strCfgName;
}


bool muhkuh_plugin::Load(wxString strPluginCfgPath)
{
	bool fResult;
	int iResult;
	wxString strMsg;
	wxLog *ptLogTarget;
	const muhkuh_plugin_desc *ptDesc;


	wxLogMessage(m_strMe + _("loading plugin '%s'"), strPluginCfgPath.c_str());

	// do not open a plugin twice
	if( m_tPluginIf.tHandle!=NULL )
	{
		wxLogError(m_strMe + _("...plugin is already open"));
		fResult = false;
	}
	else
	{
		// plugin is not open yet, accept new name
		m_strPluginCfgPath = strPluginCfgPath;

		// init plugin description with defaults
		m_strCfgName = strPluginCfgPath;
		tPluginDesc.strPluginName = strPluginCfgPath;
		tPluginDesc.strPluginId = wxEmptyString;
		tPluginDesc.tVersion.uiVersionMajor = 0;
		tPluginDesc.tVersion.uiVersionMinor = 0;
		tPluginDesc.tVersion.uiVersionSub = 0;

		// open plugin xml config
		fResult = openXml(strPluginCfgPath);
		if( fResult!=true )
		{
			// NOTE: do not use 'setInitError' here. this errormessage is just a summary. 'openXml' provided the details already.
			wxLogError(m_strMe + _("failed to read plugin xml config"));
		}
		else
		{
			// try to open the plugin

			fResult = open(m_strSoName);
			if( fResult!=true )
			{
				// NOTE: do not use 'setInitError' here. this errormessage is just a summary. 'open' provided the details already.
				wxLogError(m_strMe + _("...failed to open the plugin "));
			}
			else
			{
				ptLogTarget = wxLog::GetActiveTarget();
				iResult = fn_init(ptLogTarget, m_ptCfgNode, m_strPluginId);
				if( iResult!=0 )
				{
					strMsg.Printf(_("Plugin init failed with errorcode %d"), iResult);
					setInitError(strMsg, strPluginCfgPath);
					fResult = false;
				}
				else
				{
					// get the description
					ptDesc = m_tPluginIf.uSym.tFn.fn_get_desc();
					if( ptDesc==NULL )
					{
						setInitError(_("Failed to get plugin description"), strPluginCfgPath);
						fResult = false;
					}
					else
					{
						// copy the structure elements
						if( ptDesc->strPluginName.IsEmpty()!=false )
						{
							tPluginDesc.strPluginName = _("missing name");
						}
						else
						{
							tPluginDesc.strPluginName = ptDesc->strPluginName;
						}
						tPluginDesc.strPluginId = ptDesc->strPluginId;
						tPluginDesc.tVersion = ptDesc->tVersion;

						// plugin is ready to use now
						m_fPluginIsOk = true;
					}
				}

				if( fResult!=true )
				{
					// close the plugin
					wxLogMessage(m_strMe + _("closing plugin '%s'"), strPluginCfgPath.c_str());
					close();
				}
			}
		}
	}

	return fResult;
}


bool muhkuh_plugin::IsOk(void) const
{
	return m_fPluginIsOk;
}


void muhkuh_plugin::SetEnable(bool fPluginIsEnabled)
{
	m_fPluginIsEnabled = fPluginIsEnabled;
}


bool muhkuh_plugin::GetEnable(void) const
{
	return m_fPluginIsEnabled;
}


wxString muhkuh_plugin::GetInitError(void) const
{
	return m_strInitError;
}


void muhkuh_plugin::write_config(wxConfigBase *pConfig)
{
	pConfig->Write(wxT("path"), m_strPluginCfgPath);
	pConfig->Write(wxT("enable"), m_fPluginIsEnabled);
}


int muhkuh_plugin::fn_init(wxLog *ptLogTarget, wxXmlNode *ptCfgNode, wxString &strPluginId)
{
	int iResult;
	wxString strMsg;


	// check for loaded plugin
	if( m_tPluginIf.tHandle!=NULL && m_tPluginIf.uSym.tFn.fn_init!=NULL )
	{
		iResult = m_tPluginIf.uSym.tFn.fn_init(ptLogTarget, ptCfgNode, strPluginId);
		if( iResult!=0 )
		{
			strMsg.Printf(_("failed to init plugin, fn_init returned: %d"), iResult);
			setInitError(strMsg, m_strCfgName);
		}
	}
	else
	{
		// plugin is not open, return some general error
		iResult = -1;
	}

	return iResult;
}


int muhkuh_plugin::fn_init_lua(wxLuaState *ptLuaState)
{
	int iResult;
	wxString strMsg;


	// check for loaded plugin
	if( m_tPluginIf.tHandle!=NULL && m_tPluginIf.uSym.tFn.fn_init_lua!=NULL )
	{
		iResult = m_tPluginIf.uSym.tFn.fn_init_lua(ptLuaState);
		if( iResult!=0 )
		{
			strMsg.Printf(_("failed to init plugin lua bindings, fn_init_lua returned: %d"), iResult);
			setInitError(strMsg, m_strCfgName);
		}
	}
	else
	{
		// plugin is not open, return some general error
		iResult = -1;
	}

	return iResult;
}


int muhkuh_plugin::fn_leave(void)
{
	int iResult;


	// check for loaded plugin
	if( m_tPluginIf.tHandle!=NULL && m_tPluginIf.uSym.tFn.fn_leave!=NULL )
	{
		iResult = m_tPluginIf.uSym.tFn.fn_leave();
	}
	else
	{
		// plugin is not open, return some general error
		iResult = -1;
	}

	return iResult;
}


const muhkuh_plugin_desc *muhkuh_plugin::fn_get_desc(void) const
{
	return &tPluginDesc;
}


int muhkuh_plugin::fn_detect_interfaces(std::vector<muhkuh_plugin_instance*> *pvInterfaceList)
{
	int iResult;


	// check for loaded plugin
	if( m_fPluginIsEnabled==true && m_tPluginIf.tHandle!=NULL && m_tPluginIf.uSym.tFn.fn_detect_interfaces!=NULL )
	{
		iResult = m_tPluginIf.uSym.tFn.fn_detect_interfaces(pvInterfaceList);
	}
	else
	{
		// plugin is not open, return some general error
		iResult = -1;
	}

	return iResult;
}


const muhkuh_plugin::muhkuh_plugin_symbol_offset_t muhkuh_plugin::atPluginSymbolOffsets[] =
{
	{
		wxT("fn_init"),
		offsetof(muhkuh_plugin_function_interface_t, fn_init) / sizeof(void*)
	},
	{
		wxT("fn_init_lua"),
		offsetof(muhkuh_plugin_function_interface_t, fn_init_lua) / sizeof(void*)
	},
	{
		wxT("fn_leave"),
		offsetof(muhkuh_plugin_function_interface_t, fn_leave) / sizeof(void*)
	},
	{
		wxT("fn_get_desc"),
		offsetof(muhkuh_plugin_function_interface_t, fn_get_desc) / sizeof(void*)
	},
	{
		wxT("fn_detect_interfaces"),
		offsetof(muhkuh_plugin_function_interface_t, fn_detect_interfaces) / sizeof(void*)
	}
};


bool muhkuh_plugin::open(wxString strPluginPath)
{
	wxFileName fileName;
	wxString strFullPath;
	wxDynamicLibrary dll;
	bool fDllLoaded;
	const muhkuh_plugin_symbol_offset_t *ptCnt, *ptEnd;
	void *pvPtr;
	wxString strMsg;
	wxDllType tDllHandle;


	// try to open a plugin

	// init result structure
	tDllHandle = NULL;

	fileName.Assign(strPluginPath, wxPATH_NATIVE);

	// check the path
	if( fileName.IsOk()==false )
	{
		// path is not correct
		setInitError(_("failed to set path, IsOK returned false"), strPluginPath);
	}
	else
	{
		strFullPath = fileName.GetFullPath();
		fDllLoaded = dll.Load(strFullPath, wxDL_DEFAULT);
		if( fDllLoaded==false )
		{
			strMsg.Printf(_("failed to load the shared object from '%s'"), strFullPath.c_str());
			setInitError(strMsg, strPluginPath);
		}
		else
		{
			// loop over all symbols in the list and enter them in the struct
			ptCnt = atPluginSymbolOffsets;
			ptEnd = ptCnt + (sizeof(atPluginSymbolOffsets)/sizeof(atPluginSymbolOffsets[0]));
			pvPtr = NULL;
			while( ptCnt<ptEnd )
			{
				pvPtr = dll.GetSymbol(ptCnt->pcSymbolName);
				if( pvPtr==NULL )
				{
					strMsg.Printf(_("failed to get symbol '%s' from shared object '%s'"), ptCnt->pcSymbolName, strFullPath.c_str());
					setInitError(strMsg, strPluginPath);
					break;
				}
				m_tPluginIf.uSym.apv[ptCnt->sizOffset] = pvPtr;

				// next symbol
				++ptCnt;
			};

			// check for error (pvPtr is NULL)
			if( pvPtr!=NULL )
			{
				// pvPtr is not NULL, this means all symbols were loaded

				// get handle for the dll
				tDllHandle = dll.Detach();
			}
			else
			{
				// unload the dll
				dll.Unload();
			}
		}
	}

	m_tPluginIf.tHandle = tDllHandle;
	return (tDllHandle!=NULL);
}


void muhkuh_plugin::close(void)
{
	if( m_tPluginIf.tHandle!=NULL )
	{
		wxDynamicLibrary::Unload(m_tPluginIf.tHandle);
	}
	memset(&m_tPluginIf, 0, sizeof(muhkuh_plugin_interface));
}

