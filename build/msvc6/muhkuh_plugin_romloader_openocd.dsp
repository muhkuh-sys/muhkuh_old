# Microsoft Developer Studio Project File - Name="muhkuh_plugin_romloader_openocd" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=plugin_romloader_openocd - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "muhkuh_plugin_romloader_openocd.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "muhkuh_plugin_romloader_openocd.mak" CFG="plugin_romloader_openocd - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "plugin_romloader_openocd - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "plugin_romloader_openocd - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "plugin_romloader_openocd - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\bin"
# PROP BASE Intermediate_Dir "vcmsw_dll\plugin_romloader_openocd"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\bin"
# PROP Intermediate_Dir "vcmsw_dll\plugin_romloader_openocd"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W1 /GR /EHsc /I "$(WXWIN)\lib\vc_dll\msw" /I "$(WXWIN)\include" /O2 /I "..\..\modules" /I ".\..\.." /I "..\..\..\modules\lua\include" /Fd..\..\bin\openocd.pdb /I "..\..\plugins\romloader\openocd" /I "..\..\plugins\romloader\openocd\openocd" /I "..\..\plugins\romloader\openocd\openocd\helper" /I "..\..\plugins\romloader\openocd\openocd\jtag" /I "..\..\plugins\romloader\openocd\openocd\target" /I "..\..\plugins\romloader\openocd\openocd\xsvf" /I "..\..\plugins\romloader\openocd\win\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "WXUSINGDLL" /D "__WXMSW__" /D "HAVE_CONFIG_H" /D PKGLIBDIR=\".\" /c
# ADD CPP /nologo /FD /MD /W1 /GR /EHsc /I "$(WXWIN)\lib\vc_dll\msw" /I "$(WXWIN)\include" /O2 /I "..\..\modules" /I ".\..\.." /I "..\..\..\modules\lua\include" /Fd..\..\bin\openocd.pdb /I "..\..\plugins\romloader\openocd" /I "..\..\plugins\romloader\openocd\openocd" /I "..\..\plugins\romloader\openocd\openocd\helper" /I "..\..\plugins\romloader\openocd\openocd\jtag" /I "..\..\plugins\romloader\openocd\openocd\target" /I "..\..\plugins\romloader\openocd\openocd\xsvf" /I "..\..\plugins\romloader\openocd\win\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "WXUSINGDLL" /D "__WXMSW__" /D "HAVE_CONFIG_H" /D PKGLIBDIR=\".\" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "WXUSINGDLL" /D "__WXMSW__" /D "HAVE_CONFIG_H" /D PKGLIBDIR=\".\" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "WXUSINGDLL" /D "__WXMSW__" /D "HAVE_CONFIG_H" /D PKGLIBDIR=\".\" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "WXUSINGDLL" /d "__WXMSW__" /i "$(WXWIN)\lib\vc_dll\msw" /i "$(WXWIN)\include" /i "..\..\modules" /i ".\..\.." /i "..\..\..\modules\lua\include" /i "..\..\plugins\romloader\openocd" /i "..\..\plugins\romloader\openocd\openocd" /i "..\..\plugins\romloader\openocd\openocd\helper" /i "..\..\plugins\romloader\openocd\openocd\jtag" /i "..\..\plugins\romloader\openocd\openocd\target" /i "..\..\plugins\romloader\openocd\openocd\xsvf" /d "HAVE_CONFIG_H" /d PKGLIBDIR="." /i ..\..\plugins\romloader\openocd\win\include
# ADD RSC /l 0x409 /d "WXUSINGDLL" /d "__WXMSW__" /i "$(WXWIN)\lib\vc_dll\msw" /i "$(WXWIN)\include" /i "..\..\modules" /i ".\..\.." /i "..\..\..\modules\lua\include" /i "..\..\plugins\romloader\openocd" /i "..\..\plugins\romloader\openocd\openocd" /i "..\..\plugins\romloader\openocd\openocd\helper" /i "..\..\plugins\romloader\openocd\openocd\jtag" /i "..\..\plugins\romloader\openocd\openocd\target" /i "..\..\plugins\romloader\openocd\openocd\xsvf" /d "HAVE_CONFIG_H" /d PKGLIBDIR="." /i ..\..\plugins\romloader\openocd\win\include
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 libftdi.lib wxlua_msw28_wxluasocket.lib wxlua_msw28_wxluadebug.lib wxlua_msw28_wxbindxrc.lib wxlua_msw28_wxbindhtml.lib wxlua_msw28_wxbindadv.lib wxlua_msw28_wxbindaui.lib wxlua_msw28_wxbindnet.lib wxlua_msw28_wxbindxml.lib wxlua_msw28_wxbindcore.lib wxlua_msw28_wxbindbase.lib wxlua_msw28_wxlua.lib lua5.1.lib wxmsw28_xrc.lib wxmsw28_html.lib wxmsw28_adv.lib wxmsw28_aui.lib wxbase28_net.lib wxbase28_xml.lib wxmsw28_core.lib wxbase28.lib /nologo /dll /machine:i386 /out:"..\..\bin\openocd.dll" /pdb:"..\..\bin\openocd.pdb" /libpath:"..\..\plugins\romloader\openocd\win\lib\msvc"
# ADD LINK32 libftdi.lib wxlua_msw28_wxluasocket.lib wxlua_msw28_wxluadebug.lib wxlua_msw28_wxbindxrc.lib wxlua_msw28_wxbindhtml.lib wxlua_msw28_wxbindadv.lib wxlua_msw28_wxbindaui.lib wxlua_msw28_wxbindnet.lib wxlua_msw28_wxbindxml.lib wxlua_msw28_wxbindcore.lib wxlua_msw28_wxbindbase.lib wxlua_msw28_wxlua.lib lua5.1.lib wxmsw28_xrc.lib wxmsw28_html.lib wxmsw28_adv.lib wxmsw28_aui.lib wxbase28_net.lib wxbase28_xml.lib wxmsw28_core.lib wxbase28.lib /nologo /dll /machine:i386 /out:"..\..\bin\openocd.dll" /pdb:"..\..\bin\openocd.pdb" /libpath:"..\..\plugins\romloader\openocd\win\lib\msvc"

!ELSEIF  "$(CFG)" == "plugin_romloader_openocd - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\bin"
# PROP BASE Intermediate_Dir "vcmswd_dll\plugin_romloader_openocd"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\bin"
# PROP Intermediate_Dir "vcmswd_dll\plugin_romloader_openocd"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W1 /GR /EHsc /I "$(WXWIN)\lib\vc_dll\mswd" /I "$(WXWIN)\include" /Od /I "..\..\modules" /I ".\..\.." /I "..\..\..\modules\lua\include" /Zi /Gm /GZ /Fd..\..\bin\openocd.pdb /I "..\..\plugins\romloader\openocd" /I "..\..\plugins\romloader\openocd\openocd" /I "..\..\plugins\romloader\openocd\openocd\helper" /I "..\..\plugins\romloader\openocd\openocd\jtag" /I "..\..\plugins\romloader\openocd\openocd\target" /I "..\..\plugins\romloader\openocd\openocd\xsvf" /I "..\..\plugins\romloader\openocd\win\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "WXUSINGDLL" /D "__WXDEBUG__" /D "__WXMSW__" /D "_DEBUG" /D "HAVE_CONFIG_H" /D PKGLIBDIR=\".\" /c
# ADD CPP /nologo /FD /MDd /W1 /GR /EHsc /I "$(WXWIN)\lib\vc_dll\mswd" /I "$(WXWIN)\include" /Od /I "..\..\modules" /I ".\..\.." /I "..\..\..\modules\lua\include" /Zi /Gm /GZ /Fd..\..\bin\openocd.pdb /I "..\..\plugins\romloader\openocd" /I "..\..\plugins\romloader\openocd\openocd" /I "..\..\plugins\romloader\openocd\openocd\helper" /I "..\..\plugins\romloader\openocd\openocd\jtag" /I "..\..\plugins\romloader\openocd\openocd\target" /I "..\..\plugins\romloader\openocd\openocd\xsvf" /I "..\..\plugins\romloader\openocd\win\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "WXUSINGDLL" /D "__WXDEBUG__" /D "__WXMSW__" /D "_DEBUG" /D "HAVE_CONFIG_H" /D PKGLIBDIR=\".\" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "WXUSINGDLL" /D "__WXDEBUG__" /D "__WXMSW__" /D "_DEBUG" /D "HAVE_CONFIG_H" /D PKGLIBDIR=\".\" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "WXUSINGDLL" /D "__WXDEBUG__" /D "__WXMSW__" /D "_DEBUG" /D "HAVE_CONFIG_H" /D PKGLIBDIR=\".\" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "WXUSINGDLL" /d "__WXDEBUG__" /d "__WXMSW__" /i "$(WXWIN)\lib\vc_dll\mswd" /i "$(WXWIN)\include" /i "..\..\modules" /i ".\..\.." /i "..\..\..\modules\lua\include" /d "_DEBUG" /i "..\..\plugins\romloader\openocd" /i "..\..\plugins\romloader\openocd\openocd" /i "..\..\plugins\romloader\openocd\openocd\helper" /i "..\..\plugins\romloader\openocd\openocd\jtag" /i "..\..\plugins\romloader\openocd\openocd\target" /i "..\..\plugins\romloader\openocd\openocd\xsvf" /d "HAVE_CONFIG_H" /d PKGLIBDIR="." /i ..\..\plugins\romloader\openocd\win\include
# ADD RSC /l 0x409 /d "WXUSINGDLL" /d "__WXDEBUG__" /d "__WXMSW__" /i "$(WXWIN)\lib\vc_dll\mswd" /i "$(WXWIN)\include" /i "..\..\modules" /i ".\..\.." /i "..\..\..\modules\lua\include" /d "_DEBUG" /i "..\..\plugins\romloader\openocd" /i "..\..\plugins\romloader\openocd\openocd" /i "..\..\plugins\romloader\openocd\openocd\helper" /i "..\..\plugins\romloader\openocd\openocd\jtag" /i "..\..\plugins\romloader\openocd\openocd\target" /i "..\..\plugins\romloader\openocd\openocd\xsvf" /d "HAVE_CONFIG_H" /d PKGLIBDIR="." /i ..\..\plugins\romloader\openocd\win\include
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 libftdi.lib wxlua_msw28d_wxluasocket.lib wxlua_msw28d_wxluadebug.lib wxlua_msw28d_wxbindxrc.lib wxlua_msw28d_wxbindhtml.lib wxlua_msw28d_wxbindadv.lib wxlua_msw28d_wxbindaui.lib wxlua_msw28d_wxbindnet.lib wxlua_msw28d_wxbindxml.lib wxlua_msw28d_wxbindcore.lib wxlua_msw28d_wxbindbase.lib wxlua_msw28d_wxlua.lib lua5.1.lib wxmsw28d_xrc.lib wxmsw28d_html.lib wxmsw28d_adv.lib wxmsw28d_aui.lib wxbase28d_net.lib wxbase28d_xml.lib wxmsw28d_core.lib wxbase28d.lib /nologo /dll /machine:i386 /out:"..\..\bin\openocd.dll" /debug /pdb:"..\..\bin\openocd.pdb" /libpath:"..\..\plugins\romloader\openocd\win\lib\msvc"
# ADD LINK32 libftdi.lib wxlua_msw28d_wxluasocket.lib wxlua_msw28d_wxluadebug.lib wxlua_msw28d_wxbindxrc.lib wxlua_msw28d_wxbindhtml.lib wxlua_msw28d_wxbindadv.lib wxlua_msw28d_wxbindaui.lib wxlua_msw28d_wxbindnet.lib wxlua_msw28d_wxbindxml.lib wxlua_msw28d_wxbindcore.lib wxlua_msw28d_wxbindbase.lib wxlua_msw28d_wxlua.lib lua5.1.lib wxmsw28d_xrc.lib wxmsw28d_html.lib wxmsw28d_adv.lib wxmsw28d_aui.lib wxbase28d_net.lib wxbase28d_xml.lib wxmsw28d_core.lib wxbase28d.lib /nologo /dll /machine:i386 /out:"..\..\bin\openocd.dll" /debug /pdb:"..\..\bin\openocd.pdb" /libpath:"..\..\plugins\romloader\openocd\win\lib\msvc"

!ENDIF

# Begin Target

# Name "plugin_romloader_openocd - Win32 Release"
# Name "plugin_romloader_openocd - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\target\algorithm.c
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\target\arm720t.c
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\target\arm7_9_common.c
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\target\arm7tdmi.c
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\target\arm920t.c
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\target\arm926ejs.c
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\target\arm966e.c
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\target\arm9tdmi.c
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\target\arm_disassembler.c
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\target\arm_jtag.c
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\target\arm_simulator.c
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\target\armv4_5.c
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\target\armv4_5_cache.c
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\target\armv4_5_mmu.c
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\target\armv7m.c
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\helper\binarybuffer.c
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\target\breakpoints.c
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\helper\command.c
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\helper\configuration.c
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\target\cortex_m3.c
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\target\cortex_swjdp.c
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\target\embeddedice.c
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\target\etb.c
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\target\etm.c
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\target\etm_dummy.c
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\helper\fileio.c
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\jtag\ft2232.c
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\target\image.c
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\helper\interpreter.c
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\jtag\jtag.c
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\helper\log.c
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\target\oocd_trace.c
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\target\register.c
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\helper\replacements.c
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\romloader.cpp
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\_luaif\romloader_openocd_lua.cpp
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\_luaif\romloader_openocd_lua_bind.cpp
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\romloader_openocd_main.cpp
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\target\target.c
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\target\target_request.c
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\helper\time_support.c
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\target\trace.c
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\target\xscale.c
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\xsvf\xsvf.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\target\algorithm.h
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\target\arm720t.h
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\target\arm7_9_common.h
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\target\arm7tdmi.h
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\target\arm920t.h
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\target\arm926ejs.h
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\target\arm966e.h
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\target\arm9tdmi.h
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\target\arm_disassembler.h
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\target\arm_jtag.h
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\target\arm_simulator.h
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\target\armv4_5.h
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\target\armv4_5_cache.h
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\target\armv4_5_mmu.h
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\target\armv7m.h
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\helper\binarybuffer.h
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\target\breakpoints.h
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\helper\command.h
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\helper\configuration.h
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\target\cortex_m3.h
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\target\cortex_swjdp.h
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\target\embeddedice.h
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\target\etb.h
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\target\etm.h
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\target\etm_dummy.h
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\helper\fileio.h
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\target\image.h
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\helper\interpreter.h
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\jtag\jtag.h
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\helper\log.h
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\target\register.h
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\helper\replacements.h
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\target\target.h
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\helper\time_support.h
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\target\trace.h
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\helper\types.h
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\target\xscale.h
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\openocd\openocd\xsvf\xsvf.h
# End Source File
# End Group
# End Target
# End Project

