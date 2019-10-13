#include "aoi.h"

/* 创建Units对象 */
struct Units* aoi_units_create(void) {
  struct Units* units = allocate(sizeof(struct Units));
  if (!units)
    return NULL;
  units->next = NULL;
  units->x = 0; units->y = 0; units->id = 0;
  return units;
}

/* 销毁Units对象 */
void aoi_units_destory(struct Units* units) {
  while (1) {
    struct Units* next = units->next;
    release(units);
    if (!next)
      break;
    units = next;
  }
}

static inline void aoi_node_destory(struct Aoi* aoi) {
  struct DoubleLink* node = aoi->ylist;
  aoi->ylist = NULL;
  aoi->xlist = NULL;
  while (node) {
    struct DoubleLink* next = node->nexty;
    release(node);
    if (!next)
      break;
    node = next;
  }
  return ;
}

static inline uint32_t aoi_node_range(struct Aoi* aoi, struct DoubleLink* node, struct Units** units, uint32_t radius) {
  uint32_t quantity = 0;

  struct DoubleLink* prev = node->prevx;
  struct DoubleLink* next = node->nextx;
  int32_t maxY = node->y + radius;
  int32_t minY = node->y - radius < 0 ? 0 : node->y - radius;
  int32_t maxX = node->x + radius;
  int32_t minX = node->x - radius < 0 ? 0 : node->x - radius;

  *units = NULL;
  struct Units** list;

  while (prev) {
    if (prev->x < minX)
      break;
    if (prev->y >= minY && prev->y <= maxY){
      if (!quantity){
        *units = aoi_units_create();
        if (!*units)
          return quantity;
        (*units)->x = prev->x;  (*units)->y = prev->y;  (*units)->id = prev->id;
        list = &((*units)->next);
        quantity++;
      } else {
        *list = aoi_units_create();
        if (!list)
          return quantity;
        (*list)->x = prev->x;  (*list)->y = prev->y;  (*list)->id = prev->id;
        list = &((*list)->next);
        quantity++;
      }
    }
    prev = prev->prevx;
  }
  while (next) {
    if (next->x > maxX)
      break;
    if (next->y >= minY && next->y <= maxY){
      if (!quantity){
        *units = aoi_units_create();
        if (!*units)
          return quantity;
        (*units)->x = next->x;  (*units)->y = next->y;  (*units)->id = next->id;
        list = &((*units)->next);
        quantity++;
      } else {
        *list = aoi_units_create();
        if (!list)
          return quantity;
        (*list)->x = next->x;  (*list)->y = next->y;  (*list)->id = next->id;
        list = &((*list)->next);
        quantity++;
      }
    }
    next = next->nextx;
  }
  return quantity;
}

static inline void aoi_node_remove_x(struct Aoi* aoi, struct DoubleLink* node) {
  /* 删除思路: (A <-> B <-> C) To (A < - > C) 并做好空指针判断. */
  struct DoubleLink* prev = node->prevx;
  struct DoubleLink* next = node->nextx;
  if (prev) {
    prev->nextx = next;
    if (next)
      next->prevx = prev;
  } else {
    aoi->xlist = next;
    if (next)
      next->prevx = NULL;
  }
}

static inline void aoi_node_remove_y(struct Aoi* aoi, struct DoubleLink* node) {
  /* 删除思路: (A <-> B <-> C) To (A < - > C) 并做好空指针判断. */
  struct DoubleLink* prev = node->prevy;
  struct DoubleLink* next = node->nexty;
  if (prev) {
    prev->nexty = next;
    if (next)
      next->prevy = prev;
  } else {
    aoi->ylist = next;
    if (next)
      next->prevy = NULL;
  }
}

static inline struct DoubleLink* aoi_node_remove(struct Aoi* aoi, struct DoubleLink* node) {
  aoi_node_remove_x(aoi, node);
  aoi_node_remove_y(aoi, node);
  return node;
}

static inline void aoi_node_insert_x(struct Aoi* aoi, struct DoubleLink* node) {
  struct DoubleLink *xnode = aoi->xlist;
  if (!xnode) {
    aoi->xlist = node;
    return ;
  }
  while (1) {
    if (xnode->x < node->x) {
      if (xnode->nextx) {
        xnode = xnode->nextx;
        continue;
      }
      xnode->nextx = node;
      node->prevx = xnode;
      return;
    }
    node->nextx = xnode;
    node->prevx = xnode->prevx;
    if (xnode->prevx) {
      xnode->prevx = node;
      node->prevx->nextx = node;
      return ;
    }
    aoi->xlist = node;
    xnode->prevx = node;
    return ;
  }
}

static inline void aoi_node_insert_y(struct Aoi* aoi, struct DoubleLink* node) {
  struct DoubleLink *ynode = aoi->ylist;
  if (!ynode) {
    aoi->ylist = node;
    return ;
  }
  while (1) {
    if (ynode->y < node->y) {
      if (ynode->nexty) {
        ynode = ynode->nexty;
        continue;
      }
      ynode->nexty = node;
      node->prevy = ynode;
      return;
    }
    node->nexty = ynode;
    node->prevy = ynode->prevy;
    if (ynode->prevy) {
      ynode->prevy = node;
      node->prevy->nexty = node;
      return ;
    }
    aoi->ylist = node;
    ynode->prevy = node;
    return ;
  }
}

static inline struct DoubleLink* aoi_node_insert(struct Aoi* aoi, struct DoubleLink* node) {
  node->prevx = NULL; node->nextx = NULL;
  node->prevy = NULL; node->nexty = NULL;
  aoi_node_insert_x(aoi, node);
  aoi_node_insert_y(aoi, node);
  return node;
}

static inline void aoi_node_remove_and_insert_x(struct Aoi* aoi, struct DoubleLink* node, uint32_t to_x) {
  if (node->x == to_x)
    return;
  struct DoubleLink* prev = node->prevx;
  struct DoubleLink* next = node->nextx;
  if (!prev && !next){
    node->x = to_x;
    return;
  }
  /* 以下判断对象在链表中是否需要移动. */
  if (next && next->x < to_x) {
    struct DoubleLink *before = NULL;
    aoi_node_remove_x(aoi, node);
    node->x = to_x;
    while (1) {
      if (next) {
        if (next->x > node->x) {
          node->nextx = next;
          node->prevx = next->prevx;
          node->prevx->nextx = node->nextx->prevx = node;
          return ;
        }
      }else{
        node->prevx = before;
        node->nextx = NULL;
        before->nextx = node;
        return ;
      }
      before = next;
      next = next->nextx;
    }
  }
  if (prev && prev->x > to_x) {
    struct DoubleLink *before = NULL;
    aoi_node_remove_x(aoi, node);
    node->x = to_x;
    while (1) {
      if (prev) {
        if (prev->x < node->x) {
          node->prevx = prev;
          node->nextx = prev->nextx;
          node->prevx->nextx = node->nextx->prevx = node;
          return ;
        }
      }else{
        node->nextx = before;
        before->prevx = node;
        aoi->xlist = node;
        return;
      }
      before = prev;
      prev = prev->prevx;
    }
  }
  node->x = to_x;
  return;
}

static inline void aoi_node_remove_and_insert_y(struct Aoi* aoi, struct DoubleLink* node, uint32_t to_y) {
  if (node->y == to_y)
    return;
  struct DoubleLink* prev = node->prevy;
  struct DoubleLink* next = node->nexty;
  if (!prev && !next){
    node->y = to_y;
    return;
  }
  if (next && next->y < to_y) {
    struct DoubleLink *before = NULL;
    aoi_node_remove_y(aoi, node);
    node->y = to_y;
    while (1) {
      if (next) {
        if (next->y > node->y) {
          node->nexty = next;
          node->prevy = next->prevy;
          node->prevy->nexty = node->nexty->prevy = node;
          return ;
        }
      }else{
        node->prevy = before;
        node->nexty = NULL;
        before->nexty = node;
        return ;
      }
      before = next;
      next = next->nexty;
    }
  }
  if (prev && prev->y > to_y) {
    struct DoubleLink *before = NULL;
    aoi_node_remove_y(aoi, node);
    node->y = to_y;
    while (1) {
      if (prev) {
        if (prev->y < node->y) {
          node->prevy = prev;
          node->nexty = prev->nexty;
          node->prevy->nexty = node->nexty->prevy = node;
          return ;
        }
      }else{
        node->nexty = before;
        before->prevy = node;
        aoi->ylist = node;
        return;
      }
      before = prev;
      prev = prev->prevy;
    }
  }
  node->y = to_y;
  return;
}

static inline uint32_t aoi_node_move(struct Aoi* aoi, struct DoubleLink* node, uint32_t to_x, uint32_t to_y, struct Units** units, uint32_t* radius) {
  uint32_t quantity = 0;
  uint32_t X = 0;
  uint32_t Y = 0;
  if (node->x > to_x){
    X = node->x - to_x;
  }else{
    X = to_x - node->x;
  }
  if (node->y > to_y) {
    Y = node->y - to_y;
  }else{
    Y = to_y - node->y;
  }
  aoi_node_remove_and_insert_x(aoi, node, to_x);
  aoi_node_remove_and_insert_y(aoi, node, to_y);
  if (units && radius) {
    uint32_t offset = X > Y ? (*radius + X) : (*radius + Y);
    quantity += aoi_node_range(aoi, node, units, offset);
  }
  return quantity;
}

/* 移动 */
int32_t aoi_move(struct Aoi* aoi, struct Unit* unit, uint32_t to_x, uint32_t to_y, struct Units** units, uint32_t* radius) {
  if (aoi_maybe_not(!aoi))
    return -1;
  if (aoi_maybe_not(!unit))
    return -2;
  if (aoi_maybe_not(to_x > aoi->x))
    return -3;
  if (aoi_maybe_not(to_y > aoi->y))
    return -4;
  /* 位置相同什么都不做 */
  if (unit->x == to_x && unit->y == to_y)
    return 0;
  return aoi_node_move(aoi, (struct DoubleLink*)unit, to_x, to_y, units, radius);
}

/* 离开 */
int32_t aoi_leave(struct Aoi* aoi, struct Unit* unit, struct Units** units, uint32_t* radius) {
  if (aoi_maybe_not(!aoi))
    return -1;
  if (aoi_maybe_not(!unit))
    return -2;
  struct DoubleLink *node = (struct DoubleLink *)unit;
  aoi_node_remove(aoi, node);
  uint32_t quantity = 0;
  if (units)
    if (radius)
      quantity += aoi_node_range(aoi, node, units, *radius);
  node->prevx = NULL; node->nextx = NULL;
  node->prevy = NULL; node->nexty = NULL;
  return quantity;
}

/* 进入 */
int32_t aoi_enter(struct Aoi* aoi, struct Unit* unit, struct Units** units, uint32_t* radius) {
  if (aoi_maybe_not(!aoi))
    return -1;
  if (aoi_maybe_not(!unit))
    return -2;
  if (aoi_maybe_not(unit->x > aoi->x))
    return -3;
  if (aoi_maybe_not(unit->y > aoi->y))
    return -4;
  aoi_node_insert(aoi, (struct DoubleLink*)unit);
  uint32_t quantity = 0;
  if (units)
    if (radius)
      quantity += aoi_node_range(aoi, (struct DoubleLink *)unit, units, *radius);
  return quantity;
}

/* 创建aoi对象 */
struct Aoi* aoi_create(uint32_t x, uint32_t y) {
  struct Aoi* aoi = allocate(sizeof(struct Aoi));
  if (!aoi)
    return NULL;
  aoi->x = x, aoi->y = y;
  aoi->xlist = NULL; aoi->ylist = NULL;
  return aoi;
}

/* 销毁aoi对象 */
void aoi_destory(struct Aoi* aoi) {
  aoi_node_destory(aoi);
  aoi->x = 0; aoi->y = 0;
  release(aoi);
  return;
}

/* 创建unit对象 */
struct Unit* aoi_unit_create(int64_t id, uint32_t x, uint32_t y) {
  struct DoubleLink* node = allocate(sizeof(struct DoubleLink));
  if (!node)
    return NULL;
  node->id = id; node->x = x; node->y = y;
  node->prevx = NULL; node->nextx = NULL;
  node->prevy = NULL; node->nexty = NULL;
  return (struct Unit *)node;
}

/* 销毁unit对象 */
void aoi_unit_destory(struct Unit* unit) {
  release((struct DoubleLink *)unit);
}
