// ---------------------------------------------------------------------------
// _luaif/romloader_uart_lua.cpp was generated by genwxbind.lua 
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
#include "romloader_uart_wxlua_bindings.h"


// ---------------------------------------------------------------------------
// Bind class romloader
// ---------------------------------------------------------------------------

// Lua MetaTable Tag for Class 'romloader'
int wxluatype_romloader = WXLUA_TUNKNOWN;

static wxLuaArgType s_wxluatypeArray_wxLua_romloader_call[] = { &wxluatype_romloader, &wxluatype_TNUMBER, &wxluatype_TNUMBER, &wxluatype_TFUNCTION, &wxluatype_TNUMBER, NULL };
static int LUACALL wxLua_romloader_call(lua_State *L);
static wxLuaBindCFunc s_wxluafunc_wxLua_romloader_call[1] = {{ wxLua_romloader_call, WXLUAMETHOD_METHOD, 5, 5, s_wxluatypeArray_wxLua_romloader_call }};
// %override wxLua_romloader_call
static int LUACALL wxLua_romloader_call(lua_State *L)
{
    int iLuaCallbackTag;
    // voidptr_long vplCallbackUserData
    long vplCallbackUserData = (long)wxlua_getnumbertype(L, 5);
    // LuaFunction fnCallback
    if( lua_isfunction(L, 4) )
    {
        // push function to top of stack
        lua_pushvalue(L, 4);
        // ref function and pop it from stack
        iLuaCallbackTag = luaL_ref(L, LUA_REGISTRYINDEX);
    }
    else
    {
        wxlua_argerror(L, 4, wxT("a 'function'"));
    }
    // unsigned long ulParameterR0
    double dParameterR0 = wxlua_getnumbertype(L, 3);
    // unsigned long ulNetxAddress
    double dNetxAddress = wxlua_getnumbertype(L, 2);
    // get this
    romloader * self = (romloader *)wxluaT_getuserdatatype(L, 1, wxluatype_romloader);
    // call call
    self->call(dNetxAddress, dParameterR0, L, iLuaCallbackTag, (void*)vplCallbackUserData);

    // remove ref to function
    luaL_unref(L, LUA_REGISTRYINDEX, iLuaCallbackTag);

    return 0;
}


static wxLuaArgType s_wxluatypeArray_wxLua_romloader_connect[] = { &wxluatype_romloader, NULL };
static int LUACALL wxLua_romloader_connect(lua_State *L);
static wxLuaBindCFunc s_wxluafunc_wxLua_romloader_connect[1] = {{ wxLua_romloader_connect, WXLUAMETHOD_METHOD, 1, 1, s_wxluatypeArray_wxLua_romloader_connect }};
//  void connect()
static int LUACALL wxLua_romloader_connect(lua_State *L)
{
    // get this
    romloader * self = (romloader *)wxluaT_getuserdatatype(L, 1, wxluatype_romloader);
    // call connect
    self->connect();

    return 0;
}

static wxLuaArgType s_wxluatypeArray_wxLua_romloader_delete[] = { &wxluatype_romloader, NULL };
static wxLuaBindCFunc s_wxluafunc_wxLua_romloader_delete[1] = {{ wxlua_userdata_delete, WXLUAMETHOD_METHOD|WXLUAMETHOD_DELETE, 1, 1, s_wxluatypeArray_wxLua_romloader_delete }};

static wxLuaArgType s_wxluatypeArray_wxLua_romloader_disconnect[] = { &wxluatype_romloader, NULL };
static int LUACALL wxLua_romloader_disconnect(lua_State *L);
static wxLuaBindCFunc s_wxluafunc_wxLua_romloader_disconnect[1] = {{ wxLua_romloader_disconnect, WXLUAMETHOD_METHOD, 1, 1, s_wxluatypeArray_wxLua_romloader_disconnect }};
//  void disconnect()
static int LUACALL wxLua_romloader_disconnect(lua_State *L)
{
    // get this
    romloader * self = (romloader *)wxluaT_getuserdatatype(L, 1, wxluatype_romloader);
    // call disconnect
    self->disconnect();

    return 0;
}

static wxLuaArgType s_wxluatypeArray_wxLua_romloader_get_chiptyp[] = { &wxluatype_romloader, NULL };
static int LUACALL wxLua_romloader_get_chiptyp(lua_State *L);
static wxLuaBindCFunc s_wxluafunc_wxLua_romloader_get_chiptyp[1] = {{ wxLua_romloader_get_chiptyp, WXLUAMETHOD_METHOD, 1, 1, s_wxluatypeArray_wxLua_romloader_get_chiptyp }};
//  ROMLOADER_CHIPTYP get_chiptyp()
static int LUACALL wxLua_romloader_get_chiptyp(lua_State *L)
{
    // get this
    romloader * self = (romloader *)wxluaT_getuserdatatype(L, 1, wxluatype_romloader);
    // call get_chiptyp
    ROMLOADER_CHIPTYP returns = (self->get_chiptyp());
    // push the result number
    lua_pushnumber(L, returns);

    return 1;
}

static wxLuaArgType s_wxluatypeArray_wxLua_romloader_get_chiptyp_name[] = { &wxluatype_romloader, &wxluatype_TINTEGER, NULL };
static int LUACALL wxLua_romloader_get_chiptyp_name(lua_State *L);
static wxLuaBindCFunc s_wxluafunc_wxLua_romloader_get_chiptyp_name[1] = {{ wxLua_romloader_get_chiptyp_name, WXLUAMETHOD_METHOD, 2, 2, s_wxluatypeArray_wxLua_romloader_get_chiptyp_name }};
//  wxString get_chiptyp_name(ROMLOADER_CHIPTYP tChiptyp)
static int LUACALL wxLua_romloader_get_chiptyp_name(lua_State *L)
{
    // ROMLOADER_CHIPTYP tChiptyp
    ROMLOADER_CHIPTYP tChiptyp = (ROMLOADER_CHIPTYP)wxlua_getenumtype(L, 2);
    // get this
    romloader * self = (romloader *)wxluaT_getuserdatatype(L, 1, wxluatype_romloader);
    // call get_chiptyp_name
    wxString returns = (self->get_chiptyp_name(tChiptyp));
    // push the result string
    wxlua_pushwxString(L, returns);

    return 1;
}

static wxLuaArgType s_wxluatypeArray_wxLua_romloader_get_name[] = { &wxluatype_romloader, NULL };
static int LUACALL wxLua_romloader_get_name(lua_State *L);
static wxLuaBindCFunc s_wxluafunc_wxLua_romloader_get_name[1] = {{ wxLua_romloader_get_name, WXLUAMETHOD_METHOD, 1, 1, s_wxluatypeArray_wxLua_romloader_get_name }};
//  wxString get_name()
static int LUACALL wxLua_romloader_get_name(lua_State *L)
{
    // get this
    romloader * self = (romloader *)wxluaT_getuserdatatype(L, 1, wxluatype_romloader);
    // call get_name
    wxString returns = (self->get_name());
    // push the result string
    wxlua_pushwxString(L, returns);

    return 1;
}

static wxLuaArgType s_wxluatypeArray_wxLua_romloader_get_romcode[] = { &wxluatype_romloader, NULL };
static int LUACALL wxLua_romloader_get_romcode(lua_State *L);
static wxLuaBindCFunc s_wxluafunc_wxLua_romloader_get_romcode[1] = {{ wxLua_romloader_get_romcode, WXLUAMETHOD_METHOD, 1, 1, s_wxluatypeArray_wxLua_romloader_get_romcode }};
//  ROMLOADER_ROMCODE get_romcode()
static int LUACALL wxLua_romloader_get_romcode(lua_State *L)
{
    // get this
    romloader * self = (romloader *)wxluaT_getuserdatatype(L, 1, wxluatype_romloader);
    // call get_romcode
    ROMLOADER_ROMCODE returns = (self->get_romcode());
    // push the result number
    lua_pushnumber(L, returns);

    return 1;
}

static wxLuaArgType s_wxluatypeArray_wxLua_romloader_get_romcode_name[] = { &wxluatype_romloader, &wxluatype_TINTEGER, NULL };
static int LUACALL wxLua_romloader_get_romcode_name(lua_State *L);
static wxLuaBindCFunc s_wxluafunc_wxLua_romloader_get_romcode_name[1] = {{ wxLua_romloader_get_romcode_name, WXLUAMETHOD_METHOD, 2, 2, s_wxluatypeArray_wxLua_romloader_get_romcode_name }};
//  wxString get_romcode_name(ROMLOADER_ROMCODE tRomcode)
static int LUACALL wxLua_romloader_get_romcode_name(lua_State *L)
{
    // ROMLOADER_ROMCODE tRomcode
    ROMLOADER_ROMCODE tRomcode = (ROMLOADER_ROMCODE)wxlua_getenumtype(L, 2);
    // get this
    romloader * self = (romloader *)wxluaT_getuserdatatype(L, 1, wxluatype_romloader);
    // call get_romcode_name
    wxString returns = (self->get_romcode_name(tRomcode));
    // push the result string
    wxlua_pushwxString(L, returns);

    return 1;
}

static wxLuaArgType s_wxluatypeArray_wxLua_romloader_get_typ[] = { &wxluatype_romloader, NULL };
static int LUACALL wxLua_romloader_get_typ(lua_State *L);
static wxLuaBindCFunc s_wxluafunc_wxLua_romloader_get_typ[1] = {{ wxLua_romloader_get_typ, WXLUAMETHOD_METHOD, 1, 1, s_wxluatypeArray_wxLua_romloader_get_typ }};
//  wxString get_typ()
static int LUACALL wxLua_romloader_get_typ(lua_State *L)
{
    // get this
    romloader * self = (romloader *)wxluaT_getuserdatatype(L, 1, wxluatype_romloader);
    // call get_typ
    wxString returns = (self->get_typ());
    // push the result string
    wxlua_pushwxString(L, returns);

    return 1;
}

static wxLuaArgType s_wxluatypeArray_wxLua_romloader_is_connected[] = { &wxluatype_romloader, NULL };
static int LUACALL wxLua_romloader_is_connected(lua_State *L);
static wxLuaBindCFunc s_wxluafunc_wxLua_romloader_is_connected[1] = {{ wxLua_romloader_is_connected, WXLUAMETHOD_METHOD, 1, 1, s_wxluatypeArray_wxLua_romloader_is_connected }};
//  bool is_connected()
static int LUACALL wxLua_romloader_is_connected(lua_State *L)
{
    // get this
    romloader * self = (romloader *)wxluaT_getuserdatatype(L, 1, wxluatype_romloader);
    // call is_connected
    bool returns = (self->is_connected());
    // push the result flag
    lua_pushboolean(L, returns);

    return 1;
}

static wxLuaArgType s_wxluatypeArray_wxLua_romloader_read_data08[] = { &wxluatype_romloader, &wxluatype_TNUMBER, NULL };
static int LUACALL wxLua_romloader_read_data08(lua_State *L);
static wxLuaBindCFunc s_wxluafunc_wxLua_romloader_read_data08[1] = {{ wxLua_romloader_read_data08, WXLUAMETHOD_METHOD, 2, 2, s_wxluatypeArray_wxLua_romloader_read_data08 }};
//  double read_data08(double dNetxAddress)
static int LUACALL wxLua_romloader_read_data08(lua_State *L)
{
    // double dNetxAddress
    double dNetxAddress = (double)wxlua_getnumbertype(L, 2);
    // get this
    romloader * self = (romloader *)wxluaT_getuserdatatype(L, 1, wxluatype_romloader);
    // call read_data08
    double returns = (self->read_data08(dNetxAddress));
    // push the result number
    lua_pushnumber(L, returns);

    return 1;
}

static wxLuaArgType s_wxluatypeArray_wxLua_romloader_read_data16[] = { &wxluatype_romloader, &wxluatype_TNUMBER, NULL };
static int LUACALL wxLua_romloader_read_data16(lua_State *L);
static wxLuaBindCFunc s_wxluafunc_wxLua_romloader_read_data16[1] = {{ wxLua_romloader_read_data16, WXLUAMETHOD_METHOD, 2, 2, s_wxluatypeArray_wxLua_romloader_read_data16 }};
//  double read_data16(double dNetxAddress)
static int LUACALL wxLua_romloader_read_data16(lua_State *L)
{
    // double dNetxAddress
    double dNetxAddress = (double)wxlua_getnumbertype(L, 2);
    // get this
    romloader * self = (romloader *)wxluaT_getuserdatatype(L, 1, wxluatype_romloader);
    // call read_data16
    double returns = (self->read_data16(dNetxAddress));
    // push the result number
    lua_pushnumber(L, returns);

    return 1;
}

static wxLuaArgType s_wxluatypeArray_wxLua_romloader_read_data32[] = { &wxluatype_romloader, &wxluatype_TNUMBER, NULL };
static int LUACALL wxLua_romloader_read_data32(lua_State *L);
static wxLuaBindCFunc s_wxluafunc_wxLua_romloader_read_data32[1] = {{ wxLua_romloader_read_data32, WXLUAMETHOD_METHOD, 2, 2, s_wxluatypeArray_wxLua_romloader_read_data32 }};
//  double read_data32(double dNetxAddress)
static int LUACALL wxLua_romloader_read_data32(lua_State *L)
{
    // double dNetxAddress
    double dNetxAddress = (double)wxlua_getnumbertype(L, 2);
    // get this
    romloader * self = (romloader *)wxluaT_getuserdatatype(L, 1, wxluatype_romloader);
    // call read_data32
    double returns = (self->read_data32(dNetxAddress));
    // push the result number
    lua_pushnumber(L, returns);

    return 1;
}

static wxLuaArgType s_wxluatypeArray_wxLua_romloader_read_image[] = { &wxluatype_romloader, &wxluatype_TNUMBER, &wxluatype_TNUMBER, &wxluatype_TFUNCTION, &wxluatype_TNUMBER, NULL };
static int LUACALL wxLua_romloader_read_image(lua_State *L);
static wxLuaBindCFunc s_wxluafunc_wxLua_romloader_read_image[1] = {{ wxLua_romloader_read_image, WXLUAMETHOD_METHOD, 5, 5, s_wxluatypeArray_wxLua_romloader_read_image }};
// %override wxLua_romloader_read_image
static int LUACALL wxLua_romloader_read_image(lua_State *L)
{
    int iLuaCallbackTag;
    wxLuaState wxlState(L);
    wxString returns;
    // voidptr_long vplCallbackUserData
    long vplCallbackUserData = (long)wxlua_getnumbertype(L, 5);
    // LuaFunction fnCallback
    if( lua_isfunction(L, 4) )
    {
        // push function to top of stack
        lua_pushvalue(L, 4);
        // ref function and pop it from stack
        iLuaCallbackTag = luaL_ref(L, LUA_REGISTRYINDEX);
    }
    else
    {
        // no callback function provided
        wxlua_argerror(L, 4, wxT("a 'function'"));
    }
    // unsigned long ulSize
    double dSize = wxlua_getnumbertype(L, 3);
    // unsigned long ulNetxAddress
    double dNetxAddress = wxlua_getnumbertype(L, 2);
    // get this
    romloader * self = (romloader *)wxluaT_getuserdatatype(L, 1, wxluatype_romloader);
    // call read_image
    returns = (self->read_image(dNetxAddress, dSize, L, iLuaCallbackTag, (void*)vplCallbackUserData));

    // remove ref to function
    luaL_unref(L, LUA_REGISTRYINDEX, iLuaCallbackTag);

    // push the result string
    wxlState.lua_PushLString(returns.To8BitData(),returns.Len());

    return 1;
}


static wxLuaArgType s_wxluatypeArray_wxLua_romloader_write_data08[] = { &wxluatype_romloader, &wxluatype_TNUMBER, &wxluatype_TNUMBER, NULL };
static int LUACALL wxLua_romloader_write_data08(lua_State *L);
static wxLuaBindCFunc s_wxluafunc_wxLua_romloader_write_data08[1] = {{ wxLua_romloader_write_data08, WXLUAMETHOD_METHOD, 3, 3, s_wxluatypeArray_wxLua_romloader_write_data08 }};
//  void write_data08(double dNetxAddress, double dData)
static int LUACALL wxLua_romloader_write_data08(lua_State *L)
{
    // double dData
    double dData = (double)wxlua_getnumbertype(L, 3);
    // double dNetxAddress
    double dNetxAddress = (double)wxlua_getnumbertype(L, 2);
    // get this
    romloader * self = (romloader *)wxluaT_getuserdatatype(L, 1, wxluatype_romloader);
    // call write_data08
    self->write_data08(dNetxAddress, dData);

    return 0;
}

static wxLuaArgType s_wxluatypeArray_wxLua_romloader_write_data16[] = { &wxluatype_romloader, &wxluatype_TNUMBER, &wxluatype_TNUMBER, NULL };
static int LUACALL wxLua_romloader_write_data16(lua_State *L);
static wxLuaBindCFunc s_wxluafunc_wxLua_romloader_write_data16[1] = {{ wxLua_romloader_write_data16, WXLUAMETHOD_METHOD, 3, 3, s_wxluatypeArray_wxLua_romloader_write_data16 }};
//  void write_data16(double dNetxAddress, double dData)
static int LUACALL wxLua_romloader_write_data16(lua_State *L)
{
    // double dData
    double dData = (double)wxlua_getnumbertype(L, 3);
    // double dNetxAddress
    double dNetxAddress = (double)wxlua_getnumbertype(L, 2);
    // get this
    romloader * self = (romloader *)wxluaT_getuserdatatype(L, 1, wxluatype_romloader);
    // call write_data16
    self->write_data16(dNetxAddress, dData);

    return 0;
}

static wxLuaArgType s_wxluatypeArray_wxLua_romloader_write_data32[] = { &wxluatype_romloader, &wxluatype_TNUMBER, &wxluatype_TNUMBER, NULL };
static int LUACALL wxLua_romloader_write_data32(lua_State *L);
static wxLuaBindCFunc s_wxluafunc_wxLua_romloader_write_data32[1] = {{ wxLua_romloader_write_data32, WXLUAMETHOD_METHOD, 3, 3, s_wxluatypeArray_wxLua_romloader_write_data32 }};
//  void write_data32(double dNetxAddress, double dData)
static int LUACALL wxLua_romloader_write_data32(lua_State *L)
{
    // double dData
    double dData = (double)wxlua_getnumbertype(L, 3);
    // double dNetxAddress
    double dNetxAddress = (double)wxlua_getnumbertype(L, 2);
    // get this
    romloader * self = (romloader *)wxluaT_getuserdatatype(L, 1, wxluatype_romloader);
    // call write_data32
    self->write_data32(dNetxAddress, dData);

    return 0;
}

static wxLuaArgType s_wxluatypeArray_wxLua_romloader_write_image[] = { &wxluatype_romloader, &wxluatype_TNUMBER, &wxluatype_TSTRING, &wxluatype_TFUNCTION, &wxluatype_TNUMBER, NULL };
static int LUACALL wxLua_romloader_write_image(lua_State *L);
static wxLuaBindCFunc s_wxluafunc_wxLua_romloader_write_image[1] = {{ wxLua_romloader_write_image, WXLUAMETHOD_METHOD, 5, 5, s_wxluatypeArray_wxLua_romloader_write_image }};
// %override wxLua_romloader_write_image
static int LUACALL wxLua_romloader_write_image(lua_State *L)
{
    int iLuaCallbackTag;
    wxLuaState wxlState(L);
    // voidptr_long vplCallbackUserData
    long vplCallbackUserData = (long)wxlua_getnumbertype(L, 5);
    // LuaFunction fnCallback
    if( lua_isfunction(L, 4) )
    {
        // push function to top of stack
        lua_pushvalue(L, 4);
        // ref function and pop it from stack
        iLuaCallbackTag = luaL_ref(L, LUA_REGISTRYINDEX);
    }
    else
    {
        // no callback function provided
        wxlua_argerror(L, 4, wxT("a 'function'"));
    }
    size_t sizLen;
    const char *pcBuf;
    pcBuf = lua_tolstring(L, 3, &sizLen);
    // unsigned long ulNetxAddress
    double dNetxAddress = wxlua_getnumbertype(L, 2);
    // get this
    romloader * self = (romloader *)wxluaT_getuserdatatype(L, 1, wxluatype_romloader);
    // call write_image
    self->write_image(dNetxAddress, pcBuf, sizLen, L, iLuaCallbackTag, (void*)vplCallbackUserData);

    // remove ref to function
    luaL_unref(L, LUA_REGISTRYINDEX, iLuaCallbackTag);

    return 0;
}




// Map Lua Class Methods to C Binding Functions
wxLuaBindMethod romloader_methods[] = {
    { "call", WXLUAMETHOD_METHOD, s_wxluafunc_wxLua_romloader_call, 1, NULL },
    { "connect", WXLUAMETHOD_METHOD, s_wxluafunc_wxLua_romloader_connect, 1, NULL },
    { "delete", WXLUAMETHOD_METHOD|WXLUAMETHOD_DELETE, s_wxluafunc_wxLua_romloader_delete, 1, NULL },
    { "disconnect", WXLUAMETHOD_METHOD, s_wxluafunc_wxLua_romloader_disconnect, 1, NULL },
    { "get_chiptyp", WXLUAMETHOD_METHOD, s_wxluafunc_wxLua_romloader_get_chiptyp, 1, NULL },
    { "get_chiptyp_name", WXLUAMETHOD_METHOD, s_wxluafunc_wxLua_romloader_get_chiptyp_name, 1, NULL },
    { "get_name", WXLUAMETHOD_METHOD, s_wxluafunc_wxLua_romloader_get_name, 1, NULL },
    { "get_romcode", WXLUAMETHOD_METHOD, s_wxluafunc_wxLua_romloader_get_romcode, 1, NULL },
    { "get_romcode_name", WXLUAMETHOD_METHOD, s_wxluafunc_wxLua_romloader_get_romcode_name, 1, NULL },
    { "get_typ", WXLUAMETHOD_METHOD, s_wxluafunc_wxLua_romloader_get_typ, 1, NULL },
    { "is_connected", WXLUAMETHOD_METHOD, s_wxluafunc_wxLua_romloader_is_connected, 1, NULL },
    { "read_data08", WXLUAMETHOD_METHOD, s_wxluafunc_wxLua_romloader_read_data08, 1, NULL },
    { "read_data16", WXLUAMETHOD_METHOD, s_wxluafunc_wxLua_romloader_read_data16, 1, NULL },
    { "read_data32", WXLUAMETHOD_METHOD, s_wxluafunc_wxLua_romloader_read_data32, 1, NULL },
    { "read_image", WXLUAMETHOD_METHOD, s_wxluafunc_wxLua_romloader_read_image, 1, NULL },
    { "write_data08", WXLUAMETHOD_METHOD, s_wxluafunc_wxLua_romloader_write_data08, 1, NULL },
    { "write_data16", WXLUAMETHOD_METHOD, s_wxluafunc_wxLua_romloader_write_data16, 1, NULL },
    { "write_data32", WXLUAMETHOD_METHOD, s_wxluafunc_wxLua_romloader_write_data32, 1, NULL },
    { "write_image", WXLUAMETHOD_METHOD, s_wxluafunc_wxLua_romloader_write_image, 1, NULL },
    { 0, 0, 0, 0 },
};

int romloader_methodCount = sizeof(romloader_methods)/sizeof(wxLuaBindMethod) - 1;


