//
// Created by Stefan Antoszko on 2022-05-27.
//

#pragma once

#include "Core.hpp"
#define LUA_PATH_DEFAULT "../scripts/?.lua"
#include <lua.hpp>

namespace BIGGEngine {

    struct ScriptFunctor {
        ScriptFunctor(lua_State* L, std::string filename);
        bool operator()(Event *event);

    private:
        lua_State* m_luaState;
        // warning function for lua
        static void warnFunction(void* ud, const char* msg, int tocont);

    };

}; // namespace BIGGEngine