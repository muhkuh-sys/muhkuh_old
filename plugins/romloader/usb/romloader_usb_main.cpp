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


#include "romloader_usb_main.h"
#include "_luaif/romloader_usb_wxlua_bindings.h"

#include <wx/wx.h>
#include <wx/regex.h>

/* used in read_image/write_image   test     old value */
#define USB_EXCHANGE_WRITE_TIMEOUT   700  //  200
#define USB_EXCHANGE_READ_TIMEOUT    700  //  200
/* used in call */
#define USB_CALL_WRITE_TIMEOUT       200  //  200
#define USB_CALL_READ_TIMEOUT       1000  //  200

/*-------------------------------------*/

static muhkuh_plugin_desc plugin_desc =
{
	wxT("USB Bootloader"),
	wxT(""),
	{ 0, 0, 1 }
};

static wxLuaState *m_ptLuaState;

/*-------------------------------------*/
/*
Changes:
replaced calls to usb_* with pfn_usb_*
Each pfn_usb_* is either a valid function from libusb0.dll or a dummy.

fn_detect_interfaces calls libusb_isloaded
fn_init calls libusb_loa
fn_leave calls libusb_unload

*/

#ifdef WIN32
/* global pointers to the used libusb functions */

usb_dev_handle *(* pfn_usb_open)(struct usb_device *dev);
int             (* pfn_usb_close)(usb_dev_handle *dev);
int             (* pfn_usb_bulk_write)(usb_dev_handle *dev, int ep, char *bytes, int size, int timeout);
int             (* pfn_usb_bulk_read)(usb_dev_handle *dev, int ep, char *bytes, int size, int timeout);
int             (* pfn_usb_set_configuration)(usb_dev_handle *dev, int configuration);
int             (* pfn_usb_claim_interface)(usb_dev_handle *dev, int _interface);
int             (* pfn_usb_release_interface)(usb_dev_handle *dev, int _interface);
int             (* pfn_usb_reset)(usb_dev_handle *dev);
void            (* pfn_usb_init)(void);
struct usb_bus *(* pfn_usb_get_busses)(void);
int             (* pfn_usb_find_busses)(void);
int             (* pfn_usb_find_devices)(void);

/* non-NULL if the dll has been loaded and the function pointers have been set */ 
wxDllType tDllHandle;

/* dummy functions */
usb_dev_handle * no_usb_open(struct usb_device *dev) {return NULL;}
int              no_usb_close(usb_dev_handle *dev) {return  LIBUSB_ERROR_NOT_SUPPORTED;}
int              no_usb_bulk_write(usb_dev_handle *dev, int ep, char *bytes, int size, int timeout) {return  LIBUSB_ERROR_NOT_SUPPORTED;}
int              no_usb_bulk_read(usb_dev_handle *dev, int ep, char *bytes, int size, int timeout) {return  LIBUSB_ERROR_NOT_SUPPORTED;}
int              no_usb_set_configuration(usb_dev_handle *dev, int configuration) {return  LIBUSB_ERROR_NOT_SUPPORTED;}
int              no_usb_claim_interface(usb_dev_handle *dev, int _interface) {return  LIBUSB_ERROR_NOT_SUPPORTED;}
int              no_usb_release_interface(usb_dev_handle *dev, int _interface) {return  LIBUSB_ERROR_NOT_SUPPORTED;}
int              no_usb_reset(usb_dev_handle *dev) {return  LIBUSB_ERROR_NOT_SUPPORTED;}
void             no_usb_init(void){}
struct usb_bus * no_usb_get_busses(void) {return NULL;}
int              no_usb_find_busses(void) {return  LIBUSB_ERROR_NOT_SUPPORTED;}
int              no_usb_find_devices(void) {return  LIBUSB_ERROR_NOT_SUPPORTED;}


/* auxiliary structures for reading the symbols from the dll */
typedef struct 
{
	const char *pstrFnName;
	void** ppFnPtr;
	void* pDefaultFnPtr;
} T_DLL_SYMBOL_ENTRY;

T_DLL_SYMBOL_ENTRY atLibusbSymbols[] = {
	{wxT("usb_open")              , (void**) &pfn_usb_open,                 (void*) no_usb_open},             
	{wxT("usb_close")             , (void**) &pfn_usb_close,                (void*) no_usb_close},            
	{wxT("usb_bulk_write")        , (void**) &pfn_usb_bulk_write,           (void*) no_usb_bulk_write},       
	{wxT("usb_bulk_read")         , (void**) &pfn_usb_bulk_read,            (void*) no_usb_bulk_read},        
	{wxT("usb_set_configuration") , (void**) &pfn_usb_set_configuration,    (void*) no_usb_set_configuration},
	{wxT("usb_claim_interface")   , (void**) &pfn_usb_claim_interface,      (void*) no_usb_claim_interface},  
	{wxT("usb_release_interface") , (void**) &pfn_usb_release_interface,    (void*) no_usb_release_interface},
	{wxT("usb_reset")             , (void**) &pfn_usb_reset,                (void*) no_usb_reset},            
	{wxT("usb_init")              , (void**) &pfn_usb_init,                 (void*) no_usb_init},             
	{wxT("usb_get_busses")        , (void**) &pfn_usb_get_busses,           (void*) no_usb_get_busses},       
	{wxT("usb_find_busses")       , (void**) &pfn_usb_find_busses,          (void*) no_usb_find_busses},      
	{wxT("usb_find_devices")      , (void**) &pfn_usb_find_devices,         (void*) no_usb_find_devices},     
};

#define ARRAY_ENDADDR(arrayname) arrayname + (sizeof(arrayname)/sizeof(arrayname[0]))

void libusb_setDummyFunctions();

bool libusb_load()
{
	wxDynamicLibrary dll;
	bool fDllLoaded;
	bool fLibusbPointersInitialized;

	fDllLoaded = dll.Load(wxT("libusb0"), wxDL_DEFAULT);
	if( fDllLoaded==false )
	{
		wxLogMessage(_("romloader_usb: Failed to load libusb."));
	}
	else
	{
		wxLogMessage(_("romloader_usb: libusb loaded."));
		T_DLL_SYMBOL_ENTRY *ptPfnEntry = atLibusbSymbols;
		T_DLL_SYMBOL_ENTRY *ptPfnTableEnd = ARRAY_ENDADDR(atLibusbSymbols);
		void* pvFn;

		fLibusbPointersInitialized = true;
		while (ptPfnEntry < ptPfnTableEnd)
		{
			pvFn = dll.GetSymbol(ptPfnEntry->pstrFnName);
			*(ptPfnEntry->ppFnPtr) = pvFn;

			if (pvFn == NULL)
			{
				fLibusbPointersInitialized = false;
				wxLogError(_("romloader_usb: Symbol %s not found in libusb"), ptPfnEntry->pstrFnName);
				break;
			}

			ptPfnEntry++;
		}
	}

	if( fDllLoaded && fLibusbPointersInitialized )
	{
		// get handle for the dll
		tDllHandle = dll.Detach();
		return true;
	}
	else
	{
		// unload the dll
		if (fDllLoaded) 
		{
			dll.Unload();
		}
		tDllHandle = NULL;
		libusb_setDummyFunctions();
		wxLogMessage(_("romloader_usb: netX500/100 USB devices will not be available."));
		return false;
	}
}


void libusb_setDummyFunctions()
{
	T_DLL_SYMBOL_ENTRY *ptPfnEntry = atLibusbSymbols;
	T_DLL_SYMBOL_ENTRY *ptPfnTableEnd = ARRAY_ENDADDR(atLibusbSymbols);
	while (ptPfnEntry < ptPfnTableEnd)
	{
		*ptPfnEntry->ppFnPtr = ptPfnEntry->pDefaultFnPtr;
		ptPfnEntry++;
	}
}

void libusb_unload()
{
	if (tDllHandle!=NULL)
	{
		wxDynamicLibrary::Unload(tDllHandle);
		tDllHandle = NULL;
		wxLogMessage(_("romloader_usb: libusb unloaded."));
	}
	libusb_setDummyFunctions();
}

bool libusb_isloaded()
{
	return (tDllHandle!=NULL);
}


#else /* Linux */

bool libusb_load() {return true;}
bool libusb_isloaded() {return true;}
void libusb_unload() {}

#define  pfn_usb_init               usb_init
#define  pfn_usb_open               usb_open
#define  pfn_usb_close              usb_close
#define  pfn_usb_reset              usb_reset
#define  pfn_usb_get_busses         usb_get_busses
#define  pfn_usb_find_busses        usb_find_busses
#define  pfn_usb_find_devices       usb_find_devices
#define  pfn_usb_get_descriptor     usb_get_descriptor
#define  pfn_usb_claim_interface    usb_claim_interface
#define  pfn_usb_release_interface  usb_release_interface
#define  pfn_usb_set_configuration  usb_set_configuration
#define  pfn_usb_bulk_write         usb_bulk_write
#define  pfn_usb_bulk_read          usb_bulk_read

#endif


/*-------------------------------------*/

/*
 * libusb-0.1 compatibility functions
 */

#if ROMLOADER_USB_LIBUSB_VERSION==0

#ifdef WIN32
#define LIBUSB_BUS_PATTERN "bus-%u"
#define LIBUSB_DEV_PATTERN "\\\\.\\libusb0-%u"
#else
#define LIBUSB_BUS_PATTERN "%u"
#define LIBUSB_DEV_PATTERN "%u"
#endif


int libusb_open(libusb_device *ptDevice, libusb_device_handle **pptDevHandle)
{
	libusb_device_handle *ptDevHandle;
	int iError;

	ptDevHandle = pfn_usb_open(ptDevice);

	if( ptDevHandle!=NULL )
	{
		*pptDevHandle = ptDevHandle;
		iError = 0;
	}
	else
	{
		*pptDevHandle = NULL;
		iError = -1;
	}

	return iError;
}


void libusb_close(libusb_device_handle *dev_handle)
{
	pfn_usb_close(dev_handle);
}


int libusb_init(libusb_context **pptContext)
{
	pfn_usb_init();

	/* use something different from NULL */
	*pptContext = (libusb_context*)1;

	return LIBUSB_SUCCESS;
}




void libusb_exit(libusb_context *ptContext)
{
}


void libusb_set_debug(libusb_context *ptContext, int iLevel)
{
}


int usb_bulk_pc_to_netx(libusb_device_handle *ptDevHandle, unsigned char ucEndPointOut, const unsigned char *pucDataOut, int iLength, int *piProcessed, unsigned int uiTimeoutMs)
{
	int iError;
#ifdef WIN32
	char *pcDataOut = (char*)pucDataOut;
#else
	const char *pcDataOut = (const char*)pucDataOut;
#endif

	iError = pfn_usb_bulk_write(ptDevHandle, ucEndPointOut, pcDataOut, iLength, uiTimeoutMs);

	if( iError==iLength )
	{
		/* transfer ok! */
		if( piProcessed!=NULL )
		{
			*piProcessed = iLength;
		}
		iError = 0;
	}
	else
	{
		/* do not return 0 in case of an error */
		if( iError==0 )
		{
			iError = -1;
		} 
		else if (iError == -116)
		{
			iError = LIBUSB_ERROR_TIMEOUT;
		} 
		/* transfer failed */
		if( piProcessed!=NULL )
		{
			*piProcessed = 0;
		}
	}

	return iError;
}


int usb_bulk_netx_to_pc(libusb_device_handle *ptDevHandle, unsigned char ucEndPointIn, unsigned char *pucDataIn, int iLength, int *piProcessed, unsigned int uiTimeoutMs)
{
	int iError;

	iError = pfn_usb_bulk_read(ptDevHandle, ucEndPointIn, (char*)pucDataIn, iLength, uiTimeoutMs);

	if( iError==iLength )
	{
		/* transfer ok! */
		if( piProcessed!=NULL )
		{
			*piProcessed = iLength;
		}
		iError = 0;
	}
	else
	{
		/* do not return 0 in case of an error */
		if( iError==0 )
		{
			iError = -1;
		} 
		else if (iError == -116)
		{
			iError = LIBUSB_ERROR_TIMEOUT;
		} 
		/* transfer failed */
		if( piProcessed!=NULL )
		{
			*piProcessed = 0;
		}
	}

	return iError;
}


int libusb_get_device_descriptor(libusb_device *dev, struct libusb_device_descriptor *desc)
{
	*desc = dev->descriptor;
	return LIBUSB_SUCCESS;
}


unsigned char libusb_get_bus_number(libusb_device *dev)
{
	unsigned char ucBusNr;
	usb_bus *ptBus;
	const char *pcBusName;
	int iResult;
	unsigned int uiBusNumber;


	/* expect failure */
	ucBusNr = 0xffU;

	/* is the device valid? */
	if( dev!=NULL )
	{
		/* does the device have a bus assigned? */
		ptBus = dev->bus;
		if( ptBus!=NULL )
		{
			/* does the bus have a directory name? */
			pcBusName = ptBus->dirname;
			if( pcBusName!=NULL )
			{
				/* parse the directory name */
				iResult = sscanf(pcBusName, LIBUSB_BUS_PATTERN, &uiBusNumber);
				/* does the directory name have the expected format? */
				if( iResult==1 )
				{
					/* is the bus number in the valid range? */
					if( uiBusNumber<0x80U )
					{
						/* set the result */
						ucBusNr = (unsigned char)uiBusNumber;
					}
				}
			}
		}
	}

	return ucBusNr;
}


unsigned char libusb_get_device_address(libusb_device *dev)
{
	unsigned char ucDeviceAddress;
	const char *pcFilename;
	int iResult;
	unsigned int uiDeviceNumber;


	/* expect failure */
	ucDeviceAddress = 0xffU;

	/* is the device valid? */
	if( dev!=NULL )
	{
		/* does the bus have a directory name? */
		pcFilename = dev->filename;
		if( pcFilename!=NULL )
		{
			/* parse the directory name */
			iResult = sscanf(pcFilename, LIBUSB_DEV_PATTERN, &uiDeviceNumber);
			/* does the directory name have the expected format? */
			if( iResult==1 )
			{
				/* is the bus number in the valid range? */
				if( uiDeviceNumber<0x80U )
				{
					/* set the result */
					ucDeviceAddress = (unsigned char)uiDeviceNumber;
				}
			}
		}
	}

	return ucDeviceAddress;
}


int libusb_reset_device(libusb_device_handle *dev)
{
	return pfn_usb_reset(dev);
}


int libusb_set_configuration(libusb_device_handle *dev, int configuration)
{
	return pfn_usb_set_configuration(dev, configuration);
}


int libusb_claim_interface(libusb_device_handle *dev, int iface)
{
	return pfn_usb_claim_interface(dev, iface);
}


int libusb_release_interface(libusb_device_handle *dev, int iface)
{
	return pfn_usb_release_interface(dev, iface);
}


ssize_t libusb_get_device_list(libusb_context *ctx, libusb_device ***list)
{
	libusb_device **ptDeviceList;
	libusb_device **ptCnt;
	ssize_t ssizDeviceList;
	struct usb_bus *ptBusses;
	struct usb_bus *ptBusCnt;
	struct usb_device *ptDevCnt;


	/* detect all busses */
	pfn_usb_find_busses();
	/* detect all devices on each bus */
	pfn_usb_find_devices();

	/* init the device list */
	ptDeviceList = NULL;

	/* init the device counter */
	ssizDeviceList = 0;

	/* get the head of the bus list */
	ptBusses = pfn_usb_get_busses();

	/* loop over all bus entries and count the devices */
	ptBusCnt = ptBusses;
	while( ptBusCnt!=NULL )
	{
		/* loop over all devices on this bus */
		ptDevCnt = ptBusCnt->devices;
		while( ptDevCnt!=NULL )
		{
			/* found one more device */
			++ssizDeviceList;

			/* next device */
			ptDevCnt = ptDevCnt->next;
		}
		/* scan next bus */
		ptBusCnt = ptBusCnt->next;
	}

	/* found any devices? */
	if( ssizDeviceList>0 )
	{
		/* allocate the array */
		ptDeviceList = (libusb_device **)malloc(ssizDeviceList*sizeof(libusb_device*));
		if( ptDeviceList==NULL )
		{
			ssizDeviceList = LIBUSB_ERROR_NO_MEM;
		}
		else
		{
			ptCnt = ptDeviceList;

			/* loop over all bus entries and count the devices */
			ptBusCnt = ptBusses;
			while( ptBusCnt!=NULL )
			{
				/* loop over all devices on this bus */
				ptDevCnt = ptBusCnt->devices;
				while( ptDevCnt!=NULL )
				{
					/* add the device to the list */
					*(ptCnt++) = ptDevCnt;

					/* next device */
					ptDevCnt = ptDevCnt->next;
				}
				/* scan next bus */
				ptBusCnt = ptBusCnt->next;
			}

			*list = ptDeviceList;
		}
	}

	return ssizDeviceList;
}


void libusb_free_device_list(libusb_device **list, int unref_devices)
{
	/* free the complete list */
	free(list);
}


#endif

/*-------------------------------------*/

int fn_init(wxLog *ptLogTarget, wxXmlNode *ptCfgNode, wxString &strPluginId)
{
	wxLog *pOldLogTarget;


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
	wxLogMessage(wxT("bootloader usb plugin init"));

	/* remember id */
	plugin_desc.strPluginId = strPluginId;

	/* init the lua state */
	m_ptLuaState = NULL;

	/* ignore the result, fn_init will succeed even if libusb_load fails */
	libusb_load();

	return 0;
}


/*-------------------------------------*/

int fn_init_lua(wxLuaState *ptLuaState)
{
	bool fSuccess;


	/* remember the lua state for instance creation */
	m_ptLuaState = ptLuaState;

	/* init the lua bindings */
	fSuccess = wxLuaBinding_romloader_usb_lua_init();
	if( fSuccess!=true )
	{
		wxLogError(wxT("failed to init romloader_baka lua bindings"));
		return -1;
	}

	return 0;
}


/*-------------------------------------*/

int fn_leave(void)
{
	wxLogMessage(wxT("bootloader usb plugin leave"));
	libusb_unload();
	return 0;
}

/*-------------------------------------*/

const muhkuh_plugin_desc *fn_get_desc(void)
{
	return &plugin_desc;
}

/*-------------------------------------*/

int fn_detect_interfaces(std::vector<muhkuh_plugin_instance*> *pvInterfaceList)
{
	int iResult;
	libusb_context *ptLibUsbContext;
	ssize_t ssizDevList;
	libusb_device **ptDeviceList;
	libusb_device **ptDevCnt, **ptDevEnd;
	libusb_device *ptDev;
	libusb_device_handle *ptDevHandle;
	unsigned char ucBusNr;
	unsigned char ucDevAdr;
	bool fDeviceIsBusy;
	wxString strMe;
	std::vector<struct usb_device *> tDeviceList;
	int iInterfaces;
	bool fDeviceIsNetx;
	muhkuh_plugin_instance *ptInst;
	wxString strName;
	wxString strTyp;
	wxString strLuaCreateFn;
	wxString strErrorMsg;
	unsigned long ulDeviceId;
	void *pvDeviceId;


	/* create id for errormessages */
	strMe = plugin_desc.strPluginId + wxT(" : ");

	strTyp = plugin_desc.strPluginId;
	strLuaCreateFn = wxT("muhkuh.romloader_usb_create");

	iInterfaces = 0;

	if (!libusb_isloaded())
	{
		return iInterfaces;
	}

	/* create a new libusb context */
	iResult = libusb_init(&ptLibUsbContext);
	if( iResult!=LIBUSB_SUCCESS )
	{
		/* failed to create the context */
		wxLogError(wxT("Failed to create libusb context: ") + romloader_usb::libusb_strerror(iResult));
	}
	else
	{
		/* detect devices */
		ptDeviceList = NULL;
		ssizDevList = libusb_get_device_list(ptLibUsbContext, &ptDeviceList);
		if( ssizDevList<0 )
		{
			/* failed to detect devices */
			strErrorMsg = romloader_usb::libusb_strerror(ssizDevList);
			wxLogError(strMe + wxT("failed to detect usb devices: %s"), strErrorMsg.c_str());
		}
		else
		{
			/* loop over all devices */
			ptDevCnt = ptDeviceList;
			ptDevEnd = ptDevCnt + ssizDevList;
			while( ptDevCnt<ptDevEnd )
			{
				ptDev = *ptDevCnt;
				fDeviceIsNetx = romloader_usb::isDeviceNetx(ptDev);
				if( fDeviceIsNetx==true )
				{
					/* construct the name */
					ucBusNr = libusb_get_bus_number(ptDev);
					ucDevAdr = libusb_get_device_address(ptDev);
					ulDeviceId = ucDevAdr | (ucBusNr<<8);
					pvDeviceId = (void*)ulDeviceId;
					strName.Printf(wxT("romloader_usb_%08x"), ulDeviceId);
					iResult = libusb_open(ptDev, &ptDevHandle);
					if( iResult!=LIBUSB_SUCCESS )
					{
						/* failed to open the interface, do not add it to the list */
						wxLogError(strMe + wxT("failed to open device %s, skipping!"), strName.c_str());
					}
					else
					{
						/* set the configuration */
						iResult = libusb_set_configuration(ptDevHandle, 1);
						if( iResult!=LIBUSB_SUCCESS )
						{
							/* failed to set the configuration */
							wxLogError(wxT("failed to set the configuration of device %s, skipping!"), strName.c_str());
						}
						else
						{
							/* claim the interface, 0 is the interface number */
							iResult = libusb_claim_interface(ptDevHandle, 0);
							if( iResult!=LIBUSB_SUCCESS && iResult!=LIBUSB_ERROR_BUSY )
							{
								/* failed to claim the interface */
								wxLogError(wxT("failed to claim the interface of device %s, skipping!"), strName.c_str());
							}
							else
							{
								if( iResult!=LIBUSB_SUCCESS )
								{
									/* the interface is busy! */
									fDeviceIsBusy = true;
								}
								else
								{
									/* ok, claimed the interface -> the device is not busy */
									fDeviceIsBusy = false;
									/* release the interface */
									/* NOTE: The 'busy' information only represents the device state at detection time.
									 * This function _must_not_ keep the claim on the device or other applications will
									 * not be able to use it.
									 */
									iResult = libusb_release_interface(ptDevHandle, 0);
									if( iResult!=LIBUSB_SUCCESS )
									{
										/* failed to release the interface */
										wxLogError(wxT("failed to release the interface of device %s after a successful claim, skipping!"), strName.c_str());
									}
								}

								/* create the new instance */
								ptInst = new muhkuh_plugin_instance(strName, strTyp, fDeviceIsBusy, strLuaCreateFn, pvDeviceId);
								/* add the new instance to the list */
								pvInterfaceList->push_back(ptInst);
								/* count the created instances */
								++iInterfaces;
							}
						}

						/* close the device */
						libusb_close(ptDevHandle);
					}
				}
				/* next list item */
				++ptDevCnt;
			}

			/* free the device list */
			if( ptDeviceList!=NULL )
			{
				libusb_free_device_list(ptDeviceList, 1);
			}
		}

		/* free the libusb context */
		libusb_exit(ptLibUsbContext);
	}

	return iInterfaces;
}


/*-------------------------------------*/


void romloader_usb_close_instance(wxString &strInterface)
{
}


/*-------------------------------------*/

romloader *romloader_usb_create(void *pvHandle)
{
	unsigned long ulDeviceId;
	romloader *ptInstance = NULL;
	wxString strName;
	wxString strTyp;


	ulDeviceId = (unsigned long)pvHandle;

	// create the new instance
	strTyp = plugin_desc.strPluginId;

	strName.Printf(wxT("romloader_usb_%08x"), ulDeviceId);

	ptInstance = new romloader_usb(strName, strTyp, NULL, ulDeviceId, romloader_usb_close_instance, m_ptLuaState);

	return ptInstance;
}


/*-------------------------------------*/


romloader_usb::romloader_usb(wxString strName, wxString strTyp, const romloader_functioninterface *ptFn, unsigned long ulDeviceId, romloader_usb_plugin_fn_close_instance fn_close, wxLuaState *ptLuaState)
 : romloader(strName, strTyp, NULL, NULL, NULL, ptLuaState)
 , m_fIsConnected(false)
 , m_ptLibUsbContext(NULL)
 , m_ptUsbDevHandle(NULL)
 , m_fn_usb_close(fn_close)
{
	int iResult;


	m_strMe.Printf(wxT("romloader_usb(%p): "), this);

	wxLogDebug(m_strMe + wxT("constructor"));

	m_strInterface = strName;
	m_strTyp = strTyp;

	m_ucNetxDeviceAddress = ulDeviceId & 0xff;
	m_ucNetxBusNr = (ulDeviceId>>8) & 0xff;

	/* create a new libusb context */
	iResult = libusb_init(&m_ptLibUsbContext);
	if( iResult!=LIBUSB_SUCCESS )
	{
		wxLogError(m_strMe + _("failed to init libusb: %s"), libusb_strerror(iResult).c_str());
		m_ptLibUsbContext = NULL;
	}
}


romloader_usb::~romloader_usb(void)
{
	wxLogDebug(m_strMe + wxT("destructor"));

	disconnect();

	if( m_ptLibUsbContext!=NULL )
	{
		libusb_exit(m_ptLibUsbContext);
		m_ptLibUsbContext = NULL;
	}

	if( m_fn_usb_close!=NULL )
	{
		m_fn_usb_close(m_strInterface);
	}
}


bool romloader_usb::chip_init(void)
{
	bool fResult;


	switch( m_tChiptyp )
	{
	case ROMLOADER_CHIPTYP_NETX500:
	case ROMLOADER_CHIPTYP_NETX100:
		switch( m_tRomcode )
		{
		case ROMLOADER_ROMCODE_ABOOT:
			// aboot does not set the serial vectors
			write_data32(0x10001ff0, 0x00200582|1);		// get: usb_receiveChar
			write_data32(0x10001ff4, 0x0020054e|1);		// put: usb_sendChar
			write_data32(0x10001ff8, 0);			// peek: none
			write_data32(0x10001ffc, 0x00200566|1);		// flush: usb_sendFinish
			fResult = true;
			break;
		case ROMLOADER_ROMCODE_HBOOT:
			// hboot needs no special init
			fResult = true;
			break;
		case ROMLOADER_ROMCODE_UNKNOWN:
			fResult = false;
			break;
		}
		break;

	case ROMLOADER_CHIPTYP_NETX50:
		switch( m_tRomcode )
		{
		case ROMLOADER_ROMCODE_ABOOT:
			// this is an unknown combination
			fResult = false;
			break;
		case ROMLOADER_ROMCODE_HBOOT:
			// hboot needs no special init
			fResult = true;
			break;
		case ROMLOADER_ROMCODE_UNKNOWN:
			fResult = false;
			break;
		}
		break;

	case ROMLOADER_CHIPTYP_UNKNOWN:
		fResult = false;
		break;
	}

	return fResult;
}


/* open the connection to the device */
void romloader_usb::connect(void)
{
	int iResult;
	wxString strResponse;
	wxString strErrorMsg;


	if( m_fIsConnected!=false )
	{
		wxLogDebug(m_strMe + _("already connected, ignoring new connect request"));
	}
	else
	{
		m_ptUsbDevHandle = NULL;
		iResult = libusb_open_by_bus_and_adr(strErrorMsg);
		if( iResult==LIBUSB_SUCCESS )
		{
			if( m_ptUsbDevHandle==NULL )
			{
				strErrorMsg.Printf(_("device not found. is it still plugged in and powered?"));
			}
			else
			{
				// get netx welcome message
				iResult = usb_getNetxData(strResponse, NULL, 0, NULL);
				if( iResult!=LIBUSB_SUCCESS )
				{
					wxLogMessage(m_strMe + _("failed to receive netx response, trying to reset netx: %s"),  libusb_strerror(iResult).c_str() );

					// try to reset the device and try again
					iResult = libusb_resetDevice();
					if( iResult!=LIBUSB_SUCCESS )
					{
						strErrorMsg.Printf(_("failed to reset the netx, giving up: %s"), libusb_strerror(iResult).c_str() );
					}
					else
					{
						wxLogMessage(m_strMe + wxT("reset ok!"));

						m_ptUsbDevHandle = NULL;
						iResult = libusb_open_by_bus_and_adr(strErrorMsg);
						if( iResult==LIBUSB_SUCCESS )
						{
							if( m_ptUsbDevHandle==NULL )
							{
								strErrorMsg = _("lost device after reset!");
								iResult = LIBUSB_ERROR_OTHER;
							}
							else
							{
								// get netx welcome message
								iResult = usb_getNetxData(strResponse, NULL, 0, NULL);
								if( iResult!=LIBUSB_SUCCESS )
								{
									strErrorMsg.Printf(_("failed to receive netx response: %s"), libusb_strerror(iResult).c_str());
									libusb_closeDevice();
								}
							}
						}
					}
				}

				if( iResult==LIBUSB_SUCCESS )
				{
					m_fIsConnected = true;

					wxLogInfo(m_strMe + wxT("netx response: ") + strResponse);
					if( detect_chiptyp()!=true )
					{
						strErrorMsg = _("failed to detect chiptyp!");
						m_fIsConnected = false;
					}
					else if( chip_init()!=true )
					{
						strErrorMsg = _("failed to init chip!");
						m_fIsConnected = false;
					}
				}

				if( m_fIsConnected!=true )
				{
					libusb_closeDevice();
				}
			}
		}

		if( m_fIsConnected!=true )
		{
			wxLogError(m_strMe + strErrorMsg);
			m_ptLuaState->wxlua_Error(strErrorMsg);
		}
	}
}


/* close the connection to the device */
void romloader_usb::disconnect(void)
{
	wxString strMsg;


	wxLogDebug(m_strMe + wxT("disconnect"));

	if( m_ptUsbDevHandle!=NULL )
	{
		libusb_close(m_ptUsbDevHandle);
	}

	m_fIsConnected = false;
	m_ptUsbDevHandle = NULL;
}


/* returns the connection state of the device */
bool romloader_usb::is_connected(void) const
{
	return m_fIsConnected;
}


/* read a byte (8bit) from the netx to the pc */
double romloader_usb::read_data08(double dNetxAddress)
{
	unsigned long ulNetxAddress;
	unsigned long ulValue;
	int iResult;
	wxString strErrorMsg;
	wxString strCommand;
	wxString strResponse;
	wxString strHexValue;
	wxRegEx reDumpResponse(wxT("([0-9A-Fa-f]{8}): ([0-9A-Fa-f]{2}) "));
	bool fOk;


	ulNetxAddress = (unsigned long)dNetxAddress;
	ulValue = 0;

	// expect failure
	fOk = false;

	// construct the command
	strCommand.Printf(wxT("DUMP %08lX BYTE"), ulNetxAddress);

	if( m_fIsConnected==false )
	{
		strErrorMsg = _("not connected!");
	}
	else
	{
		// send the command
		iResult = usb_executeCommand(strCommand, strResponse);
		if( iResult!=LIBUSB_SUCCESS )
		{
			strErrorMsg.Printf(_("failed to send command: %s"), libusb_strerror(iResult).c_str());
		}
		else if( reDumpResponse.Matches(strResponse)!=true )
		{
			strErrorMsg.Printf(_("strange response from device:") + strResponse);
		}
		else
		{
			strHexValue = reDumpResponse.GetMatch(strResponse, 1);
			if( strHexValue.ToULong(&ulValue, 16)!=true )
			{
				strErrorMsg.Printf(_("failed to extract address from response:") + strResponse);
			}
			else if( ulValue!=ulNetxAddress )
			{
				strErrorMsg.Printf(_("address does not match request:") + strResponse);
			}
			else
			{
				strHexValue = reDumpResponse.GetMatch(strResponse, 2);
				if( strHexValue.ToULong(&ulValue, 16)!=true )
				{
					strErrorMsg.Printf(_("failed to extract value from response:") + strResponse);
				}
				else
				{
					wxLogDebug(m_strMe + wxT("read_data08: 0x%08lx = 0x%08lx"), ulNetxAddress, ulValue);
					fOk = true;
				}
			}
		}
	}

	if( fOk!=true )
	{
		wxLogError(m_strMe + strErrorMsg);
		m_ptLuaState->wxlua_Error(strErrorMsg);
	}

	return (double)ulValue;
}


/* read a word (16bit) from the netx to the pc */
double romloader_usb::read_data16(double dNetxAddress)
{
	unsigned long ulNetxAddress;
	unsigned long ulValue;
	int iResult;
	wxString strErrorMsg;
	wxString strCommand;
	wxString strResponse;
	wxString strHexValue;
	wxRegEx reDumpResponse(wxT("([0-9A-Fa-f]{8}): ([0-9A-Fa-f]{4}) "));
	bool fOk;


	ulNetxAddress = (unsigned long)dNetxAddress;
	ulValue = 0;

	// expect failure
	fOk = false;

	// construct the command
	strCommand.Printf(wxT("DUMP %08lX WORD"), ulNetxAddress);

	if( m_fIsConnected==false )
	{
		strErrorMsg = _("not connected!");
	}
	else
	{
		// send the command
		iResult = usb_executeCommand(strCommand, strResponse);
		if( iResult!=LIBUSB_SUCCESS )
		{
			strErrorMsg.Printf(_("failed to send command: %s"), libusb_strerror(iResult).c_str());
		}
		else if( reDumpResponse.Matches(strResponse)!=true )
		{
			strErrorMsg.Printf(_("strange response from device:") + strResponse);
		}
		else
		{
			strHexValue = reDumpResponse.GetMatch(strResponse, 1);
			if( strHexValue.ToULong(&ulValue, 16)!=true )
			{
				strErrorMsg.Printf(_("failed to extract address from response:") + strResponse);
			}
			else if( ulValue!=ulNetxAddress )
			{
				strErrorMsg.Printf(_("address does not match request:") + strResponse);
			}
			else
			{
				strHexValue = reDumpResponse.GetMatch(strResponse, 2);
				if( strHexValue.ToULong(&ulValue, 16)!=true )
				{
					strErrorMsg.Printf(_("failed to extract value from response:") + strResponse);
				}
				else
				{
					wxLogDebug(m_strMe + wxT("read_data16: 0x%08lx = 0x%08lx"), ulNetxAddress, ulValue);
					fOk = true;
				}
			}
		}
	}

	if( fOk!=true )
	{
		wxLogError(m_strMe + strErrorMsg);
		m_ptLuaState->wxlua_Error(strErrorMsg);
	}

	return (double)ulValue;
}


/* read a long (32bit) from the netx to the pc */
double romloader_usb::read_data32(double dNetxAddress)
{
	unsigned long ulNetxAddress;
	unsigned long ulValue;
	int iResult;
	wxString strErrorMsg;
	wxString strCommand;
	wxString strResponse;
	wxString strHexValue;
	wxRegEx reDumpResponse(wxT("([0-9A-Fa-f]{8}): ([0-9A-Fa-f]{8}) "));
	bool fOk;


	ulNetxAddress = (unsigned long)dNetxAddress;
	ulValue = 0;

	// expect failure
	fOk = false;

	// construct the command
	strCommand.Printf(wxT("DUMP %08lX"), ulNetxAddress);

	if( m_fIsConnected==false )
	{
		strErrorMsg = _("not connected!");
	}
	else
	{
		// send the command
		iResult = usb_executeCommand(strCommand, strResponse);
		if( iResult!=LIBUSB_SUCCESS )
		{
			strErrorMsg.Printf(wxT("failed to send command: %s"), libusb_strerror(iResult).c_str());
		}
		else if( reDumpResponse.Matches(strResponse)!=true )
		{
			strErrorMsg.Printf(wxT("strange response from device:") + strResponse);
		}
		else
		{
			strHexValue = reDumpResponse.GetMatch(strResponse, 1);
			if( strHexValue.ToULong(&ulValue, 16)!=true )
			{
				strErrorMsg.Printf(wxT("failed to extract address from response:") + strResponse);
			}
			else if( ulValue!=ulNetxAddress )
			{
				strErrorMsg.Printf(wxT("address does not match request:") + strResponse);
			}
			else
			{
				strHexValue = reDumpResponse.GetMatch(strResponse, 2);
				if( strHexValue.ToULong(&ulValue, 16)!=true )
				{
					strErrorMsg.Printf(wxT("failed to extract value from response:") + strResponse);
				}
				else
				{
					wxLogDebug(m_strMe + wxT("read_data32: 0x%08lx = 0x%08lx"), ulNetxAddress, ulValue);
					fOk = true;
				}
			}
		}
	}

	if( fOk!=true )
	{
		wxLogError(m_strMe + strErrorMsg);
		m_ptLuaState->wxlua_Error(strErrorMsg);
	}

	return (double)ulValue;
}


bool romloader_usb::parseDumpLine(const char *pcLine, size_t sizLineLen, unsigned long ulAddress, unsigned long ulElements, unsigned char *pucBuffer, wxString &strErrorMsg)
{
	bool fResult;
	int iMatches;
	unsigned long ulResultAddress;
	unsigned long ulChunkCnt;
	unsigned int uiByte;


	// expect success
	fResult = true;

	// is enough input data left?
	if( sizLineLen<(10+ulElements*3) )
	{
		strErrorMsg = wxT("strange response from netx! (line too short)");
		fResult = false;
	}
	else
	{
		// get the address
		iMatches = sscanf(pcLine, "%8lX: ", &ulResultAddress);
		if( iMatches!=1 )
		{
			strErrorMsg = wxT("strange response from netx! (failed to parse address)");
			fResult = false;
		}
		else if( ulResultAddress!=ulAddress )
		{
			strErrorMsg = wxT("address in response does not match!");
			fResult = false;
		}
		else
		{
			// advance parse ptr to data part of the line
			pcLine += 10;
			// get all bytes
			ulChunkCnt = ulElements;
			while( ulChunkCnt!=0 )
			{
				// get one hex digit
				iMatches = sscanf(pcLine, "%2X ", &uiByte);
				if( iMatches!=1 )
				{
					strErrorMsg = wxT("strange response from netx! (could not parse byte)");
					fResult = false;
					break;
				}
				// advance parse ptr to data part of the line
				pcLine += 3;
				*(pucBuffer++) = (char)uiByte;
				// one number processed
				--ulChunkCnt;
			}
		}
	}

	return fResult;
}


int romloader_usb::getLine(wxString &strData)
{
	int iEolCnt;
	size_t sizStartPos;
	char c;
	int iResult;


	iResult = LIBUSB_SUCCESS;

	strData.Empty();

	do
	{
		// save start position
		sizStartPos = sizBufPos;

		// no eol found yet
		iEolCnt = 0;

		// look for eol in current buffer
		while( sizBufPos<sizBufLen )
		{
			c = acBuf[sizBufPos];
			if( c==0x0a || c==0x0d)
			{
				++iEolCnt;
			}
			else if( iEolCnt>0 )
			{
				break;
			}
			++sizBufPos;
		}

		// get end of string
		if( iEolCnt>0 )
		{
			strData.Append(wxString::From8BitData(acBuf+sizStartPos, sizBufPos-sizStartPos-iEolCnt));
		}

		// get beginning of string
		if( iEolCnt==0 && sizStartPos<sizBufPos )
		{
			strData.Append(wxString::From8BitData(acBuf+sizStartPos, sizBufPos-sizStartPos));
		}

		// get more data
		if( iEolCnt==0 || strData.IsEmpty()==true )
		{
			acBuf[0] = 0x00;
			iResult = libusb_exchange((unsigned char*)acBuf, (unsigned char*)acBuf);
			if( iResult!=LIBUSB_SUCCESS )
			{
				strData.Printf(_("failed to receive command response: %s"), libusb_strerror(iResult).c_str());
				break;
			}
			else
			{
				sizBufLen = acBuf[0];
				if( sizBufLen==0 )
				{
					fEof = true;
					iEolCnt = 1;
					break;
				}
				else
				{
					sizBufPos = 1;
				}
			}
		}
	} while( iEolCnt==0 );

	return iResult;
}

/* read a byte array from the netx to the pc */
wxString romloader_usb::read_image(double dNetxAddress, double dSize, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData)
{
	unsigned long ulNetxAddress;
	unsigned long ulSize;
	int iResult;
	wxString strCommand;
	bool fOk;
	wxString strErrorMsg;
	wxString strResponse;
	wxString strData;
	unsigned char *pucData;
	unsigned char *pucDataCnt;
	unsigned long ulBytesLeft;
	unsigned long ulExpectedAddress;
	unsigned long ulChunkSize;


	ulNetxAddress = (unsigned long)dNetxAddress;
	ulSize = (unsigned long)dSize;

	// expect error
	fOk = false;

	// construct the command
	strCommand.Printf(wxT("DUMP %08lX %08lX BYTE"), ulNetxAddress, ulSize);

	if( m_fIsConnected==false )
	{
		strErrorMsg = _("not connected!");
	}
	else
	{
		// send the command
		iResult = usb_sendCommand(strCommand);
		if( iResult!=LIBUSB_SUCCESS )
		{
			strErrorMsg.Printf(_("failed to send command: %s"), libusb_strerror(iResult).c_str());
		}
		else
		{
			// init the buffer
			sizBufLen = 0;
			sizBufPos = 0;
			fEof = false;

			// alloc buffer
			pucData = (unsigned char*)malloc(ulSize);
			if( pucData==NULL )
			{
				strErrorMsg.Printf(wxT("failed to alloc %d bytes of input buffer!"), ulSize);
			}
			else
			{
				pucDataCnt = pucData;
				// parse the result
				ulBytesLeft = ulSize;
				ulExpectedAddress = ulNetxAddress;
				while( ulBytesLeft>0 )
				{
					fOk = callback_long(L, iLuaCallbackTag, ulSize-ulBytesLeft, pvCallbackUserData);
					if( fOk!=true )
					{
						strErrorMsg = wxT("operation canceled!");
						break;
					}

					// get the response line
					iResult = getLine(strResponse);
					if( iResult!=LIBUSB_SUCCESS )
					{
						fOk = false;
						strErrorMsg = wxT("failed to get dump response from device");
						break;
					}
					else if( fEof==true )
					{
						fOk = false;
						strErrorMsg = wxT("not enough data received from device");
						break;
					}
					else
					{
						// get the number of expected bytes in the next row
						ulChunkSize = 16;
						if( ulChunkSize>ulBytesLeft )
						{
							ulChunkSize = ulBytesLeft;
						}
						fOk = parseDumpLine(strResponse.To8BitData(), strResponse.Len(), ulExpectedAddress, ulChunkSize, pucDataCnt, strErrorMsg);
						if( fOk!=true )
						{
							break;
						}
						else
						{
							ulBytesLeft -= ulChunkSize;
							// inc address
							ulExpectedAddress += ulChunkSize;
							// inc buffer ptr
							pucDataCnt += ulChunkSize;
						}
					}
				}

				if( fOk==true )
				{
					// get data
					strData = wxString::From8BitData((const char*)pucData, ulSize);

					// wait for prompt
					iResult = usb_getNetxData(strResponse, NULL, 0, NULL);
					if( iResult!=LIBUSB_SUCCESS )
					{
						strData.Printf(_("failed to receive command response: %s"), libusb_strerror(iResult).c_str());
						fOk = false;
					}
				}

				if( fOk!=true )
				{
					strErrorMsg = strData;
					strData.Empty();
				}

				free(pucData);
			}
		}
	}

	if( fOk!=true )
	{
		wxLogError(m_strMe + strErrorMsg);
		m_ptLuaState->wxlua_Error(strErrorMsg);
	}

	return strData;
}


/* write a byte (8bit) from the pc to the netx */
void romloader_usb::write_data08(double dNetxAddress, double dData)
{
	unsigned long ulNetxAddress;
	unsigned char ucData;
	int iResult;
	wxString strErrorMsg;
	wxString strCommand;
	wxString strResponse;
	bool fOk;


	ulNetxAddress = (unsigned long)dNetxAddress;
	ucData = (unsigned char)dData;

	// assume failure
	fOk = false;

	// construct the command
	strCommand.Printf(wxT("FILL %08lX %02X BYTE"), ulNetxAddress, ucData);

	if( m_fIsConnected==false )
	{
		strErrorMsg = _("not connected!");
	}
	else
	{
		// send the command
		iResult = usb_executeCommand(strCommand, strResponse);
		if( iResult!=LIBUSB_SUCCESS )
		{
			strErrorMsg.Printf(_("failed to send command: %s"), libusb_strerror(iResult).c_str());
		}
		else if( strResponse.Cmp(wxT("\n>"))==0 )
		{
			wxLogDebug(m_strMe + wxT("write_data08: 0x%08lx = 0x%02x"), ulNetxAddress, ucData);
			fOk = true;
		}
		else
		{
			strErrorMsg = wxT("strange response from netx: ") + strResponse;
		}
	}

	if( fOk!=true )
	{
		wxLogError(m_strMe + strErrorMsg);
		m_ptLuaState->wxlua_Error(strErrorMsg);
	}
}


/* write a word (16bit) from the pc to the netx */
void romloader_usb::write_data16(double dNetxAddress, double dData)
{
	unsigned long ulNetxAddress;
	unsigned short usData;
	int iResult;
	wxString strErrorMsg;
	wxString strCommand;
	wxString strResponse;
	bool fOk;


	ulNetxAddress = (unsigned long)dNetxAddress;
	usData = (unsigned short)dData;

	// assume failure
	fOk = false;

	// construct the command
	strCommand.Printf(wxT("FILL %08lX %04X WORD"), ulNetxAddress, usData);

	if( m_fIsConnected==false )
	{
		strErrorMsg = _("not connected!");
	}
	else
	{
		// send the command
		iResult = usb_executeCommand(strCommand, strResponse);
		if( iResult!=LIBUSB_SUCCESS )
		{
			strErrorMsg.Printf(_("failed to send command: %s"), libusb_strerror(iResult).c_str());
		}
		else if( strResponse.Cmp(wxT("\n>"))==0 )
		{
			wxLogDebug(m_strMe + wxT("write_data16: 0x%08lx = 0x%04x"), ulNetxAddress, usData);
			fOk = true;
		}
		else
		{
			strErrorMsg = wxT("strange response from netx: ") + strResponse;
		}
	}

	if( fOk!=true )
	{
		wxLogError(m_strMe + strErrorMsg);
		m_ptLuaState->wxlua_Error(strErrorMsg);
	}
}


/* write a long (32bit) from the pc to the netx */
void romloader_usb::write_data32(double dNetxAddress, double dData)
{
	unsigned long ulNetxAddress;
	unsigned long ulData;
	int iResult;
	wxString strErrorMsg;
	wxString strCommand;
	wxString strResponse;
	bool fOk;


	ulNetxAddress = (unsigned long)dNetxAddress;
	ulData = (unsigned long)dData;

	// assume failure
	fOk = false;

	// construct the command
	strCommand.Printf(wxT("FILL %08lX %08X"), ulNetxAddress, ulData);

	if( m_fIsConnected==false )
	{
		strErrorMsg = _("not connected!");
	}
	else
	{
		// send the command
		iResult = usb_executeCommand(strCommand, strResponse);
		if( iResult!=LIBUSB_SUCCESS )
		{
			strErrorMsg.Printf(_("failed to send command: %s"), libusb_strerror(iResult).c_str());
		}
		else if( strResponse.Cmp(wxT("\n>"))==0 )
		{
			wxLogDebug(m_strMe + wxT("write_data32: 0x%08lx = 0x%08lx"), ulNetxAddress, ulData);
			fOk = true;
		}
		else
		{
			strErrorMsg = wxT("strange response from netx: ") + strResponse;
		}
	}

	if( fOk!=true )
	{
		wxLogError(m_strMe + strErrorMsg);
		m_ptLuaState->wxlua_Error(strErrorMsg);
	}
}


/* write a byte array from the pc to the netx */
void romloader_usb::write_image(double dNetxAddress, const char *pcData, size_t sizData, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData)
{
	unsigned long ulNetxAddress;
	int iResult;
	bool fOk;
	wxString strErrorMsg;
	wxString strResponse;


	ulNetxAddress = (unsigned long)dNetxAddress;

	// expect error
	fOk = false;

	if( m_fIsConnected==false )
	{
		strErrorMsg = _("not connected!");
	}
	else
	{
		// send the command
		iResult = usb_load(pcData, sizData, ulNetxAddress, L, iLuaCallbackTag, pvCallbackUserData);
		if( iResult!=LIBUSB_SUCCESS )
		{
			strErrorMsg.Printf(_("failed to send load command: %s"), libusb_strerror(iResult).c_str());
		}
		else
		{
			// get the response
			iResult = usb_getNetxData(strResponse, NULL, 0, NULL);
			if( iResult!=LIBUSB_SUCCESS )
			{
				strErrorMsg.Printf(_("failed to get command response: %s"), libusb_strerror(iResult).c_str());
			}
			else
			{
				// check the response
				if( strResponse.Cmp(wxT("\n>"))==0 )
				{
					// ok!
					fOk = true;
				}
				else
				{
					strErrorMsg = wxT("strange response from netx: ") + strResponse;
				}
			}
		}
	}

	if( fOk!=true )
	{
		wxLogError(m_strMe + strErrorMsg);
		m_ptLuaState->wxlua_Error(strErrorMsg);
	}
}


/* call routine */
void romloader_usb::call(double dNetxAddress, double dParameterR0, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData)
{
	unsigned long ulNetxAddress;
	unsigned long ulParameterR0;
	int iResult;
	wxString strErrorMsg;
	wxString strResponse;
	bool fOk;


	ulNetxAddress = (unsigned long)dNetxAddress;
	ulParameterR0 = (unsigned long)dParameterR0;

	fOk = false;

	if( m_fIsConnected==false )
	{
		strErrorMsg = _("not connected!");
	}
	else
	{
		// send the command
		iResult = usb_call(ulNetxAddress, ulParameterR0, L, iLuaCallbackTag, pvCallbackUserData);
		if( iResult!=LIBUSB_SUCCESS )
		{
			strErrorMsg.Printf(m_strMe + wxT("failed to send command: "), libusb_strerror(iResult).c_str());
		}
		else
		{
			fOk = true;
		}
	}

	if( fOk!=true )
	{
		wxLogError(m_strMe + strErrorMsg);
		m_ptLuaState->wxlua_Error(strErrorMsg);
	}
}


/*-------------------------------------*/

int romloader_usb::usb_load(const char *pcData, size_t sizDataLen, unsigned long ulLoadAdr, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData)
{
	const unsigned char *pucDataCnt, *pucDataEnd;
	int iResult;
	unsigned int uiCrc;
	wxString strCommand;
	unsigned char aucBufSend[64];
	unsigned char aucBufRec[64];
	size_t sizChunkSize;
	bool fIsRunning;
	long lBytesProcessed;


	pucDataCnt = (const unsigned char*)pcData;
	pucDataEnd = pucDataCnt + sizDataLen;
	// generate crc checksum
	uiCrc = 0xffff;
	// loop over all bytes
	while( pucDataCnt<pucDataEnd )
	{
		uiCrc = crc16(uiCrc, *(pucDataCnt++));
	}

	// generate load command
	strCommand.Printf(wxT("LOAD %08lX %08X %04X"), ulLoadAdr, sizDataLen, uiCrc);

	// send the command
	iResult = usb_sendCommand(strCommand);
	if( iResult==LIBUSB_SUCCESS )
	{
		// now send the data part
		pucDataCnt = (const unsigned char*)pcData;
		lBytesProcessed = 0;
		while( pucDataCnt<pucDataEnd )
		{
			// get the size of the next data chunk
			sizChunkSize = pucDataEnd - pucDataCnt;
			if( sizChunkSize>63 )
			{
				sizChunkSize = 63;
			}
			// copy data to the packet
			memcpy(aucBufSend+1, pucDataCnt, sizChunkSize);
			aucBufSend[0] = (unsigned char) sizChunkSize+1;

			fIsRunning = callback_long(L, iLuaCallbackTag, lBytesProcessed, pvCallbackUserData);
			if( fIsRunning!=true )
			{
				iResult = LIBUSB_ERROR_INTERRUPTED;
				break;
			}

			iResult = libusb_exchange(aucBufSend, aucBufRec);
			if( iResult!=LIBUSB_SUCCESS )
			{
				break;
			}
			pucDataCnt += sizChunkSize;
			lBytesProcessed += sizChunkSize;
		}

		if( pucDataCnt==pucDataEnd )
		{
			iResult = LIBUSB_SUCCESS;
		}
	}

	return iResult;
}


int romloader_usb::usb_call(unsigned long ulNetxAddress, unsigned long ulParameterR0, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData)
{
	int iResult;
	wxString strCommand;
	unsigned char aucSend[64];
	unsigned char aucRec[64];
	bool fIsRunning;
	wxString strResponse;
	wxString strCallbackData;
	size_t sizChunkRead;
	unsigned char aucSbuf[2] = { 0, 0 };


	// construct the "call" command
	strCommand.Printf(wxT("CALL %08lX %08X"), ulNetxAddress, ulParameterR0);

	// send the command
	iResult = usb_sendCommand(strCommand);
	if( iResult==LIBUSB_SUCCESS )
	{
		// wait for the call to finish
		do
		{
			// send handshake
			aucSend[0] = 0x00;
			iResult = libusb_writeBlock(aucSend, 64, USB_CALL_WRITE_TIMEOUT);
			if( iResult==LIBUSB_SUCCESS )
			{
				do
				{
					// execute callback
					fIsRunning = callback_string(L, iLuaCallbackTag, strCallbackData, pvCallbackUserData);
					strCallbackData.Empty();
					if( fIsRunning!=true )
					{
						iResult = LIBUSB_ERROR_INTERRUPTED;
					}
					else
					{
						// look for data from netx
						iResult = libusb_readBlock(aucRec, 64, USB_CALL_READ_TIMEOUT);
						if( iResult==LIBUSB_SUCCESS )
						{
							iResult = LIBUSB_ERROR_TIMEOUT;

							// received netx data, check for prompt
							sizChunkRead = aucRec[0];
							if( sizChunkRead>1 && sizChunkRead<=64 )
							{
								// get data
								// NOTE: use Append to make a real copy
								strCallbackData.Append( wxString::From8BitData((const char*)(aucRec+1), sizChunkRead-1) );

								// last packet has '\n>' at the end
								if( sizChunkRead>2 && aucRec[sizChunkRead-2]=='\n' && aucRec[sizChunkRead-1]=='>' )
								{
									// send the rest of the data
									callback_string(L, iLuaCallbackTag, strCallbackData, pvCallbackUserData);
									iResult = LIBUSB_SUCCESS;
								}
							}
							break;
						}
					}
				} while( iResult==LIBUSB_ERROR_TIMEOUT );
			}
		} while( iResult==LIBUSB_ERROR_TIMEOUT );
	}

	if( iResult==LIBUSB_SUCCESS )
	{
		aucSend[0] = 0x00;
		iResult = libusb_exchange(aucSend, aucRec);
	}

	return iResult;
}


int romloader_usb::usb_sendCommand(wxString strCommand)
{
	int iResult;
	size_t sizCmdLen;
	unsigned char abSend[64];
	unsigned char abRec[64];


	// check the command size
	// Commands must fit into one usb packet of 64 bytes, the first byte
	// is the length and the last byte must be 0x0a. This means the max
	// command size is 62 bytes.
	sizCmdLen = strCommand.Len();
	if( sizCmdLen>62 )
	{
		wxLogError(m_strMe + wxT("command exceeds maximum length of 62 chars: ") + strCommand);
		iResult = LIBUSB_ERROR_OVERFLOW;
	}
	else
	{
		wxLogDebug(m_strMe + wxT("send command '%s'"), strCommand.c_str());

		// construct the command
		memcpy(abSend+1, strCommand.To8BitData(), sizCmdLen);
		abSend[0] = (unsigned char) sizCmdLen+2;
		abSend[sizCmdLen+1] = 0x0a;

		// send the command
		iResult = libusb_exchange(abSend, abRec);
		if( iResult==LIBUSB_SUCCESS )
		{
			// terminate command
			abSend[0] = 0x00;
			iResult = libusb_exchange(abSend, abRec);
		}
	}

	return iResult;
}


int romloader_usb::usb_getNetxData(wxString &strData, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData)
{
	int iResult;
	unsigned char buf_send[64];
	unsigned char buf_rec[64];
	unsigned int ulLineLen;


	// init results
	strData.Empty();

	// receive netx data
	do
	{
		buf_send[0] = 0x00;
		iResult = libusb_exchange(buf_send, buf_rec);
		if( iResult!=LIBUSB_SUCCESS )
		{
			break;
		}
		else
		{
			ulLineLen = buf_rec[0];

			if( ulLineLen!=0 )
			{
				strData.Append(wxString::From8BitData((const char*)(buf_rec+1), ulLineLen-1));
			}
		}
	} while( ulLineLen!=0 );

	return iResult;
}


int romloader_usb::usb_executeCommand(wxString strCommand, wxString &strResponse)
{
	int iResult;


	/* send the command */
	iResult = usb_sendCommand(strCommand);
	if( iResult==LIBUSB_SUCCESS )
	{
		/* get the response */
		iResult = usb_getNetxData(strResponse, NULL, 0, NULL);
	}

	return iResult;
}


/*-------------------------------------*/


int romloader_usb::libusb_open_by_bus_and_adr(wxString &strErrorMsg)
{
	int iResult;
	ssize_t ssizDevList;
	libusb_device **ptDeviceList;
	libusb_device **ptDevCnt, **ptDevEnd;
	libusb_device *ptDev;
	libusb_device_handle *ptDevHandle;
	bool fDeviceIsNetx;
	unsigned char ucBusNr;
	unsigned char ucDevAdr;


	/* detect devices */
	ssizDevList = libusb_get_device_list(m_ptLibUsbContext, &ptDeviceList);
	if( ssizDevList<0 )
	{
		/* failed to detect devices */
		iResult = (int)ssizDevList;
		strErrorMsg.Printf(_("failed to detect usb devices: %s"), libusb_strerror(iResult).c_str());
	}
	else
	{
		/* loop over all devices */
		ptDevCnt = ptDeviceList;
		ptDevEnd = ptDevCnt + ssizDevList;
		while( ptDevCnt<ptDevEnd )
		{
			ptDev = *ptDevCnt;
			fDeviceIsNetx = isDeviceNetx(ptDev);
			if( fDeviceIsNetx==true )
			{
				/* is this the correct bus number and device address? */
				ucBusNr = libusb_get_bus_number(ptDev);
				ucDevAdr = libusb_get_device_address(ptDev);
				if( ucBusNr==m_ucNetxBusNr && ucDevAdr==m_ucNetxDeviceAddress )
				{
					iResult = libusb_open(ptDev, &ptDevHandle);
					if( iResult!=LIBUSB_SUCCESS )
					{
						/* failed to open the interface */
						strErrorMsg.Printf(_("failed to open device: %s"), libusb_strerror(iResult).c_str());
					}
					else
					{
						/* set the configuration */
						iResult = libusb_set_configuration(ptDevHandle, 1);
						if( iResult!=LIBUSB_SUCCESS )
						{
							/* failed to set the configuration */
							wxLogError(wxT("failed to set the configuration: %s"), libusb_strerror(iResult).c_str());
						}
						else
						{
							/* claim the interface, 0 is the interface number */
							iResult = libusb_claim_interface(ptDevHandle, 0);
							if( iResult!=LIBUSB_SUCCESS )
							{
								/* failed to claim the interface */
								strErrorMsg.Printf(wxT("failed to claim the interface:"), libusb_strerror(iResult).c_str());
								/* close the device */
								libusb_close(ptDevHandle);
							}
							else
							{
								m_ptUsbDevHandle = ptDevHandle;
							}
						}
					}
				}
				break;
			}
			/* next list item */
			++ptDevCnt;
		}
	}

	/* free the device list */
	libusb_free_device_list(ptDeviceList, 1);

	return iResult;
}


int romloader_usb::libusb_closeDevice(void)
{
	int iResult;


	/* release the interface */
	iResult = libusb_release_interface(m_ptUsbDevHandle, 0);
	if( iResult!=LIBUSB_SUCCESS )
	{
		/* failed to release interface */
		wxLogError(m_strMe + _("failed to release the usb interface: %s"), libusb_strerror(iResult).c_str());
	}
	else
	{
		/* close the netx device */
		libusb_close(m_ptUsbDevHandle);
	}

	return iResult;
}


int romloader_usb::libusb_resetDevice(void)
{
	int iResult;


	iResult = libusb_reset_device(m_ptUsbDevHandle);
	if( iResult==LIBUSB_SUCCESS )
	{
		libusb_close(m_ptUsbDevHandle);
	}
	else if( iResult==LIBUSB_ERROR_NOT_FOUND )
	{
		// the old device is already gone -> that's good, ignore the error
		libusb_close(m_ptUsbDevHandle);
		iResult = LIBUSB_SUCCESS;
	}

	return iResult;
}


int romloader_usb::libusb_readBlock(unsigned char *pucReceiveBuffer, unsigned int uiSize, int iTimeoutMs)
{
	int iRet;
	int iSize;
	int iTransfered;


	iSize = (int)uiSize;

	iRet = usb_bulk_netx_to_pc(m_ptUsbDevHandle, 0x81, pucReceiveBuffer, iSize, &iTransfered, iTimeoutMs);
	return iRet;
}


int romloader_usb::libusb_writeBlock(unsigned char *pucSendBuffer, unsigned int uiSize, int iTimeoutMs)
{
	int iRet;
	int iSize;
	int iTransfered;


	iSize = (int)uiSize;

	iRet = usb_bulk_pc_to_netx(m_ptUsbDevHandle, 0x01, pucSendBuffer, iSize, &iTransfered, iTimeoutMs);
	return iRet;
}


int romloader_usb::libusb_exchange(unsigned char *pucSendBuffer, unsigned char *pucReceiveBuffer)
{
	int iResult;


	iResult = libusb_writeBlock(pucSendBuffer, 64, USB_EXCHANGE_WRITE_TIMEOUT);
	if( iResult==LIBUSB_SUCCESS )
	{
		iResult = libusb_readBlock(pucReceiveBuffer, 64, USB_EXCHANGE_READ_TIMEOUT);
	}
	return iResult;
}



const romloader_usb::LIBUSB_STRERROR_T romloader_usb::atStrError[] =
{
	{ LIBUSB_SUCCESS,		_("success") },
	{ LIBUSB_ERROR_IO,		_("input/output error") },
	{ LIBUSB_ERROR_INVALID_PARAM,	_("invalid parameter") },
	{ LIBUSB_ERROR_ACCESS,		_("access denied (insufficient permissions)") },
	{ LIBUSB_ERROR_NO_DEVICE,	_("no such device (it may have been disconnected)") },
	{ LIBUSB_ERROR_NOT_FOUND,	_("entity not found") },
	{ LIBUSB_ERROR_BUSY,		_("resource busy") },
	{ LIBUSB_ERROR_TIMEOUT,		_("operation timed out") },
	{ LIBUSB_ERROR_OVERFLOW,	_("overflow") },
	{ LIBUSB_ERROR_PIPE,		_("pipe error") },
	{ LIBUSB_ERROR_INTERRUPTED,	_("system call interrupted (perhaps due to signal)") },
	{ LIBUSB_ERROR_NO_MEM,		_("insufficient memory") },
	{ LIBUSB_ERROR_NOT_SUPPORTED,	_("operation not supported or unimplemented on this platform") },
	{ LIBUSB_ERROR_OTHER,		_("other error") },
};

wxString romloader_usb::libusb_strerror(int iError)
{
	const LIBUSB_STRERROR_T *ptCnt, *ptEnd;
	const wxChar *pcMsg;
	const wxChar *pcUnknownError = _("unknown error");
	wxString strErrorMsg;


	ptCnt = atStrError;
	ptEnd = ptCnt + (sizeof(atStrError)/sizeof(atStrError[0]));
	pcMsg = pcUnknownError;
	while( ptCnt<ptEnd )
	{
		if( ptCnt->eErrNo==iError )
		{
			pcMsg = ptCnt->pcErrMsg;
			break;
		}
		else
		{
			++ptCnt;
		}
	}

	strErrorMsg.Printf(wxT("%d: %s"), iError, pcMsg);

	return strErrorMsg;
}


bool romloader_usb::isDeviceNetx(libusb_device *ptDev)
{
	bool fDeviceIsNetx;
	int iResult;
	struct libusb_device_descriptor sDevDesc;


	fDeviceIsNetx = false;
	if( ptDev!=NULL )
	{
		iResult = libusb_get_device_descriptor(ptDev, &sDevDesc);
		if( iResult==LIBUSB_SUCCESS )
		{
			fDeviceIsNetx  = true;
			fDeviceIsNetx &= ( sDevDesc.bDeviceClass==0x00 );
			fDeviceIsNetx &= ( sDevDesc.bDeviceSubClass==0x00 );
			fDeviceIsNetx &= ( sDevDesc.bDeviceProtocol==0x00 );
			fDeviceIsNetx &= ( sDevDesc.idVendor==0x0cc4 );
			fDeviceIsNetx &= ( sDevDesc.idProduct==0x0815 );
			fDeviceIsNetx &= ( sDevDesc.bcdDevice==0x0100 );
		}
	}

	return fDeviceIsNetx;
}

