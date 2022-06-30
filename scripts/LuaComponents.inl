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
    }
    {
        newVectorHandle<vec3h>(L, &pTransform->scale);
    }
    return 1;
}

int l_TransformComponentNewIndex(lua_State* L) {
    /// take in a table with members [0], [1], [2] (aka BIGGEngine.Vector)
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
