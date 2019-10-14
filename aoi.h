#ifndef __AOI_H__
#define __AOI_H__

#ifndef aoi_maybe_yes
  #define aoi_maybe_yes(x)  __builtin_expect(!!(x), 1)
#endif

#ifndef aoi_maybe_not
  #define aoi_maybe_not(x) __builtin_expect(!!(x), 0)
#endif

#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* 需要定制内存分配器的改这里 */
#include <stdlib.h>
#define release(p) free(p)
#define allocate(size) malloc(size)
/* 需要定制内存分配器的改这里 */

#ifdef __cplusplus
  extern "C" {
#endif

/* 单位 */
struct Unit {
  int64_t  id;
  uint32_t x;
  uint32_t y;
};

struct Units {
  int64_t  id;
  uint32_t x;
  uint32_t y;
  struct Units *next;
};

/* 双向链表 */
struct DoubleLink {
  int64_t  id;
  uint32_t x;
  uint32_t y;
  struct DoubleLink* prevx;
  struct DoubleLink* nextx;
  struct DoubleLink* prevy;
  struct DoubleLink* nexty;
};

/* AOI */
struct Aoi {
  uint32_t x;
  uint32_t y;
  struct DoubleLink* xlist;
  struct DoubleLink* ylist;
};

#define aoi_log(fmt, ...) {printf(fmt, ##__VA_ARGS__); fflush(stdin);}

static inline void aoi_dump(struct Aoi* aoi) {
  aoi_log("********aoi_dump*******\n")
  aoi_log("Aoi = {\r\n");
  aoi_log(" x = %u, y = %u,\n", aoi->x, aoi->y);
  uint32_t xok = 0, yok = 0;
  struct DoubleLink *xnode = aoi->xlist;
  struct DoubleLink *ynode = aoi->ylist;
  while (!xok || !yok) {
    if (!xok) {
      aoi_log(" xlist={\n")
      while (1) {
        if (!xnode){ xok++; break; }
        aoi_log("   doublinkX(%p)={id=%lld,x=%u,y=%u,prevx=%p,nextx=%p}\n", xnode, xnode->id, xnode->x, xnode->y, xnode->prevx, xnode->nextx);
        xnode = xnode->nextx;
      }
      aoi_log(" },\r\n");
    }
    if (!yok) {
      aoi_log(" ylist={\n")
      while (1) {
        if (!ynode) { yok++; break; }
        aoi_log("   doublinkY(%p)={id=%lld,y=%u,x=%u,prevy=%p,nexty=%p}\n", ynode, ynode->id, ynode->y, ynode->x, ynode->prevy, ynode->nexty);
        ynode = ynode->nexty;
      }
      aoi_log(" }\r\n");
    }
  }
  aoi_log("}\r\n");
  aoi_log("***********************\n")
}

static inline void aoi_unit_dump(struct Unit* unit) {
  aoi_log("Unit(%p)={ID=%lld,X=%u,Y=%u}\n", unit, unit->id, unit->x, unit->y);
}

static inline void aoi_units_dump(struct Units* unit) {
  aoi_log("Utils(%p)={id=%lld,x=%u,y=%u,next=%p}\n", unit, unit->id, unit->x, unit->y, unit->next);
}

static inline void aoi_node_dump(struct Unit* unit) {
  aoi_log("Node(%p)={ID=%lld,X=%u,Y=%u,prevx=%p,prevy=%p,nextx=%p,nexty=%p}\n",
    ((struct DoubleLink *)unit), ((struct DoubleLink *)unit)->id,
    ((struct DoubleLink *)unit)->x, ((struct DoubleLink *)unit)->y,
    ((struct DoubleLink *)unit)->prevx, ((struct DoubleLink *)unit)->prevy,
    ((struct DoubleLink *)unit)->nextx, ((struct DoubleLink *)unit)->nexty
  );
}

/* 移动 */
int32_t aoi_move(struct Aoi* aoi, struct Unit* unit, uint32_t to_x, uint32_t to_y, struct Units** units, uint32_t* radius);

/* 离开 */
int32_t aoi_leave(struct Aoi* aoi, struct Unit* unit, struct Units** units, uint32_t* radius);

/* 进入 */
int32_t aoi_enter(struct Aoi* aoi, struct Unit* unit, struct Units** units, uint32_t* radius);

/* 创建Unit对象 */
struct Unit* aoi_unit_create(int64_t id, uint32_t x, uint32_t y);

/* 销毁Unit对象 */
void aoi_unit_destory(struct Unit* unit);

/* 创建Units对象 */
void aoi_units_destory(struct Units* units);

/* 销毁Units对象 */
void aoi_units_destory(struct Units* units);

/* 创建aoi对象 */
struct Aoi* aoi_create(uint32_t x, uint32_t y);

/* 销毁aoi对象 */
void aoi_destory(struct Aoi* aoi);

#ifdef __cplusplus
  }
#endif

#endif
