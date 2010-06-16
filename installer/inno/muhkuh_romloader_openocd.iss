
[Components]
Name: plugin_romloader_openocd; Description: "Romloader openocd plugin"; Types: full


[Files]
Source: bin\Microsoft.VC80.CRT\*; DestDir: {app}\application\plugins\Microsoft.VC80.CRT; Components: plugin_romloader_openocd
Source: "bin\openocd.dll";                     DestDir: "{app}\application\plugins"; Components: plugin_romloader_openocd
;Source: "bin\openocd.dll.manifest";            DestDir: "{app}\application\plugins"; Components: plugin_romloader_openocd
;Source: "bin\openocd_netx50_jtagkey_win.xml";  DestDir: "{app}\application\plugins"; Components: plugin_romloader_openocd
;Source: "bin\openocd_netx500_jtagkey_win.xml"; DestDir: "{app}\application\plugins"; Components: plugin_romloader_openocd
;Source: "bin\openocd_nxhx50_win.xml";          DestDir: "{app}\application\plugins"; Components: plugin_romloader_openocd
Source: "bin\openocd_win.xml";                 DestDir: "{app}\application\plugins"; Components: plugin_romloader_openocd
Source: "bin\plugins\openocd\*";               DestDir: "{app}\application\plugins\openocd"; Components: plugin_romloader_openocd
