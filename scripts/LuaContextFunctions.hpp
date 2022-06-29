//
// Created by Stefan Antoszko on 2022-06-29.
//

#pragma once

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