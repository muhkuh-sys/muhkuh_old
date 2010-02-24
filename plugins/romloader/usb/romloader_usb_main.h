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


#include <vector>
#include <usb.h>

#include "../romloader.h"

#ifndef __BOOTLOADER_USB_MAIN_H__
#define __BOOTLOADER_USB_MAIN_H__

/*-----------------------------------*/

extern "C"
{
	WXEXPORT int fn_init(wxLog *ptLogTarget, wxXmlNode *ptCfgNode, wxString &strPluginId);
	WXEXPORT int fn_init_lua(wxLuaState *ptLuaState);
	WXEXPORT int fn_leave(void);
	WXEXPORT const muhkuh_plugin_desc *fn_get_desc(void);
	WXEXPORT int fn_detect_interfaces(std::vector<muhkuh_plugin_instance*> *pvInterfaceList);

	WXEXPORT romloader *romloader_usb_create(void *pvHandle);
}

/*-----------------------------------*/

/* NOTE: Seems like libusb-0.1 was the last version with a common api for
 * linux and windows. The windows port has it's own 1.0 api.
 * This plugin uses the libusb-1.0 api. It offers wrapper functions to link
 * with an old libusb-0.1 api.
 */
#ifndef ROMLOADER_USB_LIBUSB_VERSION
	#error "Missing libusb version, please define ROMLOADER_USB_LIBUSB_VERSION!"
#elif ROMLOADER_USB_LIBUSB_VERSION==0
	/* use libusb-0.1 */
	#include <usb.h>
	#include <errno.h>

	#define libusb_device_handle usb_dev_handle
	#define libusb_context void*
	#define libusb_device struct usb_device

	#define libusb_device_descriptor usb_device_descriptor

	enum libusb_error
	{
		LIBUSB_SUCCESS = 0,
		LIBUSB_ERROR_IO = -1,
		LIBUSB_ERROR_INVALID_PARAM = -2,
		LIBUSB_ERROR_ACCESS = -3,
		LIBUSB_ERROR_NO_DEVICE = -4,
		LIBUSB_ERROR_NOT_FOUND = -5,
		LIBUSB_ERROR_BUSY = -6,
		LIBUSB_ERROR_TIMEOUT = -7,
		LIBUSB_ERROR_OVERFLOW = -8,
		LIBUSB_ERROR_PIPE = -9,
		LIBUSB_ERROR_INTERRUPTED = -10,
		LIBUSB_ERROR_NO_MEM = -11,
		LIBUSB_ERROR_NOT_SUPPORTED = -12,
		LIBUSB_ERROR_OTHER = -99,
	};
#elif ROMLOADER_USB_LIBUSB_VERSION==1
	/* use libusb-1.0 */
	#include <libusb-1.0/libusb.h>

	#define usb_bulk_pc_to_netx libusb_bulk_transfer
	#define usb_bulk_netx_to_pc libusb_bulk_transfer
#else
	#error "The define ROMLOADER_USB_LIBUSB_VERSION specifies an unknown libusb version!"
#endif

/*-----------------------------------*/

typedef void (*romloader_usb_plugin_fn_close_instance)(wxString &strInterface);

class romloader_usb : public romloader
{
public:
	romloader_usb(wxString strName, wxString strTyp, const romloader_functioninterface *ptFn, unsigned long ulDeviceId, romloader_usb_plugin_fn_close_instance fn_close, wxLuaState *ptLuaState);
	~romloader_usb(void);

// *** lua interface start ***
	// open the connection to the device
	virtual void connect(void);
	// close the connection to the device
	virtual void disconnect(void);
	// returns the connection state of the device
	virtual bool is_connected(void) const;

	// read a byte (8bit) from the netx to the pc
	virtual double read_data08(double dNetxAddress);
	// read a word (16bit) from the netx to the pc
	virtual double read_data16(double dNetxAddress);
	// read a long (32bit) from the netx to the pc
	virtual double read_data32(double dNetxAddress);
	// read a byte array from the netx to the pc
	virtual wxString read_image(double dNetxAddress, double dSize, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData);

	// write a byte (8bit) from the pc to the netx
	virtual void write_data08(double dNetxAddress, double dData);
	// write a word (16bit) from the pc to the netx
	virtual void write_data16(double dNetxAddress, double dData);
	// write a long (32bit) from the pc to the netx
	virtual void write_data32(double dNetxAddress, double dData);
	// write a byte array from the pc to the netx
	virtual void write_image(double dNetxAddress, const char *pcData, size_t sizData, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData);

	// call routine
	virtual void call(double dNetxAddress, double dParameterR0, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData);
// *** lua interface end ***

	typedef struct
	{
		libusb_error eErrNo;
		const wxChar *pcErrMsg;
	} LIBUSB_STRERROR_T;
	static wxString libusb_strerror(int iError);
	static bool isDeviceNetx(libusb_device *ptDev);
private:
	bool chip_init(void);

	int write_data(wxString &strData, unsigned long ulLoadAdr, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData);
	bool parseDumpLine(const char *pcLine, size_t sizLineLen, unsigned long ulAddress, unsigned long ulElements, unsigned char *pucBuffer, wxString &strErrorMsg);

	int getLine(wxString &strData);

	int usb_executeCommand(wxString strCommand, wxString &strResponse);
	int usb_load(const char *pcData, size_t sizDataLen, unsigned long ulLoadAdr, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData);
	int usb_call(unsigned long ulNetxAddress, unsigned long ulParameterR0, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData);
	int usb_sendCommand(wxString strCommand);
	int usb_getNetxData(wxString &strData, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData);

	// lowlevel functions
	int libusb_open_by_bus_and_adr(wxString &strErrorMsg);
	int libusb_closeDevice(void);
	int libusb_resetDevice(void);
	int libusb_readBlock(unsigned char *pucReceiveBuffer, unsigned int uiSize, int iTimeoutMs);
	int libusb_writeBlock(unsigned char *pucSendBuffer, unsigned int uiSize, int iTimeoutMs);
	int libusb_exchange(unsigned char *pucSendBuffer, unsigned char *pucReceiveBuffer);


	static const LIBUSB_STRERROR_T atStrError[];




	bool m_fIsConnected;

	romloader_usb_plugin_fn_close_instance m_fn_usb_close;

	wxString m_strInterface;
	wxString m_strTyp;

	// formatted name for log message
	wxString m_strMe;

	/* libusb context */
	libusb_context *m_ptLibUsbContext;
	/* pointer to the usb device */
	libusb_device_handle *m_ptUsbDevHandle;

	/* bus number and device address for the connection */
	unsigned char m_ucNetxBusNr;
	unsigned char m_ucNetxDeviceAddress;

	// buffer for the read_image command
	size_t sizBufLen;
	size_t sizBufPos;
	bool fEof;
	char acBuf[64];
};

/*-----------------------------------*/

#endif	/* __BOOTLOADER_USB_MAIN_H__ */

