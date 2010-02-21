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


#include <wx/stdpaths.h>
#include <wx/txtstrm.h>
#include <wx/url.h>


#include "growbuffer.h"
#include "muhkuh_aboutDialog.h"
#include "muhkuh_app.h"
#include "muhkuh_configDialog.h"
#include "muhkuh_icons.h"
#include "muhkuh_mainFrame.h"
#include "muhkuh_version.h"
#include "readFsFile.h"

#if defined(USE_LUA)
extern "C" {
	int luaopen_muhkuh_app(lua_State *L);
}
#endif

//-------------------------------------

BEGIN_EVENT_TABLE(muhkuh_mainFrame, wxFrame)
	EVT_IDLE(muhkuh_mainFrame::OnIdle)

	EVT_MENU(wxID_EXIT,						muhkuh_mainFrame::OnQuit)
	EVT_MENU(wxID_ABOUT,						muhkuh_mainFrame::OnAbout)
	EVT_MENU(wxID_PREFERENCES,					muhkuh_mainFrame::OnConfigDialog)
	EVT_MENU(wxID_HELP,						muhkuh_mainFrame::OnHelp)

	EVT_MENU(muhkuh_mainFrame_TestTree_Execute,			muhkuh_mainFrame::OnTestExecute)
	EVT_MENU(muhkuh_mainFrame_TestTree_ShowHelp,			muhkuh_mainFrame::OnTestHelp)
	EVT_MENU(muhkuh_mainFrame_TestTree_Rescan,			muhkuh_mainFrame::OnTestRescan)
	EVT_MENU(muhkuh_mainFrame_menuShowTip,				muhkuh_mainFrame::OnShowTip)
	EVT_MENU(muhkuh_mainFrame_menuRestoreDefaultPerspective,	muhkuh_mainFrame::OnRestoreDefaultPerspective)
	EVT_MENU(muhkuh_mainFrame_menuTestCancel,			muhkuh_mainFrame::OnTestCancel)
	EVT_MENU(muhkuh_mainFrame_menuTestRescan,			muhkuh_mainFrame::OnTestRescan)
	EVT_MENU(muhkuh_mainFrame_menuViewRepositoryPane,		muhkuh_mainFrame::OnViewRepositoryPane)
	EVT_MENU(muhkuh_mainFrame_menuViewPanicButton,			muhkuh_mainFrame::OnViewPanicButton)
	EVT_MENU(muhkuh_mainFrame_menuViewTestTree,			muhkuh_mainFrame::OnViewTestTree)
	EVT_MENU(muhkuh_mainFrame_menuViewMessageLog,			muhkuh_mainFrame::OnViewMessageLog)
	EVT_MENU(muhkuh_mainFrame_menuViewWelcomePage,			muhkuh_mainFrame::OnViewWelcomePage)
	EVT_MENU(muhkuh_mainFrame_menuViewTestDetails,			muhkuh_mainFrame::OnViewTestDetails)

	EVT_COMBOBOX(muhkuh_mainFrame_RepositoryCombo_id,		muhkuh_mainFrame::OnRepositoryCombo)

	EVT_TREE_ITEM_ACTIVATED(muhkuh_mainFrame_TestTree_Ctrl,		muhkuh_mainFrame::OnTestTreeItemActivated)
	EVT_TREE_ITEM_MENU(muhkuh_mainFrame_TestTree_Ctrl,		muhkuh_mainFrame::OnTestTreeContextMenu)
	EVT_TREE_SEL_CHANGED(muhkuh_mainFrame_TestTree_Ctrl,		muhkuh_mainFrame::OnTestTreeItemSelected)

	EVT_HTML_LINK_CLICKED(muhkuh_mainFrame_Welcome_id,		muhkuh_mainFrame::OnMtdLinkClicked)
	EVT_HTML_LINK_CLICKED(muhkuh_mainFrame_TestDetails_id,		muhkuh_mainFrame::OnMtdLinkClicked)

	EVT_BUTTON(muhkuh_mainFrame_cancelTestButton_id,		muhkuh_mainFrame::OnTestCancel)
	EVT_AUINOTEBOOK_PAGE_CLOSE(muhkuh_mainFrame_Notebook_id,	muhkuh_mainFrame::OnNotebookPageClose)
	EVT_AUI_PANE_CLOSE(muhkuh_mainFrame::OnPaneClose)

	EVT_END_PROCESS(muhkuh_serverProcess_terminate,			muhkuh_mainFrame::OnServerProcessTerminate)
	EVT_TIMER(wxID_ANY,						muhkuh_mainFrame::OnTimer)

	EVT_MOVE(muhkuh_mainFrame::OnMove)
	EVT_SIZE(muhkuh_mainFrame::OnSize)
END_EVENT_TABLE()


muhkuh_mainFrame::muhkuh_mainFrame(void)
 : wxFrame(NULL, wxID_ANY, wxEmptyString)
 , m_treeCtrl(NULL)
 , m_textCtrl(NULL)
 , m_eInitState(MAINFRAME_INIT_STATE_UNCONFIGURED)
 , m_state(muhkuh_mainFrame_state_idle)
 , m_sizTestCnt(0)
 , m_ptRepositoryManager(NULL)
 , m_ptHelp(NULL)
 , m_lServerPid(0)
 , m_ptServerProcess(NULL)
 , m_tipProvider(NULL)
 , m_welcomeHtml(NULL)
 , m_testDetailsHtml(NULL)
 , m_timerIdleWakeUp(this)
 , m_textTestOutput(NULL)
#if defined(USE_LUA)
 , m_ptLua_State(NULL)
#endif
{
	wxLog *pOldLogTarget;
	wxFileName cfgName;
	wxFileConfig *ptConfig;
	int iLanguage;


	// get the application path
	cfgName.Assign(wxStandardPaths::Get().GetExecutablePath());
	m_strApplicationPath = cfgName.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);

	// get the config
	cfgName.SetFullName(wxTheApp->GetAppName()+wxT(".cfg"));
	ptConfig = new wxFileConfig(wxTheApp->GetAppName(), wxTheApp->GetVendorName(), cfgName.GetFullPath(), cfgName.GetFullPath(), wxCONFIG_USE_LOCAL_FILE);
	wxConfigBase::Set(ptConfig);
	ptConfig->SetRecordDefaults();

	// TODO: init the locale to a value from the config file
//	iLanguage = wxLANGUAGE_GERMAN;
//	iLanguage = wxLANGUAGE_ENGLISH_US;
	iLanguage = wxLocale::GetSystemLanguage();
//	wxLogMessage(_("Using language '%s'."), wxLocale::GetLanguageName(iLanguage).fn_str());
	if( m_locale.Init(iLanguage, wxLOCALE_CONV_ENCODING)==false )
	{
		// NOTE: translate this so the default system language may be used at least.
		wxLogError(_("The language '%s' is not supported by the system."), wxLocale::GetLanguageName(iLanguage).c_str());
	}
	else
	{
		// search locales in the 'locale' directory
		// TODO: use some systempath for this
		wxLocale::AddCatalogLookupPathPrefix(wxT("locale"));
		m_locale.AddCatalog(wxTheApp->GetAppName());
	}

	// create the repository manager
	m_ptRepositoryManager = new muhkuh_repository_manager();

	// create the server process notifier
	m_ptServerProcess = new muhkuh_server_process(this, muhkuh_serverProcess_terminate);

	// build version string
	m_strVersion.Printf(wxT(MUHKUH_APPLICATION_NAME) wxT(" v%d.%d.%d"), MUHKUH_VERSION_MAJ, MUHKUH_VERSION_MIN, MUHKUH_VERSION_SUB);

	// use aui manager for this frame
	m_auiMgr.SetManagedWindow(this);

	// create a status bar with text on the left side and a gauge on the right
	CreateStatusBar(2);
	// use pane 1 for menu and toolbar help
	SetStatusBarPane(0);

	// create the menu bar
	createMenu();

	// create the controls
	createControls();

	// set new log target
	m_pLogTarget = new wxLogTextCtrl(m_textCtrl);
	pOldLogTarget = wxLog::SetActiveTarget(m_pLogTarget);
	if( pOldLogTarget!=NULL )
	{
		delete pOldLogTarget;
	}
	wxLog::SetVerbose(true);
	// this is for release
//	wxLog::SetLogLevel(wxLOG_Info);
	// this is for debug, it's a lot more noisy
	wxLog::SetLogLevel(wxLOG_Debug);

	wxLogMessage(_("application started"));

	// read the config
	read_config();

	// set the window title
	if( m_strApplicationTitle.IsEmpty()==true )
	{
		// no custom title -> use the version string
		SetTitle(m_strVersion);
	}
	else
	{
		// show the custom title
		SetTitle(m_strApplicationTitle);
	}

	// set the application icon
	if( m_strApplicationIcon.IsEmpty()==true )
	{
		// set the muhkuh icon
		m_frameIcons.AddIcon(muhkuh_016_xpm);
		m_frameIcons.AddIcon(muhkuh_032_xpm);
		m_frameIcons.AddIcon(muhkuh_064_xpm);
		m_frameIcons.AddIcon(muhkuh_128_xpm);
	}
	else
	{
		// load a custom icon
		m_frameIcons.AddIcon(m_strApplicationIcon, wxBITMAP_TYPE_ICO);
	}
	SetIcons(m_frameIcons);

	// create the tip provider
	createTipProvider();

	// NOTE: do not scan the tests here, this freezes the win version
	m_eInitState  = MAINFRAME_INIT_STATE_CONFIGURED;

	InitDialog();

#if defined(USE_LUA)
	/* open a new lua state */
	m_ptLua_State = lua_open();
	if( m_ptLua_State!=NULL )
	{
		luaL_openlibs(m_ptLua_State);
		luaopen_muhkuh_app(m_ptLua_State);
	}
#endif
}


muhkuh_mainFrame::~muhkuh_mainFrame(void)
{
	write_config();

#if defined(USE_LUA)
	// delete the lua state
	if( m_ptLua_State!=NULL )
	{
		lua_close(m_ptLua_State);
	}
#endif

	// delete the help controller
	if( m_ptHelp!=NULL )
	{
		delete m_ptHelp;
		m_ptHelp = NULL;
	}

	// delete the tip provider
	if( m_tipProvider!=NULL )
	{
		delete m_tipProvider;
	}

	// force idle state
	setState(muhkuh_mainFrame_state_idle);

	finishTest();

	// delete the server notifier
	if( m_ptServerProcess!=NULL )
	{
		delete m_ptServerProcess;
	}

	// delete the repository manager
	if( m_ptRepositoryManager!=NULL )
	{
		delete m_ptRepositoryManager;
	}

	m_auiMgr.UnInit();
}


void muhkuh_mainFrame::createMenu(void)
{
	wxMenu *file_menu;
	wxMenu *test_menu;
	wxMenu *view_menu;
	wxMenu *help_menu;
	wxMenuItem *ptMenuItem;


	file_menu = new wxMenu;
	ptMenuItem = new wxMenuItem(file_menu, wxID_PREFERENCES, _("Preferences"), _("Open the preferences dialog"));
	ptMenuItem->SetBitmap(icon_famfamfam_silk_wrench);
	file_menu->Append(ptMenuItem);
	ptMenuItem = new wxMenuItem(file_menu, wxID_EXIT, wxString(_("Quit"))+wxT("\tCtrl+Q"), _("Quit the program"));
	ptMenuItem->SetBitmap(icon_famfamfam_silk_door_out);
	file_menu->Append(ptMenuItem);

	test_menu = new wxMenu;
	ptMenuItem = new wxMenuItem(test_menu, muhkuh_mainFrame_menuTestCancel, _("Cancel"), _("Cancel the running test"));
	ptMenuItem->SetBitmap(icon_famfamfam_silk_cancel);
	test_menu->Append(ptMenuItem);
	ptMenuItem = new wxMenuItem(test_menu, muhkuh_mainFrame_menuTestRescan, wxString(_("Rescan"))+wxT("\tCtrl-R"), _("Rescan the repository"));
	ptMenuItem->SetBitmap(icon_famfamfam_silk_database_refresh);
	test_menu->Append(ptMenuItem);

	view_menu = new wxMenu;
	view_menu->AppendCheckItem(muhkuh_mainFrame_menuViewRepositoryPane,	_("View Repository Selector"),		_("Toggle the visibility of the repository selector"));
	view_menu->AppendCheckItem(muhkuh_mainFrame_menuViewPanicButton,	_("View Panic Button"),			_("Toggle the visibility of the panic button"));
	view_menu->AppendCheckItem(muhkuh_mainFrame_menuViewTestTree,		_("View Test Tree"),			_("Toggle the visibility of the test tree"));
	view_menu->AppendCheckItem(muhkuh_mainFrame_menuViewMessageLog,		_("View Message Log"),			_("Toggle the visibility of the message log"));
	view_menu->AppendCheckItem(muhkuh_mainFrame_menuViewWelcomePage,	_("View Welcome Page"),			_("Toggle the visibility of the Welcome page"));
	view_menu->AppendCheckItem(muhkuh_mainFrame_menuViewTestDetails,	_("View Test Details"),			_("Toggle the visibility of the Test Details page"));
	view_menu->Append(muhkuh_mainFrame_menuRestoreDefaultPerspective,	_("Restore Default Layout"),		_("Restore the default window layout"));

	help_menu = new wxMenu;
	ptMenuItem = new wxMenuItem(help_menu, wxID_HELP, wxString(_("Help"))+wxT("\tCtrl+H"), _("Show the documentation"));
	ptMenuItem->SetBitmap(icon_famfamfam_silk_help);
	help_menu->Append(ptMenuItem);
	help_menu->AppendSeparator();
	ptMenuItem = new wxMenuItem(help_menu, wxID_ABOUT, _("About"), _("Show details about Muhkuh"));
	ptMenuItem->SetBitmap(icon_famfamfam_silk_information);
	help_menu->Append(ptMenuItem);
	help_menu->AppendSeparator();
	ptMenuItem = new wxMenuItem(help_menu, muhkuh_mainFrame_menuShowTip, _("Show Tip"), _("Show some tips about Muhkuh"));
	ptMenuItem->SetBitmap(icon_famfamfam_silk_lightbulb);
	help_menu->Append(ptMenuItem);

	m_menuBar = new wxMenuBar;
	m_menuBar->Append(file_menu, _("&File"));
	m_menuBar->Append(test_menu, _("&Test"));
	m_menuBar->Append(view_menu, _("&View"));
	m_menuBar->Append(help_menu, _("&Help"));

	SetMenuBar(m_menuBar);
}


void muhkuh_mainFrame::createControls(void)
{
	wxAuiPaneInfo paneInfo;
	long style;
	wxSize tSize;
	wxListItem listItem;


	// TODO: is this really necessary?
	SetMinSize(wxSize(400,300));

	// add the repository combo box
	m_repositoryCombo = new wxBitmapComboBox(this, muhkuh_mainFrame_RepositoryCombo_id, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY);
	paneInfo.Name(wxT("repository_selector")).CaptionVisible(true).Caption(_("Repository Selector")).Top().Position(0);
	m_auiMgr.AddPane(m_repositoryCombo, paneInfo);

	// add the cancel button
	m_buttonCancelTest = new wxButton(this, muhkuh_mainFrame_cancelTestButton_id, _("Cancel Test"));
	paneInfo.Name(wxT("panic_button")).CaptionVisible(true).Caption(_("Panic Button")).Top().Position(1);
	m_auiMgr.AddPane(m_buttonCancelTest, paneInfo);

	// create the tree
	style = wxTR_DEFAULT_STYLE | wxTR_HIDE_ROOT | wxSUNKEN_BORDER;
	m_treeCtrl = new wxTreeCtrl(this, muhkuh_mainFrame_TestTree_Ctrl, wxDefaultPosition, wxDefaultSize, style);
	paneInfo.Name(wxT("test_tree")).CaptionVisible(true).Caption(_("Test Tree")).Left().Position(0);
	m_auiMgr.AddPane(m_treeCtrl, paneInfo);

	// create the auinotebook
	m_notebook = new wxAuiNotebook(this, muhkuh_mainFrame_Notebook_id);
	paneInfo.Name(wxT("notebook")).CaptionVisible(false).Center().Position(0);
	m_auiMgr.AddPane(m_notebook, paneInfo);

	// create the "welcome" html
	createWelcomeWindow();
	// create the test details list
	createTestDetailsWindow();

	style = wxTE_MULTILINE | wxSUNKEN_BORDER | wxTE_READONLY;
	m_textCtrl = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, style);
	paneInfo.Name(wxT("message_log")).CaptionVisible(true).Caption(_("Message Log")).Bottom().Position(0);
	m_auiMgr.AddPane(m_textCtrl, paneInfo);

	m_auiMgr.Update();

	// save this layout as the default perspective
	strDefaultPerspective = m_auiMgr.SavePerspective();

	// create help controller
	m_ptHelp = new wxHtmlHelpController(wxHF_DEFAULT_STYLE);
}


void muhkuh_mainFrame::createTipProvider(void)
{
	// create the tip provider
	// TODO: check if the tips.txt file exists
	m_tipProvider = wxCreateFileTipProvider(wxT("muhkuh_tips.txt"), m_sizStartupTipsIdx);
}


void muhkuh_mainFrame::createWelcomeWindow(void)
{
	m_welcomeHtml = new wxHtmlWindow(this, muhkuh_mainFrame_Welcome_id, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);
	m_welcomeHtml->SetPage(m_strWelcomePage);
	m_welcomeHtml->SetRelatedFrame(this, m_strVersion + wxT(" - %s"));
	m_welcomeHtml->SetRelatedStatusBar(0);
	m_notebook->AddPage(m_welcomeHtml, _("Welcome"), false, m_frameIcons.GetIcon(16));
}


void muhkuh_mainFrame::createTestDetailsWindow(void)
{
	m_testDetailsHtml = new wxHtmlWindow(this, muhkuh_mainFrame_TestDetails_id, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);
	m_testDetailsHtml->SetPage(m_strTestDetails);
	m_testDetailsHtml->SetRelatedFrame(this, m_strVersion + wxT(" - %s"));
	m_testDetailsHtml->SetRelatedStatusBar(0);
	m_notebook->AddPage(m_testDetailsHtml, _("Test Details"), false, m_frameIcons.GetIcon(16));
}


wxString muhkuh_mainFrame::loadHtmlString(wxString strFileUrl)
{
	growbuffer *ptGrowBuffer;
	bool fResult;
	size_t sizDataSize;
	unsigned char *pucData;
	wxString strData;


	ptGrowBuffer = new growbuffer(65536);
	fResult = readFsFile(ptGrowBuffer, strFileUrl);
	if( fResult==true )
	{
		sizDataSize = ptGrowBuffer->getSize();
		pucData = ptGrowBuffer->getData();
		strData = wxString::From8BitData((const char*)pucData, sizDataSize);
	}
	else
	{
		strData.Printf(_("<html><body><h1>Error!</h1>Failed to load file '%s'!</body></html>"), strFileUrl.c_str());
	}
	delete ptGrowBuffer;

	return strData;
}


void muhkuh_mainFrame::reloadWelcomePage(void)
{
	wxString strPage;


	if( m_strWelcomePageFile.IsEmpty()==true )
	{
		// use the default welcome message
		strPage = _("<html><head><title>Welcome</title></head><body><h1>Welcome to <lua>return muhkuh_app.get_version()</lua></h1></body></html>");
	}
	else
	{
		// load from file
		strPage = loadHtmlString(m_strWelcomePageFile);
	}

	m_strWelcomePage = strPage;

	if( m_welcomeHtml!=NULL )
	{
		m_welcomeHtml->SetPage(m_strWelcomePage);
	}
}


void muhkuh_mainFrame::reloadDetailsPage(muhkuh_wrap_xml *ptWrapXml)
{
	wxString strTestName;
	wxString strSubTestName;
	wxString strPage;


	if( m_strDetailsPageFile.IsEmpty()==true )
	{
		strPage = wxT(	"<html><lua>\n"
				"	local strPage\n"
				"	local testDesc\n"
				"	local testName\n"
				"	local pageTitle\n"
				"	local subTestName\n"
				"\n"
				"\n"
				"	testDesc = muhkuh.GetSelectedTest()\n"
				"	if testDesc then\n"
				"		testName = testDesc:testDescription_getName()\n"
				"		pageTitle = \"Test Details for \" .. testName\n"
				"	else\n"
				"		pageTitle = \"Test Details\"\n"
				"	end\n"
				"\n"
				"	strPage =            \"<head>\\n\"\n"
				"	strPage = strPage .. \"<meta name=\\\"Keywords\\\" content=\\\"Muhkuh\\\">\\n\"\n"
				"	strPage = strPage .. \"<meta name=\\\"Author\\\" content=\\\"\" .. _G.__MUHKUH_VERSION .. \"\\\">\\n\"\n"
				"	strPage = strPage .. \"<meta name=\\\"Description\\\" content=\\\"\" .. pageTitle .. \"\\\">\\n\"\n"
				"	strPage = strPage .. \"<title>\" .. pageTitle .. \"</title>\\n\"\n"
				"	strPage = strPage .. \"</head><body>\\n\"\n"
				"\n"
				"\n"
				"	if testDesc then\n"
				"		strPage = strPage .. \"<h1><a href=\\\"mtd://\" .. testName .. \"\\\">\" .. testName .. \"</a></h1>\\n\"\n"
				"		strPage = strPage .. \"Version \" .. testDesc:testDescription_getVersion()\n"
				"		strPage = strPage .. \"<p>\\n\"\n"
				"\n"
				"		strPage = strPage .. \"<table border=1><tr><th>Subtest</th><th>Version</th></tr><tbody>\\n\"\n"
				"\n"
				"		for iCnt = 0 , testDesc:testDescription_getTestCnt()-1 do\n"
				"			if testDesc:testDescription_setTest(iCnt)~=true then\n"
				"				strPage = strPage .. \"<tr><td>Error:</td><td>Failed to query subtest!</td><tr>\\n\"\n"
				"				break\n"
				"			else\n"
				"				subTestName = testDesc:test_getName()\n"
				"\n"
				"				strPage = strPage .. \"<tr><td>\"\n"
				"				strPage = strPage .. \"<a href=\\\"mtd://\" .. testName .. \"#\" .. subTestName .. \"\\\">\" .. subTestName .. \"</a>\"\n"
				"				strPage = strPage .. \"</td><td>\"\n"
				"				strPage = strPage .. testDesc:test_getVersion()\n"
				"				strPage = strPage .. \"</td></tr>\\n\"\n"
				"			end\n"
				"		end\n"
				"\n"
				"		strPage = strPage .. \"</tbody></table>\\n\"\n"
				"	else\n"
				"		strPage = strPage .. \"<h1>\" .. pageTitle .. \"</h1>\\n\"\n"
				"		strPage = strPage .. \"No test selected.<p>Please select a test in the test tree.\"\n"
				"	end\n"
				"\n"
				"	strPage = strPage .. \"</body>\\n\"\n"
				"\n"
				"\n"
				"	return strPage\n"
				"</lua></html>\n"
		);
	}
	else
	{
		// load from file
		strPage = loadHtmlString(m_strDetailsPageFile);
	}

	m_strTestDetails = strPage;

	if( m_testDetailsHtml!=NULL )
	{
		m_testDetailsHtml->SetPage(m_strTestDetails);
	}
}


void muhkuh_mainFrame::read_config(void)
{
	wxConfigBase *pConfig;
	wxString strPerspective;
	bool fPaneIsVisible;
	bool fWelcomePageIsVisible;
	bool fTestDetailsPageIsVisible;
	int iPageIdx;
	bool fWinMaximized;


	// get the config
	pConfig = wxConfigBase::Get();
	if( pConfig==NULL )
	{
		return;
	}

	// get mainframe position and size
	pConfig->SetPath(wxT("/MainFrame"));
	m_framePosition.x = pConfig->Read(wxT("x"), 50);
	m_framePosition.y = pConfig->Read(wxT("y"), 50);
	m_frameSize.SetWidth( pConfig->Read(wxT("w"), 640) );
	m_frameSize.SetHeight( pConfig->Read(wxT("h"), 480) );
	pConfig->Read(wxT("maximized"), &fWinMaximized, false);
	strPerspective = pConfig->Read(wxT("perspective"), wxEmptyString);
	pConfig->Read(wxT("showwelcome"), &fWelcomePageIsVisible, true);
	pConfig->Read(wxT("showtestdetails"), &fTestDetailsPageIsVisible, true);
	pConfig->Read(wxT("showtips"), &m_fShowStartupTips, true);
	m_sizStartupTipsIdx = pConfig->Read(wxT("tipidx"), (long)0);
	m_strWelcomePageFile = pConfig->Read(wxT("welcomepage"), wxEmptyString);
	m_strDetailsPageFile = pConfig->Read(wxT("detailspage"), wxEmptyString);
	m_strApplicationTitle = pConfig->Read(wxT("customtitle"), wxEmptyString);
	m_strApplicationIcon = pConfig->Read(wxT("customicon"), wxEmptyString);

	// get lua settings
	pConfig->SetPath(wxT("/Lua"));
	m_strLuaIncludePath = pConfig->Read(wxT("includepaths"), wxT("lua/?.lua"));
	m_strLuaStartupCode = pConfig->Read(wxT("startupcode"), wxT("require(\"wx\")\nrequire(\"bit\")\nrequire(\"muhkuh\")\nrequire(\"muhkuh_system\")muhkuh_system.boot_xml()\n"));

	// get all repositories
	m_ptRepositoryManager->read_config(pConfig);
	// show all repositories in the combo box
	updateRepositoryCombo();
	// set the active repository
	m_repositoryCombo->Select(m_ptRepositoryManager->GetActiveRepository());

	// set window properties
	SetSize(m_framePosition.x, m_framePosition.y, m_frameSize.GetWidth(), m_frameSize.GetHeight());
	// set fullscreen mode
	Maximize(fWinMaximized);
	// set perspective
	if( strPerspective.IsEmpty()==false )
	{
		m_auiMgr.LoadPerspective(strPerspective,true);
		// set the "view" buttons according to the new perspective
		fPaneIsVisible = m_auiMgr.GetPane(m_repositoryCombo).IsShown();
		m_menuBar->Check(muhkuh_mainFrame_menuViewRepositoryPane, fPaneIsVisible);
		fPaneIsVisible = m_auiMgr.GetPane(m_buttonCancelTest).IsShown();
		m_menuBar->Check(muhkuh_mainFrame_menuViewPanicButton, fPaneIsVisible);
		fPaneIsVisible = m_auiMgr.GetPane(m_treeCtrl).IsShown();
		m_menuBar->Check(muhkuh_mainFrame_menuViewTestTree, fPaneIsVisible);
		fPaneIsVisible = m_auiMgr.GetPane(m_textCtrl).IsShown();
		m_menuBar->Check(muhkuh_mainFrame_menuViewMessageLog, fPaneIsVisible);
	}
	m_menuBar->Check(muhkuh_mainFrame_menuViewWelcomePage, fWelcomePageIsVisible);
	if( m_welcomeHtml!=NULL && fWelcomePageIsVisible==false )
	{
		// close welcome page
		iPageIdx = m_notebook->GetPageIndex(m_welcomeHtml);
		if( iPageIdx!=wxNOT_FOUND )
		{
			m_notebook->DeletePage(iPageIdx);
		}
		// forget the pointer
		m_welcomeHtml = NULL;
	}
	m_menuBar->Check(muhkuh_mainFrame_menuViewTestDetails, fTestDetailsPageIsVisible);
	if( m_testDetailsHtml!=NULL && fTestDetailsPageIsVisible==false )
	{
		// close test details page
		iPageIdx = m_notebook->GetPageIndex(m_testDetailsHtml);
		if( iPageIdx!=wxNOT_FOUND )
		{
			m_notebook->DeletePage(iPageIdx);
		}
		// forget the pointer
		m_testDetailsHtml = NULL;	
	}
}


void muhkuh_mainFrame::write_config(void)
{
	wxConfigBase *pConfig;
	wxString strPerspective;
	int iPageIdx;
	bool fWelcomePageIsVisible;
	bool fTestDetailsPageIsVisible;
	bool fWinMaximized;


	pConfig = wxConfigBase::Get();
	if( pConfig == NULL )
	{
		return;
	}

	fWinMaximized = IsMaximized();

	// save the frame position
	strPerspective = m_auiMgr.SavePerspective();

	// save tip provider data
	if( m_tipProvider!=NULL )
	{
		m_sizStartupTipsIdx = m_tipProvider->GetCurrentTip();
	}

	// save visibility of welcome and test details page
	if( m_welcomeHtml!=NULL )
	{
		// close welcome page
		iPageIdx = m_notebook->GetPageIndex(m_welcomeHtml);
		fWelcomePageIsVisible =  (iPageIdx!=wxNOT_FOUND);
	}
	else
	{
		fWelcomePageIsVisible = false;
	}
	if( m_testDetailsHtml!=NULL )
	{
		// close test details page
		iPageIdx = m_notebook->GetPageIndex(m_testDetailsHtml);
		fTestDetailsPageIsVisible = (iPageIdx!=wxNOT_FOUND);
	}
	else
	{
		fTestDetailsPageIsVisible = false;
	}

	pConfig->SetPath(wxT("/MainFrame"));
	pConfig->Write(wxT("x"),		(long)m_framePosition.x);
	pConfig->Write(wxT("y"),		(long)m_framePosition.y);
	pConfig->Write(wxT("w"),		(long)m_frameSize.GetWidth());
	pConfig->Write(wxT("h"),		(long)m_frameSize.GetHeight());
	pConfig->Write(wxT("maximized"),	fWinMaximized);
	pConfig->Write(wxT("perspective"),	strPerspective);
	pConfig->Write(wxT("showwelcome"),	fWelcomePageIsVisible);
	pConfig->Write(wxT("showtestdetails"),	fTestDetailsPageIsVisible);
	pConfig->Write(wxT("showtips"),		m_fShowStartupTips);
	pConfig->Write(wxT("tipidx"),		(long)m_sizStartupTipsIdx);
	pConfig->Write(wxT("welcomepage"),	m_strWelcomePageFile);
	pConfig->Write(wxT("detailspage"),	m_strDetailsPageFile);
	pConfig->Write(wxT("customtitle"),	m_strApplicationTitle);
	pConfig->Write(wxT("customicon"),	m_strApplicationIcon);

	pConfig->SetPath(wxT("/"));

	// get lua settings
	pConfig->SetPath(wxT("/Lua"));
	pConfig->Write(wxT("includepaths"), m_strLuaIncludePath);
	pConfig->Write(wxT("startupcode"), m_strLuaStartupCode);
	pConfig->SetPath(wxT("/"));

	// save repositories
	m_ptRepositoryManager->write_config(pConfig);
}


void muhkuh_mainFrame::setState(muhkuh_mainFrame_state tNewState)
{
	int iWelcomePageIdx;
	int iTestDetailsPageIdx;
	bool fRepositorySelected;


	// leave old state
	switch(m_state)
	{
	case muhkuh_mainFrame_state_scanning:
		break;

	case muhkuh_mainFrame_state_idle:
		break;

	case muhkuh_mainFrame_state_testing:
		break;
	}

	iWelcomePageIdx = m_notebook->GetPageIndex(m_welcomeHtml);
	iTestDetailsPageIdx = m_notebook->GetPageIndex(m_testDetailsHtml);

	// enter new state
	switch(tNewState)
	{
	case muhkuh_mainFrame_state_scanning:
		m_buttonCancelTest->Enable(false);
		m_menuBar->Enable(muhkuh_mainFrame_menuTestCancel, false);
		m_menuBar->Enable(muhkuh_mainFrame_menuViewWelcomePage, false);
		m_menuBar->Enable(muhkuh_mainFrame_menuViewTestDetails, false);
		m_repositoryCombo->Enable(false);
		m_treeCtrl->Enable(false);
		m_menuBar->Enable(wxID_PREFERENCES, false);
		m_menuBar->Enable(muhkuh_mainFrame_menuTestRescan, false);
		if( iWelcomePageIdx!=wxNOT_FOUND )
		{
			m_welcomeHtml->Enable(false);
		}
		if( iTestDetailsPageIdx!=wxNOT_FOUND )
		{
			m_testDetailsHtml->Enable(false);
		}
		break;

	case muhkuh_mainFrame_state_idle:
		m_buttonCancelTest->Enable(false);
		m_menuBar->Enable(muhkuh_mainFrame_menuTestCancel, false);
		m_menuBar->Enable(muhkuh_mainFrame_menuViewWelcomePage, true);
		m_menuBar->Enable(muhkuh_mainFrame_menuViewTestDetails, true);
		m_repositoryCombo->Enable(true);
		m_treeCtrl->Enable(true);
		m_menuBar->Enable(wxID_PREFERENCES, true);
		fRepositorySelected = (m_repositoryCombo->GetSelection() != wxNOT_FOUND);
		m_menuBar->Enable(muhkuh_mainFrame_menuTestRescan, fRepositorySelected);
		if( iWelcomePageIdx!=wxNOT_FOUND )
		{
			m_welcomeHtml->Enable(true);
		}
		if( iTestDetailsPageIdx!=wxNOT_FOUND )
		{
			m_testDetailsHtml->Enable(true);
		}
		break;

	case muhkuh_mainFrame_state_testing:
		m_buttonCancelTest->Enable(true);
		m_menuBar->Enable(muhkuh_mainFrame_menuTestCancel, true);
		m_menuBar->Enable(muhkuh_mainFrame_menuViewWelcomePage, false);
		m_menuBar->Enable(muhkuh_mainFrame_menuViewTestDetails, false);
		m_repositoryCombo->Enable(false);
		m_treeCtrl->Enable(false);
		m_menuBar->Enable(wxID_PREFERENCES, false);
		m_menuBar->Enable(muhkuh_mainFrame_menuViewTestDetails, false);
		m_menuBar->Enable(muhkuh_mainFrame_menuTestRescan, false);
		if( iWelcomePageIdx!=wxNOT_FOUND )
		{
			m_welcomeHtml->Enable(false);
		}
		if( iTestDetailsPageIdx!=wxNOT_FOUND )
		{
			m_testDetailsHtml->Enable(false);
		}
		break;
	}

	// set new state
	m_state = tNewState;
}


void muhkuh_mainFrame::OnTimer(wxTimerEvent &WXUNUSED(event))
{
	wxWakeUpIdle();
}


void muhkuh_mainFrame::OnIdle(wxIdleEvent& event)
{
	wxString strStatus;
	wxString strMemStatus;
	int iRepositoryIndex;
	bool fHasMoreInput;


	switch(m_state)
	{
	case muhkuh_mainFrame_state_scanning:
		strStatus = _("Scanning test descriptions...");
		break;

	case muhkuh_mainFrame_state_idle:
		if( m_eInitState==MAINFRAME_INIT_STATE_CONFIGURED )
		{
			// NOTE: this must be the first statement in this case, or it will be executed with every idle event
			m_eInitState = MAINFRAME_INIT_STATE_SCANNED;

			// show welcome and details page
			reloadWelcomePage();
			reloadDetailsPage(NULL);

			// add all help books
			m_ptHelp->AddBook(wxFileName(wxT("muhkuh.htb")), true);

			if( m_fShowStartupTips==true && m_tipProvider!=NULL )
			{
				m_fShowStartupTips = wxShowTip(this, m_tipProvider);
			}

			iRepositoryIndex = m_ptRepositoryManager->GetActiveRepository();
			scanTests(iRepositoryIndex);
		}

		// show the number of loaded tests
		if( m_sizTestCnt==1 )
		{
			strStatus = _("1 test loaded...");
		}
		else
		{
			strStatus.Printf(_("%d tests loaded..."), m_sizTestCnt);
		}
		break;

	case muhkuh_mainFrame_state_testing:
		strStatus.Printf(_("Test '%s' in progress..."), m_strRunningTestName.c_str());

		fHasMoreInput = process_server_output();
		if( fHasMoreInput==true )
		{
			/* send this idle event again */
			event.RequestMore();
		}
		break;
	}

#if defined(USE_LUA)
	// get the Lua Memory in kilobytes
	if( m_ptLua_State!=NULL )
	{
		int iLuaMemKb;


		iLuaMemKb = lua_getgccount(m_ptLua_State);
		strMemStatus.Printf(_("Lua uses %d kilobytes"), iLuaMemKb);
		strStatus += strMemStatus;
	}
#endif

	// set the status text
	SetStatusText(strStatus, 1);

	event.Skip();
}


void muhkuh_mainFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	int iResult;
	bool fCloseApp = false;


	// is a test running? -> first ask to terminate it
	switch(m_state)
	{
	case muhkuh_mainFrame_state_scanning:
		// The progress dialog is modal, if a close request comes in now, it's something serious.
		fCloseApp = true;
		break;

	case muhkuh_mainFrame_state_testing:
		// a test is still running, ask for confirmation
		iResult = wxMessageBox(_("A test is still running. Are you sure you want to cancel it?"), m_strRunningTestName, wxYES_NO, this);
		if( iResult==wxYES )
		{
			wxLogMessage(_("Script canceled on user request!"));

			// test done -> go back to idle state
			setState(muhkuh_mainFrame_state_idle);

			finishTest();

			// close the application
			fCloseApp = true;
		}
		break;

	case muhkuh_mainFrame_state_idle:
	default:
		// ok, nothing running -> quit
		fCloseApp = true;
		break;
	}

	if( fCloseApp==true )
	{
		Close(true);
	}
}


void muhkuh_mainFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
	muhkuh_aboutDialog *pAboutDialog;


	pAboutDialog = new muhkuh_aboutDialog(this, m_strVersion, m_frameIcons);
	pAboutDialog->ShowModal();
	pAboutDialog->Destroy();
}


void muhkuh_mainFrame::OnConfigDialog(wxCommandEvent& WXUNUSED(event))
{
	muhkuh_configDialog *cfgDlg;
	muhkuh_repository_manager *ptTmpRepositoryManager;
	int iNewSelection;
	wxString strMessage;
	wxBitmap tRepoBitmap;
	wxSize tRepoBitmapSize;


	// clone the repository manager
	ptTmpRepositoryManager = new muhkuh_repository_manager(m_ptRepositoryManager);

	// show config dialog
	cfgDlg = new muhkuh_configDialog(this, m_strApplicationPath, m_strWelcomePageFile, m_strDetailsPageFile, ptTmpRepositoryManager, m_strLuaIncludePath, m_strLuaStartupCode);
	if( cfgDlg->ShowModal()==wxID_OK )
	{
		m_strLuaIncludePath = cfgDlg->GetLuaIncludePath();
		wxLogDebug(wxT("New Lua include path: ") + m_strLuaIncludePath);
		m_strLuaStartupCode = cfgDlg->GetLuaStartupCode();
		wxLogDebug(wxT("New Lua startup code: ") + m_strLuaStartupCode);

		m_strWelcomePageFile = cfgDlg->GetWelcomePageFile();
		wxLogDebug(wxT("New Welcome Page File: ") + m_strWelcomePageFile);
		m_strDetailsPageFile = cfgDlg->GetDetailsPageFile();
		wxLogDebug(wxT("New Details Page File: ") + m_strDetailsPageFile);

		write_config();

		// copy tmp repository manager over current one
		delete m_ptRepositoryManager;
		m_ptRepositoryManager = ptTmpRepositoryManager;

		reloadWelcomePage();
		reloadDetailsPage(NULL);

		// show all repositories in the combo box
		updateRepositoryCombo();

		// get new test list
		iNewSelection = m_ptRepositoryManager->GetActiveRepository();
		m_repositoryCombo->Select(iNewSelection);
		scanTests(iNewSelection);
	}
	else
	{
		// changes canceled -> delete the temp managers
		delete ptTmpRepositoryManager;
	}
	// delete config dialog
	cfgDlg->Destroy();
}


void muhkuh_mainFrame::OnHelp(wxCommandEvent& WXUNUSED(event))
{
	wxLogMessage(wxT("OnHelp"));

	m_ptHelp->DisplayContents();
}


void muhkuh_mainFrame::OnTestExecute(wxCommandEvent& WXUNUSED(event))
{
	wxTreeItemId tItemId;
	const testTreeItemData *ptItemData;
	muhkuh_wrap_xml *ptWrapXml;


	// get the selected item from the tree
	tItemId = m_treeCtrl->GetSelection();
	if ( tItemId.IsOk()==true )
	{
		ptItemData = (const testTreeItemData*)m_treeCtrl->GetItemData(tItemId);
		if( ptItemData!=NULL )
		{
			ptWrapXml = ptItemData->getXmlDescription();
			if( ptWrapXml!=NULL )
			{
				// execute the main test
				executeTest(ptWrapXml, 0);
			}
		}
	}
}


void muhkuh_mainFrame::executeTest(muhkuh_wrap_xml *ptTestData, unsigned int uiIndex)
{
	bool fResult;
	wxString strMsg;
	wxString strStartupCode;
	wxString strServerCmd;
	wxString strNow;
	wxFile tFile;


	m_strRunningTestName = ptTestData->testDescription_getName();
	m_sizRunningTest_RepositoryIdx = ptTestData->getRepositoryIndex();
	m_sizRunningTest_TestIdx = ptTestData->getTestIndex();

	wxLogMessage(_("execute test '%s', index %d"), m_strRunningTestName.c_str(), uiIndex);


	// create the startup code
	strStartupCode.Append(wxT("-- execute this test\n"));
	strStartupCode.Append(wxT("_G.__MUHKUH_TEST_XML = '"));
	strStartupCode.Append(m_ptRepositoryManager->getTestlistXmlUrl(m_sizRunningTest_RepositoryIdx, m_sizRunningTest_TestIdx));
	strStartupCode.Append(wxT("'\n\n"));

	strStartupCode.Append(wxT("-- include path\n"));
	strStartupCode.Append(wxT("package['path'] = '"));
	strStartupCode.Append(m_strLuaIncludePath);
	strStartupCode.Append(wxT("'\n\n"));

	strStartupCode.Append(m_strLuaStartupCode);


	// create the temp file with the lua init commands
	m_strRunningTestTempFileName = wxFileName::CreateTempFileName(wxT("muhkuh_lua"));
	fResult = tFile.Create(m_strRunningTestTempFileName.c_str(), true);
	if( fResult!=true )
	{
		strMsg.Printf(_("Failed to create temporary file '%s'!"), m_strRunningTestTempFileName.c_str());
		wxMessageBox(strMsg, _("Server startup error"), wxICON_ERROR, this);
	}
	else
	{
		// write the startup string to the temp file
		tFile.Write(strStartupCode);
		tFile.Close();

		// set state to 'testing'
		// NOTE: this must be done before the call to 'RunString', or the state will not change before the first idle event
		setState(muhkuh_mainFrame_state_testing);

		strServerCmd.Printf(wxT("lua %s"), m_strRunningTestTempFileName.c_str());
		wxLogMessage(wxT("starting server: ") + strServerCmd);

		m_lServerPid = wxExecute(strServerCmd, wxEXEC_ASYNC|wxEXEC_MAKE_GROUP_LEADER, m_ptServerProcess);
		if( m_lServerPid==0 )
		{
			strMsg.Printf(_("Failed to start the server with command: %s"), strServerCmd.c_str());
			wxMessageBox(strMsg, _("Server startup error"), wxICON_ERROR, this);
		}
		else
		{
			strNow = wxDateTime::Now().Format(wxT("%F %T"));
			// create start message for the report tab
			strMsg.Printf(_("%s: started test '%s'.\n"), strNow.c_str(), m_strRunningTestName.c_str());
			// create a new notebook tab
			m_textTestOutput = new wxTextCtrl(this, wxID_ANY, strMsg, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxSUNKEN_BORDER | wxTE_READONLY);
			strMsg = m_strRunningTestName + wxT(" - ") + strNow;
			m_notebook->AddPage(m_textTestOutput, strMsg, true, m_frameIcons.GetIcon(16));

			m_textTestOutput->AppendText(wxT("startup code:\n"));
			m_textTestOutput->AppendText(wxT("-----------------------------------------------------------------------------\n"));
			m_textTestOutput->AppendText(strStartupCode);
			m_textTestOutput->AppendText(wxT("-----------------------------------------------------------------------------\n"));

			// start the timer to poll the server for input
			m_timerIdleWakeUp.Start(100);
		}
	}
}


bool muhkuh_mainFrame::process_server_output(void)
{
	bool fProcessedInput;
	wxInputStream *ptIs;
	wxString strNow;
	wxString strMsg;


	fProcessedInput = false;

	if( m_ptServerProcess!=NULL && m_textTestOutput!=NULL )
	{
		strNow = wxDateTime::Now().Format(wxT("%F %T"));

		if( m_ptServerProcess->IsInputAvailable()==true )
		{
			ptIs = m_ptServerProcess->GetInputStream();
			wxTextInputStream tis(*ptIs);
			m_textTestOutput->AppendText(strNow);
			m_textTestOutput->AppendText(wxT(":MSG:"));
			m_textTestOutput->AppendText(tis.ReadLine());
			m_textTestOutput->AppendText(wxT("\n"));
			fProcessedInput = true;
		}

		if( m_ptServerProcess->IsErrorAvailable()==true )
		{
			ptIs = m_ptServerProcess->GetErrorStream();
			wxTextInputStream tis(*ptIs);
			m_textTestOutput->AppendText(strNow);
			m_textTestOutput->AppendText(wxT(":ERR:"));
			m_textTestOutput->AppendText(tis.ReadLine());
			m_textTestOutput->AppendText(wxT("\n"));
			fProcessedInput = true;
		}
	}

	return fProcessedInput;
}


void muhkuh_mainFrame::finishTest(void)
{
	bool fResult;


	wxLogMessage(_("Test '%s' finished, cleaning up..."), m_strRunningTestName.c_str());

	m_timerIdleWakeUp.Stop();

	/* show all server output */
	do
	{
		fResult = process_server_output();
	} while( fResult==true );

	if( m_textTestOutput!=NULL )
	{
		m_textTestOutput->AppendText(wxDateTime::Now().Format(wxT("%F %T")).c_str());
		m_textTestOutput->AppendText(wxT("Test '"));
		m_textTestOutput->AppendText(m_strRunningTestName);
		m_textTestOutput->AppendText(wxT("' finished.\n"));
	}

	/* forget current server output tab */
	m_textTestOutput = NULL;

	// remove the temp file
	if( m_strRunningTestTempFileName.IsEmpty()==false && wxFileExists(m_strRunningTestTempFileName)==true )
	{
		fResult = wxRemoveFile(m_strRunningTestTempFileName);
		if( fResult!=true )
		{
			wxLogWarning(_("Failed to remove the temp file '%s'."), m_strRunningTestTempFileName.c_str());
		}
	}
}


void muhkuh_mainFrame::updateRepositoryCombo(void)
{
	muhkuh_repository::REPOSITORY_TYP_E eTyp;
	const char **ppcXpm;
	wxString strReposEntry;
	wxBitmap tRepoBitmap;
	size_t sizCnt, sizMax;


	m_repositoryCombo->Clear();
	sizMax = m_ptRepositoryManager->GetRepositoryCount();
	for(sizCnt=0; sizCnt<sizMax; ++sizCnt)
	{
		// get string representation of the new entry
		strReposEntry = m_ptRepositoryManager->GetStringRepresentation(sizCnt);
		// get bitmap for the entry
		eTyp = m_ptRepositoryManager->GetTyp(sizCnt);
		switch( eTyp )
		{
		case muhkuh_repository::REPOSITORY_TYP_DIRSCAN:
			ppcXpm = icon_famfamfam_silk_folder_table;
			break;
		case muhkuh_repository::REPOSITORY_TYP_FILELIST:
			ppcXpm = icon_famfamfam_silk_database;
			break;
		case muhkuh_repository::REPOSITORY_TYP_SINGLEXML:
			ppcXpm = icon_famfamfam_silk_script;
			break;
		case muhkuh_repository::REPOSITORY_TYP_ALLLOCAL:
			ppcXpm = icon_famfamfam_silk_folder_database;
			break;
		default:
			ppcXpm = icon_famfamfam_silk_exclamation;
			break;
		}
		tRepoBitmap = wxBitmap(ppcXpm);
		// add to combo box
		m_repositoryCombo->Append(strReposEntry, tRepoBitmap);
	}
}


void muhkuh_mainFrame::OnTestHelp(wxCommandEvent& WXUNUSED(event))
{
	m_ptHelp->DisplayContents();
}


void muhkuh_mainFrame::OnTestRescan(wxCommandEvent& WXUNUSED(event))
{
	int iRepositoryIdx;


	if( m_ptRepositoryManager!=NULL )
	{
		iRepositoryIdx = m_ptRepositoryManager->GetActiveRepository();
		scanTests(iRepositoryIdx);
	}
}


void muhkuh_mainFrame::OnViewRepositoryPane(wxCommandEvent &event)
{
	m_auiMgr.GetPane(m_repositoryCombo).Show(event.IsChecked());
	m_auiMgr.Update();
}


void muhkuh_mainFrame::OnViewPanicButton(wxCommandEvent &event)
{
	m_auiMgr.GetPane(m_buttonCancelTest).Show(event.IsChecked());
	m_auiMgr.Update();
}


void muhkuh_mainFrame::OnViewTestTree(wxCommandEvent &event)
{
	m_auiMgr.GetPane(m_treeCtrl).Show(event.IsChecked());
	m_auiMgr.Update();
}


void muhkuh_mainFrame::OnViewMessageLog(wxCommandEvent &event)
{
	m_auiMgr.GetPane(m_textCtrl).Show(event.IsChecked());
	m_auiMgr.Update();
}


void muhkuh_mainFrame::OnViewWelcomePage(wxCommandEvent &event)
{
	int iPageIdx;


	if( event.IsChecked()==true )
	{
		// show page
		if( m_welcomeHtml==NULL )
		{
			// page does not exist yet -> create and add
			createWelcomeWindow();
		}
	}
	else
	{
		// hide welcome page
		iPageIdx = m_notebook->GetPageIndex(m_welcomeHtml);
		if( iPageIdx!=wxNOT_FOUND )
		{
			m_notebook->DeletePage(iPageIdx);
			m_welcomeHtml = NULL;
		}
	}
}


void muhkuh_mainFrame::OnViewTestDetails(wxCommandEvent &event)
{
	int iPageIdx;


	if( event.IsChecked()==true )
	{
		// show page
		if( m_testDetailsHtml==NULL )
		{
			// page does not exist yet -> create and add
			createTestDetailsWindow();
		}
	}
	else
	{
		// hide welcome page
		iPageIdx = m_notebook->GetPageIndex(m_testDetailsHtml);
		if( iPageIdx!=wxNOT_FOUND )
		{
			m_notebook->DeletePage(iPageIdx);
			m_testDetailsHtml = NULL;
		}
	}
}


void muhkuh_mainFrame::OnShowTip(wxCommandEvent &event)
{
	if( m_tipProvider!=NULL )
	{
		m_fShowStartupTips = wxShowTip(this, m_tipProvider);
	}
}


void muhkuh_mainFrame::OnRestoreDefaultPerspective(wxCommandEvent &event)
{
	int iPageIdx;


	// restore the default perspective
	m_auiMgr.LoadPerspective(strDefaultPerspective, true);

	// create welcome notebook
	if( m_welcomeHtml!=NULL )
	{
		iPageIdx = m_notebook->GetPageIndex(m_welcomeHtml);
	}
	else
	{
		iPageIdx = wxNOT_FOUND;
	}
	if( iPageIdx==wxNOT_FOUND )
	{
		createWelcomeWindow();
		m_menuBar->Check(muhkuh_mainFrame_menuViewWelcomePage, true);
	}

	// create test details notebook
	if( m_testDetailsHtml!=NULL )
	{
		iPageIdx = m_notebook->GetPageIndex(m_testDetailsHtml);
	}
	else
	{
		iPageIdx = wxNOT_FOUND;
	}
	if( iPageIdx==wxNOT_FOUND )
	{
		createTestDetailsWindow();
		m_menuBar->Check(muhkuh_mainFrame_menuViewTestDetails, true);
	}
}


void muhkuh_mainFrame::OnTestCancel(wxCommandEvent& WXUNUSED(event))
{
	int iResult;


	// is a test running?
	if( m_lServerPid!=0 )
	{
		iResult = wxMessageBox(_("Are you sure you want to cancel this test?"), m_strRunningTestName, wxYES_NO, this);
		if( iResult==wxYES )
		{
			wxLogMessage(_("Script canceled on user request!"));

			// try to kill the process
			wxProcess::Kill(m_lServerPid, wxSIGKILL, wxKILL_CHILDREN);
			m_lServerPid = 0;

			finishTest();

			// test done -> go back to idle state
			setState(muhkuh_mainFrame_state_idle);
		}
	}
}


void muhkuh_mainFrame::OnRepositoryCombo(wxCommandEvent &event)
{
	int iActiveRepoIdx;


	// get new selection
	iActiveRepoIdx = event.GetSelection();

	// set new repository
	scanTests(iActiveRepoIdx);
}


void muhkuh_mainFrame::OnTestTreeContextMenu(wxTreeEvent &event)
{
	wxTreeItemId itemId;
	wxPoint pt;
	wxMenu *ptMenu;
	wxString strTitle;


	// get item data structure
	itemId = event.GetItem();
	pt = event.GetPoint();
	if( itemId.IsOk() )
	{
		strTitle.Printf(_("Menu for %s"), m_treeCtrl->GetItemText(itemId).c_str());
		ptMenu = new wxMenu(strTitle);
		ptMenu->Append(muhkuh_mainFrame_TestTree_Execute,	_("&Execute..."));
		ptMenu->Append(muhkuh_mainFrame_TestTree_ShowHelp,	_("&Show Help..."));
		PopupMenu(ptMenu, pt);
		delete ptMenu;
	}
}


void muhkuh_mainFrame::OnTestTreeItemActivated(wxTreeEvent &event)
{
	wxTreeItemId itemId;


	// get item data structure
	itemId = event.GetItem();
	if( itemId.IsOk() )
	{
		wxCommandEvent exec_event(wxEVT_COMMAND_MENU_SELECTED, muhkuh_mainFrame_TestTree_Execute);
		wxPostEvent( this, exec_event );
	}
}


void muhkuh_mainFrame::OnTestTreeItemSelected(wxTreeEvent &event)
{
	wxTreeItemId itemId;
	const testTreeItemData *ptItemData;
	muhkuh_wrap_xml *ptWrapXml;
	wxListItem listItem;


	// get item data structure
	ptWrapXml = NULL;
	itemId = event.GetItem();
	if( itemId.IsOk() )
	{
		ptItemData = (const testTreeItemData*)m_treeCtrl->GetItemData(itemId);
		if( ptItemData!=NULL )
		{
			ptWrapXml = ptItemData->getXmlDescription();
		}
	}

	reloadDetailsPage(ptWrapXml);
}


void muhkuh_mainFrame::scanTests(int iActiveRepositoryIdx)
{
	bool bRes;
	size_t sizTestCnt;


	// clear all old tests
	m_sizTestCnt = 0;
	// clear the test details
	reloadDetailsPage(NULL);
	// clear the complete tree
	m_treeCtrl->DeleteAllItems();
	// add a root item
	m_treeCtrl->AddRoot(wxT("Root"));

	if( iActiveRepositoryIdx!=wxNOT_FOUND )
	{
		// set new state
		setState(muhkuh_mainFrame_state_scanning);

		// show process dialog
		m_scannerProgress = new wxProgressDialog(
			_("Scanning available test descriptions"),
			_("Please wait..."),
			1,
			this,
			wxPD_CAN_ABORT | wxPD_APP_MODAL | wxPD_ELAPSED_TIME | wxPD_ESTIMATED_TIME | wxPD_REMAINING_TIME
		);

		// create the testlist
		bRes = m_ptRepositoryManager->createTestlist(iActiveRepositoryIdx, muhkuh_mainFrame::repositoryScannerCallback, m_scannerProgress);

		// destroy the progress dialog, it's not possible to update the max count
		m_scannerProgress->Destroy();

		if( bRes==true )
		{
			// vaild directory or repository -> accept the index
			m_ptRepositoryManager->SetActiveRepository(iActiveRepositoryIdx);

			// create a new progress dialog with the correct max count
			sizTestCnt = m_ptRepositoryManager->getTestlistCount(iActiveRepositoryIdx);
			if( sizTestCnt>0 )
			{
				// show process dialog
				m_scannerProgress = new wxProgressDialog(
					_("Scanning available test descriptions"),
					_("Please wait..."),
					sizTestCnt,
					this,
					wxPD_CAN_ABORT | wxPD_APP_MODAL | wxPD_ELAPSED_TIME | wxPD_ESTIMATED_TIME | wxPD_REMAINING_TIME
				);

				addAllTests(iActiveRepositoryIdx);

				// destroy the scanner progress dialog
				m_scannerProgress->Destroy();
			}
		}
		else
		{
			// failed to create test list -> this repository is not valid
			m_repositoryCombo->Select(wxNOT_FOUND);
			m_ptRepositoryManager->SetActiveRepository(wxNOT_FOUND);
		}
	}

	setState(muhkuh_mainFrame_state_idle);
}


void muhkuh_mainFrame::addAllTests(int iActiveRepositoryIdx)
{
	size_t sizMax, sizCnt;
	wxString strMessage;
	bool fResult;
	bool fScannerIsRunning;


	// assume that cancel was not pressed yet
	fScannerIsRunning = true;

	// loop over all tests
	sizMax = m_ptRepositoryManager->getTestlistCount(iActiveRepositoryIdx);
	sizCnt = 0;
	while( sizCnt<sizMax && fScannerIsRunning==true )
	{
		// check for cancel button
		strMessage.Printf(_("scanning '%s'..."), m_ptRepositoryManager->getTestlistPrintUrl(iActiveRepositoryIdx, sizCnt).c_str());
		fScannerIsRunning = m_scannerProgress->Update(sizCnt, strMessage, NULL);

		fResult = scanFileXml(iActiveRepositoryIdx, sizCnt);

		// next item
		++sizCnt;

		if( fResult==false )
		{
			continue;
		}

		// one more test loaded
		++m_sizTestCnt;
	}
}


bool muhkuh_mainFrame::cutPath(wxString &strPath, wxArrayString *ptElems)
{
	size_t sizCnt, sizLen, sizPos;
	bool fResult;
	wxURI tUri;


	// cut the name into the path elements
	sizCnt = 0;
	sizLen = strPath.Length();
	// does the string start with a slash?
	if( strPath[0]==wxT('/') )
	{
		// yes -> ignore this slash
		++sizCnt;
	}
	sizPos = sizCnt;
	// loop over all path elements
	while( sizCnt<sizLen )
	{
		// get the next path element
		if( strPath[sizCnt]==wxT('/') )
		{
			if( sizPos<sizCnt )
			{
				// get the path element
				ptElems->Add(tUri.Unescape(strPath.Mid(sizPos, sizCnt-sizPos)));
			}
			// get start position for next path element
			sizPos = sizCnt+1;
		}
		// next char
		++sizCnt;
	}
	// is one element left?
	if( sizPos>=sizLen )
	{
		fResult = false;
	}
	else
	{
		// add the file element
		ptElems->Add(tUri.Unescape(strPath.Mid(sizPos, sizLen-sizPos)));
		fResult = true;
	}

	return fResult;
}


bool muhkuh_mainFrame::addTestTree(testTreeItemData *ptTestTreeItem)
{
	wxString strTestPath;
	wxString strPathElement;
	size_t sizCnt, sizLen;
	wxTreeItemId itemId;
	wxTreeItemId searchId;
	wxTreeItemId prevId;
	wxTreeItemIdValue searchCookie;
	int iCmp;
	wxArrayString aPathElems;
	bool fIsFirst;
	wxString strMsg;
	bool fResult;
	testTreeItemData *ptOldTreeItem;


	// get the full test name
	strTestPath = ptTestTreeItem->getTestLabel();

	fResult = cutPath(strTestPath, &aPathElems);
	if( fResult==false )
	{
		// path not ok
		strMsg.Printf(_("The testname '%s' is no valid path to a test."), strTestPath.c_str());
		wxMessageBox(strMsg, _("Failed to add test"), wxICON_ERROR, this);
	}
	else
	{
		// now create the complete path in the tree

		// start at the root item
		itemId = m_treeCtrl->GetRootItem();

		// loop over all elements except the last one in the path
		sizCnt = 0;
		sizLen = aPathElems.Count() - 1;
		while( fResult==true && sizCnt<sizLen )
		{
			// get the path element
			strPathElement = aPathElems.Item(sizCnt);

			// look for this element in the test tree
			searchId = m_treeCtrl->GetFirstChild(itemId, searchCookie);
			fIsFirst = true;
			do
			{
				if( searchId.IsOk()==false )
				{
					// no items in this branch, just add the new item
					searchId = m_treeCtrl->AppendItem(itemId, strPathElement, -1, -1, NULL);
					break;
				}
				else
				{
					iCmp = m_treeCtrl->GetItemText(searchId).CmpNoCase(strPathElement);
					if( iCmp==0 )
					{
						// a child with the requested name already exists
						// is this a folder or a test?
						ptOldTreeItem = (testTreeItemData*)m_treeCtrl->GetItemData(searchId);
						if( ptOldTreeItem!=NULL )
						{
							strMsg.Printf(_("The path '%s' conflicts with a test in this path!"), strTestPath.c_str());
							wxMessageBox(strMsg, _("Failed to add test"), wxICON_ERROR, this);
							fResult = false;
						}
					}
					else if( iCmp<0 )
					{
						// remember previous entry for the insert operation
						prevId = searchId;
						// move to next child
						searchId = m_treeCtrl->GetNextChild(itemId, searchCookie);
						// switch from append to insert operation
						fIsFirst = false;
					}
					else
					{
						// create a new child if the item can't appear anymore because of the sort order
						if( fIsFirst==true )
						{
							// append the new item before the fist one in this branch
							searchId = m_treeCtrl->PrependItem(itemId, strPathElement, -1, -1, NULL);
						}
						else
						{
							// insert the new item before the current search position
							searchId = m_treeCtrl->InsertItem(itemId, prevId, strPathElement, -1, -1, NULL);
						}
						// the child with the requested name was just created
						break;
					}
				}
			} while( iCmp!=0 );

			// the child is the new parent node
			itemId = searchId;

			// next path element
			++sizCnt;
		}

		if( fResult==true )
		{
			// ok, path created, now append the test item
			strPathElement = aPathElems.Last();
			// look for this element in the test tree
			fIsFirst = true;
			searchId = m_treeCtrl->GetFirstChild(itemId, searchCookie);
			do
			{
				// reached end of list?
				if( searchId.IsOk()==false )
				{
					// end of list reached, just add the new item
					searchId = m_treeCtrl->AppendItem(itemId, strPathElement, -1, -1, ptTestTreeItem);
					// set iCmp to something != 0
					iCmp = 1;
					break;
				}
				else
				{
					iCmp = m_treeCtrl->GetItemText(searchId).CmpNoCase(strPathElement);
					if( iCmp<0 )
					{
						// remember previous entry for the insert operation
						prevId = searchId;
						// move to next child
						searchId = m_treeCtrl->GetNextChild(itemId, searchCookie);
						// switch from append to insert operation
						fIsFirst = false;
					}
					else if( iCmp>0 )
					{
						// create a new child if the item can't appear anymore because of the sort order
						if( fIsFirst==true )
						{
							// append the new item before the fist one in this branch
							searchId = m_treeCtrl->PrependItem(itemId, strPathElement, -1, -1, ptTestTreeItem);
						}
						else
						{
							// insert the new item before the current search position
							searchId = m_treeCtrl->InsertItem(itemId, prevId, strPathElement, -1, -1, ptTestTreeItem);
						}
						// the child with the requested name was just created
						break;
					}
				}
			} while( iCmp!=0 );
	
			// check for error (element already exists)
			if( iCmp==0 )
			{
				strMsg.Printf(_("The test '%s' already exists, skipping new instance!"), strTestPath.c_str());
				wxMessageBox(strMsg, _("Failed to add test"), wxICON_ERROR, this);
				fResult = false;
			}
		}
	}

	return fResult;
}


bool muhkuh_mainFrame::scanFileXml(size_t sizRepositoryIdx, size_t sizTestIdx)
{
	wxString strXmlUrl;
	wxFileSystem fileSystem;
	wxFSFile *ptFsFile;
	wxInputStream *ptInputStream;
	wxString strXmlData;
	muhkuh_wrap_xml *ptWrapXml;
	testTreeItemData *ptItemData;
	wxString strTestName;
	wxString strHelpFile;
	wxString strMsg;
	bool fResult;


	fResult = false;

	// get the url for the xml file
	strXmlUrl = m_ptRepositoryManager->getTestlistXmlUrl(sizRepositoryIdx, sizTestIdx);

	// test if file exists
	wxLogVerbose(_("Reading testdescription '%s'"), strXmlUrl.c_str());
	ptFsFile = fileSystem.OpenFile(strXmlUrl);
	if( ptFsFile==NULL )
	{
		wxLogError(_("could not read xml file '%s'"), strXmlUrl.c_str());
	}
	else
	{
		// ok, file exists. read all data into a string
		ptInputStream = ptFsFile->GetStream();
		// stream read, construct xml tree
		ptWrapXml = new muhkuh_wrap_xml();
		if( ptWrapXml!=NULL )
		{
			if( ptWrapXml->initialize(ptInputStream, sizRepositoryIdx, sizTestIdx)==false )
			{
				wxLogError(_("Failed to read the xml file for testdescription '%s'"), strXmlUrl.c_str());
			}
			else
			{
				// get the main test name
				strTestName = ptWrapXml->testDescription_getName();
				// TODO: get the help file name
				strHelpFile.Empty();
				// create the tree item object
				ptItemData = new testTreeItemData(strTestName, strHelpFile, ptWrapXml);
				if( addTestTree(ptItemData)!=true )
				{
					delete ptItemData;
				}
				else
				{
					fResult = true;
				}
			}
		}
		delete ptFsFile;
	}

	return fResult;
}


void muhkuh_mainFrame::OnNotebookPageClose(wxAuiNotebookEvent &event)
{
	int iSelection;
	wxWindow *ptWin;


	// get the selected Page
	iSelection = event.GetSelection();
	ptWin = m_notebook->GetPage(iSelection);

	// close the welcome page?
	if( ptWin==m_welcomeHtml )
	{
		m_menuBar->Check(muhkuh_mainFrame_menuViewWelcomePage, false);
		// forget the pointer
		m_welcomeHtml = NULL;
	}
	// close the test details page?
	else if( ptWin==m_testDetailsHtml )
	{
		m_menuBar->Check(muhkuh_mainFrame_menuViewTestDetails, false);
		// forget the pointer
		m_testDetailsHtml = NULL;
	}
}


void muhkuh_mainFrame::OnPaneClose(wxAuiManagerEvent &event)
{
	wxWindow *ptWindow;


	ptWindow = event.pane->window;
	if( ptWindow==m_repositoryCombo )
	{
		m_menuBar->Check(muhkuh_mainFrame_menuViewRepositoryPane, false);
	}
	else if( ptWindow==m_buttonCancelTest )
	{
		m_menuBar->Check(muhkuh_mainFrame_menuViewPanicButton, false);
	}
	else if( ptWindow==m_treeCtrl )
	{
		m_menuBar->Check(muhkuh_mainFrame_menuViewTestTree, false);
	}
	else if( ptWindow==m_textCtrl )
	{
		m_menuBar->Check(muhkuh_mainFrame_menuViewMessageLog, false);
	}
}


void muhkuh_mainFrame::OnServerProcessTerminate(wxProcessEvent &event)
{
	int iPid;


	iPid = event.GetPid();
	if( iPid==m_lServerPid )
	{
		wxLogMessage(_("The server terminated with returncode %d."), event.GetExitCode());
		m_lServerPid = 0;
		setState(muhkuh_mainFrame_state_idle);
		finishTest();
	}
	else
	{
		wxLogWarning(_("Ignoring terminate event from unknown process %d!"), iPid);
	}
}


void muhkuh_mainFrame::OnMtdLinkClicked(wxHtmlLinkEvent &event)
{
	wxString strHref;
	wxURI tUri;
	wxString strElem;
	wxString strPath;
	wxArrayString aPathElems;
	size_t sizCnt, sizLen;
	wxTreeItemId parentId;
	wxTreeItemId searchId;
	wxTreeItemIdValue searchCookie;
	bool fResult;
	int iCmp;
	const testTreeItemData *ptItemData;
	muhkuh_wrap_xml *ptWrapXml;
	unsigned int uiCnt, uiMax;
	wxString strFragment;


	strHref = event.GetLinkInfo().GetHref();
	tUri.Create(strHref);
	if( tUri.HasScheme()==true && tUri.GetScheme().Cmp(wxT("mtd"))==0 )
	{
		// a mtd link must at least have a server or a path
		if( tUri.HasServer()==false && tUri.HasPath()==false )
		{
			// no server and no path -> no good mtd link
			wxLogError(_("mtd link has no valid path: '%s'"), tUri.Unescape(strHref).c_str());
		}
		else
		{
			if( tUri.HasServer()==true )
			{
				strPath = tUri.GetServer();
			}
			if( tUri.HasPath()==true )
			{
				// separate server and path with '/'
				strPath += wxT("/");
				strElem = tUri.GetPath();
				// cut off leading '/'
				if( strElem[0]==wxT('/') )
				{
					// cut off the first char
					strElem = strElem.Mid(1);
				}
				strPath += strElem;
			}

			// cut the path into elements
			fResult = cutPath(strPath, &aPathElems);
			if( fResult==false )
			{
				// path not ok
				wxLogError(wxT("The testname '%s' is no valid path to a test."), tUri.Unescape(strPath).c_str());
			}
			else
			{
				// path is ok, now search the tree for the test
				sizCnt = 0;
				sizLen = aPathElems.Count();

				// start at the root item
				parentId = m_treeCtrl->GetRootItem();

				iCmp = 1;
				while( sizCnt<sizLen )
				{
					// look for this element in the test tree
					searchId = m_treeCtrl->GetFirstChild(parentId, searchCookie);
					if( searchId.IsOk()==false )
					{
						// no items in this branch
						iCmp = 1;
						break;
					}
					else
					{
						do
						{
							iCmp = m_treeCtrl->GetItemText(searchId).Cmp(aPathElems.Item(sizCnt));
							if( iCmp==0 )
							{
								// found the element
								parentId = searchId;
								break;
							}
							else
							{
								// move to next child
								searchId = m_treeCtrl->GetNextChild(parentId, searchCookie);
							}
						} while( searchId.IsOk()==true );
					}

					// next path element
					++sizCnt;
				}

				// found all elements?
				if( iCmp!=0 || searchId.IsOk()!=true )
				{
					// no
					wxLogError(wxT("The test '%s' could not be found."), tUri.Unescape(strPath).c_str());
				}
				else
				{
					// yes, found all elements -> searchId is the test node!
					ptItemData = (const testTreeItemData*)m_treeCtrl->GetItemData(searchId);
					if( ptItemData!=NULL )
					{
						ptWrapXml = ptItemData->getXmlDescription();
						if( ptWrapXml!=NULL )
						{
							uiCnt = 0;
							if( tUri.HasFragment()==true )
							{
								strFragment = tUri.Unescape(tUri.GetFragment());
								uiMax = ptWrapXml->testDescription_getTestCnt();
								iCmp = 1;
								while( uiCnt<uiMax )
								{
									fResult = ptWrapXml->testDescription_setTest(uiCnt);
									if( fResult!=true )
									{
										wxLogError(_("failed to query subtest '%s' in test '%s'"), strFragment.c_str(), strPath.c_str());
										break;
									}
									else
									{
										++uiCnt;
										iCmp = strFragment.Cmp(ptWrapXml->test_getName());
										if( iCmp==0 )
										{
											// found subtest
											break;
										}
									}
								}
								// found test
								if( iCmp!=0 )
								{
									wxLogError(_("test '%s' has no subtest '%s'"), strPath.c_str(), strFragment.c_str());
								}
							}
							else
							{
								iCmp = 0;
							}

							if( iCmp==0 )
							{
								// execute the main test
								wxLogMessage(_("executing test!"));
								executeTest(ptWrapXml, uiCnt);
							}
						}
					}
				}
			}
		}
	}
	else
	{
		event.Skip();
	}
}


void muhkuh_mainFrame::OnMove(wxMoveEvent &event)
{
	// is the frame in normal state?
	if( IsIconized()==false && IsFullScreen()==false && IsMaximized()==false )
	{
		// frame is in normal state -> remember the position
		m_framePosition = event.GetPosition();
	}
}


void muhkuh_mainFrame::OnSize(wxSizeEvent &event)
{
	// is the frame in normal state?
	if( IsIconized()==false && IsFullScreen()==false && IsMaximized()==false )
	{
		// frame is in normal state -> remember size
		m_frameSize = event.GetSize();
	}
}


#if defined(USE_LUA)
wxString muhkuh_mainFrame::htmlTag_lua(const wxString &strLuaCode)
{
	muhkuh_mainFrame *ptMainframe;
	wxString strText;


	ptMainframe = ::wxGetApp().ptMainframe;
	if( ptMainframe!=NULL )
	{
		strText = ptMainframe->local_htmlTag_lua(strLuaCode);
	}
	else
	{
		strText = _("Lua Scripting not supported!");
	}

	return strText;
}


const muhkuh_mainFrame::LUA_ERROR_TO_STR_T muhkuh_mainFrame::atLuaErrorToString[] =
{
	{ 0,			wxT("") },
	{ LUA_YIELD,		wxT("Thread is suspended") },
	{ LUA_ERRRUN,		wxT("Error while running chunk") },
	{ LUA_ERRSYNTAX,	wxT("Syntax error during pre-compilation") },
	{ LUA_ERRMEM,		wxT("Memory allocation error") },
	{ LUA_ERRERR,		wxT("Generic error or an error occurred while running the error handler") },
	{ LUA_ERRFILE,		wxT("Error occurred while opening file") }
};

wxString muhkuh_mainFrame::lua_error_to_string(int iLuaError)
{
	wxString strMessage;


	if( iLuaError<(sizeof(atLuaErrorToString)/sizeof(atLuaErrorToString[0])) )
	{
		strMessage = atLuaErrorToString[iLuaError].pcMessage;
	}
	else
	{
		strMessage.Printf(_("Unknown lua error code: %d"), iLuaError);
	}

	return strMessage;
}


const muhkuh_mainFrame::LUA_TYPE_TO_STR_T muhkuh_mainFrame::atLuaTypeToString[] =
{
	{ LUA_TNIL,		wxT("nil") },
	{ LUA_TBOOLEAN,		wxT("boolean") },
	{ LUA_TLIGHTUSERDATA,	wxT("light userdata") },
	{ LUA_TNUMBER,		wxT("number") },
	{ LUA_TSTRING,		wxT("string") },
	{ LUA_TTABLE,		wxT("table") },
	{ LUA_TFUNCTION,	wxT("function") },
	{ LUA_TUSERDATA,	wxT("userdata") },
	{ LUA_TTHREAD,		wxT("thread") }
};

wxString muhkuh_mainFrame::lua_type_to_string(int iLuaType)
{
	wxString strType;


	if( iLuaType<(sizeof(atLuaTypeToString)/sizeof(atLuaTypeToString[0])) )
	{
		strType = atLuaTypeToString[iLuaType].pcMessage;
	}
	else
	{
		strType.Printf(_("Unknown lua type: %d"), iLuaType);
	}

	return strType;
}


bool muhkuh_mainFrame::lua_get_errorinfo(lua_State *L, int iStatus, int iTop, wxString *pstrErrorMsg, int *piLineNum)
{
	int iNewTop;
	wxString strErrorMsg;


	if( iStatus!=0 )
	{
		iNewTop = lua_gettop(L);

		strErrorMsg = lua_error_to_string(iStatus);

		switch(iStatus)
		{
		case LUA_ERRMEM:
		case LUA_ERRERR:
			if( iNewTop>iTop )
			{
				strErrorMsg += wxT("\n");
			}
			break;

		case LUA_ERRRUN:
		case LUA_ERRFILE:
		case LUA_ERRSYNTAX:
		default:
			if( iNewTop>iTop )
			{
				strErrorMsg += wxT("\n") + wxString::FromAscii(lua_tostring(L, -1));
			}
			break;
		}
	}

	strErrorMsg += wxT("\n");

	// Why can't I fill a lua_Debug here? Try to get the line number
	// by parsing the error message that looks like this, 3 is linenumber
	// [string "a = 1("]:3: unexpected symbol near `<eof>'
	wxString strLine = strErrorMsg;
	long lLineNum = -1;
	while( strLine.IsEmpty()==false )
	{
		// search through the str to find ']:LONG:' pattern
		strLine = strLine.AfterFirst(wxT(']'));
		if ((strLine.Length() > 0) && (strLine.GetChar(0) == wxT(':')))
		{
			strLine = strLine.AfterFirst(wxT(':'));
			if (strLine.IsEmpty() || strLine.BeforeFirst(wxT(':')).ToLong(&lLineNum))
			{
				break;
			}
		}
	}

	/* pops the message if any */
	lua_settop(L, iTop);

	if(pstrErrorMsg!=NULL)
	{
		*pstrErrorMsg = strErrorMsg;
	}
	if(piLineNum!=NULL)
	{
		*piLineNum = (int)lLineNum;
	}

	return true;
}


wxString muhkuh_mainFrame::local_htmlTag_lua(const wxString &strLuaCode)
{
	int iTopPre;
	int iTopPost;
	int iResult;
	int iLineNr;
	int iResultType;
	wxString strHtmlCode;
	wxString strMsg;
	wxString strErrorMsg;


	if( m_ptLua_State!=NULL )
	{
		iTopPre = lua_gettop(m_ptLua_State);
		iResult = luaL_loadbuffer(m_ptLua_State, strLuaCode.fn_str(), strLuaCode.Len(), "html lua tag");
		if( iResult!=0 )
		{
			strMsg = lua_error_to_string(iResult);
			strHtmlCode.Printf(_("failed to load lua script, error %d: %s"), iResult, strMsg.c_str());
		}
		else
		{
			iResult = lua_pcall(m_ptLua_State, 0, 1, 0);
			if( iResult!=0 )
			{
				lua_get_errorinfo(m_ptLua_State, iResult, iTopPre, &strErrorMsg, &iLineNr);
				strMsg.Printf(_("html lua tag: error %d in line %d: ") + strErrorMsg, iResult, iLineNr);
				wxLogError(strMsg);
				strHtmlCode = strMsg;
			}
			else
			{
				iTopPost = lua_gettop(m_ptLua_State);
				if( iTopPre<iTopPost )
				{
					// check return type
					iResultType = lua_type(m_ptLua_State, 0);
					if( iResultType==LUA_TSTRING )
					{
						strHtmlCode = wxString::FromAscii(lua_tostring(m_ptLua_State, 0));
					}
					else
					{
						strMsg = lua_type_to_string(iResultType);
						strHtmlCode.Printf(_("html lua tag: invalid return type, must be string, is %s"), strMsg.c_str());
					}
				}
			}
			lua_settop(m_ptLua_State, iTopPre);
		}
	}
	else
	{
		strHtmlCode = _("Lua Scripting not supported!");
	}

	return strHtmlCode;
}
#endif

bool muhkuh_mainFrame::repositoryScannerCallback(void *pvUser, wxString strMessage, int iProgressPos, int iProgressMax)
{
	bool fScannerIsRunning;
	wxProgressDialog *ptScannerProgress;
	double dProgress;


	if( pvUser!=NULL )
	{
		ptScannerProgress = (wxProgressDialog*)pvUser;

		if( iProgressPos<0 || iProgressMax<0 )
		{
			// mo pos or max -> pulse the dialog
			fScannerIsRunning = ptScannerProgress->Pulse(strMessage, NULL);
		}
		else
		{
			// set new position
			dProgress = ((double)m_iRepositoryProgressMax * (double)iProgressPos) / (double)iProgressMax;
			fScannerIsRunning = ptScannerProgress->Update((int)dProgress, strMessage, NULL);
		}
	}
	else
	{
		// no progress dialog
		fScannerIsRunning = true;
	}

	// update gui for new scanner message
	wxTheApp->Yield();

	return fScannerIsRunning;
}

