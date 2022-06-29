#pragma once

#include "LuaContextFunctions.hpp"
#include "LuaVector.hpp"

#include "../src/Context.hpp"

#include <lua.hpp>

int l_getWindowSize(lua_State* L) {
    newVector(L, 2);
    auto vec = Context::getWindowSize();
    lua_pushinteger(L, vec.x);
    lua_rawseti(L, -2, 1);
    lua_pushinteger(L, vec.y);
    lua_rawseti(L, -2, 2);
    return 1;
}
int l_getWindowFramebufferSize(lua_State* L) {
    newVector(L, 2);
    auto vec = Context::getWindowFramebufferSize();
    lua_pushinteger(L, vec.x);
    lua_rawseti(L, -2, 1);
    lua_pushinteger(L, vec.y);
    lua_rawseti(L, -2, 2);
    return 1;
}
int l_getWindowContentScale(lua_State* L) {
    newVector(L, 2);
    auto vec = Context::getWindowContentScale();
    lua_pushnumber(L, vec.x);
    lua_rawseti(L, -2, 1);
    lua_pushnumber(L, vec.y);
    lua_rawseti(L, -2, 2);
    return 1;
}
int l_getWindowPosition(lua_State* L) {
    newVector(L, 2);
    auto vec = Context::getWindowPosition();
    lua_pushinteger(L, vec.x);
    lua_rawseti(L, -2, 1);
    lua_pushinteger(L, vec.y);
    lua_rawseti(L, -2, 2);
    return 1;
}
int l_getWindowIconified(lua_State* L) {
    lua_pushboolean(L, Context::getWindowIconified());
    return 1;
}
int l_getWindowMaximized(lua_State* L) {
    lua_pushboolean(L, Context::getWindowMaximized());
    return 1;
}
int l_getWindowVisible(lua_State* L) {
    lua_pushboolean(L, Context::getWindowVisible());
    return 1;
}
int l_getWindowFocus(lua_State* L) {
    lua_pushboolean(L, Context::getWindowFocus());
    return 1;
}
int l_getKey(lua_State* L) { // KeyEnum key
    int isInteger = false;
    lua_Integer i = lua_tointegerx(L, 1, &isInteger);
    luaL_argcheck(L, isInteger, 1, "'BIGGEngine.ActionEnum' expected");

    lua_pushinteger(L, static_cast<lua_Integer>(Context::getKey(static_cast<BIGGEngine::KeyEnum>(i))));
    return 1;

}
int l_getMousePosition(lua_State* L) {
    newVector(L, 2);
    auto vec = Context::getMousePosition();
    lua_pushnumber(L, vec.x);
    lua_rawseti(L, -2, 1);
    lua_pushnumber(L, vec.y);
    lua_rawseti(L, -2, 2);
    return 1;
}
int l_getMouseHover(lua_State* L) { return 0; }
int l_getMouseButton(lua_State* L) { return 0; } // MouseButtonEnum button
int l_getClipboardString(lua_State* L) { return 0; }

int l_setWindowShouldClose(lua_State* L) { return 0; } // bool shouldClose
int l_setWindowSizeLimits(lua_State* L) { return 0; } // int minWidth, int minHeight, int maxWidth, int maxHeight
int l_setWindowAspectRatio(lua_State* L){ return 0; }  // int numerator, int denominator
int l_setWindowTitle(lua_State* L) { return 0; } // std::string title
int l_setWindowVisible(lua_State* L) { return 0; } // bool visible
int l_setClipboardString(lua_State* L) { return 0; } // std::string text