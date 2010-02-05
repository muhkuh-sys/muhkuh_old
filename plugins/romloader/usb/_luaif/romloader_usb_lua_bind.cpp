// ---------------------------------------------------------------------------
// _luaif/romloader_usb_lua_bind.cpp was generated by genwxbind.lua 
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
#include "romloader_usb_wxlua_bindings.h"


// ---------------------------------------------------------------------------
// wxLuaGetEventList_romloader_usb_lua() is called to register events
// ---------------------------------------------------------------------------

wxLuaBindEvent* wxLuaGetEventList_romloader_usb_lua(size_t &count)
{
    static wxLuaBindEvent eventList[] =
    {

        { 0, 0, 0 },
    };
    count = sizeof(eventList)/sizeof(wxLuaBindEvent) - 1;
    return eventList;
}

// ---------------------------------------------------------------------------
// wxLuaGetDefineList_romloader_usb_lua() is called to register %define and %enum
// ---------------------------------------------------------------------------

wxLuaBindNumber* wxLuaGetDefineList_romloader_usb_lua(size_t &count)
{
    static wxLuaBindNumber numberList[] =
    {
        { "ROMLOADER_CHIPTYP_NETX100", ROMLOADER_CHIPTYP_NETX100 },
        { "ROMLOADER_CHIPTYP_NETX50", ROMLOADER_CHIPTYP_NETX50 },
        { "ROMLOADER_CHIPTYP_NETX500", ROMLOADER_CHIPTYP_NETX500 },
        { "ROMLOADER_CHIPTYP_UNKNOWN", ROMLOADER_CHIPTYP_UNKNOWN },
        { "ROMLOADER_ROMCODE_ABOOT", ROMLOADER_ROMCODE_ABOOT },
        { "ROMLOADER_ROMCODE_HBOOT", ROMLOADER_ROMCODE_HBOOT },
        { "ROMLOADER_ROMCODE_UNKNOWN", ROMLOADER_ROMCODE_UNKNOWN },

        { 0, 0 },
    };
    count = sizeof(numberList)/sizeof(wxLuaBindNumber) - 1;
    return numberList;
}

// ---------------------------------------------------------------------------

// wxLuaGetStringList_romloader_usb_lua() is called to register %define %string
// ---------------------------------------------------------------------------

wxLuaBindString* wxLuaGetStringList_romloader_usb_lua(size_t &count)
{
    static wxLuaBindString stringList[] =
    {

        { 0, 0 },
    };
    count = sizeof(stringList)/sizeof(wxLuaBindString) - 1;
    return stringList;
}

// ---------------------------------------------------------------------------
// wxLuaGetObjectList_romloader_usb_lua() is called to register object and pointer bindings
// ---------------------------------------------------------------------------

wxLuaBindObject* wxLuaGetObjectList_romloader_usb_lua(size_t &count)
{
    static wxLuaBindObject objectList[] =
    {

        { 0, 0, 0, 0 },
    };
    count = sizeof(objectList)/sizeof(wxLuaBindObject) - 1;
    return objectList;
}

// ---------------------------------------------------------------------------
// wxLuaGetFunctionList_romloader_usb_lua() is called to register global functions
// ---------------------------------------------------------------------------

static wxLuaArgType s_wxluatypeArray_wxLua_function_romloader_usb_create[] = { &wxluatype_TNUMBER, NULL };
// %function %gc romloader *romloader_usb_create(voidptr_long pvHandle)
static int LUACALL wxLua_function_romloader_usb_create(lua_State *L)
{
    // voidptr_long pvHandle
    long pvHandle = (long)wxlua_getnumbertype(L, 1);
    // call romloader_usb_create
    romloader* returns = (romloader*)romloader_usb_create((void*)pvHandle);
    if (!wxluaO_isgcobject(L, returns)) wxluaO_addgcobject(L, returns);
    // push the result datatype
    wxluaT_pushuserdatatype(L, returns, wxluatype_romloader);

    return 1;
}
static wxLuaBindCFunc s_wxluafunc_wxLua_function_romloader_usb_create[1] = {{ wxLua_function_romloader_usb_create, WXLUAMETHOD_CFUNCTION, 1, 1, s_wxluatypeArray_wxLua_function_romloader_usb_create }};

// ---------------------------------------------------------------------------
// wxLuaGetFunctionList_romloader_usb_lua() is called to register global functions
// ---------------------------------------------------------------------------

wxLuaBindMethod* wxLuaGetFunctionList_romloader_usb_lua(size_t &count)
{
    static wxLuaBindMethod functionList[] =
    {
        { "romloader_usb_create", WXLUAMETHOD_CFUNCTION, s_wxluafunc_wxLua_function_romloader_usb_create, 1, NULL },

        { 0, 0, 0, 0 }, 
    };
    count = sizeof(functionList)/sizeof(wxLuaBindMethod) - 1;
    return functionList;
}



// ---------------------------------------------------------------------------
// wxLuaGetClassList_romloader_usb_lua() is called to register classes
// ---------------------------------------------------------------------------

static const char* wxluaclassname_romloader = "romloader";

// ---------------------------------------------------------------------------
// Lua Tag Method Values and Tables for each Class
// ---------------------------------------------------------------------------

extern wxLuaBindMethod romloader_methods[];
extern int romloader_methodCount;




wxLuaBindClass* wxLuaGetClassList_romloader_usb_lua(size_t &count)
{
    static wxLuaBindClass classList[] =
    {
        { wxluaclassname_romloader, romloader_methods, romloader_methodCount, CLASSINFO(romloader), &wxluatype_romloader, NULL, NULL, g_wxluanumberArray_None, 0, }, 

        { 0, 0, 0, 0, 0, 0, 0 }, 
    };
    count = sizeof(classList)/sizeof(wxLuaBindClass) - 1;

    return classList;
}

// ---------------------------------------------------------------------------
// wxLuaBinding_romloader_usb_lua() - the binding class
// ---------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxLuaBinding_romloader_usb_lua, wxLuaBinding)

wxLuaBinding_romloader_usb_lua::wxLuaBinding_romloader_usb_lua() : wxLuaBinding()
{
    m_bindingName   = wxT("romloader_usb_lua");
    m_nameSpace     = wxT("muhkuh");
    m_classArray    = wxLuaGetClassList_romloader_usb_lua(m_classCount);
    m_numberArray   = wxLuaGetDefineList_romloader_usb_lua(m_numberCount);
    m_stringArray   = wxLuaGetStringList_romloader_usb_lua(m_stringCount);
    m_eventArray    = wxLuaGetEventList_romloader_usb_lua(m_eventCount);
    m_objectArray   = wxLuaGetObjectList_romloader_usb_lua(m_objectCount);
    m_functionArray = wxLuaGetFunctionList_romloader_usb_lua(m_functionCount);
}



// ---------------------------------------------------------------------------

bool wxLuaBinding_romloader_usb_lua_init()
{
    static wxLuaBinding_romloader_usb_lua m_binding;
    if (wxLuaBinding::GetBindingList()->Find(&m_binding)) return false;

    wxLuaBinding::GetBindingList()->Append(&m_binding);
    return true;
}


