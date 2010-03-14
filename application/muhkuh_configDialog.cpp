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
#include "muhkuh_flags.h"
#include "muhkuh_icons.h"


BEGIN_EVENT_TABLE(muhkuh_configDialog, wxDialog)
	EVT_RADIOBUTTON(muhkuh_configDialog_RadioWelcomeBuiltIn,	muhkuh_configDialog::OnRadioWelcomeBuiltIn)
	EVT_RADIOBUTTON(muhkuh_configDialog_RadioWelcomeFile,		muhkuh_configDialog::OnRadioWelcomeFile)
	EVT_RADIOBUTTON(muhkuh_configDialog_RadioDetailsBuiltIn,	muhkuh_configDialog::OnRadioDetailsBuiltIn)
	EVT_RADIOBUTTON(muhkuh_configDialog_RadioDetailsFile,		muhkuh_configDialog::OnRadioDetailsFile)
	EVT_BUTTON(muhkuh_configDialog_StartPageBrowse,			muhkuh_configDialog::OnBrowseStartPageButton)
	EVT_BUTTON(muhkuh_configDialog_DetailsPageBrowse,		muhkuh_configDialog::OnBrowseDetailPageButton)

	EVT_TOOL(muhkuh_configDialog_AddRepository,			muhkuh_configDialog::OnNewRepositoryButton)
	EVT_TOOL(muhkuh_configDialog_EditRepository,			muhkuh_configDialog::OnEditRepositoryButton)
	EVT_TOOL(muhkuh_configDialog_RemoveRepository,			muhkuh_configDialog::OnDeleteRepositoryButton)
	EVT_TREE_SEL_CHANGED(muhkuh_configDialog_RepositoryList,	muhkuh_configDialog::OnRepositorySelect)
	EVT_TREE_KEY_DOWN(muhkuh_configDialog_RepositoryList,		muhkuh_configDialog::OnRepositoryKey)

	EVT_TOOL(muhkuh_configDialog_LuaAddPath,			muhkuh_configDialog::OnAddLuaIncludePathButton)
	EVT_TOOL(muhkuh_configDialog_LuaDeletePath,			muhkuh_configDialog::OnRemoveLuaIncludePathButton)
	EVT_TOOL(muhkuh_configDialog_LuaEditPath,			muhkuh_configDialog::OnEditLuaIncludePathButton)
	EVT_TOOL(muhkuh_configDialog_LuaMovePathUp,			muhkuh_configDialog::OnMoveUpLuaIncludePathButton)
	EVT_TOOL(muhkuh_configDialog_LuaMovePathDown,			muhkuh_configDialog::OnMoveDownLuaIncludePathButton)
	EVT_LISTBOX(muhkuh_configDialog_LuaPathList,			muhkuh_configDialog::OnLuaIncludePathSelectionChanged)
	EVT_LISTBOX_DCLICK(muhkuh_configDialog_LuaPathList,		muhkuh_configDialog::OnEditLuaIncludePathButton)
END_EVENT_TABLE()


muhkuh_configDialog::muhkuh_configDialog(wxWindow *parent, const wxString strApplicationPath, wxString strWelcomeFile, wxString strDetailsFile, muhkuh_repository_manager *ptRepositoryManager, wxString strLuaIncludePath, wxString strLuaStartupCode)
 : wxDialog(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER)
 , m_ptRepositoryManager(ptRepositoryManager)
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

	// loop over all repositories and add them to the list
	sizIdx = 0;
	sizCnt = m_ptRepositoryManager->GetRepositoryCount();
	while(sizIdx<sizCnt)
	{
		ShowNewRepository(sizIdx);
		// next entry
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
	ptTreeBookImageList->Add( wxIcon(lua_xpm) );
	ptTreeBookImageList->Add( wxIcon(icon_famfamfam_silk_world) );
	m_treeBook->AssignImageList(ptTreeBookImageList);

	m_treeBook->AddPage(createControls_application(m_treeBook), _("Application"), true, 0);
	m_treeBook->AddPage(createControls_repository(m_treeBook), _("Repositories"), false, 1);
	m_treeBook->AddPage(createControls_lua(m_treeBook), _("Lua"), false, 2);
	m_treeBook->AddPage(createControls_language(m_treeBook), _("Language"), false, 3);

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


wxPanel *muhkuh_configDialog::createControls_language(wxWindow *ptParent)
{
	wxPanel *ptPanel;
	wxBoxSizer *ptMainSizer;
	wxStaticBoxSizer *ptLanguageBox;
	wxImageList *ptImageList;
	wxListCtrl *ptListCtrl;
	wxStaticText *ptNote;


	// create the plugin page
	ptPanel = new wxPanel(ptParent);

	// create the main sizer
	ptMainSizer = new wxBoxSizer(wxVERTICAL);
	ptPanel->SetSizer(ptMainSizer);

	ptLanguageBox = new wxStaticBoxSizer(wxVERTICAL, ptPanel, _("Application Language"));
	ptMainSizer->Add(ptLanguageBox, 1, wxEXPAND);

	ptImageList = new wxImageList(16, 11, true, 241);
	ptImageList->Add( wxIcon(icon_famfamfam_flags_ad) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_ae) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_af) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_ag) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_ai) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_al) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_am) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_an) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_ao) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_ar) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_as) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_at) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_au) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_aw) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_ax) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_az) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_ba) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_bb) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_bd) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_be) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_bf) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_bg) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_bh) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_bi) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_bj) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_bm) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_bn) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_bo) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_br) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_bs) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_bt) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_bv) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_bw) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_by) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_bz) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_ca) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_cc) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_cd) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_cf) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_cg) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_ch) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_ci) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_ck) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_cl) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_cm) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_cn) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_co) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_cr) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_cs) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_cu) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_cv) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_cx) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_cy) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_cz) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_de) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_dj) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_dk) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_dm) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_do) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_dz) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_ec) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_ee) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_eg) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_eh) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_er) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_es) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_et) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_fi) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_fj) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_fk) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_fm) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_fo) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_fr) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_ga) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_gb) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_gd) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_ge) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_gf) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_gh) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_gi) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_gl) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_gm) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_gn) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_gp) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_gq) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_gr) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_gs) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_gt) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_gu) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_gw) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_gy) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_hk) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_hm) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_hn) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_hr) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_ht) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_hu) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_id) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_ie) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_il) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_in) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_io) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_iq) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_ir) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_is) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_it) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_jm) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_jo) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_jp) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_ke) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_kg) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_kh) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_ki) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_km) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_kn) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_kp) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_kr) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_kw) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_ky) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_kz) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_la) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_lb) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_lc) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_li) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_lk) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_lr) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_ls) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_lt) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_lu) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_lv) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_ly) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_ma) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_mc) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_md) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_me) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_mg) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_mh) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_mk) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_ml) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_mm) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_mn) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_mo) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_mp) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_mq) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_mr) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_ms) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_mt) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_mu) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_mv) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_mw) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_mx) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_my) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_mz) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_na) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_nc) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_ne) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_nf) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_ng) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_ni) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_nl) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_no) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_np) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_nr) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_nu) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_nz) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_om) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_pa) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_pe) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_pf) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_pg) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_ph) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_pk) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_pl) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_pm) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_pn) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_pr) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_ps) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_pt) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_pw) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_py) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_qa) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_re) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_ro) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_rs) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_ru) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_rw) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_sa) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_sb) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_sc) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_sd) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_se) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_sg) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_sh) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_si) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_sj) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_sk) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_sl) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_sm) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_sn) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_so) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_sr) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_st) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_sv) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_sy) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_sz) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_tc) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_td) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_tf) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_tg) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_th) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_tj) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_tk) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_tl) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_tm) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_tn) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_to) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_tr) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_tt) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_tv) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_tw) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_tz) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_ua) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_ug) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_um) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_us) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_uy) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_uz) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_va) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_vc) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_ve) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_vg) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_vi) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_vn) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_vu) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_wf) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_ws) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_ye) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_yt) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_za) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_zm) );
	ptImageList->Add( wxIcon(icon_famfamfam_flags_zw) );


	ptListCtrl = new wxListCtrl(ptPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_NO_HEADER|wxLC_REPORT|wxLC_SINGLE_SEL);
	ptListCtrl->AssignImageList(ptImageList, wxIMAGE_LIST_SMALL);
	ptListCtrl->InsertColumn(0, wxT("blah"), wxLIST_FORMAT_LEFT, -1);

	ptListCtrl->InsertItem(   0, wxT("Andorra"),                                         0);	/* ad */
	ptListCtrl->InsertItem(   1, wxT("United Arab Emirates"),                            1);	/* ae */
	ptListCtrl->InsertItem(   2, wxT("Afghanistan"),                                     2);	/* af */
	ptListCtrl->InsertItem(   3, wxT("Antigua and Barbuda"),                             3);	/* ag */
	ptListCtrl->InsertItem(   4, wxT("Anguilla"),                                        4);	/* ai */
	ptListCtrl->InsertItem(   5, wxT("Albania"),                                         5);	/* al */
	ptListCtrl->InsertItem(   6, wxT("Armenia"),                                         6);	/* am */
	ptListCtrl->InsertItem(   7, wxT("Netherlands Antilles"),                            7);	/* an */
	ptListCtrl->InsertItem(   8, wxT("Angola"),                                          8);	/* ao */
	ptListCtrl->InsertItem(   9, wxT("Argentina"),                                       9);	/* ar */
	ptListCtrl->InsertItem(  10, wxT("American Samoa"),                                 10);	/* as */
	ptListCtrl->InsertItem(  11, wxT("Austria"),                                        11);	/* at */
	ptListCtrl->InsertItem(  12, wxT("Australia"),                                      12);	/* au */
	ptListCtrl->InsertItem(  13, wxT("Aruba"),                                          13);	/* aw */
	ptListCtrl->InsertItem(  14, wxT("Åland Islands"),                                  14);	/* ax */
	ptListCtrl->InsertItem(  15, wxT("Azerbaijan"),                                     15);	/* az */
	ptListCtrl->InsertItem(  16, wxT("Bosnia and Herzegovina"),                         16);	/* ba */
	ptListCtrl->InsertItem(  17, wxT("Barbados"),                                       17);	/* bb */
	ptListCtrl->InsertItem(  18, wxT("Bangladesh"),                                     18);	/* bd */
	ptListCtrl->InsertItem(  19, wxT("Belgium"),                                        19);	/* be */
	ptListCtrl->InsertItem(  20, wxT("Burkina Faso"),                                   20);	/* bf */
	ptListCtrl->InsertItem(  21, wxT("Bulgaria"),                                       21);	/* bg */
	ptListCtrl->InsertItem(  22, wxT("Bahrain"),                                        22);	/* bh */
	ptListCtrl->InsertItem(  23, wxT("Burundi"),                                        23);	/* bi */
	ptListCtrl->InsertItem(  24, wxT("Benin"),                                          24);	/* bj */
	ptListCtrl->InsertItem(  25, wxT("Saint Barthélemy"),                               25);	/* bl */
	ptListCtrl->InsertItem(  26, wxT("Bermuda"),                                        26);	/* bm */
	ptListCtrl->InsertItem(  27, wxT("Brunei Darussalam"),                              27);	/* bn */
	ptListCtrl->InsertItem(  28, wxT("Bolivia, Plurinational State of"),                28);	/* bo */
	ptListCtrl->InsertItem(  29, wxT("Brazil"),                                         29);	/* br */
	ptListCtrl->InsertItem(  30, wxT("Bahamas"),                                        30);	/* bs */
	ptListCtrl->InsertItem(  31, wxT("Bhutan"),                                         31);	/* bt */
	ptListCtrl->InsertItem(  32, wxT("Bouvet Island"),                                  32);	/* bv */
	ptListCtrl->InsertItem(  33, wxT("Botswana"),                                       33);	/* bw */
	ptListCtrl->InsertItem(  34, wxT("Belarus"),                                        34);	/* by */
	ptListCtrl->InsertItem(  35, wxT("Belize"),                                         35);	/* bz */
	ptListCtrl->InsertItem(  36, wxT("Canada"),                                         36);	/* ca */
	ptListCtrl->InsertItem(  37, wxT("Cocos (Keeling) Islands"),                        37);	/* cc */
	ptListCtrl->InsertItem(  38, wxT("Congo, the Democratic Republic of the"),          38);	/* cd */
	ptListCtrl->InsertItem(  39, wxT("Central African Republic"),                       39);	/* cf */
	ptListCtrl->InsertItem(  40, wxT("Congo"),                                          40);	/* cg */
	ptListCtrl->InsertItem(  41, wxT("Switzerland"),                                    41);	/* ch */
	ptListCtrl->InsertItem(  42, wxT("Côte d'Ivoire"),                                  42);	/* ci */
	ptListCtrl->InsertItem(  43, wxT("Cook Islands"),                                   43);	/* ck */
	ptListCtrl->InsertItem(  44, wxT("Chile"),                                          44);	/* cl */
	ptListCtrl->InsertItem(  45, wxT("Cameroon"),                                       45);	/* cm */
	ptListCtrl->InsertItem(  46, wxT("China"),                                          46);	/* cn */
	ptListCtrl->InsertItem(  47, wxT("Colombia"),                                       47);	/* co */
	ptListCtrl->InsertItem(  48, wxT("Costa Rica"),                                     48);	/* cr */
	ptListCtrl->InsertItem(  49, wxT("Cuba"),                                           49);	/* cu */
	ptListCtrl->InsertItem(  50, wxT("Cape Verde"),                                     50);	/* cv */
	ptListCtrl->InsertItem(  51, wxT("Christmas Island"),                               51);	/* cx */
	ptListCtrl->InsertItem(  52, wxT("Cyprus"),                                         52);	/* cy */
	ptListCtrl->InsertItem(  53, wxT("Czech Republic"),                                 53);	/* cz */
	ptListCtrl->InsertItem(  54, wxT("Germany"),                                        54);	/* de */
	ptListCtrl->InsertItem(  55, wxT("Djibouti"),                                       55);	/* dj */
	ptListCtrl->InsertItem(  56, wxT("Denmark"),                                        56);	/* dk */
	ptListCtrl->InsertItem(  57, wxT("Dominica"),                                       57);	/* dm */
	ptListCtrl->InsertItem(  58, wxT("Dominican Republic"),                             58);	/* do */
	ptListCtrl->InsertItem(  59, wxT("Algeria"),                                        59);	/* dz */
	ptListCtrl->InsertItem(  60, wxT("Ecuador"),                                        60);	/* ec */
	ptListCtrl->InsertItem(  61, wxT("Estonia"),                                        61);	/* ee */
	ptListCtrl->InsertItem(  62, wxT("Egypt"),                                          62);	/* eg */
	ptListCtrl->InsertItem(  63, wxT("Western Sahara"),                                 63);	/* eh */
	ptListCtrl->InsertItem(  64, wxT("Eritrea"),                                        64);	/* er */
	ptListCtrl->InsertItem(  65, wxT("Spain"),                                          65);	/* es */
	ptListCtrl->InsertItem(  66, wxT("Ethiopia"),                                       66);	/* et */
	ptListCtrl->InsertItem(  67, wxT("Finland"),                                        67);	/* fi */
	ptListCtrl->InsertItem(  68, wxT("Fiji"),                                           68);	/* fj */
	ptListCtrl->InsertItem(  69, wxT("Falkland Islands (Malvinas)"),                    69);	/* fk */
	ptListCtrl->InsertItem(  70, wxT("Micronesia, Federated States of"),                70);	/* fm */
	ptListCtrl->InsertItem(  71, wxT("Faroe Islands"),                                  71);	/* fo */
	ptListCtrl->InsertItem(  72, wxT("France"),                                         72);	/* fr */
	ptListCtrl->InsertItem(  73, wxT("Gabon"),                                          73);	/* ga */
	ptListCtrl->InsertItem(  74, wxT("United Kingdom"),                                 74);	/* gb */
	ptListCtrl->InsertItem(  75, wxT("Grenada"),                                        75);	/* gd */
	ptListCtrl->InsertItem(  76, wxT("Georgia"),                                        76);	/* ge */
	ptListCtrl->InsertItem(  77, wxT("French Guiana"),                                  77);	/* gf */
	ptListCtrl->InsertItem(  78, wxT("Ghana"),                                          78);	/* gh */
	ptListCtrl->InsertItem(  79, wxT("Gibraltar"),                                      79);	/* gi */
	ptListCtrl->InsertItem(  80, wxT("Greenland"),                                      80);	/* gl */
	ptListCtrl->InsertItem(  81, wxT("Gambia"),                                         81);	/* gm */
	ptListCtrl->InsertItem(  82, wxT("Guinea"),                                         82);	/* gn */
	ptListCtrl->InsertItem(  83, wxT("Guadeloupe"),                                     83);	/* gp */
	ptListCtrl->InsertItem(  84, wxT("Equatorial Guinea"),                              84);	/* gq */
	ptListCtrl->InsertItem(  85, wxT("Greece"),                                         85);	/* gr */
	ptListCtrl->InsertItem(  86, wxT("South Georgia and the South Sandwich Islands"),   86);	/* gs */
	ptListCtrl->InsertItem(  87, wxT("Guatemala"),                                      87);	/* gt */
	ptListCtrl->InsertItem(  88, wxT("Guam"),                                           88);	/* gu */
	ptListCtrl->InsertItem(  89, wxT("Guinea-Bissau"),                                  89);	/* gw */
	ptListCtrl->InsertItem(  90, wxT("Guyana"),                                         90);	/* gy */
	ptListCtrl->InsertItem(  91, wxT("Hong Kong"),                                      91);	/* hk */
	ptListCtrl->InsertItem(  92, wxT("Heard Island and McDonald Islands"),              92);	/* hm */
	ptListCtrl->InsertItem(  93, wxT("Honduras"),                                       93);	/* hn */
	ptListCtrl->InsertItem(  94, wxT("Croatia"),                                        94);	/* hr */
	ptListCtrl->InsertItem(  95, wxT("Haiti"),                                          95);	/* ht */
	ptListCtrl->InsertItem(  96, wxT("Hungary"),                                        96);	/* hu */
	ptListCtrl->InsertItem(  97, wxT("Indonesia"),                                      97);	/* id */
	ptListCtrl->InsertItem(  98, wxT("Ireland"),                                        98);	/* ie */
	ptListCtrl->InsertItem(  99, wxT("Israel"),                                         99);	/* il */
	ptListCtrl->InsertItem( 100, wxT("India"),                                         100);	/* in */
	ptListCtrl->InsertItem( 101, wxT("British Indian Ocean Territory"),                101);	/* io */
	ptListCtrl->InsertItem( 102, wxT("Iraq"),                                          102);	/* iq */
	ptListCtrl->InsertItem( 103, wxT("Iran, Islamic Republic of"),                     103);	/* ir */
	ptListCtrl->InsertItem( 104, wxT("Iceland"),                                       104);	/* is */
	ptListCtrl->InsertItem( 105, wxT("Italy"),                                         105);	/* it */
	ptListCtrl->InsertItem( 106, wxT("Jamaica"),                                       106);	/* jm */
	ptListCtrl->InsertItem( 107, wxT("Jordan"),                                        107);	/* jo */
	ptListCtrl->InsertItem( 108, wxT("Japan"),                                         108);	/* jp */
	ptListCtrl->InsertItem( 109, wxT("Kenya"),                                         109);	/* ke */
	ptListCtrl->InsertItem( 110, wxT("Kyrgyzstan"),                                    110);	/* kg */
	ptListCtrl->InsertItem( 111, wxT("Cambodia"),                                      111);	/* kh */
	ptListCtrl->InsertItem( 112, wxT("Kiribati"),                                      112);	/* ki */
	ptListCtrl->InsertItem( 113, wxT("Comoros"),                                       113);	/* km */
	ptListCtrl->InsertItem( 114, wxT("Saint Kitts and Nevis"),                         114);	/* kn */
	ptListCtrl->InsertItem( 115, wxT("Korea, Democratic People's Republic of"),        115);	/* kp */
	ptListCtrl->InsertItem( 116, wxT("Korea, Republic of"),                            116);	/* kr */
	ptListCtrl->InsertItem( 117, wxT("Kuwait"),                                        117);	/* kw */
	ptListCtrl->InsertItem( 118, wxT("Cayman Islands"),                                118);	/* ky */
	ptListCtrl->InsertItem( 119, wxT("Kazakhstan"),                                    119);	/* kz */
	ptListCtrl->InsertItem( 120, wxT("Lao People's Democratic Republic"),              120);	/* la */
	ptListCtrl->InsertItem( 121, wxT("Lebanon"),                                       121);	/* lb */
	ptListCtrl->InsertItem( 122, wxT("Saint Lucia"),                                   122);	/* lc */
	ptListCtrl->InsertItem( 123, wxT("Liechtenstein"),                                 123);	/* li */
	ptListCtrl->InsertItem( 124, wxT("Sri Lanka"),                                     124);	/* lk */
	ptListCtrl->InsertItem( 125, wxT("Liberia"),                                       125);	/* lr */
	ptListCtrl->InsertItem( 126, wxT("Lesotho"),                                       126);	/* ls */
	ptListCtrl->InsertItem( 127, wxT("Lithuania"),                                     127);	/* lt */
	ptListCtrl->InsertItem( 128, wxT("Luxembourg"),                                    128);	/* lu */
	ptListCtrl->InsertItem( 129, wxT("Latvia"),                                        129);	/* lv */
	ptListCtrl->InsertItem( 130, wxT("Libyan Arab Jamahiriya"),                        130);	/* ly */
	ptListCtrl->InsertItem( 131, wxT("Morocco"),                                       131);	/* ma */
	ptListCtrl->InsertItem( 132, wxT("Monaco"),                                        132);	/* mc */
	ptListCtrl->InsertItem( 133, wxT("Moldova, Republic of"),                          133);	/* md */
	ptListCtrl->InsertItem( 134, wxT("Montenegro"),                                    134);	/* me */
	ptListCtrl->InsertItem( 135, wxT("Madagascar"),                                    135);	/* mg */
	ptListCtrl->InsertItem( 136, wxT("Marshall Islands"),                              136);	/* mh */
	ptListCtrl->InsertItem( 137, wxT("Macedonia, the former Yugoslav Republic of"),    137);	/* mk */
	ptListCtrl->InsertItem( 138, wxT("Mali"),                                          138);	/* ml */
	ptListCtrl->InsertItem( 139, wxT("Myanmar"),                                       139);	/* mm */
	ptListCtrl->InsertItem( 140, wxT("Mongolia"),                                      140);	/* mn */
	ptListCtrl->InsertItem( 141, wxT("Macao"),                                         141);	/* mo */
	ptListCtrl->InsertItem( 142, wxT("Northern Mariana Islands"),                      142);	/* mp */
	ptListCtrl->InsertItem( 143, wxT("Martinique"),                                    143);	/* mq */
	ptListCtrl->InsertItem( 144, wxT("Mauritania"),                                    144);	/* mr */
	ptListCtrl->InsertItem( 145, wxT("Montserrat"),                                    145);	/* ms */
	ptListCtrl->InsertItem( 146, wxT("Malta"),                                         146);	/* mt */
	ptListCtrl->InsertItem( 147, wxT("Mauritius"),                                     147);	/* mu */
	ptListCtrl->InsertItem( 148, wxT("Maldives"),                                      148);	/* mv */
	ptListCtrl->InsertItem( 149, wxT("Malawi"),                                        149);	/* mw */
	ptListCtrl->InsertItem( 150, wxT("Mexico"),                                        150);	/* mx */
	ptListCtrl->InsertItem( 151, wxT("Malaysia"),                                      151);	/* my */
	ptListCtrl->InsertItem( 152, wxT("Mozambique"),                                    152);	/* mz */
	ptListCtrl->InsertItem( 153, wxT("Namibia"),                                       153);	/* na */
	ptListCtrl->InsertItem( 154, wxT("New Caledonia"),                                 154);	/* nc */
	ptListCtrl->InsertItem( 155, wxT("Niger"),                                         155);	/* ne */
	ptListCtrl->InsertItem( 156, wxT("Norfolk Island"),                                156);	/* nf */
	ptListCtrl->InsertItem( 157, wxT("Nigeria"),                                       157);	/* ng */
	ptListCtrl->InsertItem( 158, wxT("Nicaragua"),                                     158);	/* ni */
	ptListCtrl->InsertItem( 159, wxT("Netherlands"),                                   159);	/* nl */
	ptListCtrl->InsertItem( 160, wxT("Norway"),                                        160);	/* no */
	ptListCtrl->InsertItem( 161, wxT("Nepal"),                                         161);	/* np */
	ptListCtrl->InsertItem( 162, wxT("Nauru"),                                         162);	/* nr */
	ptListCtrl->InsertItem( 163, wxT("Niue"),                                          163);	/* nu */
	ptListCtrl->InsertItem( 164, wxT("New Zealand"),                                   164);	/* nz */
	ptListCtrl->InsertItem( 165, wxT("Oman"),                                          165);	/* om */
	ptListCtrl->InsertItem( 166, wxT("Panama"),                                        166);	/* pa */
	ptListCtrl->InsertItem( 167, wxT("Peru"),                                          167);	/* pe */
	ptListCtrl->InsertItem( 168, wxT("French Polynesia"),                              168);	/* pf */
	ptListCtrl->InsertItem( 169, wxT("Papua New Guinea"),                              169);	/* pg */
	ptListCtrl->InsertItem( 170, wxT("Philippines"),                                   170);	/* ph */
	ptListCtrl->InsertItem( 171, wxT("Pakistan"),                                      171);	/* pk */
	ptListCtrl->InsertItem( 172, wxT("Poland"),                                        172);	/* pl */
	ptListCtrl->InsertItem( 173, wxT("Saint Pierre and Miquelon"),                     173);	/* pm */
	ptListCtrl->InsertItem( 174, wxT("Pitcairn"),                                      174);	/* pn */
	ptListCtrl->InsertItem( 175, wxT("Puerto Rico"),                                   175);	/* pr */
	ptListCtrl->InsertItem( 176, wxT("Palestinian Territory, Occupied"),               176);	/* ps */
	ptListCtrl->InsertItem( 177, wxT("Portugal"),                                      177);	/* pt */
	ptListCtrl->InsertItem( 178, wxT("Palau"),                                         178);	/* pw */
	ptListCtrl->InsertItem( 179, wxT("Paraguay"),                                      179);	/* py */
	ptListCtrl->InsertItem( 180, wxT("Qatar"),                                         180);	/* qa */
	ptListCtrl->InsertItem( 181, wxT("Réunion"),                                       181);	/* re */
	ptListCtrl->InsertItem( 182, wxT("Romania"),                                       182);	/* ro */
	ptListCtrl->InsertItem( 183, wxT("Serbia"),                                        183);	/* rs */
	ptListCtrl->InsertItem( 184, wxT("Russian Federation"),                            184);	/* ru */
	ptListCtrl->InsertItem( 185, wxT("Rwanda"),                                        185);	/* rw */
	ptListCtrl->InsertItem( 186, wxT("Saudi Arabia"),                                  186);	/* sa */
	ptListCtrl->InsertItem( 187, wxT("Solomon Islands"),                               187);	/* sb */
	ptListCtrl->InsertItem( 188, wxT("Seychelles"),                                    188);	/* sc */
	ptListCtrl->InsertItem( 189, wxT("Sudan"),                                         189);	/* sd */
	ptListCtrl->InsertItem( 190, wxT("Sweden"),                                        190);	/* se */
	ptListCtrl->InsertItem( 191, wxT("Singapore"),                                     191);	/* sg */
	ptListCtrl->InsertItem( 192, wxT("Saint Helena, Ascension and Tristan da Cunha"),  192);	/* sh */
	ptListCtrl->InsertItem( 193, wxT("Slovenia"),                                      193);	/* si */
	ptListCtrl->InsertItem( 194, wxT("Svalbard and Jan Mayen"),                        194);	/* sj */
	ptListCtrl->InsertItem( 195, wxT("Slovakia"),                                      195);	/* sk */
	ptListCtrl->InsertItem( 196, wxT("Sierra Leone"),                                  196);	/* sl */
	ptListCtrl->InsertItem( 197, wxT("San Marino"),                                    197);	/* sm */
	ptListCtrl->InsertItem( 198, wxT("Senegal"),                                       198);	/* sn */
	ptListCtrl->InsertItem( 199, wxT("Somalia"),                                       199);	/* so */
	ptListCtrl->InsertItem( 200, wxT("Suriname"),                                      200);	/* sr */
	ptListCtrl->InsertItem( 201, wxT("Sao Tome and Principe"),                         201);	/* st */
	ptListCtrl->InsertItem( 202, wxT("El Salvador"),                                   202);	/* sv */
	ptListCtrl->InsertItem( 203, wxT("Syrian Arab Republic"),                          203);	/* sy */
	ptListCtrl->InsertItem( 204, wxT("Swaziland"),                                     204);	/* sz */
	ptListCtrl->InsertItem( 205, wxT("Turks and Caicos Islands"),                      205);	/* tc */
	ptListCtrl->InsertItem( 206, wxT("Chad"),                                          206);	/* td */
	ptListCtrl->InsertItem( 207, wxT("French Southern Territories"),                   207);	/* tf */
	ptListCtrl->InsertItem( 208, wxT("Togo"),                                          208);	/* tg */
	ptListCtrl->InsertItem( 209, wxT("Thailand"),                                      209);	/* th */
	ptListCtrl->InsertItem( 210, wxT("Tajikistan"),                                    210);	/* tj */
	ptListCtrl->InsertItem( 211, wxT("Tokelau"),                                       211);	/* tk */
	ptListCtrl->InsertItem( 212, wxT("Timor-Leste"),                                   212);	/* tl */
	ptListCtrl->InsertItem( 213, wxT("Turkmenistan"),                                  213);	/* tm */
	ptListCtrl->InsertItem( 214, wxT("Tunisia"),                                       214);	/* tn */
	ptListCtrl->InsertItem( 215, wxT("Tonga"),                                         215);	/* to */
	ptListCtrl->InsertItem( 216, wxT("Turkey"),                                        216);	/* tr */
	ptListCtrl->InsertItem( 217, wxT("Trinidad and Tobago"),                           217);	/* tt */
	ptListCtrl->InsertItem( 218, wxT("Tuvalu"),                                        218);	/* tv */
	ptListCtrl->InsertItem( 219, wxT("Taiwan, Province of China"),                     219);	/* tw */
	ptListCtrl->InsertItem( 220, wxT("Tanzania, United Republic of"),                  220);	/* tz */
	ptListCtrl->InsertItem( 221, wxT("Ukraine"),                                       221);	/* ua */
	ptListCtrl->InsertItem( 222, wxT("Uganda"),                                        222);	/* ug */
	ptListCtrl->InsertItem( 223, wxT("United States Minor Outlying Islands"),          223);	/* um */
	ptListCtrl->InsertItem( 224, wxT("United States"),                                 224);	/* us */
	ptListCtrl->InsertItem( 225, wxT("Uruguay"),                                       225);	/* uy */
	ptListCtrl->InsertItem( 226, wxT("Uzbekistan"),                                    226);	/* uz */
	ptListCtrl->InsertItem( 227, wxT("Holy See (Vatican City State)"),                 227);	/* va */
	ptListCtrl->InsertItem( 228, wxT("Saint Vincent and the Grenadines"),              228);	/* vc */
	ptListCtrl->InsertItem( 229, wxT("Venezuela, Bolivarian Republic of"),             229);	/* ve */
	ptListCtrl->InsertItem( 230, wxT("Virgin Islands, British"),                       230);	/* vg */
	ptListCtrl->InsertItem( 231, wxT("Virgin Islands, U.S."),                          231);	/* vi */
	ptListCtrl->InsertItem( 232, wxT("Viet Nam"),                                      232);	/* vn */
	ptListCtrl->InsertItem( 233, wxT("Vanuatu"),                                       233);	/* vu */
	ptListCtrl->InsertItem( 234, wxT("Wallis and Futuna"),                             234);	/* wf */
	ptListCtrl->InsertItem( 235, wxT("Samoa"),                                         235);	/* ws */
	ptListCtrl->InsertItem( 236, wxT("Yemen"),                                         236);	/* ye */
	ptListCtrl->InsertItem( 237, wxT("Mayotte"),                                       237);	/* yt */
	ptListCtrl->InsertItem( 238, wxT("South Africa"),                                  238);	/* za */
	ptListCtrl->InsertItem( 239, wxT("Zambia"),                                        239);	/* zm */
	ptListCtrl->InsertItem( 240, wxT("Zimbabwe"),                                      240);	/* zw */

	ptListCtrl->SetColumnWidth(0, wxLIST_AUTOSIZE);
	ptLanguageBox->Add(ptListCtrl, 1, wxEXPAND);

	ptNote = new wxStaticText(ptPanel, wxID_ANY, _("Please note:\nChanging the application language requires a restart to take effect."));
	ptMainSizer->Add(ptNote, 0, wxEXPAND);

	return ptPanel;
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
