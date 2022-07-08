//
// Created by Stefan Antoszko on 2022-06-30.
//

#pragma once

// ----------------- Transform Component ----------------------------

/// Pushes a BIGGEngine.TransformComponent onto the stack.
void newTransformComponent(lua_State* L, Transform* pTransform) {
    void* userdata = lua_newuserdatauv(L, 0, 1);
    lua_pushlightuserdata(L, pTransform);
    lua_setiuservalue(L, -2, 1);

    luaL_setmetatable(L, g_TransformComponentMTName);
}

int l_TransformComponentIndex(lua_State* L) {
    void* userdata = luaL_checkudata(L, 1, g_TransformComponentMTName);
    const char* key = luaL_checkstring(L, 2);

    luaL_argcheck(L, userdata != nullptr, 1, "transform component expected");

    bool isPos = strcmp("position", key) == 0;
    bool isRot = strcmp("rotation", key) == 0;
    bool isScale = strcmp("scale", key) == 0;
    luaL_argcheck(L, isPos || isRot || isScale, 2, "'position' or 'rotation' or 'scale' expected");

    lua_getiuservalue(L, 1, 1); // pushes entity onto stack
    auto* pTransform = static_cast<Transform*>(lua_touserdata(L, -1));
    lua_pop(L, 1);

    if(isPos) {
        newVectorHandle<vec3h>(L, &pTransform->position);
    } else if(isRot) {
        newVectorHandle<vec3h>(L, &pTransform->rotation);
    } else {
        newVectorHandle<vec3h>(L, &pTransform->scale);
    }
    return 1;
}

int l_TransformComponentNewIndex(lua_State* L) {
    //table, key, value as arguments
    void* userdata = luaL_checkudata(L, 1, g_TransformComponentMTName);
    const char* key = luaL_checkstring(L, 2);
    int valueType = lua_type(L, 3);
    luaL_argcheck(L, (valueType == LUA_TTABLE) or (valueType == LUA_TUSERDATA), 3, "valid Vector object expected");

    // get key
    bool isPos = strcmp("position", key) == 0;
    bool isRot = strcmp("rotation", key) == 0;
    bool isScale = strcmp("scale", key) == 0;
    luaL_argcheck(L, isPos || isRot || isScale, 2, "'position' or 'rotation' or 'scale' expected");

    // get table
    lua_getiuservalue(L, 1, 1); // pushes entity onto stack
    auto* pTransform = static_cast<Transform*>(lua_touserdata(L, -1));
    lua_pop(L, 1);

    //get value
    if(valueType == LUA_TTABLE) {

        for(int i = 1, isNumber = false; i <= 3; i++) {
            lua_rawgeti(L, 3, i);

            lua_Number number = lua_tonumberx(L, -1, &isNumber);
            if(isNumber and not lua_isnil(L, -1)) {
                if(isPos) {
                    pTransform->position[i - 1] = number;
                } else if(isRot) {
                    pTransform->rotation[i - 1] = number;
                } else if(isScale) {
                    pTransform->scale[i - 1] = number;
                }
            } else {
                luaL_argerror(L, 3, "table must have 3 non-nil numeric values at indices [1], [2], [3]");
            }
            lua_pop(L, 1);
        }
    } else if(valueType == LUA_TUSERDATA) {

        void* userdata = luaL_checkudata(L, 3, g_vecMTName<vec3h>);
        luaL_argcheck(L, userdata != nullptr, 3, "vector3 handle expected");

        lua_getiuservalue(L, 3, 1); // pushes entity onto stack
        auto* pVec = static_cast<vec3h*>(lua_touserdata(L, -1));
        lua_pop(L, 1);

        if(isPos) {
            pTransform->position = *pVec;
        } else if(isRot) {
            pTransform->rotation = *pVec;
        } else if(isScale) {
            pTransform->scale = *pVec;
        }
    }

    return 0;
}

int l_TransformComponentToString(lua_State* L) {
    return 0;
}

/// Expects a stack with table 'BIGGEngine' at the top.
void open_TransformComponentMT(lua_State* L) {
    luaL_newmetatable(L, g_TransformComponentMTName);
    // set __index
    lua_getfield(L, -2, g_TransformComponentIndexFuncName);
    lua_setfield(L, -2, "__index");
    // set __newindex
    lua_getfield(L, -2, g_TransformComponentNewIndexFuncName);
    lua_setfield(L, -2, "__newindex");

    lua_pop(L, 1);
}
