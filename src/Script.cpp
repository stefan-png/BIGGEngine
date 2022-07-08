//
// Created by Stefan Antoszko on 2022-05-27.
//

// TODO __tostring metamethods for all lua types (Entity, Component, Vector)
// - DONE! add Context functions in lua (getWindowSize, getMousePos, etc) 100%
// - add rest of Components 0%
// - add rest of event callbacks 0%

#include "Script.hpp"
#include "Macros.hpp"

#define BIGG_PROFILE_SCRIPT_FUNCTION            _BIGG_PROFILE_CATEGORY_FUNCTION("script")
#define BIGG_PROFILE_SCRIPT_SCOPE(_format, ...) _BIGG_PROFILE_CATEGORY_SCOPE("script", _format, ##__VA_ARGS__)

#include <utility> // for helper functions

#define CHECK_LUA(L, x) if((x)) { \
    lua_error(L);                 \
}

namespace BIGGEngine {
namespace Scripting {
namespace {

bool isDefaultName(const char* s) {
    return
            strcmp(s, "_G") == 0 ||
            strcmp(s, "_VERSION") == 0 ||
            strcmp(s, "assert") == 0 ||
            strcmp(s, "collectgarbage") == 0 ||
            strcmp(s, "dofile") == 0 ||
            strcmp(s, "error") == 0 ||
            strcmp(s, "getmetatable") == 0 ||
            strcmp(s, "ipairs") == 0 ||
            strcmp(s, "load") == 0 ||
            strcmp(s, "loadfile") == 0 ||
            strcmp(s, "next") == 0 ||
            strcmp(s, "pairs") == 0 ||
            strcmp(s, "pcall") == 0 ||
            strcmp(s, "print") == 0 ||
            strcmp(s, "rawequal") == 0 ||
            strcmp(s, "rawget") == 0 ||
            strcmp(s, "rawlen") == 0 ||
            strcmp(s, "rawset") == 0 ||
            strcmp(s, "require") == 0 ||
            strcmp(s, "select") == 0 ||
            strcmp(s, "setmetatable") == 0 ||
            strcmp(s, "tonumber") == 0 ||
            strcmp(s, "tostring") == 0 ||
            strcmp(s, "type") == 0 ||
            strcmp(s, "warn") == 0 ||
            strcmp(s, "xpcall") == 0 ||
            strcmp(s, "coroutine") == 0 ||
            strcmp(s, "debug") == 0 ||
            strcmp(s, "io") == 0 ||
            strcmp(s, "math") == 0 ||
            strcmp(s, "os") == 0 ||
            strcmp(s, "package") == 0 ||
            strcmp(s, "string") == 0 ||
            strcmp(s, "table") == 0 ||
            strcmp(s, "utf8") == 0 ||
            strcmp(s, "BIGGEngine") == 0;
}

struct Buffer {
    Buffer() = default;

    Buffer(void* data, size_t size) {
        BIGG_PROFILE_INIT_FUNCTION;
        if(size <= 0) return;
        m_data = malloc(size);
        memcpy(this->m_data, data, size);
        m_size = size;
    }

    ~Buffer() {
        BIGG_PROFILE_SCRIPT_FUNCTION;
        free(m_data);
    }

    void push(const void* data, size_t size) {
        BIGG_PROFILE_SCRIPT_FUNCTION;
        if(size <= 0) return;

        void* d = malloc(m_size + size);
        memcpy((char*)d + m_size, data, size);    // since can't do pointer arithmetic on void*

        memcpy(d, m_data, m_size);  // could copy nothing if m_size == 0
        free(m_data);               // could free nothing if m_data == nullptr

        m_data = d;
        m_size += size;
    }

    void* data() { return m_data; }

    size_t size() { return m_size; }

    friend int writer(lua_State* L, const void* p, size_t sz, void* ud);

    friend const char* reader(lua_State* L, void* ud, size_t* size);

private:
    void* m_data = nullptr;
    size_t m_size = 0;
};

int writer(lua_State* L, const void* p, size_t sz, void* ud) {
    Buffer* buf = static_cast<Buffer*>(ud);
    buf->push(p, sz);
    return 0;
}

const char* reader(lua_State*, void* ud, size_t* size) {
    Buffer* buf = static_cast<Buffer*>(ud);
    *size = buf->size();
    return static_cast<const char*>(buf->data());
}

struct ScriptHandle {
    ScriptHandle(entt::hashed_string name, std::string_view filepath, uint16_t subscribePriority);

    ~ScriptHandle();

    // I'm expecting these to be hardcoded into the executable.
    entt::hashed_string::hash_type name;      // name to be registered
    Buffer m_buffer;            // buffer for caching the script.
    lua_State* m_luaState = nullptr;
};

// global state
std::unordered_map<entt::hashed_string::hash_type, ScriptHandle> g_registeredScripts;

// ----------------- Lua Enums ---------------------

// all the open_Xxx functions expect a stack with table 'BIGGEngine' at the top.
void open_MouseButtonEnum(lua_State* L);
void open_KeyEnum(lua_State* L);
void open_ActionEnum(lua_State* L);
void open_ModsEnum(lua_State* L);

#include "../scripts/LuaEnums.inl"

// ------------ Lua Vector ----------------------

const char* g_VectorMTName = "BIGGEngine.Vector";
const char* g_VectorIndexFuncName = "VectorIndex";
const char* g_VectorNewIndexFuncName = "VectorNewIndex";
const char* g_VectorToStringFuncName = "VectorToString";

/// creates and pushes a new table onto the stack with metatable BIGGEngine.Vector.
/// @param dimension is how many elements the table will have (eg. 2 for vec2)
void newVector(lua_State* L, int dimension);

int l_VectorIndex(lua_State* L);
int l_VectorNewIndex(lua_State* L);
int l_VectorToString(lua_State* L);

/// Expects a stack with table 'BIGGEngine' at the top.
void open_VectorMT(lua_State* L);

#include "../scripts/LuaVector.inl"

// -------------------- Context Functions ------------------------

const char* g_GetWindowSizeFuncName             = "getWindowSize";
const char* g_GetWindowFramebufferSizeFuncName  = "getWindowFramebufferSize";
const char* g_GetWindowContentScaleFuncName     = "getWindowContentScale";
const char* g_GetWindowPositionFuncName         = "getWindowPosition";
const char* g_GetWindowIconifiedFuncName        = "getWindowIconified";
const char* g_GetWindowMaximizedFuncName        = "getWindowMaximized";
const char* g_GetWindowVisibleFuncName          = "getWindowVisible";
const char* g_GetWindowFocusFuncName            = "getWindowFocus";
const char* g_GetKeyFuncName                    = "getKey";
const char* g_GetMousePositionFuncName          = "getMousePosition";
const char* g_GetMouseHoverFuncName             = "getMouseHover";
const char* g_GetMouseButtonFuncName            = "getMouseButton";
const char* g_GetClipboardStringFuncName        = "getClipboardString";
const char* g_SetWindowShouldCloseFuncName      = "setWindowShouldClose";
const char* g_SetWindowSizeLimitsFuncName       = "setWindowSizeLimits";
const char* g_SetWindowAspectRatioFuncName      = "setWindowAspectRatio";
const char* g_SetWindowTitleFuncName            = "setWindowTitle";
const char* g_SetClipboardStringFuncName        = "setClipboardString";
const char* g_SetWindowVisibleFuncName          = "setWindowVisible";

int l_getWindowSize(lua_State* L);
int l_getWindowFramebufferSize(lua_State* L);
int l_getWindowContentScale(lua_State* L);
int l_getWindowPosition(lua_State* L);
int l_getWindowIconified(lua_State* L);
int l_getWindowMaximized(lua_State* L);
int l_getWindowVisible(lua_State* L);
int l_getWindowFocus(lua_State* L);
int l_getKey(lua_State* L); // KeyEnum key
int l_getMousePosition(lua_State* L);
int l_getMouseHover(lua_State* L);
int l_getMouseButton(lua_State* L); // MouseButtonEnum button
int l_getClipboardString(lua_State* L);

int l_setWindowShouldClose(lua_State* L); // bool shouldClose
int l_setWindowSizeLimits(lua_State* L); // int minWidth, int minHeight, int maxWidth, int maxHeight
int l_setWindowAspectRatio(lua_State* L); // int numerator, int denominator
int l_setWindowTitle(lua_State* L); // std::string title
int l_setWindowVisible(lua_State* L); // bool visible
int l_setClipboardString(lua_State* L); // std::string text

// TODO set window icon
// TODO get/set monitor???
#include "../scripts/LuaContextFunctions.inl"

// -------------- Vector Handle -------------------------------

using vec2h = glm::vec2;
using ivec2h = glm::ivec2;
using vec3h = glm::vec3;
using ivec3h = glm::ivec3;

template<typename T>
const char* g_vecMTName;
template<> const char* g_vecMTName<vec2h> = "BIGGEngine.Vec2Handle";
template<> const char* g_vecMTName<ivec2h> = "BIGGEngine.IVec2Handle";
template<> const char* g_vecMTName<vec3h> = "BIGGEngine.Vec3Handle";
template<> const char* g_vecMTName<ivec3h> = "BIGGEngine.IVec3Handle";

template<typename T>
const char* g_vecIndexFuncName;
template<> const char* g_vecIndexFuncName<vec2h> = "Vec2HandleIndex";
template<> const char* g_vecIndexFuncName<ivec2h> = "IVec2HandleIndex";
template<> const char* g_vecIndexFuncName<vec3h> = "Vec3HandleIndex";
template<> const char* g_vecIndexFuncName<ivec3h> = "IVec3HandleIndex";

template<typename T>
const char* g_vecNewIndexFuncName;
template<> const char* g_vecNewIndexFuncName<vec2h> = "Vec2HandleNewIndex";
template<> const char* g_vecNewIndexFuncName<ivec2h> = "IVec2HandleNewIndex";
template<> const char* g_vecNewIndexFuncName<vec3h> = "Vec3HandleNewIndex";
template<> const char* g_vecNewIndexFuncName<ivec3h> = "IVec3HandleNewIndex";

template<typename T>
const char* g_vecToStringFuncName;
template<> const char* g_vecToStringFuncName<vec2h> = "Vec2HandleToString";
template<> const char* g_vecToStringFuncName<ivec2h> = "IVec2HandleToString";
template<> const char* g_vecToStringFuncName<vec3h> = "Vec3HandleToString";
template<> const char* g_vecToStringFuncName<ivec3h> = "IVec3HandleToString";

template<typename VecT>
void newVectorHandle(lua_State* L, VecT* pVec);

template<typename VecT>
int l_vectorHandleIndex(lua_State* L);
template<typename VecT>
int l_vectorHandleNewIndex(lua_State* L);
template<typename VecT>
int l_vectorHandleToString(lua_State* L);

template<typename VecT>
void open_VectorHandle(lua_State* L);

// ---------------------- Lua Components -----------------------------

#include "../scripts/LuaVectorHandle.inl"

const char* g_TransformComponentMTName = "BIGGEngine.TransformComponent";
const char* g_TransformComponentIndexFuncName = "TransformComponentIndex";
const char* g_TransformComponentNewIndexFuncName = "TransformComponentNewIndex";
const char* g_TransformComponentToStringFuncName = "TransformComponentToString";

/// Pushes a BIGGEngine.TransformComponent onto the stack.
void newTransformComponent(lua_State* L, Transform* pTransform);
int l_TransformComponentIndex(lua_State* L);
int l_TransformComponentNewIndex(lua_State* L);
int l_TransformComponentToString(lua_State* L);

/// Expects a stack with table 'BIGGEngine' at the top.
void open_TransformComponentMT(lua_State* L);

#include "../scripts/LuaComponents.inl"

// --------------------- Lua Entity -----------------------------

const char* g_EntityMTName = "BIGGEngine.Entity";
const char* g_EntityIndexFuncName = "EntityIndex";
const char* g_EntityToStringFuncName = "EntityToString";

/// Pushes a Entity onto the stack.
void newEntity(lua_State* L, entt::entity entity) {
    lua_newuserdatauv(L, 0, 1);
    lua_pushinteger(L, static_cast<lua_Integer>(entity));
    lua_setiuservalue(L, -2, 1);

    luaL_setmetatable(L, g_EntityMTName);
}

int l_EntityIndex(lua_State* L) {
    void* userdata = (luaL_checkudata(L, 1, g_EntityMTName));
    const char* key = luaL_checkstring(L, 2);

    luaL_argcheck(L, userdata != nullptr, 1, "entity expected");

    bool isTransform = strcmp("transform", key) == 0;
    luaL_argcheck(L, isTransform, 2, "'transform' expected");

    lua_getiuservalue(L, 1, 1); // pushes entity onto stack
    entt::entity entity = static_cast<entt::entity>(lua_tointeger(L, -1));
    lua_pop(L, 1);

    if(isTransform) {
        newTransformComponent(L, &ECS::get().get<Transform>(entity));
    }
    return 1;
}

/// Expects a stack with table 'BIGGEngine' at the top.
void open_Entity(lua_State* L) {
    luaL_newmetatable(L, g_EntityMTName);
    // set __index
    lua_getfield(L, -2, g_EntityIndexFuncName);
    lua_setfield(L, -2, "__index");
    lua_pop(L, 1);
}

// ------------------- Lua misc funcs ---------------------------

int l_log(lua_State* L) {
    int numArgs = lua_gettop(L);

    for(int i = 0; i < numArgs; i++) {
        luaL_argcheck(L, !lua_isnil(L, i + 1), i + 1, "valid type expected, got nil");
    }

    lua_getglobal(L, "string");     // library 'string'
    lua_getfield(L, -1, "format");  // function 'format'
    lua_insert(L, 1);               // insert function 'string.format' to bottom of stack
    lua_pop(L, 1);                  // pop table 'string' from stack
    // now stack looks like:
    //      function 'string.format'
    //      arg 1
    //      ...
    //      arg n
    if(lua_pcall(L, numArgs, 1, 0)) {
        lua_error(L);
    }
    const char* msg = luaL_checkstring(L, -1);
    lua_pop(L, 1);

    spdlog::source_loc sourceLoc;
    lua_Debug debug;
    if(lua_getstack(L, 1, &debug)) {  // check function at level 1
        lua_getinfo(L, "lnS", &debug);  // get info about it

        sourceLoc.filename = debug.short_src;
        sourceLoc.line = debug.currentline;
        sourceLoc.funcname = (debug.name)?debug.name:SPDLOG_FUNCTION;

    } else {
        BIGG_LOG_WARN("lua_getstack() failed.");
    }

    Log::m_scriptLogger->log(sourceLoc, spdlog::level::level_enum::info, msg);
    return 0;
}

// --------------------- End Lua Funcs -----------------------

const luaL_Reg BIGGEnginelib[] = {

        // Misc funcs
        {"log",                                l_log},

        // Vector Handle Funcs
        {g_vecIndexFuncName<vec2h>,            l_vectorHandleIndex<vec2h>},
        {g_vecIndexFuncName<ivec2h>,           l_vectorHandleIndex<ivec2h>},
        {g_vecIndexFuncName<vec3h>,            l_vectorHandleIndex<vec3h>},
        {g_vecIndexFuncName<ivec3h>,           l_vectorHandleIndex<ivec3h>},
        {g_vecNewIndexFuncName<vec2h>,         l_vectorHandleNewIndex<vec2h>},
        {g_vecNewIndexFuncName<ivec2h>,        l_vectorHandleNewIndex<ivec2h>},
        {g_vecNewIndexFuncName<vec3h>,         l_vectorHandleNewIndex<vec3h>},
        {g_vecNewIndexFuncName<ivec3h>,        l_vectorHandleNewIndex<ivec3h>},
        {g_vecToStringFuncName<vec2h>,         l_vectorHandleToString<vec2h>},
        {g_vecToStringFuncName<ivec2h>,        l_vectorHandleToString<ivec2h>},
        {g_vecToStringFuncName<vec3h>,         l_vectorHandleToString<vec3h>},
        {g_vecToStringFuncName<ivec3h>,        l_vectorHandleToString<ivec3h>},

        // Vector Funcs
        {g_VectorIndexFuncName,                l_VectorIndex},
        {g_VectorNewIndexFuncName,             l_VectorNewIndex},
        {g_VectorToStringFuncName,             l_VectorToString},

        // Context Funcs
        {g_GetWindowSizeFuncName,              l_getWindowSize},
        {g_GetWindowFramebufferSizeFuncName,   l_getWindowFramebufferSize},
        {g_GetWindowContentScaleFuncName,      l_getWindowContentScale},
        {g_GetWindowPositionFuncName,          l_getWindowPosition},
        {g_GetWindowIconifiedFuncName,         l_getWindowIconified},
        {g_GetWindowMaximizedFuncName,         l_getWindowMaximized},
        {g_GetWindowVisibleFuncName,           l_getWindowVisible},
        {g_GetWindowFocusFuncName,             l_getWindowFocus},
        {g_GetKeyFuncName,                     l_getKey},
        {g_GetMousePositionFuncName,           l_getMousePosition},
        {g_GetMouseHoverFuncName,              l_getMouseHover},
        {g_GetMouseButtonFuncName,             l_getMouseButton},
        {g_GetClipboardStringFuncName,         l_getClipboardString},
        {g_SetWindowShouldCloseFuncName,       l_setWindowShouldClose},
        {g_SetWindowSizeLimitsFuncName,        l_setWindowSizeLimits},
        {g_SetWindowAspectRatioFuncName,       l_setWindowAspectRatio},
        {g_SetWindowTitleFuncName,             l_setWindowTitle},
        {g_SetClipboardStringFuncName,         l_setClipboardString},
        {g_SetWindowVisibleFuncName,           l_setWindowVisible},

        // Component funcs
        {g_TransformComponentIndexFuncName,    l_TransformComponentIndex},
        {g_TransformComponentNewIndexFuncName, l_TransformComponentNewIndex},
        {g_EntityIndexFuncName,                l_EntityIndex},
        {nullptr,                              nullptr},
};

int open_BIGGEnginelib(lua_State* L) {
    luaL_newlib(L, BIGGEnginelib);            // create table BIGGEngine
    open_KeyEnum(L);
    open_ActionEnum(L);
    open_ModsEnum(L);
    open_MouseButtonEnum(L);

    open_VectorHandle<vec2h>(L);
    open_VectorHandle<ivec2h>(L);
    open_VectorHandle<vec3h>(L);
    open_VectorHandle<ivec3h>(L);

    open_TransformComponentMT(L);
    open_Entity(L);
    open_VectorMT(L);

    return 1;
}

/// Pushes onto the stack a copy a lua table. Ignores metatables. Does not work with nested tables.
/// Only copies non-default data (ie. default libs, functions, etc.)
/// @param srcIndex is the stack index of the source lua table
void copyTableShallow(lua_State* L, int srcIndex) {
    if(!lua_istable(L, srcIndex)) {
        // any POD is copied by value. any userdata, threads, or functions are copied by reference.
        lua_pushvalue(L, srcIndex);
        return;
    }
    srcIndex = lua_absindex(L, srcIndex);
    lua_newtable(L);
    int outputIndex = lua_absindex(L, -1);

    lua_pushnil(L);  // first key
    while(lua_next(L, srcIndex) != 0) {
        if(lua_isstring(L, -2)) {
            if(isDefaultName(luaL_checkstring(L, -2))) {
                lua_pop(L, 1);
                continue;
            }
        }
        lua_pushvalue(L, -2);     // stack looks like -3 = key, -2 = value, -1 = key
        lua_insert(L, -2);        // stack looks like -3 = key, -2 = key, -1 = value
        lua_rawset(L, outputIndex); // removes 2 objects, leaving 1 key for next call to lua_next().
    }
}

/// set key-value pairs from table at @p srcIndex to table at @p destIndex.
/// any key-value pairs from @p destIndex not in @p src index are ignored.
void loadTableShallow(lua_State* L, int srcIndex, int destIndex) {
    if(!lua_istable(L, srcIndex) || !lua_istable(L, destIndex)) {
        // any POD is copied by value. any userdata, threads, or functions are copied by reference.
        return;
    }
    srcIndex = lua_absindex(L, srcIndex);
    destIndex = lua_absindex(L, destIndex);
    lua_pushnil(L);  // first key
    while(lua_next(L, srcIndex) != 0) {
        lua_pushvalue(L, -2);     // stack looks like -3 = key, -2 = value, -1 = key
        lua_insert(L, -2);        // stack looks like -3 = key, -2 = key, -1 = value
        lua_rawset(L, destIndex); // removes 2 objects, leaving 1 key for next call to lua_next().
    }
}

/// set key-value pairs from table at @p srcIndex to table at @p destIndex.
/// any key-value pairs from @p destIndex not in @p src index are ignored.
/// will recurse on nested tables. Keys cannot be a table, only values.
void loadTableDeepRecursive(lua_State* L, int srcIndex, int destIndex) {
    if(!lua_istable(L, srcIndex) || !lua_istable(L, destIndex)) {
        // any POD is copied by value. any userdata, threads, or functions are copied by reference.
        return;
    }
    srcIndex = lua_absindex(L, srcIndex);
    destIndex = lua_absindex(L, destIndex);
    lua_pushnil(L);  // first key
    while(lua_next(L, srcIndex) != 0) {
        lua_pushvalue(L, -2);     // stack looks like -3 = key, -2 = value, -1 = key
        lua_insert(L, -2);        // stack looks like -3 = key, -2 = key, -1 = value

        if(lua_istable(L, -1)) { // if value is a subtable

            lua_pushvalue(L, -2);      // stack looks like -1 = key
            lua_rawget(L, destIndex);      // stack looks like -1 = result

            if(!lua_istable(L, -1)) {  // if nil or non-table, replace the old value with a new table
                lua_pop(L, 1);
                lua_newtable(L);
            }
            loadTableDeepRecursive(L, -2, -1); // RECURSE!

            lua_remove(L, -2); // remove the src subtable
        }

        lua_rawset(L, destIndex); // removes 2 objects, leaving 1 key for next call to lua_next().
    }
}

/// Pushes onto the stack a copy a lua table. Ignores metatables.
/// Only copies non-default data (ie. default libs, functions, etc.)
/// @param srcIndex is the stack index of the source lua table
void copyTableDeepRecursive(lua_State* L, int srcIndex) {
    if(!lua_istable(L, srcIndex)) {
        // any POD is copied by value. any userdata, threads, or functions are copied by reference.
        lua_pushvalue(L, srcIndex);
        return;
    }
    srcIndex = lua_absindex(L, srcIndex);
    lua_newtable(L);
    int outputIndex = lua_absindex(L, -1);

    lua_pushnil(L);  // first key
    while(lua_next(L, srcIndex) != 0) {
        if(lua_isstring(L, -2)) {
            if(isDefaultName(luaL_checkstring(L, -2))) {
                lua_pop(L, 1);
                continue;
            }
        }
        lua_pushvalue(L, -2);     // stack looks like -3 = key, -2 = value, -1 = key
        lua_insert(L, -2);        // stack looks like -3 = key, -2 = key, -1 = value

        if(lua_istable(L, -1)) { // if value is a subtable

            lua_pushvalue(L, -2);      // stack looks like -1 = key
            lua_rawget(L, outputIndex);      // stack looks like -1 = result

            if(!lua_istable(L, -1)) {  // if nil or non-table, replace the old value with a new table
                lua_pop(L, 1);
                lua_newtable(L);
            }
            loadTableDeepRecursive(L, -2, -1); // RECURSE!

            lua_remove(L, -2); // remove the src subtable
        }

        lua_rawset(L, outputIndex); // removes 2 objects, leaving 1 key for next call to lua_next().
    }
}

/// prints a lua table at stack position @p tableIndex to std::out
/// @param tableIdx is the stack index of the table to print
/// @param _depth is for internal use only
void printTable(lua_State* L, int tableIndex, int _depth = 0) {
    tableIndex = lua_absindex(L, tableIndex);
    lua_pushnil(L);  // first key
    while(lua_next(L, tableIndex) != 0) {
        // uses 'key' (at index -2) and 'value' (at index -1)
        const char* key;
        if(lua_isnumber(L, -2)) {
            lua_Number numKey = lua_tonumber(L, -2);
            key = std::to_string(numKey).c_str();
        } else {
            key = lua_tostring(L, -2);
        }
        int valueType = lua_type(L, -1);
        if(lua_isnil(L, -1)) {
            BIGG_LOG_INFO("{} nil {} = nil", std::string(_depth * 4, ' '), key);
        } else if(lua_isinteger(L, -1)) {
            BIGG_LOG_INFO("{} integer {} = {}", std::string(_depth * 4, ' '), key, lua_tointeger(L, -1));
        } else if(lua_isnumber(L, -1)) {
            BIGG_LOG_INFO("{} double {} = {}", std::string(_depth * 4, ' '), key, lua_tonumber(L, -1));
        } else if(lua_isboolean(L, -1)) {
            BIGG_LOG_INFO("{} bool {} = {}", std::string(_depth * 4, ' '), key, (lua_toboolean(L, -1)?"true":"false"));
        } else if(lua_isstring(L, -1)) {
            BIGG_LOG_INFO("{} string {} = {}", std::string(_depth * 4, ' '), key, lua_tostring(L, -1));
        } else if(lua_istable(L, -1)) {

            BIGG_LOG_INFO("{} table {} {{", std::string(_depth * 4, ' '), key);
            if(_depth < 1) printTable(L, -1, _depth + 1);
            BIGG_LOG_INFO("{} }}", std::string(_depth * 4, ' '));
        } else if(lua_isfunction(L, -1)) {
            BIGG_LOG_INFO("{} function {} = ???", std::string(_depth * 4, ' '), key);
        } else if(lua_isuserdata(L, -1)) {
            BIGG_LOG_INFO("{} userdata {} = {}", std::string(_depth * 4, ' '), key, lua_touserdata(L, -1));
        } else if(lua_isthread(L, -1)) {
            BIGG_LOG_INFO("{} thread {} = {}", std::string(_depth * 4, ' '), key, (void*)lua_tothread(L, -1));
        } else if(lua_islightuserdata(L, -1)) {
            BIGG_LOG_INFO("{} light userdata {} = {}", std::string(_depth * 4, ' '), key, lua_touserdata(L, -1));
        } else {
            BIGG_LOG_INFO("{} unknown type {} = ???", std::string(_depth * 4, ' '), key);
        }
        lua_pop(L, 1); // pop the value and leave the key for next lua_next() call
    }
}

// Script Handle implementation

ScriptHandle::ScriptHandle(entt::hashed_string name, std::string_view filepath, uint16_t subscribePriority)
        : name(name.value()) {

    BIGG_PROFILE_SCRIPT_FUNCTION;

    {
        BIGG_PROFILE_SCRIPT_SCOPE("setup member vars '{}'", name.data());

        m_luaState = luaL_newstate();
        luaL_openlibs(m_luaState);
        luaL_requiref(m_luaState, "BIGGEngine", open_BIGGEnginelib, 1);
        // If glb is true, also stores the module into the global modname. aka global 'BIGGEngine'
//            luaL_requiref(L, "BIGGEngine", open_mylib, 1);
        lua_pop(m_luaState, 1); // pop the requiref.

        auto warnFunc = [](void*, const char* msg, int) {
            BIGG_LOG_WARN(msg);
        };

        lua_setwarnf(m_luaState, static_cast<lua_WarnFunction>(warnFunc), nullptr);

        // load file. Pushes the chunk as a Lua function onto the stack.
        // TODO filepath.data() may not be null terminated
        CHECK_LUA(m_luaState, luaL_loadfile(m_luaState, filepath.data()));

        // Does NOT pop the Lua function off the stack.
//        if (lua_dump(m_luaState, writer, &m_buffer, false)) {
//            lua_error(m_luaState);
//        }
//        BIGG_LOG_DEBUG("Cached script '{}' path: '{}' size: {} bytes.", name, filepath, m_buffer.size());
    } // BIGG_PROFILE_SCRIPT_SCOPE("setup member vars {}", name.data());


    BIGG_PROFILE_SCRIPT_SCOPE("set callbacks '{}'", name.data());   // rest of function

    lua_getglobal(m_luaState, "BIGGEngine"); // push _ENV.BIGGEngine to stack

    lua_pushstring(m_luaState, name.data());           // push new name
    lua_setfield(m_luaState, -2, "ScriptName"); // pops name from stack, but keeps table BIGGEngine

    lua_pop(m_luaState, 1);                         // pop table _ENV.BIGGEngine

    CHECK_LUA(m_luaState, lua_pcall(m_luaState, 0, LUA_MULTRET, 0)); // call chunk

    lua_getglobal(m_luaState, "BIGGEngine");
    lua_getfield(m_luaState, -1, "Init");
    if(lua_isfunction(m_luaState, -1)) {
        CHECK_LUA(m_luaState, lua_pcall(m_luaState, 0, 0, 0)); // call Init
    } else {
        BIGG_LOG_WARN("no init function found for script {}", name.data());
        lua_pop(m_luaState, 1);
    }

    lua_getfield(m_luaState, -1, "WindowCreate");
    if(lua_isfunction(m_luaState, -1)) {
        // setup callback
        auto callback = [=](WindowCreateEvent event) -> bool {   // captures this.m_luaState and name
            _BIGG_PROFILE_CATEGORY_SCOPE("script", "callback");

            lua_getglobal(m_luaState, "BIGGEngine");
            BIGG_ASSERT(lua_istable(m_luaState, -1), "BIGGEngine is not a table! '{}'", name.data());

            // for each ScriptComponent
            auto& registry = ECS::get();
            auto view = registry.view<ScriptComponent>();
            for(auto[entity, scriptComponent] : view.each()) {

                // Omit all scripts with a different name
                if(scriptComponent.m_name != name.value()) continue;

                // load saved globals
                lua_rawgetp(m_luaState, LUA_REGISTRYINDEX, (void*)&scriptComponent);
                lua_rawgeti(m_luaState, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
                loadTableDeepRecursive(m_luaState, -2, -1);
                lua_pop(m_luaState, 2);

                // load 'this' entity handle userdata
                lua_getglobal(m_luaState, "this");
                if(lua_isnoneornil(m_luaState, -1)) {
                    newEntity(m_luaState, entity);
                    lua_setglobal(m_luaState, "this");
                } else {
                    lua_pushinteger(m_luaState, static_cast<lua_Integer>(entity));
                    lua_setiuservalue(m_luaState, -2, 1);
                }
                lua_pop(m_luaState, 1);

                // get function callback
                lua_getfield(m_luaState, -1, "WindowCreate");
                BIGG_ASSERT(lua_isfunction(m_luaState, -1), "WindowCreate is not a function! '{}'", name.data());

                // call lua function
                if(lua_pcall(m_luaState, 0, 1, 0)) { // pops args and function from stack
                    lua_error(m_luaState);
                    return false;
                }
                // get return value
                bool ret = false;
                if(!lua_isboolean(m_luaState, -1)) {
                    // error! should have returned a bool!
                    BIGG_LOG_WARN("lua BIGGEngine.WindowCreate didn't return a bool!");
                }
                ret = lua_toboolean(m_luaState, -1);
                lua_pop(m_luaState, 1); // pop the return value from the stack

                // save globals
                lua_pushglobaltable(m_luaState);
                printTable(m_luaState, -1);
                copyTableDeepRecursive(m_luaState, -1);
                lua_rawsetp(m_luaState, LUA_REGISTRYINDEX,
                            (void*)&scriptComponent); // key is light userdata of this ScriptComponent
                lua_pop(m_luaState, 1);

                if(ret) return true;    // only the first instance of this script will be ran.
            }

            lua_pop(m_luaState, 1); // pop table BIGGEngine
            return false;
        };
        Events::subscribe<WindowCreateEvent>(subscribePriority, callback);

        lua_pop(m_luaState, 1); // pop WindowCreate
    } else {
        lua_pop(m_luaState, 1); // pop the nil
    }

    lua_getfield(m_luaState, -1, "MouseButton");
    if(lua_isfunction(m_luaState, -1)) {
        // setup callback
        auto callback = [=](MouseButtonEvent event) -> bool {   // captures this.m_luaState and name
            _BIGG_PROFILE_CATEGORY_SCOPE("script", "callback");

            lua_getglobal(m_luaState, "BIGGEngine");
            BIGG_ASSERT(lua_istable(m_luaState, -1), "BIGGEngine is not a table! '{}'", name.data());

            // for each ScriptComponent
            auto& registry = ECS::get();
            auto view = registry.view<ScriptComponent>();
            for(auto[entity, scriptComponent] : view.each()) {

                // Omit all scripts with a different name
                if(scriptComponent.m_name != name.value()) continue;

                // load saved globals
                lua_rawgetp(m_luaState, LUA_REGISTRYINDEX, (void*)&scriptComponent);
                lua_rawgeti(m_luaState, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
                loadTableDeepRecursive(m_luaState, -2, -1);
                lua_pop(m_luaState, 2);

                // load 'this' entity handle userdata
                lua_getglobal(m_luaState, "this");
                if(lua_isnoneornil(m_luaState, -1)) {
                    newEntity(m_luaState, entity);
                    lua_setglobal(m_luaState, "this");
                } else {
                    lua_pushinteger(m_luaState, static_cast<lua_Integer>(entity));
                    lua_setiuservalue(m_luaState, -2, 1);
                }
                lua_pop(m_luaState, 1);

                // get function callback
                lua_getfield(m_luaState, -1, "MouseButton");
                BIGG_ASSERT(lua_isfunction(m_luaState, -1), "MouseButton is not a function! '{}'", name.data());

                // push args
                lua_pushinteger(m_luaState, static_cast<lua_Integer>(event.m_button));
                lua_pushinteger(m_luaState, static_cast<lua_Integer>(event.m_action));
                lua_pushinteger(m_luaState, static_cast<lua_Integer>(event.m_mods));
                // call lua function
                if(lua_pcall(m_luaState, 3, 1, 0)) { // pops args and function from stack
                    lua_error(m_luaState);
                    return false;
                }
                // get return value
                if(!lua_isboolean(m_luaState, -1)) {
                    // error! should have returned a bool!
                    BIGG_LOG_WARN("lua BIGGEngine.MouseButton didn't return a bool!");
                    lua_pop(m_luaState, 1);
                    return false;
                }
                bool ret = lua_toboolean(m_luaState, -1);
                lua_pop(m_luaState, 1); // pop the return value from the stack

                // save globals
                lua_pushglobaltable(m_luaState);
                copyTableDeepRecursive(m_luaState, -1);
                lua_rawsetp(m_luaState, LUA_REGISTRYINDEX,
                            (void*)&scriptComponent); // key is light userdata of this ScriptComponent
                lua_pop(m_luaState, 1);

                if(ret) return true;    // only the first instance of this script will be ran.
            }

            lua_pop(m_luaState, 1); // pop table BIGGEngine
            return false;
        };
        Events::subscribe<MouseButtonEvent>(subscribePriority, callback);

        lua_pop(m_luaState, 1); // pop MouseButton
    } else {
        lua_pop(m_luaState, 1); // pop the nil
    }

    // TODO repeat for all other callbacks
    lua_pop(m_luaState, 1); // pop BIGGEngine
}

ScriptHandle::~ScriptHandle() {
    BIGG_PROFILE_SHUTDOWN_FUNCTION;
    lua_close(m_luaState);
}

} // anonymous namespace

// --------------------------- Public things --------------------------------

/// registers a script's name, loads, caches, and runs the script.
void registerScript(entt::hashed_string name, std::string_view filepath, uint16_t subscribePriority) {
    // If a key equivalent to k already exists in the container, does nothing.
    // calls constructor of ScriptHandle
    g_registeredScripts.try_emplace(name.value(), name, filepath, subscribePriority);
}
} // namepace Scripting

ScriptComponent::ScriptComponent(entt::hashed_string::hash_type name) : m_name(name) {
    // save globals
    using namespace Scripting;
    if(g_registeredScripts.count(m_name) == 0) {
        return;
    }
    const ScriptHandle& handle = g_registeredScripts.at(m_name);
    lua_State* L = handle.m_luaState;

    lua_pushglobaltable(L);
    copyTableShallow(L, -1);
    lua_rawsetp(L, LUA_REGISTRYINDEX, (void*)this); // key is light userdata of this ScriptComponent
    lua_pop(L, 1);
}

ScriptComponent::~ScriptComponent() {
    using namespace Scripting;
    if(g_registeredScripts.count(m_name) == 0) {
        return;
    }
    const ScriptHandle& handle = g_registeredScripts.at(m_name);
    lua_State* L = handle.m_luaState;

    lua_pushnil(L);
    lua_rawsetp(L, LUA_REGISTRYINDEX, (void*)this);

}

} // namespace BIGGEngine {
