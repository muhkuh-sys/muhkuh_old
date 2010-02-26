
[Components]
Name: plugin_romloader_baka;    Description: "Romloader baka plugin";    Types: full


[Files]
Source: bin\Microsoft.VC80.CRT\*; DestDir: {app}\application\plugins\Microsoft.VC80.CRT; Components: plugin_romloader_baka
Source: "bin\baka.dll"; DestDir: "{app}\application\plugins"; Components: plugin_romloader_baka
Source: "bin\baka.xml"; DestDir: "{app}\application\plugins"; Components: plugin_romloader_baka

