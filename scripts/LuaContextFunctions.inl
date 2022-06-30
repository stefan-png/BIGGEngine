#pragma once

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
    lua_Integer i = luaL_checkinteger(L, 1);

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
int l_getMouseHover(lua_State* L) {
    lua_pushboolean(L, Context::getMouseHover());
    return 1;
}
int l_getMouseButton(lua_State* L) { // MouseButtonEnum button
    lua_Integer i = luaL_checkinteger(L, 1);

    lua_pushinteger(L, static_cast<lua_Integer>(Context::getMouseButton(static_cast<BIGGEngine::MouseButtonEnum>(i))));
    return 1;
}
int l_getClipboardString(lua_State* L) {
    lua_pushstring(L, Context::getClipboardString().c_str());
    return 1;
}

int l_setWindowShouldClose(lua_State* L) {  // bool shouldClose
    luaL_checktype(L, 1, LUA_TBOOLEAN);
    bool shouldClose = lua_toboolean(L, 1);

    Context::setWindowShouldClose(shouldClose);
    return 0;
}
int l_setWindowSizeLimits(lua_State* L) { // int minWidth, int minHeight, int maxWidth, int maxHeight
    lua_Integer minWidth = luaL_checkinteger(L, 1);
    lua_Integer minHeight = luaL_checkinteger(L, 2);
    lua_Integer maxWidth = luaL_checkinteger(L, 3);
    lua_Integer maxHeight = luaL_checkinteger(L, 4);

    Context::setWindowSizeLimits(minWidth, minHeight, maxWidth, maxHeight);

    return 0;
}
int l_setWindowAspectRatio(lua_State* L) { // int numerator, int denominator
    lua_Integer numerator = luaL_checkinteger(L, 1);
    lua_Integer denominator = luaL_checkinteger(L, 2);

    Context::setWindowAspectRatio(numerator, denominator);

    return 0;
}
int l_setWindowTitle(lua_State* L) {  // std::string title
    const char* s = luaL_checkstring(L, 1);

    Context::setWindowTitle(s);

    return 0;
}
int l_setWindowVisible(lua_State* L) { // bool visible
    luaL_checktype(L, 1, LUA_TBOOLEAN);
    bool visible = lua_toboolean(L, 1);

    Context::setWindowVisible(visible);

    return 0;
}
int l_setClipboardString(lua_State* L) { // std::string text
    const char* s = luaL_checkstring(L, 1);

    Context::setClipboardString(s);

    return 0;
}