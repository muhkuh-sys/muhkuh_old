; Muhkuh Inno Setup Cfg

#define AppName "Muhkuh"
#define AppVersion GetFileVersion("..\..\bin\muhkuh.exe")
#define AppVerName AppName+" "+AppVersion
#define InstallerName "muhkuh_"+AppVersion+"_setup"

#define SourceDir "..\.."
#define OutputDir "."

; include the common muhkuh settings
#include "muhkuh_common.iss"


[Setup]
DefaultDirName={pf}\{#AppName}
DefaultGroupName={#AppName}

; misc info
AppPublisher=Muhkuh team
AppPublisherURL=http://muhkuh.sourceforge.net/
AppSupportURL=http://www.sourceforge.net/projects/muhkuh
AppUpdatesURL=http://www.sourceforge.net/projects/muhkuh
VersionInfoCopyright=Copyright (C) 2010, Muhkuh team
AppCopyright=Copyright (C) 2010, Muhkuh team

InfoBeforeFile=changelog.txt
LicenseFile=docs\gpl-2.0.txt

; icon stuff
SetupIconFile=icons\custom\muhkuh.ico

[Messages]
BeveledLabel=Muhkuh

[Files]
Source: docs\gpl-2.0.txt; DestDir: {app}\docs; Components: muhkuh

[Tasks]
Name: associate; Description: &Associate .mtd files with the Muhkuh application; GroupDescription: File associations:

[Registry]
Root: HKCR; Subkey: .mtd; ValueType: string; ValueName: ; ValueData: MuhkuhTestDesctiption; Flags: uninsdeletevalue
Root: HKCR; Subkey: MuhkuhTestDesctiption; ValueType: string; ValueName: ; ValueData: Muhkuh Test Description; Flags: uninsdeletekey
Root: HKCR; Subkey: MuhkuhTestDesctiption\DefaultIcon; ValueType: string; ValueName: ; ValueData: {app}\application\serverkuh.exe,0
Root: HKCR; Subkey: MuhkuhTestDesctiption\shell\open\command; ValueType: string; ValueName: ; ValueData: """{app}\application\serverkuh.exe"" ""-c"" ""{app}\application\Muhkuh.cfg"" ""-i"" ""0"" ""%1#zip:test_description.xml"""

; include the components
#include "muhkuh_app.iss"
#include "muhkuh_romloader_baka.iss"
#include "muhkuh_romloader_uart.iss"
#include "muhkuh_romloader_usb.iss"
#include "muhkuh_romloader_openocd.iss"
#include "lua_scripts.iss"
;#include "muhkuh_demos.iss"

; create muhkuh icons
#include "muhkuh_icons.iss"
