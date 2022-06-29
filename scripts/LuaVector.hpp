#pragma once

const char* g_VectorMTName = "BIGGEngine.Vector";
const char* g_VectorIndexFuncName = "VectorIndex";
const char* g_VectorNewIndexFuncName = "VectorNewIndex";
const char* g_VectorToStringFuncName = "VectorToString";

/// Registers the metatable "BIGGEngine.Vector"
/// Expects a stack with table 'BIGGEngine' at the top.
void open_VectorMT(lua_State* L);

/// creates and pushes a new table onto the stack with metatable BIGGEngine.Vector.
/// @param dimension is how many elements the table will have (eg. 2 for vec2)
void newVector(lua_State* L, int dimension);

int l_VectorIndex(lua_State* L);
int l_VectorNewIndex(lua_State* L);

/// Outputs a string like "(1.00, 0.53, -3000.23, ???)" ('???' indicates a non-number type)
int l_VectorToString(lua_State* L);