// ---------------------------------------------------------------------------
// _luaif/muhkuh_lua_bind.cpp was generated by genwxbind.lua 
//
// Any changes made to this file will be lost when the file is regenerated.
// ---------------------------------------------------------------------------

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
     #include "wx/wx.h"
#endif


#include "wxlua/include/wxlstate.h"
#include "muhkuh_wxlua_bindings.h"


// ---------------------------------------------------------------------------
// wxLuaGetEventList_muhkuh_lua() is called to register events
// ---------------------------------------------------------------------------

wxLuaBindEvent* wxLuaGetEventList_muhkuh_lua(size_t &count)
{
    static wxLuaBindEvent eventList[] =
    {

        { 0, 0, 0 },
    };
    count = sizeof(eventList)/sizeof(wxLuaBindEvent) - 1;
    return eventList;
}

// ---------------------------------------------------------------------------
// wxLuaGetDefineList_muhkuh_lua() is called to register %define and %enum
// ---------------------------------------------------------------------------

wxLuaBindNumber* wxLuaGetDefineList_muhkuh_lua(size_t &count)
{
    static wxLuaBindNumber numberList[] =
    {

        { 0, 0 },
    };
    count = sizeof(numberList)/sizeof(wxLuaBindNumber) - 1;
    return numberList;
}

// ---------------------------------------------------------------------------

// wxLuaGetStringList_muhkuh_lua() is called to register %define %string
// ---------------------------------------------------------------------------

wxLuaBindString* wxLuaGetStringList_muhkuh_lua(size_t &count)
{
    static wxLuaBindString stringList[] =
    {

        { 0, 0 },
    };
    count = sizeof(stringList)/sizeof(wxLuaBindString) - 1;
    return stringList;
}

// ---------------------------------------------------------------------------
// wxLuaGetObjectList_muhkuh_lua() is called to register object and pointer bindings
// ---------------------------------------------------------------------------

wxLuaBindObject* wxLuaGetObjectList_muhkuh_lua(size_t &count)
{
    static wxLuaBindObject objectList[] =
    {

        { 0, 0, 0, 0 },
    };
    count = sizeof(objectList)/sizeof(wxLuaBindObject) - 1;
    return objectList;
}

// ---------------------------------------------------------------------------
// wxLuaGetFunctionList_muhkuh_lua() is called to register global functions
// ---------------------------------------------------------------------------

// %function %gc muhkuh_plugin_instance *GetNextPlugin()
static int LUACALL wxLua_function_GetNextPlugin(lua_State *L)
{
    // call GetNextPlugin
    muhkuh_plugin_instance* returns = (muhkuh_plugin_instance*)GetNextPlugin();
    if (!wxluaO_isgcobject(L, returns)) wxluaO_addgcobject(L, returns);
    // push the result datatype
    wxluaT_pushuserdatatype(L, returns, wxluatype_muhkuh_plugin_instance);

    return 1;
}
static wxLuaBindCFunc s_wxluafunc_wxLua_function_GetNextPlugin[1] = {{ wxLua_function_GetNextPlugin, WXLUAMETHOD_CFUNCTION, 0, 0, g_wxluaargtypeArray_None }};

// %function muhkuh_wrap_xml *GetSelectedTest()
static int LUACALL wxLua_function_GetSelectedTest(lua_State *L)
{
    // call GetSelectedTest
    muhkuh_wrap_xml* returns = (muhkuh_wrap_xml*)GetSelectedTest();
    // push the result datatype
    wxluaT_pushuserdatatype(L, returns, wxluatype_muhkuh_wrap_xml);

    return 1;
}
static wxLuaBindCFunc s_wxluafunc_wxLua_function_GetSelectedTest[1] = {{ wxLua_function_GetSelectedTest, WXLUAMETHOD_CFUNCTION, 0, 0, g_wxluaargtypeArray_None }};

static wxLuaArgType s_wxluatypeArray_wxLua_function_ScanPlugins[] = { &wxluatype_TSTRING, NULL };
// %function void ScanPlugins(wxString strPattern)
static int LUACALL wxLua_function_ScanPlugins(lua_State *L)
{
    // wxString strPattern
    wxString strPattern = wxlua_getwxStringtype(L, 1);
    // call ScanPlugins
    ScanPlugins(strPattern);

    return 0;
}
static wxLuaBindCFunc s_wxluafunc_wxLua_function_ScanPlugins[1] = {{ wxLua_function_ScanPlugins, WXLUAMETHOD_CFUNCTION, 1, 1, s_wxluatypeArray_wxLua_function_ScanPlugins }};

static wxLuaArgType s_wxluatypeArray_wxLua_function_include[] = { &wxluatype_TSTRING, NULL };
// %function void include(wxString strFileName)
static int LUACALL wxLua_function_include(lua_State *L)
{
    // wxString strFileName
    wxString strFileName = wxlua_getwxStringtype(L, 1);
    // call include
    include(strFileName);

    return 0;
}
static wxLuaBindCFunc s_wxluafunc_wxLua_function_include[1] = {{ wxLua_function_include, WXLUAMETHOD_CFUNCTION, 1, 1, s_wxluatypeArray_wxLua_function_include }};

static wxLuaArgType s_wxluatypeArray_wxLua_function_load[] = { &wxluatype_TSTRING, NULL };
// %override wxLua_function_load
static int LUACALL wxLua_function_load(lua_State *L)
{
    wxLuaState wxlState(L);
    wxString returns;
    // wxString strFileName
    wxString strFileName = wxlState.GetwxStringType(1);
    // call load
    returns = (load(strFileName));
    // push the result string
    wxlState.lua_PushLString(returns.To8BitData(), returns.Len());

    return 1;
}
static wxLuaBindCFunc s_wxluafunc_wxLua_function_load[1] = {{ wxLua_function_load, WXLUAMETHOD_CFUNCTION, 1, 1, s_wxluatypeArray_wxLua_function_load }};

// ---------------------------------------------------------------------------
// wxLuaGetFunctionList_muhkuh_lua() is called to register global functions
// ---------------------------------------------------------------------------

wxLuaBindMethod* wxLuaGetFunctionList_muhkuh_lua(size_t &count)
{
    static wxLuaBindMethod functionList[] =
    {
        { "GetNextPlugin", WXLUAMETHOD_CFUNCTION, s_wxluafunc_wxLua_function_GetNextPlugin, 1, NULL },
        { "GetSelectedTest", WXLUAMETHOD_CFUNCTION, s_wxluafunc_wxLua_function_GetSelectedTest, 1, NULL },
        { "ScanPlugins", WXLUAMETHOD_CFUNCTION, s_wxluafunc_wxLua_function_ScanPlugins, 1, NULL },
        { "include", WXLUAMETHOD_CFUNCTION, s_wxluafunc_wxLua_function_include, 1, NULL },
        { "load", WXLUAMETHOD_CFUNCTION, s_wxluafunc_wxLua_function_load, 1, NULL },

        { 0, 0, 0, 0 }, 
    };
    count = sizeof(functionList)/sizeof(wxLuaBindMethod) - 1;
    return functionList;
}



// ---------------------------------------------------------------------------
// wxLuaGetClassList_muhkuh_lua() is called to register classes
// ---------------------------------------------------------------------------

static const char* wxluaclassname_muhkuh_plugin_instance = "muhkuh_plugin_instance";
static const char* wxluaclassname_muhkuh_wrap_xml = "muhkuh_wrap_xml";

// ---------------------------------------------------------------------------
// Lua Tag Method Values and Tables for each Class
// ---------------------------------------------------------------------------

extern wxLuaBindMethod muhkuh_plugin_instance_methods[];
extern int muhkuh_plugin_instance_methodCount;
extern wxLuaBindMethod muhkuh_wrap_xml_methods[];
extern int muhkuh_wrap_xml_methodCount;




wxLuaBindClass* wxLuaGetClassList_muhkuh_lua(size_t &count)
{
    static wxLuaBindClass classList[] =
    {
        { wxluaclassname_muhkuh_plugin_instance, muhkuh_plugin_instance_methods, muhkuh_plugin_instance_methodCount, NULL, &wxluatype_muhkuh_plugin_instance, NULL, NULL, g_wxluanumberArray_None, 0, }, 
        { wxluaclassname_muhkuh_wrap_xml, muhkuh_wrap_xml_methods, muhkuh_wrap_xml_methodCount, NULL, &wxluatype_muhkuh_wrap_xml, NULL, NULL, g_wxluanumberArray_None, 0, }, 

        { 0, 0, 0, 0, 0, 0, 0 }, 
    };
    count = sizeof(classList)/sizeof(wxLuaBindClass) - 1;

    return classList;
}

// ---------------------------------------------------------------------------
// wxLuaBinding_muhkuh_lua() - the binding class
// ---------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxLuaBinding_muhkuh_lua, wxLuaBinding)

wxLuaBinding_muhkuh_lua::wxLuaBinding_muhkuh_lua() : wxLuaBinding()
{
    m_bindingName   = wxT("muhkuh_lua");
    m_nameSpace     = wxT("muhkuh");
    m_classArray    = wxLuaGetClassList_muhkuh_lua(m_classCount);
    m_numberArray   = wxLuaGetDefineList_muhkuh_lua(m_numberCount);
    m_stringArray   = wxLuaGetStringList_muhkuh_lua(m_stringCount);
    m_eventArray    = wxLuaGetEventList_muhkuh_lua(m_eventCount);
    m_objectArray   = wxLuaGetObjectList_muhkuh_lua(m_objectCount);
    m_functionArray = wxLuaGetFunctionList_muhkuh_lua(m_functionCount);
}



// ---------------------------------------------------------------------------

bool wxLuaBinding_muhkuh_lua_init()
{
    static wxLuaBinding_muhkuh_lua m_binding;
    if (wxLuaBinding::GetBindingList()->Find(&m_binding)) return false;

    wxLuaBinding::GetBindingList()->Append(&m_binding);
    return true;
}


