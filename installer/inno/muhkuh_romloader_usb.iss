
[Components]
Name: plugin_romloader_usb;     Description: "Romloader usb plugin";     Types: full


[Files]
Source: bin\Microsoft.VC80.CRT\*;                                DestDir: {app}\application\plugins\Microsoft.VC80.CRT; Components: plugin_romloader_usb
Source: "bin\bootloader_usb.dll";                                DestDir: "{app}\application\plugins";  Components: plugin_romloader_usb
;Source: "bin\bootloader_usb.dll.manifest";                       DestDir: "{app}\application\plugins";  Components: plugin_romloader_usb
Source: "bin\bootloader_usb.xml";                                DestDir: "{app}\application\plugins";  Components: plugin_romloader_usb
Source: "plugins\romloader\usb\win\driver\libusb0.dll";          DestDir: "{app}\application";          Components: plugin_romloader_usb
Source: "plugins\romloader\usb\win\driver\libusb0.dll";          DestDir: "{app}\driver\romloader_usb"; Components: plugin_romloader_usb
Source: "plugins\romloader\usb\win\driver\libusb0.sys";          DestDir: "{app}\driver\romloader_usb"; Components: plugin_romloader_usb
Source: "plugins\romloader\usb\win\driver\netx_bootmonitor.cat"; DestDir: "{app}\driver\romloader_usb"; Components: plugin_romloader_usb
Source: "plugins\romloader\usb\win\driver\netx_bootmonitor.inf"; DestDir: "{app}\driver\romloader_usb"; Components: plugin_romloader_usb

