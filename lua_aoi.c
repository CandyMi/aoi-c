#define LUA_LIB

#define release(ptr) free(ptr)
#define allocate(size) malloc(size)

#include "aoi.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"


static inline void lua_aoi_register(lua_State *L, const char* meta, luaL_Reg libs[]) {
  if (LUA_MINSTACK - lua_gettop(L) < 3)
    luaL_checkstack(L, 3, "check lua stack error.");

  luaL_newmetatable(L, meta);

  lua_pushstring (L, "__index");
  lua_pushvalue(L, -2);
  lua_rawset(L, -3);
  
  lua_pushliteral(L, "__mode");
  lua_pushliteral(L, "kv");
  lua_rawset(L, -3);

  luaL_setfuncs(L, libs, 0);

  lua_pop(L, 1);
}

static int laoi_dump(lua_State* L) {
  aoi_dump(luaL_checkudata(L, 1, "__Aoi__"));
  return 0;
}

static int laoi_unit_dump(lua_State* L) {
  aoi_unit_dump(luaL_checkudata(L, 1, "__Unit__"));
  return 0;
}

static int laoi_enter(lua_State* L) {
  struct Aoi* aoi = luaL_checkudata(L, 1, "__Aoi__");
  struct Unit* unit = luaL_checkudata(L, 2, "__Unit__");
  if (unit->x > aoi->x || unit->y > aoi->y)
    return luaL_error(L, "Aoi Error: 无效的Unit(x || y)");
  
  lua_Integer radius = luaL_optinteger(L, 3, 0);
  if ( 0 >= radius )
    return aoi_enter(aoi, unit, NULL, NULL);

  struct Units* neighbor = NULL;
  int quantity = aoi_enter(aoi, unit, &neighbor, (uint32_t*)&radius);
  if (0 >= quantity)
    return 0;

  int index = 1;
  lua_createtable(L, quantity, 0);
  for (struct Units* us = neighbor; us != NULL; us = us -> next){
    lua_createtable(L, 0, 3);

    lua_pushliteral(L, "id");
    lua_pushinteger(L, us->id);
    lua_rawset(L, -3);

    lua_pushliteral(L, "x");
    lua_pushinteger(L, us->x);
    lua_rawset(L, -3);

    lua_pushliteral(L, "y");
    lua_pushinteger(L, us->y);
    lua_rawset(L, -3);
    lua_rawseti(L, -2, index++);
  }
  aoi_units_destory(neighbor);
  return 1;
}

static int laoi_move(lua_State* L) {
  struct Aoi* aoi = luaL_checkudata(L, 1, "__Aoi__");
  struct Unit* unit = luaL_checkudata(L, 2, "__Unit__");
  if (unit->x > aoi->x || unit->y > aoi->y)
    return luaL_error(L, "Aoi Error: 无效的Unit(x || y)");

  lua_Integer to_x = luaL_checkinteger(L, 3);
  lua_Integer to_y = luaL_checkinteger(L, 4);
  if (to_x > aoi->x || to_y > aoi->y)
    return luaL_error(L, "Aoi Error: 无效的Unit(x || y)");

  lua_Integer radius = luaL_optinteger(L, 5, 0);
  if (0 >= radius)
    return aoi_move(aoi, unit, to_x, to_y, NULL, NULL);

  struct Units* neighbor = NULL;
  int quantity = aoi_move(aoi, unit, to_x, to_y, &neighbor, (uint32_t*)&radius);
  if (0 >= quantity)
    return 0;

  int index = 1;
  lua_createtable(L, quantity, 0);
  for (struct Units* us = neighbor; us != NULL; us = us -> next){
    lua_createtable(L, 0, 3);

    lua_pushliteral(L, "id");
    lua_pushinteger(L, us->id);
    lua_rawset(L, -3);

    lua_pushliteral(L, "x");
    lua_pushinteger(L, us->x);
    lua_rawset(L, -3);

    lua_pushliteral(L, "y");
    lua_pushinteger(L, us->y);
    lua_rawset(L, -3);
    lua_rawseti(L, -2, index++);
  }
  aoi_units_destory(neighbor);
  return 1;
}

static int laoi_leave(lua_State* L) {
  struct Aoi* aoi = luaL_checkudata(L, 1, "__Aoi__");
  struct Unit* unit = luaL_checkudata(L, 2, "__Unit__");
  if (unit->x > aoi->x || unit->y > aoi->y)
    return luaL_error(L, "Aoi Error: 无效的Unit(x || y)");

  lua_Integer radius = luaL_optinteger(L, 3, 0);
  if (0 >= radius)
    return aoi_leave(aoi, unit, NULL, NULL);

  struct Units* neighbor = NULL;
  int quantity = aoi_leave(aoi, unit, &neighbor, (uint32_t*)&radius);
  if (0 >= quantity)
    return 0;
  int index = 1;
  lua_createtable(L, quantity, 0);
  for (struct Units* us = neighbor; us != NULL; us = us -> next){
    lua_createtable(L, 0, 3);

    lua_pushliteral(L, "id");
    lua_pushinteger(L, us->id);
    lua_rawset(L, -3);

    lua_pushliteral(L, "x");
    lua_pushinteger(L, us->x);
    lua_rawset(L, -3);

    lua_pushliteral(L, "y");
    lua_pushinteger(L, us->y);
    lua_rawset(L, -3);
    lua_rawseti(L, -2, index++);
  }
  aoi_units_destory(neighbor);
  return 1;
}

static int laoi_aoi_xy(lua_State* L) {
  struct Aoi* aoi = (struct Aoi *)luaL_checkudata(L, 1, "__Aoi__");
  lua_pushinteger(L, aoi->x);
  lua_pushinteger(L, aoi->y);
  return 2;
}

static int laoi_new_aoi(lua_State* L) {
  lua_Integer x = luaL_checkinteger(L, 1);
  lua_Integer y = luaL_checkinteger(L, 2);

  struct Aoi* aoi = lua_newuserdata(L, sizeof(struct Aoi));
  if (!aoi)
    return 0;
  aoi->x = x;
  aoi->y = y;
  aoi->xlist = NULL;
  aoi->ylist = NULL;
  luaL_setmetatable(L, "__Aoi__");
  return 1;
}

static int laoi_unit_xy(lua_State* L) {
  struct Unit* unit = (struct Unit *)luaL_checkudata(L, 1, "__Unit__");
  lua_pushinteger(L, unit->x);
  lua_pushinteger(L, unit->y);
  return 2;
}

static int laoi_new_unit(lua_State* L) {
  lua_Integer id = luaL_checkinteger(L, 1);
  lua_Integer x = luaL_checkinteger(L, 2);
  lua_Integer y = luaL_checkinteger(L, 3);
  struct DoubleLink* node = lua_newuserdata(L, sizeof(struct DoubleLink));
  if (!node)
    return 0;
  node->id = id; node->x = x; node->y = y;
  node->prevx = NULL; node->nextx = NULL;
  node->prevy = NULL; node->nexty = NULL;
  luaL_setmetatable(L, "__Unit__");
  return 1;
}


LUAMOD_API int luaopen_aoi(lua_State *L) {
  luaL_checkversion(L);
  // 创建Aoi对象元表
  lua_aoi_register(L, "__Aoi__", (luaL_Reg []) {
    {"dump",  laoi_dump},
    {"move",  laoi_move},
    {"enter", laoi_enter},
    {"leave", laoi_leave},
    {"get_xy", laoi_aoi_xy},
    {NULL, NULL},
  });
  // 创建Unit对象元表
  lua_aoi_register(L, "__Unit__", (luaL_Reg []) {
    {"get_xy", laoi_unit_xy},
    {"dump", laoi_unit_dump},
    {NULL, NULL},
  });
  // 注册创建函数
  luaL_Reg aoi_libs[] = {
    {"new_aoi", laoi_new_aoi},
    {"new_unit", laoi_new_unit},
    {NULL, NULL},
  };
  luaL_newlib(L, aoi_libs);
  return 1;
}