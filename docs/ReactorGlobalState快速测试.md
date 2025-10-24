# ReactorGlobalState 快速测试指南

## 步骤 1：编译新代码

```bash
cd /home/lvkas/projects/mooseprojects
conda activate moose
make -j8
```

如果编译成功，说明 `ReactorGlobalState` 类已正确添加到项目中。

---

## 步骤 2：简单测试（不修改现有代码）

创建一个测试文件来验证 `ReactorGlobalState` 的功能：

**创建测试文件** `test_global_state.C`：

```cpp
#include "ReactorGlobalState.h"
#include <iostream>

int main()
{
  std::cout << "\n====== 测试 ReactorGlobalState ======\n" << std::endl;
  
  // 获取单例实例
  auto & state = ReactorGlobalState::instance();
  
  // 测试默认值
  std::cout << "测试 1: 默认值" << std::endl;
  state.printState();
  
  // 测试设置和获取
  std::cout << "\n测试 2: 设置新值" << std::endl;
  state.setBurnStep(5);
  state.setMaxBurnSteps(20);
  state.setCalcType(1);
  state.printState();
  
  // 测试 getter
  std::cout << "\n测试 3: 使用 getter" << std::endl;
  std::cout << "getBurnStep() = " << state.getBurnStep() << std::endl;
  std::cout << "getMaxBurnSteps() = " << state.getMaxBurnSteps() << std::endl;
  std::cout << "getCalcType() = " << state.getCalcType() << std::endl;
  
  // 测试 Fortran 更新标志
  std::cout << "\n测试 4: Fortran 更新标志" << std::endl;
  std::cout << "初始状态: " << (state.isFortranUpdated() ? "已更新" : "未更新") << std::endl;
  state.setFortranUpdated(true);
  std::cout << "设置后: " << (state.isFortranUpdated() ? "已更新" : "未更新") << std::endl;
  
  // 测试重置
  std::cout << "\n测试 5: 重置" << std::endl;
  state.reset();
  state.printState();
  
  // 测试单例特性
  std::cout << "\n测试 6: 单例验证" << std::endl;
  auto & state2 = ReactorGlobalState::instance();
  state2.setBurnStep(99);
  std::cout << "通过 state 读取: " << state.getBurnStep() << std::endl;
  std::cout << "通过 state2 读取: " << state2.getBurnStep() << std::endl;
  std::cout << "两者相同: " << (&state == &state2 ? "是" : "否") << std::endl;
  
  std::cout << "\n====== 测试完成 ======\n" << std::endl;
  
  return 0;
}
```

编译测试程序：
```bash
g++ -std=c++17 -I/home/lvkas/projects/mooseprojects/include/base \
    test_global_state.C \
    /home/lvkas/projects/mooseprojects/src/base/ReactorGlobalState.C \
    -o test_global_state -pthread
```

运行测试：
```bash
./test_global_state
```

**预期输出**：
```
====== 测试 ReactorGlobalState ======

测试 1: 默认值

====== ReactorGlobalState 当前状态 ======
  燃耗步: 1 / 10
  计算类型: 2
  Fortran已更新: 否
========================================


测试 2: 设置新值

====== ReactorGlobalState 当前状态 ======
  燃耗步: 5 / 20
  计算类型: 1
  Fortran已更新: 否
========================================


测试 3: 使用 getter
getBurnStep() = 5
getMaxBurnSteps() = 20
getCalcType() = 1

测试 4: Fortran 更新标志
初始状态: 未更新
设置后: 已更新

测试 5: 重置

====== ReactorGlobalState 当前状态 ======
  燃耗步: 1 / 10
  计算类型: 2
  Fortran已更新: 否
========================================


测试 6: 单例验证
通过 state 读取: 99
通过 state2 读取: 99
两者相同: 是

====== 测试完成 ======
```

---

## 步骤 3：在现有代码中简单集成测试

### 最小修改测试

只需在 `ReactorCouplingUserObject` 的构造函数末尾添加几行：

```cpp
// 在 ReactorCouplingUserObject.C 的构造函数末尾添加
#include "ReactorGlobalState.h"  // 文件顶部添加

ReactorCouplingUserObject::ReactorCouplingUserObject(...)
{
  // ... 所有现有代码保持不变 ...
  
  // ===== 添加这几行测试 =====
  std::cout << "\n[测试] 设置全局状态..." << std::endl;
  ReactorGlobalState::instance().setBurnStep(_burn_step);
  ReactorGlobalState::instance().setMaxBurnSteps(_max_burn_steps);
  ReactorGlobalState::instance().setCalcType(_calc_type);
  
  std::cout << "[测试] 读取全局状态..." << std::endl;
  ReactorGlobalState::instance().printState();
  // ========================
}
```

重新编译并运行你的输入文件：
```bash
make -j8
mpiexec -n 4 ./mooseprojects-opt -i neutronic.i
```

如果看到输出中有全局状态的信息，说明集成成功！

---

## 步骤 4：在 MultiApp 中测试读取

在 `NeutronicsMultiApp.C` 的 `initialSetup()` 方法开头添加：

```cpp
#include "ReactorGlobalState.h"  // 文件顶部

void
NeutronicsMultiApp::initialSetup()
{
  MultiApp::initialSetup();
  
  // ===== 添加这几行测试 =====
  std::cout << "\n[NeutronicsMultiApp 测试] 读取全局状态..." << std::endl;
  unsigned int step = ReactorGlobalState::instance().getBurnStep();
  unsigned int max_steps = ReactorGlobalState::instance().getMaxBurnSteps();
  std::cout << "  读取到燃耗步: " << step << " / " << max_steps << std::endl;
  // ========================
  
  // ... 其他代码 ...
}
```

---

## 步骤 5：并行测试

测试多进程下的行为：

在 `ReactorCouplingUserObject::execute()` 开头添加：

```cpp
void
ReactorCouplingUserObject::execute()
{
  // ===== 测试并行行为 =====
  int rank = _fe_problem.comm().rank();
  int size = _fe_problem.comm().size();
  
  std::cout << "[Rank " << rank << "/" << size << "] execute() 调用" << std::endl;
  
  if (rank == 0)
  {
    std::cout << "[Rank 0] 更新全局状态..." << std::endl;
    ReactorGlobalState::instance().setBurnStep(_burn_step);
    ReactorGlobalState::instance().printState();
  }
  
  _fe_problem.comm().barrier();
  
  std::cout << "[Rank " << rank << "] barrier 后，读取燃耗步: " 
            << ReactorGlobalState::instance().getBurnStep() << std::endl;
  // ========================
  
  // ... 其他代码 ...
}
```

用 4 个进程运行：
```bash
mpiexec -n 4 ./mooseprojects-opt -i neutronic.i
```

**观察输出**：
- 所有进程都会打印 "execute() 调用"
- 只有 rank 0 会打印 "更新全局状态"
- 每个进程读取的燃耗步可能不同（因为是独立的实例）

---

## 步骤 6：验证 Fortran 调用次数

添加计数器验证 Fortran 只被调用一次：

在 `FortranInterface.C` 中：

```cpp
// 添加静态变量
static int fortran_call_count = 0;

void
FortranInterface::updateBurnupStep()
{
  int rank = _comm.rank();
  
  // 增加调用计数
  fortran_call_count++;
  
  std::cout << "[Rank " << rank << "] FortranInterface::updateBurnupStep() "
            << "第 " << fortran_call_count << " 次调用" << std::endl;
  
  // 如果不使用 rank 检查，会看到多次调用
  // 添加 rank 检查后，只会看到一次
  
  // ... 原有代码 ...
}
```

---

## 预期结果对比

### 没有 rank 检查（4 个进程）：
```
[Rank 0] FortranInterface::updateBurnupStep() 第 1 次调用
[Rank 1] FortranInterface::updateBurnupStep() 第 1 次调用
[Rank 2] FortranInterface::updateBurnupStep() 第 1 次调用
[Rank 3] FortranInterface::updateBurnupStep() 第 1 次调用
```
❌ 问题：Fortran 被调用 4 次

### 有 rank 检查（4 个进程）：
```
[Rank 0] FortranInterface::updateBurnupStep() 第 1 次调用
```
✅ 正确：Fortran 只被调用 1 次

---

## 常见问题排查

### Q1：编译错误 "ReactorGlobalState.h not found"
**解决**：确保 CMakeLists.txt 包含了 include 路径
```cmake
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/base)
```

### Q2：链接错误 "undefined reference to ReactorGlobalState"
**解决**：确保 `ReactorGlobalState.C` 被编译并链接
```cmake
file(GLOB_RECURSE src_files src/*.C)
```

### Q3：每个进程的状态不一致
**这是正常的！** 每个 MPI 进程有独立的 `ReactorGlobalState` 实例。

解决方法：
- 只在 rank 0 更新状态
- 通过 Fortran 文件或 MPI 通信同步
- 或者让所有进程都设置相同的值

---

## 下一步

测试通过后，可以按照 `如何使用ReactorGlobalState修改现有代码.md` 进行完整集成。






