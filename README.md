<h1 align="center">简易十字链表AOI(Area Of Interest)算法实现</h5>

## 编译与运行(complier and run)

  * 使用`make build`命令编译动态库.

  * 使用`make test`编译`test.c`进行代码测试.

  * 使用`make clean`清理编译文件.

## 用法(usage)

### API 介绍

  * 创建Aoi对象

  ```C
  struct Aoi* aoi_create(uint32_t x, uint32_t y);
  ```

  * 销毁Aoi对象

  ```C
  void aoi_destory(struct Aoi* aoi);
  ```

  * 创建Unit对象

  ```C
  struct Unit* aoi_unit_create(int64_t id, uint32_t x, uint32_t y);
  ```

  * 销毁Unit对象

  ```C
  void aoi_unit_destory(struct Unit* unit);
  ```

  * 销毁units对象

  ```C
  struct Units* aoi_units_destory(void);
  ```

  * Aoi进入

  ```C
  int32_t aoi_enter(struct Aoi* aoi, struct Unit* unit, struct Units** units, uint32_t* radius);
  ```

  * Aoi离开

  ```C
  int32_t aoi_leave(struct Aoi* aoi, struct Unit* unit, struct Units** units, uint32_t* radius);
  ```

  * Aoi移动

  ```C
  int32_t aoi_move(struct Aoi* aoi, struct Unit* unit, uint32_t to_x, uint32_t to_y, struct Units** units, uint32_t* radius);
  ```

  示例参见`test.c`.

## 算法复杂度

  `aoi_enter` O(n)

  `aoi_move` O(lgn)

  `aoi_leave` O(1)

## 适用性

  十字链表主要用于优化查找的场景, 所以牺牲了插入的性能来提升查询的性能(因为敢兴趣点区域总在附近的链表上).

  `move`阶段我们删除的时候可以直接断开链表链接, 然后根据之前的链表判断是移动点位后再次进行插入. 这会比直接`aoi_enter`效率要高, 因为移动一般是小范围的.

  这里再介绍一种场景. 对于某个单位短时间内(1ms)对于大范围的move, 例如: `Unit(500, 500) -> Unit(1, 1)`.

  这种不属于`move`的范畴, 它更类似于游戏场景中的瞬移(Teleport). 使用move也许无法得到正确答案. 因为我们Aoi算法中无法判断这种情况.

  并且这种情况使用`move`是完全没有意义的, 这时候计算出的时间复杂度大部分情况下可能还不如`aoi_enter` + `aoi_leave`, 且中间路程上的单位计算也完全没意义.

  十字链表算法适用于大量(临时)Scense场景, 它将空间复杂度降低为O(1). 比传统网格类算法有空间上的优势(因为是基于Unit).

## 协议

  [MIT](https://github.com/CandyMi/aoi-c/blob/master/LICENSE)
