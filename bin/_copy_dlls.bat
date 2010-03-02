set WX_VENDOR=_muhkuh

@echo off
echo copy wxwidgets dlls
copy %WXWIN%\lib\vc_dll\wxbase28_net_vc%WX_VENDOR%.dll .
copy %WXWIN%\lib\vc_dll\wxbase28_vc%WX_VENDOR%.dll .
copy %WXWIN%\lib\vc_dll\wxbase28_xml_vc%WX_VENDOR%.dll .
copy %WXWIN%\lib\vc_dll\wxmsw28_adv_vc%WX_VENDOR%.dll .
copy %WXWIN%\lib\vc_dll\wxmsw28_aui_vc%WX_VENDOR%.dll .
copy %WXWIN%\lib\vc_dll\wxmsw28_core_vc%WX_VENDOR%.dll .
copy %WXWIN%\lib\vc_dll\wxmsw28_gl_vc%WX_VENDOR%.dll .
copy %WXWIN%\lib\vc_dll\wxmsw28_html_vc%WX_VENDOR%.dll .
copy %WXWIN%\lib\vc_dll\wxmsw28_media_vc%WX_VENDOR%.dll .
copy %WXWIN%\lib\vc_dll\wxmsw28_qa_vc%WX_VENDOR%.dll .
copy %WXWIN%\lib\vc_dll\wxmsw28_richtext_vc%WX_VENDOR%.dll .
copy %WXWIN%\lib\vc_dll\wxmsw28_stc_vc%WX_VENDOR%.dll .
copy %WXWIN%\lib\vc_dll\wxmsw28_vc%WX_VENDOR%.dll .
copy %WXWIN%\lib\vc_dll\wxmsw28_xrc_vc%WX_VENDOR%.dll .

echo copy wxlua dlls
copy %WXLUA%\lib\vc_dll\lua5.1.dll .
copy %WXLUA%\lib\vc_dll\wxlua_msw28_wxbindadv.dll .
copy %WXLUA%\lib\vc_dll\wxlua_msw28_wxbindaui.dll .
copy %WXLUA%\lib\vc_dll\wxlua_msw28_wxbindbase.dll .
copy %WXLUA%\lib\vc_dll\wxlua_msw28_wxbindcore.dll .
copy %WXLUA%\lib\vc_dll\wxlua_msw28_wxbindgl.dll .
copy %WXLUA%\lib\vc_dll\wxlua_msw28_wxbindhtml.dll .
copy %WXLUA%\lib\vc_dll\wxlua_msw28_wxbindmedia.dll .
copy %WXLUA%\lib\vc_dll\wxlua_msw28_wxbindnet.dll .
copy %WXLUA%\lib\vc_dll\wxlua_msw28_wxbindstc.dll .
copy %WXLUA%\lib\vc_dll\wxlua_msw28_wxbindxml.dll .
copy %WXLUA%\lib\vc_dll\wxlua_msw28_wxbindxrc.dll .
copy %WXLUA%\lib\vc_dll\wxlua_msw28_wxlua.dll .
copy %WXLUA%\lib\vc_dll\wxlua_msw28_wxluadebug.dll .
copy %WXLUA%\lib\vc_dll\wxlua_msw28_wxluasocket.dll .

echo copy system dlls
copy %SystemRoot%\system32\msvcr71.dll .
copy %SystemRoot%\system32\msvcp71.dll .

