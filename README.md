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

  编译方法可参考`make lua`并自行替换为实际路径与命名.

#### 函数原型

```Lua
Aoi.new_aoi(X, Y)
```

```Lua
Aoi.new_unit(ID, X, Y)
```

```Lua
unit:dump(nil)
```

```Lua
unit:get_xy(nil)
```

```Lua
aoi:dump(nil)
```

```Lua
aoi:get_xy(nil)
```

```Lua
aoi:enter(unit, radius) -- radius 可省略
```

```Lua
aoi:leave(unit, radius) -- radius 可省略
```

```Lua
aoi:move(unit, to_x, to_y, radius)  -- radius 可省略
```

#### 基本函数

```Lua
local Aoi = require "aoi"

-- 创建aoi对象
local aoi = Aoi.new_aoi(512, 512)

-- 创建unit对象
local unit = Aoi.new_unit(1, 100, 200)

-- 获取场景边界值
print(aoi:get_xy())

-- 获取unit位置
print(unit:get_xy())

-- 打印Aoi信息
aoi:dump()

-- 打印unit信息
unit:dump()
```

```Lua
-- 事件函数返回值类似这个数组.
unit_list = { {id = 1, x = 100, y = 200}, {id = 2, x = 150, y = 200}, {id = 3, x = 20, y = 200} }
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

#### 关于Lua C API 参数与返回值

  * 相关函数的调用方式必须使用冒号(`:`), 此要求是强制性的语法限制. 内部使用了元表来检查对象的类型.

  * 无需手动释放内存, 所有内存分配与释放交由`Lua VM`完成. 减少手动管理内存带来的头脑风暴.

  * `enter`/`move`/`leave`最后都有一个缺省参数(`radius`), 这个参数用来确认开发者是否需要获取以当前坐标点为中心的范围内地玩家列表.
  默认情况下(`radius <= 0 或 nil`)仅执行进入/移动/离开动作, 并且将不会有返回值. 无效的`unit(x,y)`位置还将会引发一个异常.

  * `move`不可在`enter`之前或者`leave`之后调用, 调用`enter`必须是一个新的或已经`leave`的`unit`.

  * `unit`列表通常用来反应进行相关事件操作后需要通知的其它单位. 所以如果在实际生产环境中, 您应该要合理设置一个`radius`值.

  * `dump`函数通常用来美化调试信息, 开发者可以利用这2个函数轻松的进行调试.

## 算法复杂度

  `aoi_enter` O(n)

  `aoi_move` O(lgn)

  `aoi_leave` O(1)

## 算法的适用性

  十字链表主要用于优化基于`Unit`的快速搜索场景, 以牺牲插入的性能来提升查询与删除效率. 提供更加合理的`Aoi`管理方式.

  十字链表适用于大量(或临时)Scense场景, 它基于有向图的链式数据结构. 其对比二维网格、灯塔算法有空间上的绝对优势.

  算法遵循`所有API都依赖于单位产生触发事件`, 这意味着无事件(`enter`/`move`/`leave`)发生就无需进行调整.

  在以下业务场景中, 它通常会工作的很好:

  1. 轻松管理单个场景中`5000` ~ `10000`个`Unit`的`leave`/`move`事件触发.

  2. 快速收集`兴趣区域范围Unit列表`, 这通常会比其他算法要快很多.

  还可以从下面的描述中得到一些建议:

  1. `短时间的长距离移动(Teleport)`不应该使用`move`, 这样只会得到错误的结果. 推荐使用`leave` + `enter`来获得结果集.

  2. 算法不适应于大量`enter`场景, 因为算法对应的业务也没有这样的情况. 如有这个需求, 可以对特殊场景设定合理的`enter`位置进行性能优化.

## 协议

  [MIT](https://github.com/CandyMi/aoi-c/blob/master/LICENSE)
