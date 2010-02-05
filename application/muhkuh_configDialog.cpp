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


#include "muhkuh_id.h"
#include "muhkuh_configDialog.h"
#include "muhkuh_config_reposEntryDialog.h"
#include "muhkuh_icons.h"


BEGIN_EVENT_TABLE(muhkuh_configDialog, wxDialog)
	EVT_RADIOBUTTON(muhkuh_configDialog_RadioWelcomeBuiltIn,	muhkuh_configDialog::OnRadioWelcomeBuiltIn)
	EVT_RADIOBUTTON(muhkuh_configDialog_RadioWelcomeFile,		muhkuh_configDialog::OnRadioWelcomeFile)
	EVT_RADIOBUTTON(muhkuh_configDialog_RadioDetailsBuiltIn,	muhkuh_configDialog::OnRadioDetailsBuiltIn)
	EVT_RADIOBUTTON(muhkuh_configDialog_RadioDetailsFile,		muhkuh_configDialog::OnRadioDetailsFile)
	EVT_BUTTON(muhkuh_configDialog_StartPageBrowse,			muhkuh_configDialog::OnBrowseStartPageButton)
	EVT_BUTTON(muhkuh_configDialog_DetailsPageBrowse,		muhkuh_configDialog::OnBrowseDetailPageButton)
	EVT_CHECKBOX(muhkuh_configAutostart_CheckboxAutostart,		muhkuh_configDialog::OnCheckAutostart)

	EVT_TOOL(muhkuh_configDialog_AddRepository,			muhkuh_configDialog::OnNewRepositoryButton)
	EVT_TOOL(muhkuh_configDialog_EditRepository,			muhkuh_configDialog::OnEditRepositoryButton)
	EVT_TOOL(muhkuh_configDialog_RemoveRepository,			muhkuh_configDialog::OnDeleteRepositoryButton)
	EVT_TREE_SEL_CHANGED(muhkuh_configDialog_RepositoryList,	muhkuh_configDialog::OnRepositorySelect)
	EVT_TREE_KEY_DOWN(muhkuh_configDialog_RepositoryList,		muhkuh_configDialog::OnRepositoryKey)

	EVT_TOOL(muhkuh_configDialog_AddPlugin,				muhkuh_configDialog::OnAddPluginButton)
	EVT_TOOL(muhkuh_configDialog_RemovePlugin,			muhkuh_configDialog::OnRemovePluginButton)
	EVT_TOOL(muhkuh_configDialog_EnablePlugin,			muhkuh_configDialog::OnEnablePluginButton)
	EVT_TOOL(muhkuh_configDialog_DisablePlugin,			muhkuh_configDialog::OnDisablePluginButton)
	EVT_TREE_SEL_CHANGED(muhkuh_configDialog_PluginList,		muhkuh_configDialog::OnPluginSelectionChanged)
	EVT_TREE_SEL_CHANGING(muhkuh_configDialog_PluginList,		muhkuh_configDialog::OnPluginSelectionChanging)
	EVT_TREE_KEY_DOWN(muhkuh_configDialog_PluginList,		muhkuh_configDialog::OnPluginKey)

	EVT_TOOL(muhkuh_configDialog_LuaAddPath,			muhkuh_configDialog::OnAddLuaIncludePathButton)
	EVT_TOOL(muhkuh_configDialog_LuaDeletePath,			muhkuh_configDialog::OnRemoveLuaIncludePathButton)
	EVT_TOOL(muhkuh_configDialog_LuaEditPath,			muhkuh_configDialog::OnEditLuaIncludePathButton)
	EVT_TOOL(muhkuh_configDialog_LuaMovePathUp,			muhkuh_configDialog::OnMoveUpLuaIncludePathButton)
	EVT_TOOL(muhkuh_configDialog_LuaMovePathDown,			muhkuh_configDialog::OnMoveDownLuaIncludePathButton)
	EVT_LISTBOX(muhkuh_configDialog_LuaPathList,			muhkuh_configDialog::OnLuaIncludePathSelectionChanged)
	EVT_LISTBOX_DCLICK(muhkuh_configDialog_LuaPathList,		muhkuh_configDialog::OnEditLuaIncludePathButton)
END_EVENT_TABLE()


muhkuh_configDialog::muhkuh_configDialog(wxWindow *parent, const wxString strApplicationPath, wxString strWelcomeFile, wxString strDetailsFile, muhkuh_plugin_manager *ptPluginManager, muhkuh_repository_manager *ptRepositoryManager, wxString strLuaIncludePath, wxString strLuaStartupCode, bool fAutostartEnabled, bool fAutoexitEnabled, wxString strAutostartTest)
 : wxDialog(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER)
 , m_ptRepositoryManager(ptRepositoryManager)
 , m_ptPluginManager(ptPluginManager)
{
	size_t sizCnt, sizIdx;
	wxConfigBase *pConfig;
	int iPosX;
	int iPosY;
	int iSizW;
	int iSizH;
	int iColonPos;
	bool fUseBuiltin;


	m_strApplicationPath = strApplicationPath;

	// set the title
	SetTitle(_("Muhkuh Settings"));

	// split up the include paths
	while( strLuaIncludePath.IsEmpty()==false )
	{
		iColonPos = strLuaIncludePath.Find(wxT(';'));
		if( iColonPos==wxNOT_FOUND )
		{
			m_astrLuaPaths.Add( strLuaIncludePath );
			break;
		}
		else
		{
			if( iColonPos>0 )
			{
				m_astrLuaPaths.Add( strLuaIncludePath.Left(iColonPos) );
			}
			strLuaIncludePath = strLuaIncludePath.Mid(iColonPos+1);
		}
	}

	// create the controls
	createControls();

	// get dialog settings
	pConfig = wxConfigBase::Get();
	// set default values
	iPosX = wxDefaultPosition.x;
	iPosY = wxDefaultPosition.y;
	iSizW = 320;
	iSizH = 200;

	if( pConfig!=NULL )
	{
		// get mainframe position and size
		pConfig->SetPath(wxT("/ConfigFrame"));
		iPosX = pConfig->Read(wxT("x"), iPosX);
		iPosY = pConfig->Read(wxT("y"), iPosY);
		iSizW = pConfig->Read(wxT("w"), iSizW);
		iSizH = pConfig->Read(wxT("h"), iSizH);
	}
	SetSize(iPosX, iPosY, iSizW, iSizH);

	// set the welcome file
	fUseBuiltin = strWelcomeFile.IsEmpty();
	m_ptRadioBuiltinWelcome->SetValue(fUseBuiltin);
	m_ptRadioFileWelcome->SetValue(!fUseBuiltin);
	m_ptTextStartPage->SetValue(strWelcomeFile);
	m_ptTextStartPage->Enable(!fUseBuiltin);
	m_ptButtonStartPage->Enable(!fUseBuiltin);

	// set the details file
	fUseBuiltin = strDetailsFile.IsEmpty();
	m_ptRadioBuiltinDetails->SetValue(fUseBuiltin);
	m_ptRadioFileDetails->SetValue(!fUseBuiltin);
	m_ptTextDetailsPage->SetValue(strDetailsFile);
	m_ptTextDetailsPage->Enable(!fUseBuiltin);
	m_ptButtonDetailsPage->Enable(!fUseBuiltin);

	// set autostart values
	switchAutostartElements(fAutostartEnabled);
	m_ptCheckAutostart->SetValue(fAutostartEnabled);
	m_ptCheckAutoexit->SetValue(fAutoexitEnabled);
	m_ptTextAutostartTest->SetValue(strAutostartTest);

	// loop over all repositories and add them to the list
	sizIdx = 0;
	sizCnt = m_ptRepositoryManager->GetRepositoryCount();
	while(sizIdx<sizCnt)
	{
		ShowNewRepository(sizIdx);
		// next entry
		++sizIdx;
	}

	// loop over all plugins and add them to the list
	sizIdx = 0;
	sizCnt = m_ptPluginManager->getPluginCount();
	while(sizIdx<sizCnt)
	{
		ShowNewPlugin(sizIdx);
		++sizIdx;
	}

	// set the lua startup code
	m_ptStartupCodeText->SetValue(strLuaStartupCode);

	// set focus to the tree book
	m_treeBook->SetFocus();
}


muhkuh_configDialog::~muhkuh_configDialog(void)
{
	wxConfigBase *pConfig;
	wxSize tSize;
	wxPoint tPos;


	tPos = GetPosition();
	tSize = GetSize();

	// set dialog settings
	pConfig = wxConfigBase::Get();
	if( pConfig!=NULL )
	{
		// get mainframe position and size
		pConfig->SetPath(wxT("/ConfigFrame"));
		pConfig->Write(wxT("x"), tPos.x);
		pConfig->Write(wxT("y"), tPos.y);
		pConfig->Write(wxT("w"), tSize.GetWidth());
		pConfig->Write(wxT("h"), tSize.GetHeight());
	}
}


void muhkuh_configDialog::createControls(void)
{
	wxBoxSizer *ptMainSizer;
	wxBoxSizer *ptbuttonSizer;
	wxImageList *ptTreeBookImageList;
	wxButton *ptButtonOk;
	wxButton *ptButtonCancel;


	// init the controls
	ptMainSizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(ptMainSizer);

	// create the treebook
	m_treeBook = new wxTreebook(this, muhkuh_configDialog_TreeBook);
	ptMainSizer->Add(m_treeBook, 1, wxEXPAND);
	ptTreeBookImageList = new wxImageList(16, 16, true, 4);
	ptTreeBookImageList->Add( wxIcon(icon_famfamfam_silk_application_form_edit) );
	ptTreeBookImageList->Add( wxIcon(icon_famfamfam_silk_database) );
	ptTreeBookImageList->Add( wxIcon(icon_famfamfam_silk_plugin) );
	ptTreeBookImageList->Add( wxIcon(lua_xpm) );
	m_treeBook->AssignImageList(ptTreeBookImageList);

	m_treeBook->AddPage(createControls_application(m_treeBook), _("Application"), true, 0);
	m_treeBook->AddPage(createControls_repository(m_treeBook), _("Repositories"), false, 1);
	m_treeBook->AddPage(createControls_plugin(m_treeBook), _("Plugins"), false, 2);
	m_treeBook->AddPage(createControls_lua(m_treeBook), _("Lua"), false, 3);

	ptbuttonSizer = new wxBoxSizer(wxHORIZONTAL);
	ptMainSizer->Add(ptbuttonSizer, 0, wxEXPAND);
	ptButtonOk = new wxButton(this, wxID_OK, _("Ok"));
	ptButtonCancel = new wxButton(this, wxID_CANCEL, _("Cancel"));
	ptbuttonSizer->AddStretchSpacer(1);
	ptbuttonSizer->Add(ptButtonOk);
	ptbuttonSizer->AddStretchSpacer(1);
	ptbuttonSizer->Add(ptButtonCancel);
	ptbuttonSizer->AddStretchSpacer(1);

	ptMainSizer->SetSizeHints(this);
}


wxPanel *muhkuh_configDialog::createControls_application(wxWindow *ptParent)
{
	wxPanel *ptApplicationPanel;
	wxBoxSizer *ptMainSizer;
	wxStaticBoxSizer *ptWelcomePageSizer;
	wxStaticBoxSizer *ptDetailsPageSizer;
	wxBoxSizer *ptWelcomeFileSizer;
	wxBoxSizer *ptDetailsFileSizer;
	wxStaticBoxSizer *ptAutostartSizer;
	wxFlexGridSizer *ptAutostartGrid;
	wxStaticText *ptLabelAutostart;
	wxStaticText *ptLabelAutostartTest;
	wxStaticText *ptLabelAutoexit;


	// create the repository page
	ptApplicationPanel = new wxPanel(ptParent);

	// create the main sizer
	ptMainSizer = new wxBoxSizer(wxVERTICAL);
	ptApplicationPanel->SetSizer(ptMainSizer);

	// create the welcome page sizer
	ptWelcomePageSizer = new wxStaticBoxSizer(wxVERTICAL, ptApplicationPanel, _("Welcome Page"));
	ptMainSizer->Add(ptWelcomePageSizer, 1, wxEXPAND);

	// create the radio button "builtin"
	m_ptRadioBuiltinWelcome = new wxRadioButton(ptApplicationPanel, muhkuh_configDialog_RadioWelcomeBuiltIn, _("Built-In"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	ptWelcomePageSizer->Add(m_ptRadioBuiltinWelcome, 0, 0);

	// create the radio button "file"
	m_ptRadioFileWelcome = new wxRadioButton(ptApplicationPanel, muhkuh_configDialog_RadioWelcomeFile, _("File"), wxDefaultPosition, wxDefaultSize, 0);
	ptWelcomePageSizer->Add(m_ptRadioFileWelcome, 0, 0);

	// create the file sizer
	ptWelcomeFileSizer = new wxBoxSizer(wxHORIZONTAL);
	ptWelcomePageSizer->Add(ptWelcomeFileSizer, 0, wxEXPAND|wxLEFT, 32);

	// fill the input grid
	m_ptTextStartPage = new wxTextCtrl(ptApplicationPanel, wxID_ANY);
	m_ptButtonStartPage = new wxBitmapButton(ptApplicationPanel, muhkuh_configDialog_StartPageBrowse, icon_famfamfam_silk_folder);
	ptWelcomeFileSizer->Add(m_ptTextStartPage, 1, wxEXPAND);
	ptWelcomeFileSizer->Add(m_ptButtonStartPage);



	// create the details page sizer
	ptDetailsPageSizer = new wxStaticBoxSizer(wxVERTICAL, ptApplicationPanel, _("Details Page"));
	ptMainSizer->Add(ptDetailsPageSizer, 1, wxEXPAND);

	// create the radio button "builtin"
	m_ptRadioBuiltinDetails = new wxRadioButton(ptApplicationPanel, muhkuh_configDialog_RadioDetailsBuiltIn, _("Built-In"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	ptDetailsPageSizer->Add(m_ptRadioBuiltinDetails, 0, 0);

	// create the radio button "file"
	m_ptRadioFileDetails = new wxRadioButton(ptApplicationPanel, muhkuh_configDialog_RadioDetailsFile, _("File"), wxDefaultPosition, wxDefaultSize, 0);
	ptDetailsPageSizer->Add(m_ptRadioFileDetails, 0, 0);

	// create the file sizer
	ptDetailsFileSizer = new wxBoxSizer(wxHORIZONTAL);
	ptDetailsPageSizer->Add(ptDetailsFileSizer, 0, wxEXPAND|wxLEFT, 32);

	// fill the input grid
	m_ptTextDetailsPage = new wxTextCtrl(ptApplicationPanel, wxID_ANY);
	m_ptButtonDetailsPage = new wxBitmapButton(ptApplicationPanel, muhkuh_configDialog_DetailsPageBrowse, icon_famfamfam_silk_folder);
	ptDetailsFileSizer->Add(m_ptTextDetailsPage, 1, wxEXPAND);
	ptDetailsFileSizer->Add(m_ptButtonDetailsPage);


	// create the autostart sizer
	ptAutostartSizer = new wxStaticBoxSizer(wxVERTICAL, ptApplicationPanel, _("Autostart Test"));
	ptMainSizer->Add(ptAutostartSizer, 1, wxEXPAND);

	// create the autostart checkbox
	ptAutostartGrid = new wxFlexGridSizer(1, 4, 0, 0);
	ptAutostartGrid->AddGrowableCol(2, 1);
	ptAutostartSizer->Add(ptAutostartGrid, 0, wxEXPAND);

	ptLabelAutostart = new wxStaticText(ptApplicationPanel, wxID_ANY, _("Enable Autostart :"));
	m_ptCheckAutostart = new wxCheckBox(ptApplicationPanel, muhkuh_configAutostart_CheckboxAutostart, wxT(""), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
	ptAutostartGrid->Add(ptLabelAutostart);
	ptAutostartGrid->AddSpacer(4);
	ptAutostartGrid->Add(m_ptCheckAutostart, 0, wxEXPAND);
	ptAutostartGrid->AddSpacer(4);

	ptLabelAutostartTest = new wxStaticText(ptApplicationPanel, wxID_ANY, _("Autostart Test :"));
	m_ptTextAutostartTest = new wxTextCtrl(ptApplicationPanel, wxID_ANY);
	m_ptButtonBrowseAutostartTest = new wxBitmapButton(ptApplicationPanel, muhkuh_configAutostart_BrowseTests, icon_famfamfam_silk_folder);
	ptAutostartGrid->Add(ptLabelAutostartTest, 0, wxALIGN_CENTER_VERTICAL);
	ptAutostartGrid->AddSpacer(4);
	ptAutostartGrid->Add(m_ptTextAutostartTest, 0, wxEXPAND);
	ptAutostartGrid->Add(m_ptButtonBrowseAutostartTest);

	ptLabelAutoexit = new wxStaticText(ptApplicationPanel, wxID_ANY, _("Enable Autoexit :"));
	m_ptCheckAutoexit = new wxCheckBox(ptApplicationPanel, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
	ptAutostartGrid->Add(ptLabelAutoexit);
	ptAutostartGrid->AddSpacer(4);
	ptAutostartGrid->Add(m_ptCheckAutoexit, 0, wxEXPAND);
	ptAutostartGrid->AddSpacer(4);


	return ptApplicationPanel;
}


wxPanel *muhkuh_configDialog::createControls_repository(wxWindow *ptParent)
{
	wxPanel *ptRepositoryPanel;
	wxBoxSizer *ptMainSizer;
	wxImageList *ptRepoImageList;


	// create the imagelist
	// NOTE: this must match the image selection in 'get_imagelist_index'
	ptRepoImageList = new wxImageList(16, 16, true, 5);
	// add 'undefined' bitmap
	ptRepoImageList->Add( wxIcon(icon_famfamfam_silk_exclamation) );
	// add 'dirscan' bitmap
	ptRepoImageList->Add( wxIcon(icon_famfamfam_silk_folder_table) );
	// add 'filelist' bitmap
	ptRepoImageList->Add( wxIcon(icon_famfamfam_silk_database) );
	// add 'singlexml' bitmap
	ptRepoImageList->Add( wxIcon(icon_famfamfam_silk_script) );
	// add 'alllocal' bitmap
	ptRepoImageList->Add( wxIcon(icon_famfamfam_silk_folder_database) );

	// create the repository page
	ptRepositoryPanel = new wxPanel(ptParent);

	// create the main sizer
	ptMainSizer = new wxBoxSizer(wxVERTICAL);
	ptRepositoryPanel->SetSizer(ptMainSizer);

	// create the repository list
	m_repositoryTree = new wxTreeCtrl(ptRepositoryPanel, muhkuh_configDialog_RepositoryList, wxDefaultPosition, wxDefaultSize, wxTR_NO_BUTTONS|wxTR_NO_LINES|wxTR_HIDE_ROOT|wxTR_SINGLE);
	m_repositoryTree->AddRoot(wxEmptyString);
	m_repositoryTree->AssignImageList(ptRepoImageList);
	ptMainSizer->Add(m_repositoryTree, 1, wxEXPAND);

	// create the repository toolbar
	m_repositoryToolBar = new wxToolBar(ptRepositoryPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize,  wxTB_HORIZONTAL|wxNO_BORDER|wxTB_TEXT);
	m_repositoryToolBar->AddTool(muhkuh_configDialog_AddRepository, _("Add"), icon_famfamfam_silk_database_add, wxNullBitmap, wxITEM_NORMAL, _("Add Repository"), _("Add a new repository to the list"));
	m_repositoryToolBar->AddTool(muhkuh_configDialog_EditRepository, _("Edit"), icon_famfamfam_silk_database_edit, wxNullBitmap, wxITEM_NORMAL, _("Edit Repository"), _("Edit the settings of the selected repository"));
	m_repositoryToolBar->EnableTool(muhkuh_configDialog_EditRepository, false);
	m_repositoryToolBar->AddTool(muhkuh_configDialog_RemoveRepository, _("Remove"), icon_famfamfam_silk_database_delete, wxNullBitmap, wxITEM_NORMAL, _("Remove Repository"), _("Remove the selected repository from the list"));
	m_repositoryToolBar->EnableTool(muhkuh_configDialog_RemoveRepository, false);
	m_repositoryToolBar->Realize();
	ptMainSizer->Add(m_repositoryToolBar, 0, wxEXPAND);

	return ptRepositoryPanel;
}


wxPanel *muhkuh_configDialog::createControls_plugin(wxWindow *ptParent)
{
	wxPanel *ptPluginPanel;
	wxBoxSizer *ptMainSizer;
	wxImageList *ptPluginImageList;


	// create imagelist for 5 images with 16x16 pixels
	ptPluginImageList = new wxImageList(16, 16, true, 5);
	ptPluginImageList->Add( wxIcon(icon_famfamfam_silk_cross) );
	ptPluginImageList->Add( wxIcon(icon_famfamfam_silk_tick) );
	ptPluginImageList->Add( wxIcon(icon_famfamfam_silk_exclamation) );
	ptPluginImageList->Add( wxIcon(icon_famfamfam_silk_key) );
	ptPluginImageList->Add( wxIcon(icon_famfamfam_silk_tag_blue) );

	// create the plugin page
	ptPluginPanel = new wxPanel(ptParent);

	// create the main sizer
	ptMainSizer = new wxBoxSizer(wxVERTICAL);
	ptPluginPanel->SetSizer(ptMainSizer);

	// create the plugin list
	m_pluginTree = new wxTreeCtrl(ptPluginPanel, muhkuh_configDialog_PluginList, wxDefaultPosition, wxDefaultSize, wxTR_HAS_BUTTONS|wxTR_NO_LINES|wxTR_HIDE_ROOT|wxTR_SINGLE);
	m_pluginTree->AddRoot(wxEmptyString);
	m_pluginTree->AssignImageList(ptPluginImageList);
	ptMainSizer->Add(m_pluginTree, 1, wxEXPAND);

	// create the plugin toolbar
	m_pluginToolBar = new wxToolBar(ptPluginPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize,  wxTB_HORIZONTAL|wxNO_BORDER|wxTB_TEXT);
	m_pluginToolBar->AddTool(muhkuh_configDialog_AddPlugin, _("Add"), icon_famfamfam_silk_plugin_add, wxNullBitmap, wxITEM_NORMAL, _("Add Plugin"), _("Add a new plugin to the list"));
	m_pluginToolBar->AddTool(muhkuh_configDialog_RemovePlugin, _("Remove"), icon_famfamfam_silk_plugin_delete, wxNullBitmap, wxITEM_NORMAL, _("Remove Plugin"), _("Remove the selected plugin from the list"));
	m_pluginToolBar->EnableTool(muhkuh_configDialog_RemovePlugin, false);
	m_pluginToolBar->AddTool(muhkuh_configDialog_EnablePlugin, _("Enable"), icon_famfamfam_silk_plugin_go, wxNullBitmap, wxITEM_NORMAL, _("Enable Plugin"), _("Enable the selected plugin"));
	m_pluginToolBar->EnableTool(muhkuh_configDialog_EnablePlugin, false);
	m_pluginToolBar->AddTool(muhkuh_configDialog_DisablePlugin, _("Disable"), icon_famfamfam_silk_plugin_disabled, wxNullBitmap, wxITEM_NORMAL, _("Disable Plugin"), _("Disable the selected plugin"));
	m_pluginToolBar->EnableTool(muhkuh_configDialog_DisablePlugin, false);
	m_pluginToolBar->Realize();
	ptMainSizer->Add(m_pluginToolBar, 0, wxEXPAND);

	return ptPluginPanel;
}


wxPanel *muhkuh_configDialog::createControls_lua(wxWindow *ptParent)
{
	wxPanel *ptLuaPanel;
	wxBoxSizer *ptMainSizer;
	wxStaticBoxSizer *ptPathSizer;
	wxStaticBoxSizer *ptStartupCodeSizer;
	long lStyle;


	// create the plugin page
	ptLuaPanel = new wxPanel(ptParent);

	// create the main sizer
	ptMainSizer = new wxBoxSizer(wxVERTICAL);
	ptLuaPanel->SetSizer(ptMainSizer);

	// create the path sizer
	ptPathSizer = new wxStaticBoxSizer(wxVERTICAL, ptLuaPanel, _("Include Paths"));
	ptMainSizer->Add(ptPathSizer, 1, wxEXPAND);
	lStyle = wxLB_SINGLE|wxLB_HSCROLL;
	m_ptPathListBox = new muhkuh_dirlistbox(ptLuaPanel, muhkuh_configDialog_LuaPathList, wxDefaultPosition, wxDefaultSize, m_astrLuaPaths, m_strApplicationPath, lStyle);
	ptPathSizer->Add(m_ptPathListBox, 1, wxEXPAND);

	// create the path toolbar
	m_luaPathToolBar = new wxToolBar(ptLuaPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize,  wxTB_HORIZONTAL|wxNO_BORDER|wxTB_TEXT);
	m_luaPathToolBar->AddTool(muhkuh_configDialog_LuaAddPath, _("Add"), icon_famfamfam_silk_folder_add, wxNullBitmap, wxITEM_NORMAL, _("Add Path"), _("Add a new include path to the list"));
	m_luaPathToolBar->AddTool(muhkuh_configDialog_LuaDeletePath, _("Remove"), icon_famfamfam_silk_folder_delete, wxNullBitmap, wxITEM_NORMAL, _("Remove Path"), _("Remove the selected include path from the list"));
	m_luaPathToolBar->AddTool(muhkuh_configDialog_LuaEditPath, _("Edit"), icon_famfamfam_silk_folder_edit, wxNullBitmap, wxITEM_NORMAL, _("Edit Path"), _("Edit the selected include path"));
	m_luaPathToolBar->AddTool(muhkuh_configDialog_LuaMovePathUp, _("Up"), icon_famfamfam_silk_arrow_up, wxNullBitmap, wxITEM_NORMAL, _("Move Path Up"), _("Move the selected include path up"));
	m_luaPathToolBar->AddTool(muhkuh_configDialog_LuaMovePathDown, _("Down"), icon_famfamfam_silk_arrow_down, wxNullBitmap, wxITEM_NORMAL, _("Move Path Down"), _("Move the selected include path down"));
	m_luaPathToolBar->EnableTool(muhkuh_configDialog_LuaDeletePath, false);
	m_luaPathToolBar->EnableTool(muhkuh_configDialog_LuaEditPath, false);
	m_luaPathToolBar->EnableTool(muhkuh_configDialog_LuaMovePathUp, false);
	m_luaPathToolBar->EnableTool(muhkuh_configDialog_LuaMovePathDown, false);
	m_luaPathToolBar->Realize();
	ptPathSizer->Add(m_luaPathToolBar, 0, wxEXPAND);

	// create the startup code sizer
	ptStartupCodeSizer = new wxStaticBoxSizer(wxVERTICAL, ptLuaPanel, _("Startup Code"));
	ptMainSizer->Add(ptStartupCodeSizer, 1, wxEXPAND);
	m_ptStartupCodeText = new wxTextCtrl(ptLuaPanel, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_LEFT|wxTE_DONTWRAP);
	ptStartupCodeSizer->Add(m_ptStartupCodeText, 1, wxEXPAND);

	return ptLuaPanel;
}


void muhkuh_configDialog::OnRadioWelcomeBuiltIn(wxCommandEvent &event)
{
	m_ptTextStartPage->Enable(false);
	m_ptButtonStartPage->Enable(false);
}


void muhkuh_configDialog::OnRadioWelcomeFile(wxCommandEvent &event)
{
	m_ptTextStartPage->Enable(true);
	m_ptButtonStartPage->Enable(true);
}


void muhkuh_configDialog::OnRadioDetailsBuiltIn(wxCommandEvent &event)
{
	m_ptTextDetailsPage->Enable(false);
	m_ptButtonDetailsPage->Enable(false);
}


void muhkuh_configDialog::OnRadioDetailsFile(wxCommandEvent &event)
{
	m_ptTextDetailsPage->Enable(true);
	m_ptButtonDetailsPage->Enable(true);
}


void muhkuh_configDialog::OnBrowseStartPageButton(wxCommandEvent &event)
{
	wxFileDialog *pageDialog;
	wxFileName fileName;


	fileName.Assign(m_ptTextStartPage->GetValue());
	fileName.Normalize(wxPATH_NORM_ALL, m_strApplicationPath ,wxPATH_NATIVE);

	pageDialog = new wxFileDialog(this, _("Select the Welcome page"), fileName.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR), fileName.GetFullName(), wxT("HTML Page (*.html;*.htm)|*.html;*.htm|All Files (*.*)|*.*"));

	if( pageDialog->ShowModal()==wxID_OK )
	{
		m_ptTextStartPage->SetValue(pageDialog->GetPath());
	}
	pageDialog->Destroy();
}


void muhkuh_configDialog::OnBrowseDetailPageButton(wxCommandEvent &event)
{
	wxFileDialog *pageDialog;
	wxFileName fileName;


	fileName.Assign(m_ptTextDetailsPage->GetValue());
	fileName.Normalize(wxPATH_NORM_ALL, m_strApplicationPath ,wxPATH_NATIVE);

	pageDialog = new wxFileDialog(this, _("Select the Test Details page"), fileName.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR), fileName.GetFullName(), wxT("HTML Page (*.html;*.htm)|*.html;*.htm|All Files (*.*)|*.*"));

	if( pageDialog->ShowModal()==wxID_OK )
	{
		m_ptTextDetailsPage->SetValue(pageDialog->GetPath());
	}
	pageDialog->Destroy();
}


void muhkuh_configDialog::OnCheckAutostart(wxCommandEvent &event)
{
	switchAutostartElements(event.IsChecked());
}


void muhkuh_configDialog::OnNewRepositoryButton(wxCommandEvent &WXUNUSED(event))
{
	repository_add();
}


void muhkuh_configDialog::OnEditRepositoryButton(wxCommandEvent &WXUNUSED(event))
{
	wxTreeItemId tItem;
	treeItemIdData *ptData;
	long lRepositoryIdx;
	muhkuh_config_reposEntryDialog *ptEntryDialog;
	muhkuh_repository *ptRepos;
	int iItemImage;
	muhkuh_repository::REPOSITORY_TYP_E eTyp;


	// get the selected item
	tItem = m_repositoryTree->GetSelection();
	// was something selected?
	if( tItem.IsOk()==true )
	{
		// get the repository id
		ptData = (treeItemIdData*)m_pluginTree->GetItemData(tItem);
		if( ptData!=NULL )
		{
			lRepositoryIdx = ptData->m_lId;

			ptRepos = m_ptRepositoryManager->GetRepository(lRepositoryIdx);

			// clone the entry
			ptEntryDialog = new muhkuh_config_reposEntryDialog(this, m_strApplicationPath, ptRepos);
			if( ptEntryDialog->ShowModal()==wxID_OK )
			{
				// update the item
				m_repositoryTree->SetItemText(tItem, ptRepos->GetStringRepresentation());
				// get bitmap for the entry
				eTyp = ptRepos->GetTyp();
				iItemImage = get_imagelist_index(eTyp);
				m_repositoryTree->SetItemImage(tItem, iItemImage);
			}
			ptEntryDialog->Destroy();
		}
	}
}


void muhkuh_configDialog::OnDeleteRepositoryButton(wxCommandEvent &WXUNUSED(event))
{
	repository_delete();
}


void muhkuh_configDialog::OnRepositorySelect(wxTreeEvent &event)
{
	wxTreeItemId tItem;
	bool fPluginSelected;
	treeItemIdData *ptData;


	tItem = event.GetItem();
	fPluginSelected = tItem.IsOk();
	if( fPluginSelected==true )
	{
		ptData = (treeItemIdData*)m_pluginTree->GetItemData(tItem);
		fPluginSelected = (ptData!=NULL);
	}	
	m_repositoryToolBar->EnableTool(muhkuh_configDialog_EditRepository,	fPluginSelected);
	m_repositoryToolBar->EnableTool(muhkuh_configDialog_RemoveRepository,	fPluginSelected);
}


void muhkuh_configDialog::OnRepositoryKey(wxTreeEvent &event)
{
	int iKeyCode;


	iKeyCode = event.GetKeyEvent().GetKeyCode();
	switch( iKeyCode )
	{
	case WXK_DELETE:
		// delete the selected repository
		repository_delete();
		break;
	case WXK_INSERT:
		// add a new repository
		repository_add();
		break;
	default:
		// the event was not processed by this routine
		event.Skip();
		break;
	}
}


void muhkuh_configDialog::OnAddPluginButton(wxCommandEvent &WXUNUSED(event))
{
	plugin_add();
}


void muhkuh_configDialog::OnRemovePluginButton(wxCommandEvent &WXUNUSED(event))
{
	plugin_delete();
}


void muhkuh_configDialog::OnEnablePluginButton(wxCommandEvent &WXUNUSED(event))
{
	plugin_enable(true);
}


void muhkuh_configDialog::OnDisablePluginButton(wxCommandEvent &WXUNUSED(event))
{
	plugin_enable(false);
}


void muhkuh_configDialog::OnPluginSelectionChanging(wxTreeEvent &event)
{
	wxTreeItemId tItem;
	treeItemIdData *ptData;
	bool fAllowChange;


	// don't allow change by default
	fAllowChange = false;

	// only allow items with tree data to be selected
	tItem = event.GetItem();
	if( tItem.IsOk()==true )
	{
		ptData = (treeItemIdData*)m_pluginTree->GetItemData(tItem);
		if( ptData!=NULL )
		{
			fAllowChange = true;
		}
	}

	// allow or veto the change request
	if( fAllowChange==true )
	{
		event.Allow();
	}
	else
	{
		event.Veto();
	}
}


void muhkuh_configDialog::OnPluginKey(wxTreeEvent &event)
{
	int iKeyCode;
	wxTreeItemId tItem;
	treeItemIdData *ptData;
	long lPluginIdx;
	bool fEnabled;


	iKeyCode = event.GetKeyEvent().GetKeyCode();
	switch( iKeyCode )
	{
	case WXK_DELETE:
		// delete the selected repository
		plugin_delete();
		break;

	case WXK_INSERT:
		// add a new repository
		plugin_add();
		break;

	case ' ':
		// toggle plugin enable

		// get the selected item
		tItem = m_pluginTree->GetSelection();
		// was something selected?
		if( tItem.IsOk()==true )
		{
			// get the plugin id
			ptData = (treeItemIdData*)m_pluginTree->GetItemData(tItem);
			if( ptData!=NULL )
			{
				lPluginIdx = ptData->m_lId;
				// get the current state
				fEnabled = m_ptPluginManager->GetEnable(lPluginIdx);
				// invert the state
				plugin_enable(!fEnabled);
			}
		}
		break;

	default:
		// the event was not processed by this routine
		event.Skip();
		break;
	}
}


void muhkuh_configDialog::OnPluginSelectionChanged(wxTreeEvent &event)
{
	SetPluginButtons(event.GetItem());
}


void muhkuh_configDialog::SetPluginButtons(wxTreeItemId tItem)
{
	treeItemIdData *ptData;
	long lPluginIdx;
	bool fPluginSelected;
	bool fPluginIsOk;
	bool fPluginIsEnabled;
	bool fPluginCanBeEnabled;
	bool fPluginCanBeDisabled;


	fPluginSelected = tItem.IsOk();
	// set default values
	fPluginCanBeEnabled = false;
	fPluginCanBeDisabled = false;

	if( fPluginSelected==true )
	{
		ptData = (treeItemIdData*)m_pluginTree->GetItemData(tItem);
		if( ptData!=NULL )
		{
			lPluginIdx = ptData->m_lId;
			fPluginIsOk = m_ptPluginManager->IsOk(lPluginIdx);
			fPluginIsEnabled = m_ptPluginManager->GetEnable(lPluginIdx);

			fPluginCanBeEnabled = fPluginSelected && fPluginIsOk && !fPluginIsEnabled;
			fPluginCanBeDisabled = fPluginSelected && fPluginIsEnabled;
		}
	}

	m_pluginToolBar->EnableTool(muhkuh_configDialog_RemovePlugin,	fPluginSelected);
	m_pluginToolBar->EnableTool(muhkuh_configDialog_EnablePlugin,	fPluginCanBeEnabled);
	m_pluginToolBar->EnableTool(muhkuh_configDialog_DisablePlugin,	fPluginCanBeDisabled);
}


void muhkuh_configDialog::ShowNewRepository(long lIdx)
{
	wxTreeItemId tRootItem;
	treeItemIdData *ptData;
	int iImageIdx;
	wxString strName;
	muhkuh_repository::REPOSITORY_TYP_E eTyp;


	// append all plugins to the root item
	tRootItem = m_repositoryTree->GetRootItem();

	strName = m_ptRepositoryManager->GetStringRepresentation(lIdx);
	eTyp = m_ptRepositoryManager->GetTyp(lIdx);
	iImageIdx = get_imagelist_index(eTyp);
	ptData = new treeItemIdData(lIdx);

	m_repositoryTree->AppendItem(tRootItem, strName, iImageIdx, -1, ptData);
}


void muhkuh_configDialog::ShowNewPlugin(long lIdx)
{
	bool fPluginIsOk;
	wxTreeItemId tRootItem;
	wxTreeItemId tPluginItem;
	const muhkuh_plugin_desc *ptPluginDesc;
	wxString strName;
	wxString strId;
	wxString strVersion;
	treeItemIdData *ptData;


	// append all plugins to the root item
	tRootItem = m_pluginTree->GetRootItem();

	/* get the plugin description */
	fPluginIsOk = m_ptPluginManager->IsOk(lIdx);
	ptPluginDesc = m_ptPluginManager->getPluginDescription(lIdx);
	if( ptPluginDesc==NULL )
	{
		wxLogError(_("failed to get plugin description!"));
	}
	else
	{
		strName = m_ptPluginManager->GetConfigName(lIdx);

		// create the new data item
		ptData = new treeItemIdData(lIdx);

		if( fPluginIsOk==true )
		{
			strId = ptPluginDesc->strPluginId;
			strVersion.Printf(wxT("V%d.%d.%d"), ptPluginDesc->tVersion.uiVersionMajor, ptPluginDesc->tVersion.uiVersionMinor, ptPluginDesc->tVersion.uiVersionSub);

			// set the plugin item
			tPluginItem = m_pluginTree->AppendItem(tRootItem, strName, -1, -1, ptData);
			m_pluginTree->AppendItem(tPluginItem, strId, 3);
			m_pluginTree->AppendItem(tPluginItem, strVersion, 4);
			ShowPluginImage(tPluginItem);
		}
		else
		{
			tPluginItem = m_pluginTree->AppendItem(tRootItem, strName, 2, -1, ptData);
			// append the errormessage
			m_pluginTree->AppendItem(tPluginItem, m_ptPluginManager->GetInitError(lIdx), 2, -1, NULL);
		}
	}
}


void muhkuh_configDialog::ShowPluginImage(wxTreeItemId tPluginItem)
{
	treeItemIdData *ptData;
	long lIdx;
	int iImageIdx;


	// get the tree item data
	if( tPluginItem.IsOk()==true )
	{
		ptData = (treeItemIdData*)m_pluginTree->GetItemData(tPluginItem);
		if( ptData!=NULL )
		{
			lIdx = ptData->m_lId;

			if( m_ptPluginManager->IsOk(lIdx)==false )
			{
				iImageIdx = 2;
			}
			else if( m_ptPluginManager->GetEnable(lIdx)==false )
			{
				iImageIdx = 0;
			}
			else
			{
				iImageIdx = 1;
			}
			m_pluginTree->SetItemImage(tPluginItem, iImageIdx);
		}
	}
}


void muhkuh_configDialog::repository_add(void)
{
	muhkuh_config_reposEntryDialog *ptEntryDialog;
	muhkuh_repository *ptRepos;
	long lIdx;


	ptRepos = new muhkuh_repository(_("new repository"));
	ptEntryDialog = new muhkuh_config_reposEntryDialog(this, m_strApplicationPath, ptRepos);
	if( ptEntryDialog->ShowModal()==wxID_OK )
	{
		// add to list
		lIdx = m_ptRepositoryManager->addRepository(ptRepos);
		// show
		ShowNewRepository(lIdx);
	}
	else
	{
		delete ptRepos;
	}
	ptEntryDialog->Destroy();
}


void muhkuh_configDialog::repository_delete(void)
{
	wxTreeItemId tItem;
	treeItemIdData *ptData;
	long lRepositoryIdx;
	wxTreeItemId tRootItem;
	wxTreeItemIdValue tCookie;


	// get the selected item
	tItem = m_repositoryTree->GetSelection();
	// was something selected?
	if( tItem.IsOk()==true )
	{
		// get the repository id
		ptData = (treeItemIdData*)m_pluginTree->GetItemData(tItem);
		if( ptData!=NULL )
		{
			lRepositoryIdx = ptData->m_lId;

			// erase from the listctrl
			m_repositoryTree->Delete(tItem);
			// erase from the vector
			m_ptRepositoryManager->removeRepository(lRepositoryIdx);

			// rebuild idlist
			tRootItem = m_repositoryTree->GetRootItem();
			lRepositoryIdx = 0;
			tItem = m_repositoryTree->GetFirstChild(tRootItem, tCookie);
			while( tItem.IsOk()==true )
			{
				// set the plugin id
				ptData = (treeItemIdData*)m_repositoryTree->GetItemData(tItem);
				if( ptData!=NULL )
				{
					ptData->m_lId = lRepositoryIdx;
				}
				// inc id
				++lRepositoryIdx;
				// move to next child
				tItem = m_repositoryTree->GetNextChild(tRootItem, tCookie);
			}
		}
	}
}


void muhkuh_configDialog::plugin_add(void)
{
	wxFileDialog *pluginDialog;
	wxFileName fileName;
	wxString strPluginName;
	wxString strDialogInitPath;
	long lIdx;
	bool fPluginIsOk;


	strDialogInitPath = wxEmptyString;
/*
	if( m_fUseRelativePaths==true )
	{
		fileName.Assign(strDialogInitPath);
		if(fileName.Normalize(wxPATH_NORM_ALL, m_strApplicationPath ,wxPATH_NATIVE))
		{
			strDialogInitPath = fileName.GetFullPath();
		}
	}
*/
	pluginDialog = new wxFileDialog(this, _("Select the new plugin"), strDialogInitPath, wxEmptyString, wxT("*.xml"), wxFD_OPEN|wxFD_FILE_MUST_EXIST);

	if( pluginDialog->ShowModal()==wxID_OK )
	{
		strPluginName = pluginDialog->GetPath();
		wxLogMessage(_("open plugin '%s'"), strPluginName.c_str());
		lIdx = m_ptPluginManager->addPlugin(strPluginName);
		if( lIdx>=0 )
		{
			// do not accept broken plugins
			fPluginIsOk = m_ptPluginManager->IsOk(lIdx);
			if( fPluginIsOk==true )
			{
				ShowNewPlugin(lIdx);
			}
			else
			{
				m_ptPluginManager->removePlugin(lIdx);
			}
		}
	}
	pluginDialog->Destroy();
}


void muhkuh_configDialog::plugin_delete(void)
{
	wxTreeItemId tItem;
	treeItemIdData *ptData;
	long lPluginIdx;
	wxTreeItemId tRootItem;
	wxTreeItemIdValue tCookie;


	// get the selected item
	tItem = m_pluginTree->GetSelection();
	// was something selected?
	if( tItem.IsOk()==true )
	{
		// disable all buttons (in case if no update event follows the delete)
		SetPluginButtons(wxTreeItemId());

		// get the plugin id
		ptData = (treeItemIdData*)m_pluginTree->GetItemData(tItem);
		if( ptData!=NULL )
		{
			lPluginIdx = ptData->m_lId;

			// erase from the tree
			m_pluginTree->Delete(tItem);

			// erase from the manager
			m_ptPluginManager->removePlugin(lPluginIdx);

			// rebuild idlist
			tRootItem = m_pluginTree->GetRootItem();
			lPluginIdx = 0;
			tItem = m_pluginTree->GetFirstChild(tRootItem, tCookie);
			while( tItem.IsOk()==true )
			{
				// set the plugin id
				ptData = (treeItemIdData*)m_pluginTree->GetItemData(tItem);
				if( ptData!=NULL )
				{
					ptData->m_lId = lPluginIdx;
				}
				// inc id
				++lPluginIdx;
				// move to next child
				tItem = m_pluginTree->GetNextChild(tRootItem, tCookie);
			}
		}
	}
}


void muhkuh_configDialog::plugin_enable(bool fEnablePlugin)
{
	wxTreeItemId tItem;
	treeItemIdData *ptData;
	long lPluginIdx;


	// get the selected item
	tItem = m_pluginTree->GetSelection();
	// was something selected?
	if( tItem.IsOk()==true )
	{
		// get the plugin id
		ptData = (treeItemIdData*)m_pluginTree->GetItemData(tItem);
		if( ptData!=NULL )
		{
			lPluginIdx = ptData->m_lId;

			m_ptPluginManager->SetEnable(lPluginIdx, fEnablePlugin);
			ShowPluginImage(tItem);
			SetPluginButtons(tItem);
		}
	}
}


void muhkuh_configDialog::OnAddLuaIncludePathButton(wxCommandEvent &event)
{
	wxDirDialog *includePathDialog;
	wxFileName fileName;
	wxString strDialogInitPath;
	wxString strPath;
	int iSelection;


	strDialogInitPath = m_strApplicationPath;

	includePathDialog = new wxDirDialog(this, _("Choose the lua include path"));
	includePathDialog->SetPath(strDialogInitPath);

	if( includePathDialog->ShowModal()==wxID_OK )
	{
		strPath = includePathDialog->GetPath() + wxFileName::GetPathSeparator() + wxT("?.lua");
		iSelection = m_ptPathListBox->Append(strPath);
		m_ptPathListBox->SetSelection(iSelection);
		luaIncludePathUpdateButtons(iSelection);
	}
	includePathDialog->Destroy();
}


void muhkuh_configDialog::OnRemoveLuaIncludePathButton(wxCommandEvent &event)
{
	int iSelection;
	int iCount;


	iSelection = m_ptPathListBox->GetSelection();
	if( iSelection!=wxNOT_FOUND )
	{
		m_ptPathListBox->Delete(iSelection);

		// try to get a new position
		iCount = m_ptPathListBox->GetCount();
		if( iCount==0 )
		{
			iSelection = -1;
		}
		else if( iSelection>=iCount )
		{
			--iSelection;
		}
		m_ptPathListBox->SetSelection(iSelection);
		// update buttons for the new position
		luaIncludePathUpdateButtons(iSelection);
	}
}


void muhkuh_configDialog::OnEditLuaIncludePathButton(wxCommandEvent &event)
{
	int iSelection;


	iSelection = m_ptPathListBox->GetSelection();
	if( iSelection!=wxNOT_FOUND )
	{
		// show an edit component and browse button
		m_ptPathListBox->StartEdit(iSelection);
	}
}


void muhkuh_configDialog::OnMoveUpLuaIncludePathButton(wxCommandEvent &event)
{
	int iSelection;
	wxString strPath0;
	wxString strPath1;


	iSelection = m_ptPathListBox->GetSelection();
	if( iSelection!=wxNOT_FOUND && iSelection>0 )
	{
		// get strings
		strPath0 = m_ptPathListBox->GetString(iSelection-1);
		strPath1 = m_ptPathListBox->GetString(iSelection);

		// exchange strings
		m_ptPathListBox->SetString(iSelection-1, strPath1);
		m_ptPathListBox->SetString(iSelection, strPath0);
	}

	m_ptPathListBox->SetSelection(iSelection-1);
	luaIncludePathUpdateButtons(iSelection-1);
}


void muhkuh_configDialog::OnMoveDownLuaIncludePathButton(wxCommandEvent &event)
{
	int iSelection;
	int iEntries;
	wxString strPath0;
	wxString strPath1;


	iSelection = m_ptPathListBox->GetSelection();
	iEntries = m_ptPathListBox->GetCount();
	if( iSelection!=wxNOT_FOUND && iSelection+1<iEntries )
	{
		// get strings
		strPath0 = m_ptPathListBox->GetString(iSelection);
		strPath1 = m_ptPathListBox->GetString(iSelection+1);

		// exchange strings
		m_ptPathListBox->SetString(iSelection, strPath1);
		m_ptPathListBox->SetString(iSelection+1, strPath0);
	}

	m_ptPathListBox->SetSelection(iSelection+1);
	luaIncludePathUpdateButtons(iSelection+1);
}


void muhkuh_configDialog::OnLuaIncludePathSelectionChanged(wxCommandEvent &event)
{
	int iSelection;


	iSelection = event.GetSelection();
	luaIncludePathUpdateButtons(iSelection);
	// cancel any edit operation
	m_ptPathListBox->CancelEdit();
}

void muhkuh_configDialog::luaIncludePathUpdateButtons(int iSelection)
{
	bool fPathSelected;
	bool fCanMoveUp;
	bool fCanMoveDown;


	if( iSelection!=wxNOT_FOUND )
	{
		fPathSelected = true;
		fCanMoveUp = (iSelection>0);
		fCanMoveDown = ((iSelection>=0) && ((((unsigned int)iSelection)+1)<m_ptPathListBox->GetCount()));
	}
	else
	{
		fPathSelected = false;
		fCanMoveUp = false;
		fCanMoveDown = false;
	}


	m_luaPathToolBar->EnableTool(muhkuh_configDialog_LuaDeletePath, fPathSelected);
	m_luaPathToolBar->EnableTool(muhkuh_configDialog_LuaEditPath, fPathSelected);
	m_luaPathToolBar->EnableTool(muhkuh_configDialog_LuaMovePathUp, fCanMoveUp);
	m_luaPathToolBar->EnableTool(muhkuh_configDialog_LuaMovePathDown, fCanMoveDown);
}


wxString muhkuh_configDialog::GetWelcomePageFile(void) const
{
	wxString strFile;


	if( m_ptRadioFileWelcome->GetValue()==true )
	{
		strFile = m_ptTextStartPage->GetValue();
	}

	return strFile;
}


wxString muhkuh_configDialog::GetDetailsPageFile(void) const
{
	wxString strFile;


	if( m_ptRadioFileDetails->GetValue()==true )
	{
		strFile = m_ptTextDetailsPage->GetValue();
	}

	return strFile;
}


wxString muhkuh_configDialog::GetLuaIncludePath(void) const
{
	return m_ptPathListBox->GetPaths(wxT(';'));
}


wxString muhkuh_configDialog::GetLuaStartupCode(void) const
{
	return m_ptStartupCodeText->GetValue();
}


bool muhkuh_configDialog::GetAutostartEnable(void) const
{
	return m_ptCheckAutostart->GetValue();
}


bool muhkuh_configDialog::GetAutoexitEnable(void) const
{
	return m_ptCheckAutoexit->GetValue();
}


wxString muhkuh_configDialog::GetAutostartTest(void) const
{
	return m_ptTextAutostartTest->GetValue();
}


void muhkuh_configDialog::switchAutostartElements(bool fEnabled)
{
	m_ptCheckAutoexit->Enable(fEnabled);
	m_ptTextAutostartTest->Enable(fEnabled);
	m_ptButtonBrowseAutostartTest->Enable(fEnabled);
}


int muhkuh_configDialog::get_imagelist_index(muhkuh_repository::REPOSITORY_TYP_E eTyp)
{
	int iItemImage;


	// this is the default value
	iItemImage = 0;

	// NOTE: this must match the imagelist init in 'createControls_repository'
	switch( eTyp )
	{
	case muhkuh_repository::REPOSITORY_TYP_DIRSCAN:
		iItemImage = 1;
		break;
	case muhkuh_repository::REPOSITORY_TYP_FILELIST:
		iItemImage = 2;
		break;
	case muhkuh_repository::REPOSITORY_TYP_SINGLEXML:
		iItemImage = 3;
		break;
	case muhkuh_repository::REPOSITORY_TYP_ALLLOCAL:
		iItemImage = 4;
		break;
	}

	return iItemImage;
}
