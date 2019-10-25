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
  void aoi_units_destory(struct Units* units);
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

## Lua C API

  编译方法可参考`make lua`并自行替换为实际路径与命名

#### 基本函数

```Lua
local Aoi = require "aoi"

-- 创建aoi对象
local aoi = Aoi.new_aoi(512, 512)

-- 创建unit对象
local unit = Aoi.new_unit(1, 100, 200)

-- 获取地图边界值
print(aoi:get_xy())

-- 获取unit位置
print(unit:get_xy())

-- 打印Aoi信息
aoi:dump()

-- 打印unit信息
unit:dump()
```

#### 进入、移动、离开

```Lua
local Aoi = require "aoi"

-- 创建aoi对象
local aoi = Aoi.new_aoi(512, 512)

-- 创建unit对象
local u1 = Aoi.new_unit(1, 100, 200)
local u2 = Aoi.new_unit(2, 150, 200)
local u3 = Aoi.new_unit(3, 200, 200)

-- 进入
aoi:enter(u1)
aoi:enter(u2)
aoi:enter(u3)

-- 打印进入后的Aoi信息
aoi:dump()

-- 从当前位置移动到to_x, to_y
--[[
  u1(100, 200) ->  u1(50, 200)
  u2(150, 200) -> u1(100, 200)
  u3(200, 200) -> u1(150, 200)
]]
aoi:move(u1, 50, 200)
aoi:move(u2, 100, 200)
aoi:move(u3, 150, 200)

-- 打印移动后的Aoi信息
aoi:dump()

-- 离开
aoi:leave(u1)
aoi:leave(u2)
aoi:leave(u3)

-- 打印离开后的Aoi信息
aoi:dump()
```

#### 关于Lua C API 参数返回值

  `enter`/`move`/`leave`最后都有一个缺省参数(`radius`), 这个参数用来确认开发者是否需要获取以当前坐标点为中心的范围内地玩家列表.

  默认情况下(`radius = 0 或 nil`)仅作进入/移动/离开, 所以这3个方法将不会有返回值. 并且无效的`unitx(x,y)`位置还将会引发一个异常.

  当需要获取的范围单位列表的时候, 开发者需要传递一个有效的radius参数. aoi库将会根据实际情况(参数)进行搜索.

  搜索范围单位列表通常对性能不会很大影响. 但是开发者并没有这个需求的话, 不传递radius会是一个很好的选择.

  需要注意的是: 单位列表通常用来反应进行相关事件操作后需要通知的其它单位. 所以如果在实际生产环境中, 您应该要合理设置一个radius值.

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
