//
// Created by Stefan Antoszko on 2022-06-30.
//

#pragma once

// Vector MT and VectorHandle MT.
// myvec = this.transform.position + {0, 1, 2}
// adding a vector handle + vector = vector
// adding a vector handle + vectorHandle = vector
// adding a vector handle + temporary table = vector
// adding a handle + temporary table = vector

/// Leaves lua Stack unchanged.
/// May throw a lua error if the first argument is not a vector handle.
/// @returns a vector pointer.
template<typename VecT>
VecT* getVectorHandleArg(lua_State* L) {

    void* userdata = luaL_checkudata(L, 1, g_vecMTName<VecT>);
    luaL_argcheck(L, userdata != nullptr, 1, "vector handle expected");

    lua_getiuservalue(L, 1, 1); // pushes entity onto stack
    auto* pVec = static_cast<VecT*>(lua_touserdata(L, -1));
    lua_pop(L, 1);

    return pVec;
}

/// Leaves lua Stack unchanged.
/// May throw a lua error with message @p errorMsg if the @p argIndex argument is not a valid lua string.
const char* getStringArg(lua_State* L, int argIndex, const char* errorMsg = "valid string expected") {
    const char* s = luaL_checkstring(L, argIndex);
    luaL_argcheck(L, s != nullptr, argIndex, errorMsg);
    return s;
}

/// May throw a lua error if the first argument is not a vector handle or if second argument
/// is an invalid index.
template<typename VecT>
int l_vectorHandleIndex(lua_State* L) {
    const VecT* pVec = getVectorHandleArg<VecT>(L);
    lua_Integer index = luaL_checkinteger(L, 2);

    luaL_argcheck(L, index > 0 && index <= VecT::length(), 2, "invalid index");

    lua_pushnumber(L, pVec->operator[](index - 1)); // index-1 since lua indices are one higher than c indices.
    return 1;
}

/// May throw a lua error if the first argument is not a vector handle or if second argument
/// is an invalid index.
template<typename VecT>
int l_vectorHandleNewIndex(lua_State* L) {
    VecT* pVec = getVectorHandleArg<VecT>(L);
    lua_Integer index = luaL_checkinteger(L, 2);
    lua_Number value = luaL_checknumber(L, 3);

    luaL_argcheck(L, index > 0 && index <= VecT::length(), 2, "invalid index");

    pVec->operator[](index - 1) = value;
    return 0;
}

template<typename VecT>
int l_vectorHandleToString(lua_State* L) {
    VecT* pVec = getVectorHandleArg<VecT>(L);

    std::string s;
    if(VecT::length() == 2) {
        s = fmt::format("({:.3f}, {:.3f})", pVec->x, pVec->y);
    } else if(VecT::length() == 3) {
        s = fmt::format("({:.3f}, {:.3f}, {:.3f})", pVec->x, pVec->y, (*pVec)[2]);
    }
    lua_pushstring(L, s.c_str());
    return 1;
}


/// Pushes a BIGGEngine.VectorHandle<VecT> onto the stack.
template<typename VecT>
void newVectorHandle(lua_State* L, VecT* pVec) {
    if(pVec == nullptr) {
        // create a new userdata which is really an array. set its user value to itself
        // so other functions work the same.
        auto* userdata = static_cast<VecT*>(lua_newuserdatauv(L, sizeof(typename VecT::value_type) * VecT::length(),
                                                              1));
        (*userdata) = VecT(0); // init it to 0.
        lua_pushlightuserdata(L, userdata);
        lua_setiuservalue(L, -2, 1);
    } else {
        lua_newuserdatauv(L, 0, 1);
        lua_pushlightuserdata(L, pVec);
        lua_setiuservalue(L, -2, 1);
    }

    luaL_setmetatable(L, g_vecMTName<VecT>);
}

/// Registers metatable BIGGEngine.[x]Vector[y]Handle. Leaves lua stack unchanged.
/// where [x] is optionally @p I for integer and [y] is @p 2 or @p 3.
/// Expects a stack with table 'BIGGEngine' at the top.
template<typename VecT>
void open_VectorHandle(lua_State* L) {
    luaL_newmetatable(L, g_vecMTName<VecT>);
    lua_getfield(L, -2, g_vecIndexFuncName<VecT>); // push function BIGGEngine.VecIndex to top
    lua_setfield(L, -2, "__index"); // set mt.__index = BIGGEngine.VecIndex

    lua_getfield(L, -2, g_vecNewIndexFuncName<VecT>);
    lua_setfield(L, -2, "__newindex");

    lua_getfield(L, -2, g_vecToStringFuncName<VecT>);
    lua_setfield(L, -2, "__tostring");

    lua_pop(L, 1); // pop metatable Vec3Handle
}