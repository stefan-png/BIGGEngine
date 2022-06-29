#pragma once

#include "LuaVector.hpp"
#include <fmt/format.h>

void open_VectorMT(lua_State* L) {
    luaL_newmetatable(L, g_VectorMTName);

    lua_getfield(L, -2, g_VectorIndexFuncName);
    lua_setfield(L, -2, "__index");

    lua_getfield(L, -2, g_VectorNewIndexFuncName);
    lua_setfield(L, -2, "__newindex");

    lua_getfield(L, -2, g_VectorToStringFuncName);
    lua_setfield(L, -2, "__tostring");

    lua_pop(L, 1);
}

void newVector(lua_State* L, int dimension) {
    lua_createtable(L, dimension, 0);
    luaL_setmetatable(L, g_VectorMTName);
}

int l_VectorIndex(lua_State* L){
    luaL_checkany(L, 2); // check 2nd arg exists, throw lua error if not.
    lua_settop(L, 2); // remove any extra args to make sure the top one is the key

    if(lua_isinteger(L, 2)) {
        lua_rawget(L, 1);
        return 1;
    }
    const char* const lut[] = {
            "x",
            "y",
            "z",
            "w",
            nullptr,
    };

    int key = luaL_checkoption(L, 2, nullptr, lut);
    lua_rawgeti(L, 1, key);
    return 1;
}

int l_VectorNewIndex(lua_State* L){
    luaL_checkany(L, 2); // check 2nd arg exists, throw lua error if not.
    lua_settop(L, 2); // remove any extra args to make sure the top one is the key

    if(lua_isinteger(L, 2)) {
        lua_rawset(L, 1);
        return 0;
    }
    const char* const lut[] = {
            "x",
            "y",
            "z",
            "w",
            nullptr,
    };

    int key = luaL_checkoption(L, 2, nullptr, lut);
    lua_rawseti(L, 1, key);
    return 0;
}

int l_VectorToString(lua_State* L) {
    std::string out = "(";

    // declare 2 ints, loop forever, and increment i.
    for( int i = 1, isNumber = false; ; i++) {
        lua_rawgeti(L, 1, i);

        if(lua_isnil(L, -1)) {
            lua_pop(L, 1);
            out += ")";
            break;
        }
        if(i != 1) {
            out += ", ";
        }

        lua_Number number = lua_tonumberx(L, -1, &isNumber);
        if(isNumber) {
            out += fmt::format("{:.3f}", number);
        } else {
            out += "???";
        }
        lua_pop(L, 1);
    }
    lua_pushstring(L, out.c_str());
    return 1;
}