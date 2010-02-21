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

#include <wx/wx.h>

#include <wx/aui/aui.h>
#include <wx/bmpcbox.h>
#include <wx/dir.h>
#include <wx/html/helpctrl.h>
#include <wx/image.h>
#include <wx/intl.h>
#include <wx/listctrl.h>
#include <wx/process.h>
#include <wx/progdlg.h>
#include <wx/splitter.h>
#include <wx/tipdlg.h>


#if defined(USE_LUA)
extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}
#endif

#ifndef __MUHKUH_MAINFRAME_H__
#define __MUHKUH_MAINFRAME_H__

#include "muhkuh_id.h"
#include "muhkuh_repository_manager.h"
#include "muhkuh_testTreeItemData.h"
#include "muhkuh_wrap_xml.h"



class muhkuh_server_process : public wxProcess
{
public:
	muhkuh_server_process(wxEvtHandler *parent, int id)
	: wxProcess(parent, id)
	{
		/* catch stdout and stderr */
		Redirect();
	}
};


//-------------------------------------
// Define the main frame

class muhkuh_mainFrame: public wxFrame
{
public:
	muhkuh_mainFrame(void);
	~muhkuh_mainFrame(void);

	// menu callbacks
	void OnQuit(wxCommandEvent &event);
	void OnAbout(wxCommandEvent &event);
	void OnConfigDialog(wxCommandEvent &event);
	void OnHelp(wxCommandEvent &event);
	void OnShowTip(wxCommandEvent &event);
	void OnRestoreDefaultPerspective(wxCommandEvent &event);

	void OnTimer(wxTimerEvent &event);
	void OnIdle(wxIdleEvent &event);

	void OnTestExecute(wxCommandEvent &event);
	void OnTestHelp(wxCommandEvent &event);
	void OnTestRescan(wxCommandEvent &event);
	void OnTestCancel(wxCommandEvent &event);
	void OnViewRepositoryPane(wxCommandEvent &event);
	void OnViewPanicButton(wxCommandEvent &event);
	void OnViewTestTree(wxCommandEvent &event);
	void OnViewMessageLog(wxCommandEvent &event);
	void OnViewWelcomePage(wxCommandEvent &event);
	void OnViewTestDetails(wxCommandEvent &event);

	void OnRepositoryCombo(wxCommandEvent &event);

	void OnTestTreeContextMenu(wxTreeEvent &event);
	void OnTestTreeItemActivated(wxTreeEvent &event);
	void OnTestTreeItemSelected(wxTreeEvent &event);

	void OnMtdLinkClicked(wxHtmlLinkEvent &event);

	void OnNotebookPageClose(wxAuiNotebookEvent &event);
	void OnPaneClose(wxAuiManagerEvent &event);

	void OnServerProcessTerminate(wxProcessEvent &event);

	void OnMove(wxMoveEvent &event);
	void OnSize(wxSizeEvent &event);

	static bool repositoryScannerCallback(void *pvUser, wxString strMessage, int iProgressPos, int iProgressMax);
	static const int m_iRepositoryProgressMax = 10000;

	// main frame status
	typedef enum {
		muhkuh_mainFrame_state_scanning = 0,
		muhkuh_mainFrame_state_idle,
		muhkuh_mainFrame_state_testing
	} muhkuh_mainFrame_state;

	// main frame init state
	typedef enum
	{
		MAINFRAME_INIT_STATE_UNCONFIGURED,
		MAINFRAME_INIT_STATE_CONFIGURED,
		MAINFRAME_INIT_STATE_SCANNED
	} MAINFRAME_INIT_STATE_E;

#if defined(USE_LUA)
	static wxString htmlTag_lua(const wxString &strLuaCode);
	wxString local_htmlTag_lua(const wxString &strLuaCode);

	typedef struct
	{
		int iLuaError;
		const wxChar *pcMessage;
	} LUA_ERROR_TO_STR_T;

	typedef struct
	{
		int iLuaError;
		const wxChar *pcMessage;
	} LUA_TYPE_TO_STR_T;
#endif

private:
	void createMenu(void);
	void createControls(void);
	void createTipProvider(void);
	void createWelcomeWindow(void);
	void createTestDetailsWindow(void);

	void read_config(void);
	void write_config(void);

	void executeTest(muhkuh_wrap_xml *ptTestData, unsigned int uiIndex);
	bool process_server_output(void);
	void finishTest(void);

	void setState(muhkuh_mainFrame_state tNewState);

	void scanTests(int iActiveRepositoryIdx);
	void addAllTests(int iActiveRepositoryIdx);
	bool scanFileXml(size_t sizRepositoryIdx, size_t sizTestIdx);

	void updateRepositoryCombo(void);

	bool cutPath(wxString &strPath, wxArrayString *ptElems);
	bool addTestTree(testTreeItemData *ptTestTreeItem);

	wxString loadHtmlString(wxString strFileUrl);
	void reloadWelcomePage(void);
	void reloadDetailsPage(muhkuh_wrap_xml *ptWrapXml);

	// main frame controls
	wxAuiManager m_auiMgr;
	// the default perspective
	wxString strDefaultPerspective;

	// gui elements
	wxBitmapComboBox *m_repositoryCombo;
	wxTreeCtrl *m_treeCtrl;
	wxTextCtrl *m_textCtrl;
	wxAuiNotebook *m_notebook;
	wxHtmlWindow *m_welcomeHtml;
	wxHtmlWindow *m_testDetailsHtml;
	wxButton *m_buttonCancelTest;
	wxMenuBar *m_menuBar;

	// log target for all plugins
	wxLog *m_pLogTarget;

	// the version string
	wxString m_strVersion;

	// the application icons in different sizes
	wxIconBundle m_frameIcons;

	// state of the init process
	MAINFRAME_INIT_STATE_E m_eInitState;

	// the process id of the server task
	long m_lServerPid;
	// the server process notification
	muhkuh_server_process *m_ptServerProcess;
	wxTimer m_timerIdleWakeUp;
	wxTextCtrl *m_textTestOutput;

	// main frame state
	muhkuh_mainFrame_state m_state;
	// TODO: replace the test name and idx with the xml wrapper object
	// name of the running test
	wxString m_strRunningTestName;
	size_t m_sizRunningTest_RepositoryIdx;
	size_t m_sizRunningTest_TestIdx;

	// scanner progress dialog
	wxProgressDialog *m_scannerProgress;

	// number of loaded test descriptions
	size_t m_sizTestCnt;

	// the repository manager
	muhkuh_repository_manager *m_ptRepositoryManager;

	// help controller
	wxHtmlHelpController *m_ptHelp;
	// tip provider
	wxTipProvider *m_tipProvider;
	bool m_fShowStartupTips;
	size_t m_sizStartupTipsIdx;

	// path settings
	wxString m_strApplicationPath;

	// the welcome page
	wxString m_strWelcomePage;
	// the test details page
	wxString m_strTestDetails;

	/* NOTE: Keep the configuration dialog the same, even if lua or python
	         is not compiled in. Just disable the controls, but show the
		 user that it is there.
        */
	// the lua include path
	wxString m_strLuaIncludePath;
	// lua startup code
	wxString m_strLuaStartupCode;
	// the temp file with the settings and the startup code
	wxString m_strRunningTestTempFileName;

	// the welcome page file
	wxString m_strWelcomePageFile;
	// the details page file
	wxString m_strDetailsPageFile;

	// the application title
	wxString m_strApplicationTitle;
	wxString m_strApplicationIcon;

	// the locale object
	wxLocale m_locale;

	// frame size and position
	wxPoint m_framePosition;
	wxSize m_frameSize;


	// lua stuff
#if defined(USE_LUA)
	void init_lua(void);

	static const LUA_ERROR_TO_STR_T atLuaErrorToString[];
	wxString lua_error_to_string(int iLuaError);
	static const LUA_TYPE_TO_STR_T atLuaTypeToString[];
	wxString lua_type_to_string(int iLuaType);
	bool lua_get_errorinfo(lua_State *L, int iStatus, int iTop, wxString *pstrErrorMsg, int *piLineNum);
	lua_State *m_ptLua_State;
#endif

    DECLARE_EVENT_TABLE()
};


extern muhkuh_mainFrame *g_ptMainFrame;


#endif	// __MUHKUH_MAINFRAME_H__

