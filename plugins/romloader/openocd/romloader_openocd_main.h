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

#include "../romloader.h"

#ifndef __ROMLOADER_OPENOCD_MAIN_H__
#define __ROMLOADER_OPENOCD_MAIN_H__

/*-----------------------------------*/

extern "C"
{
	WXEXPORT int fn_init(wxLog *ptLogTarget, wxXmlNode *ptCfgNode, wxString &strPluginId);
	WXEXPORT int fn_init_lua(wxLuaState *ptLuaState);
	WXEXPORT int fn_leave(void);
	WXEXPORT const muhkuh_plugin_desc *fn_get_desc(void);
	WXEXPORT int fn_detect_interfaces(std::vector<muhkuh_plugin_instance*> *pvInterfaceList);

	WXEXPORT romloader *romloader_openocd_create(void *pvHandle);
}

class romloader_openocd : public romloader
{
public:
	romloader_openocd(wxString strName, wxString strTyp, 
									 const romloader_functioninterface *ptFn, 
									 void *pvHandle, 
									 muhkuh_plugin_fn_close_instance fn_close, 
									 wxLuaState *ptLuaState);
	~romloader_openocd(void);

	void connect(void);

private:
	// formatted name for log message
	wxString m_strMe;
};


/*-----------------------------------*/

#endif	/* __ROMLOADER_OPENOCD_MAIN_H__ */

