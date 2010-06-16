set WX_VENDOR=_muhkuh
set WX_BUILD=
rem @echo off

echo copy wxwidgets dlls
copy %WXWIN%\lib\vc_dll\wxbase28%WX_BUILD%_net_vc%WX_VENDOR%.dll .
copy %WXWIN%\lib\vc_dll\wxbase28%WX_BUILD%_vc%WX_VENDOR%.dll .
copy %WXWIN%\lib\vc_dll\wxbase28%WX_BUILD%_xml_vc%WX_VENDOR%.dll .
copy %WXWIN%\lib\vc_dll\wxmsw28%WX_BUILD%_adv_vc%WX_VENDOR%.dll .
copy %WXWIN%\lib\vc_dll\wxmsw28%WX_BUILD%_aui_vc%WX_VENDOR%.dll .
copy %WXWIN%\lib\vc_dll\wxmsw28%WX_BUILD%_core_vc%WX_VENDOR%.dll .
copy %WXWIN%\lib\vc_dll\wxmsw28%WX_BUILD%_gl_vc%WX_VENDOR%.dll .
copy %WXWIN%\lib\vc_dll\wxmsw28%WX_BUILD%_html_vc%WX_VENDOR%.dll .
copy %WXWIN%\lib\vc_dll\wxmsw28%WX_BUILD%_media_vc%WX_VENDOR%.dll .
copy %WXWIN%\lib\vc_dll\wxmsw28%WX_BUILD%_qa_vc%WX_VENDOR%.dll .
copy %WXWIN%\lib\vc_dll\wxmsw28%WX_BUILD%_richtext_vc%WX_VENDOR%.dll .
copy %WXWIN%\lib\vc_dll\wxmsw28%WX_BUILD%_stc_vc%WX_VENDOR%.dll .
copy %WXWIN%\lib\vc_dll\wxmsw28%WX_BUILD%_vc%WX_VENDOR%.dll .
copy %WXWIN%\lib\vc_dll\wxmsw28%WX_BUILD%_xrc_vc%WX_VENDOR%.dll .

echo copy wxlua dlls
copy %WXLUA%\lib\vc_dll\lua5.1.dll .
copy %WXLUA%\lib\vc_dll\wxlua_msw28%WX_BUILD%_wxbindadv.dll .
copy %WXLUA%\lib\vc_dll\wxlua_msw28%WX_BUILD%_wxbindaui.dll .
copy %WXLUA%\lib\vc_dll\wxlua_msw28%WX_BUILD%_wxbindbase.dll .
copy %WXLUA%\lib\vc_dll\wxlua_msw28%WX_BUILD%_wxbindcore.dll .
copy %WXLUA%\lib\vc_dll\wxlua_msw28%WX_BUILD%_wxbindgl.dll .
copy %WXLUA%\lib\vc_dll\wxlua_msw28%WX_BUILD%_wxbindhtml.dll .
copy %WXLUA%\lib\vc_dll\wxlua_msw28%WX_BUILD%_wxbindmedia.dll .
copy %WXLUA%\lib\vc_dll\wxlua_msw28%WX_BUILD%_wxbindnet.dll .
copy %WXLUA%\lib\vc_dll\wxlua_msw28%WX_BUILD%_wxbindstc.dll .
copy %WXLUA%\lib\vc_dll\wxlua_msw28%WX_BUILD%_wxbindxml.dll .
copy %WXLUA%\lib\vc_dll\wxlua_msw28%WX_BUILD%_wxbindxrc.dll .
copy %WXLUA%\lib\vc_dll\wxlua_msw28%WX_BUILD%_wxlua.dll .
copy %WXLUA%\lib\vc_dll\wxlua_msw28%WX_BUILD%_wxluadebug.dll .
copy %WXLUA%\lib\vc_dll\wxlua_msw28%WX_BUILD%_wxluasocket.dll .

@echo copy system dlls
rem @copy %SystemRoot%\system32\msvcr71.dll .
rem @copy %SystemRoot%\system32\msvcp71.dll .
@xcopy /i /y "C:\Programme\Microsoft Visual Studio 8\VC\redist\x86\Microsoft.VC80.CRT" .\Microsoft.VC80.CRT
