#include "aoi.h"

#include <unistd.h>
#include <math.h>
#include <time.h>

#define LOG(fmt, ...) printf(fmt, ##__VA_ARGS__)


void test_specify_enter() {

  struct Aoi *aoi = aoi_create(1000, 1000);

  aoi_dump(aoi);

  struct Unit* unit = aoi_unit_create(100, 500, 500);

  aoi_unit_dump(unit);

  aoi_enter(aoi, unit, NULL, NULL);

  aoi_dump(aoi);

  aoi_leave(aoi, unit, NULL, NULL);

  aoi_unit_destory(unit);

  aoi_dump(aoi);

  aoi_destory(aoi);

}

void test_rand_enter() {

  struct Aoi *aoi = aoi_create(1000, 1000);

  srand((unsigned) time(NULL));

  struct Unit* units[10] = {};

  aoi_dump(aoi);

  for (uint32_t i = 1; i<= 10; i ++) {

    units[i - 1] = aoi_unit_create(i, rand() % 1000, rand() % 1000);

    aoi_enter(aoi, units[i - 1], NULL, NULL);

  }

  aoi_dump(aoi);

  for (uint32_t i = 1; i<= 10; i ++) {

    aoi_leave(aoi, units[i - 1], NULL, NULL);

    aoi_unit_destory(units[i]);

  }

  aoi_dump(aoi);

  aoi_destory(aoi);

}

void test_enter_and_leave() {

  struct Aoi *aoi = aoi_create(1000, 1000);

  struct Unit* units[10] = {};

  for (uint32_t i = 1; i<= 10; i ++) {

    units[i - 1] = aoi_unit_create(i, i * 10, i * 10);

    aoi_enter(aoi, units[i - 1], NULL, NULL);

  }

  aoi_dump(aoi);

  for (uint32_t i = 1; i<= 10; i ++) {

    aoi_leave(aoi, units[i - 1], NULL, NULL);

    aoi_unit_destory(units[i]);

  }

  aoi_dump(aoi);

  aoi_destory(aoi);

}

void test_enter_and_neighbor() {

  struct Aoi *aoi = aoi_create(1000, 1000);

  struct Unit* units[10] = {};

  for (uint32_t i = 1; i<= 10; i ++) {

    units[i - 1] = aoi_unit_create(i, i * 10, i * 10);

    aoi_enter(aoi, units[i - 1], NULL, NULL);

  }

  aoi_dump(aoi);

  int32_t quantity = 0;

  uint32_t radius = 15;

  struct Units* neighbor = NULL;

  struct Unit* u = aoi_unit_create(10000, 25, 25);

  quantity = aoi_enter(aoi, u, &neighbor, &radius);

  LOG("quantity = %d\n", quantity);

  if (quantity > 0) {

    for (struct Units* us = neighbor; us != NULL; us = us -> next)

      aoi_units_dump(us);

    aoi_units_destory(neighbor);
  }

  aoi_dump(aoi);

  for (uint32_t i = 1; i<= 10; i ++) {

    quantity = aoi_leave(aoi, units[i - 1], &neighbor, &radius);

    if (quantity > 0) {

      for (struct Units* us = neighbor; us != NULL; us = us -> next)

        aoi_units_dump(us);

      aoi_units_destory(neighbor);
    }

    aoi_unit_destory(units[i]);

  }

  aoi_destory(aoi);

}

void test_move() {

  struct Aoi *aoi = aoi_create(1000, 1000);

  struct Unit* u1 = aoi_unit_create(101, 100, 100);

  struct Unit* u2 = aoi_unit_create(102, 200, 200);

  struct Unit* u3 = aoi_unit_create(103, 300, 300);

  struct Units* neighbor = NULL;

  uint32_t radius = 50;

  aoi_enter(aoi, u1, NULL, NULL);

  aoi_enter(aoi, u2, NULL, NULL);

  aoi_enter(aoi, u3, NULL, NULL);

  aoi_dump(aoi);

  /* 测试是否需要插入 */
  uint32_t quantity = aoi_move(aoi, u2, 250, 250, &neighbor, &radius);

  if (quantity) {

    aoi_units_dump(neighbor);

    aoi_units_destory(neighbor);

  }

  aoi_dump(aoi);

  aoi_leave(aoi, u1, NULL, NULL);

  aoi_unit_destory(u1);

  aoi_leave(aoi, u2, NULL, NULL);

  aoi_unit_destory(u2);

  aoi_leave(aoi, u3, NULL, NULL);

  aoi_unit_destory(u3);

  aoi_dump(aoi);

  aoi_destory(aoi);

}


int main(int argc, char const *argv[]) {

  /* 测试进入指定位置 */
  // test_specify_enter();

  /* 测试顺序进入 */
  // test_rand_enter();

  /* 测试随机进入任意位置 */
  // test_enter_and_leave();

  /* 测试进入与离开时获取临近范围单位 */
  // test_enter_and_neighbor();

  /* 测试移动 */
  // test_move();

  return 0;
}
