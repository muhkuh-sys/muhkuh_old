/***************************************************************************
 *   Copyright (C) 2008 by Christoph Thelen                                *
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


#include "serverkuh_version.h"
#include "serverkuh_app.h"

#include <wx/filesys.h>
#include <wx/fs_inet.h>
#include <wx/fs_zip.h>
#include <wx/socket.h>
#include <wx/stdpaths.h>

//-------------------------------------
// Serverkuh needs several wx features

#if !wxUSE_THREADS
	#error "Serverkuh requires thread support!"
#endif

#if !wxUSE_STREAMS
	#error "Serverkuh requires stream support!"
#endif

#if !wxUSE_ZIPSTREAM
	#error "Serverkuh requires zipstream support!"
#endif

#if !wxUSE_ZLIB
	#error "Serverkuh requires zlib support!"
#endif

//-------------------------------------

IMPLEMENT_APP(serverkuh_app)


const wxCmdLineEntryDesc cmdLineDesc[] =
{
	{ wxCMD_LINE_OPTION, wxT("c"), wxT("config-file"), _T("use the specified configuration file instead of the default Muhkuh.cfg"), wxCMD_LINE_VAL_STRING },

	{ wxCMD_LINE_OPTION, wxT("v"), wxT("verbose"), _T("set verbose level"), wxCMD_LINE_VAL_NUMBER },
	{ wxCMD_LINE_SWITCH, wxT("h"), wxT("help"), _T("display this help and exit.")},
	{ wxCMD_LINE_OPTION, wxT("i"), wxT("index"), _T("execute single subtest"), wxCMD_LINE_VAL_NUMBER },
	{ wxCMD_LINE_OPTION, wxT("d"), wxT("debug"), _T("set debug server"), wxCMD_LINE_VAL_STRING },

	{ wxCMD_LINE_SWITCH, NULL, wxT("version"), _T("display version information and exit.") },

  { wxCMD_LINE_PARAM,  NULL, NULL, _T("test"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL},
	{ wxCMD_LINE_NONE }
};

const wxChar cmdLineLogo[] =
	wxT("serverkuh is .... ")
	wxT("\n");


void serverkuh_app::showHelp(wxCmdLineParser *ptParser)
{
	// show general usage
	ptParser->Usage();

	wxLogMessage(
		wxT("\n")
		wxT("Example:\n")
		wxT("  serverkuh -c serverkuh.cfg -i 0\n")
		wxT("      \"file://d%%3a/Muhkuh/bootwizard/test_description.xml\"\n")
		wxT("\n")
		wxT("Muhkuh home page: www.sf.net/projects/muhkuh\n")
	);
}


void serverkuh_app::showVersion(void)
{
  wxLogMessage(
	wxT(SERVERKUH_APPLICATION_NAME) wxT(" ") wxT(SERVERKUH_VERSION_STRING) wxT("\n")
	wxT("Copyright (c) 2008, the Muhkuh team.\n")
	wxT("There is NO warranty.  You may redistribute this software\n")
	wxT("under the terms of the GNU General Public License.\n")
	wxT("For more information about these matters, see the files named COPYING.\n")
  );
}

bool serverkuh_app::OnInit()
{
	int iRes;
	serverkuh_mainFrame *mainframe;
	wxCmdLineParser tParser;
	bool fOk;
	int argcServerkuh;
	wxString strArg;


	mainframe = NULL;

	wxSocketBase::Initialize();

	wxInitAllImageHandlers();
	// add virtual filesystem for zip archives
	wxFileSystem::AddHandler(new wxZipFSHandler);
	// add virtual filesystem for http and ftp
	wxFileSystem::AddHandler(new wxInternetFSHandler);

	// ... from wxLua demo application
#ifdef __WXGTK__
	// this call is very important since otherwise scripts using the decimal
	// point '.' could not work with those locales which use a different symbol
	// (e.g. the comma) for the decimal point...
	// It doesn't work to put os.setlocale('c', 'numeric') in the Lua file that
	// you want to use decimal points in. That's because the file has been lexed
	// and compiler before the locale has changed, so the lexer - the part that
	// recognises numbers - will use the old locale.
	setlocale(LC_NUMERIC, "C");
#endif

	// set application and vendor name
	SetVendorName(wxT(SERVERKUH_APPLICATION_NAME) wxT(" team"));
	SetAppName(wxT(SERVERKUH_APPLICATION_NAME));

	// if the command line contains a "--", we want to pass
	// everything before it Serverkuh command line parser,
	// and everything behind it to the Lua script
	argcServerkuh = 0;
	while(argcServerkuh<argc)
	{
		strArg = argv[argcServerkuh];
		if( strArg.Cmp(wxT("--"))==0 )
		{
			break;
		}
		++argcServerkuh;
	}

	tParser.SetCmdLine(argcServerkuh, argv);
	tParser.SetSwitchChars(wxT("-"));
	tParser.EnableLongOptions(true);
	tParser.SetDesc(cmdLineDesc);
	tParser.SetLogo(cmdLineLogo);

	iRes = tParser.Parse(false);

	if( iRes!=0 )
	{
		showHelp(&tParser);
		fOk = false;
	}
	else if( tParser.Found(wxT("h"))==true )
	{
		showHelp(&tParser);
		fOk = false;
	}
	else if( tParser.Found(wxT("version"))==true )
	{
		showVersion();
		fOk = false;
	}
	else if( tParser.GetParamCount()==0 )
	{
		showHelp(&tParser);
	}
	else
	{
		// create the muhkuh main frame
		mainframe = new serverkuh_mainFrame(&tParser);

		// set the parameters for Lua scripts
		if( (argcServerkuh+1)<argc)
		{
			mainframe->setLuaArgs(argv+argcServerkuh+1, argc-(argcServerkuh+1));
		}

		// show the frame
		mainframe->Show(true);
		SetTopWindow(mainframe);
		fOk = true;
	}

	return fOk;
}


int serverkuh_app::OnExit()
{
	/*
	wxConfigBase *ptConfig;

	ptConfig = wxConfigBase::Set(NULL);
	if( ptConfig!=NULL )
	{
		delete ptConfig;
	}
	*/
	return 0;
}
