# 如何修改现有代码使用 ReactorGlobalState

## 步骤 1：修改 ReactorCouplingUserObject

### 修改头文件

在 `include/userobjects/ReactorCouplingUserObject.h` 中添加：

```cpp
#include "ReactorGlobalState.h"  // 添加这一行

class ReactorCouplingUserObject : public GeneralUserObject
{
  // ... 现有代码保持不变
};
```

### 修改实现文件

在 `src/userobjects/ReactorCouplingUserObject.C` 中：

```cpp
#include "ReactorCouplingUserObject.h"
#include "ReactorGlobalState.h"  // 添加这一行
// ... 其他 includes

ReactorCouplingUserObject::ReactorCouplingUserObject(const InputParameters &parameters)
    : GeneralUserObject(parameters),
      _calc_type(getParam<MooseEnum>("calc_type")),
      _burn_step(getParam<unsigned int>("burn_step")),
      _max_burn_steps(getParam<unsigned int>("max_burn_steps")),
      // ... 其他成员初始化
{
  // 验证燃耗步
  if (_burn_step > _max_burn_steps)
    mooseError("...");

  // ===== 新增：初始化全局状态 =====
  ReactorGlobalState::instance().setBurnStep(_burn_step);
  ReactorGlobalState::instance().setMaxBurnSteps(_max_burn_steps);
  ReactorGlobalState::instance().setCalcType(_calc_type);
  
  std::cout << "\n====== ReactorGlobalState 初始化 ======" << std::endl;
  ReactorGlobalState::instance().printState();
  // ====================================

  // 初始化计算模块（原有代码）
  _neutronics_calculator = std::make_unique<NeutronicsCalculator>(
      _fe_problem, _neutronics_app_name, _burn_step);
  
  _coupled_calculator = std::make_unique<CoupledCalculator>(...);
  
  _fortran_interface = std::make_unique<FortranInterface>(_burn_step, _max_burn_steps);

  std::cout << "\n====== ReactorCouplingUserObject: 模块化构造完成 ======" << std::endl;
}

void ReactorCouplingUserObject::execute()
{
  Real time = _fe_problem.time();
  unsigned int current_step = std::floor(time + 0.0001);

  std::cout << "ReactorCouplingUserObject: EXECUTE METHOD CALLED, TIME=" << time 
            << ", BURNUP STEP=" << _burn_step << std::endl;

  bool success = false;

  // ===== 新增：只在 rank 0 更新 Fortran =====
  if (_fe_problem.comm().rank() == 0)
  {
    // 更新全局状态（在执行计算前）
    ReactorGlobalState::instance().setBurnStep(_burn_step);
    std::cout << "Rank 0: 更新全局燃耗步为 " << _burn_step << std::endl;
  }
  
  // 同步所有进程
  _fe_problem.comm().barrier();
  // ==========================================

  if (_burn_step == 1)
  {
    success = executeFirstStep();
  }
  else if (_burn_step >= 2)
  {
    success = executeSubsequentStep();
  }

  if (!success)
  {
    std::cout << "执行燃耗步 " << _burn_step << " 失败" << std::endl;
  }
  else
  {
    std::cout << "执行燃耗步 " << _burn_step << " 成功" << std::endl;
  }

  _burn_step++;
  
  // ===== 新增：更新全局状态 =====
  if (_fe_problem.comm().rank() == 0)
  {
    ReactorGlobalState::instance().setBurnStep(_burn_step);
  }
  // ==============================
}

bool ReactorCouplingUserObject::executeFirstStep()
{
  std::cout << "\n====== ReactorCouplingUserObject: 执行首次燃耗步 ======" << std::endl;
  std::cout << "计算类型: " << _calc_type << std::endl;

  // ===== 修改：只在 rank 0 更新 Fortran =====
  if (_fe_problem.comm().rank() == 0)
  {
    _fortran_interface->updateBurnupStep();
    ReactorGlobalState::instance().setFortranUpdated(true);
    std::cout << "Rank 0: Fortran 更新完成" << std::endl;
  }
  
  // 同步所有进程
  _fe_problem.comm().barrier();
  // =========================================

  // 根据计算类型执行对应的计算（原有逻辑）
  if (_calc_type == 1) // 仅中子学
  {
    std::cout << "调用中子学计算模块..." << std::endl;
    _neutronics_calculator->executeFirst();
    return true;
  }
  else if (_calc_type == 2) // 耦合计算
  {
    std::cout << "调用耦合计算模块..." << std::endl;
    return _coupled_calculator->executeFirst();
  }

  return false;
}

// executeSubsequentStep() 同样的修改
bool ReactorCouplingUserObject::executeSubsequentStep()
{
  std::cout << "\n====== ReactorCouplingUserObject: 执行后续燃耗步 ======" << std::endl;

  // ===== 修改：只在 rank 0 更新 Fortran =====
  if (_fe_problem.comm().rank() == 0)
  {
    _fortran_interface->updateBurnupStep();
    ReactorGlobalState::instance().setFortranUpdated(true);
  }
  
  _fe_problem.comm().barrier();
  // =========================================

  // 根据计算类型执行对应的计算
  if (_calc_type == 1)
  {
    std::cout << "调用中子学计算模块..." << std::endl;
    _neutronics_calculator->executeSubsequent();
    return true;
  }
  else if (_calc_type == 2)
  {
    std::cout << "调用耦合计算模块..." << std::endl;
    return _coupled_calculator->executeSubsequent();
  }

  return false;
}
```

## 步骤 2：修改 FortranInterface

### 修改头文件

在 `include/userobjects/FortranInterface.h` 中：

```cpp
#pragma once
#include "libmesh/parallel.h"  // 添加这一行

extern "C"
{
  void update_burnup_step(int step, int max_steps);
}

class FortranInterface
{
public:
  // 修改构造函数签名
  FortranInterface(unsigned int & burn_step, 
                   unsigned int max_burn_steps,
                   const libMesh::Parallel::Communicator & comm);  // 添加 comm 参数

  void updateBurnupStep();

protected:
  unsigned int & _burn_step;
  const unsigned int _max_burn_steps;
  const libMesh::Parallel::Communicator & _comm;  // 添加这个成员
};
```

### 修改实现文件

在 `src/userobjects/FortranInterface.C` 中：

```cpp
#include "FortranInterface.h"
#include "ReactorGlobalState.h"  // 添加这一行
#include <iostream>

FortranInterface::FortranInterface(unsigned int & burn_step, 
                                   unsigned int max_burn_steps,
                                   const libMesh::Parallel::Communicator & comm)
  : _burn_step(burn_step), _max_burn_steps(max_burn_steps), _comm(comm)  // 添加 comm
{
}

void
FortranInterface::updateBurnupStep()
{
  // ===== 修改：只在 rank 0 调用 Fortran =====
  if (_comm.rank() == 0)
  {
    std::cout << "FortranInterface: 传递燃耗步信息到Fortran程序 (仅 rank 0)" << std::endl;
    std::cout << "  当前燃耗步: " << _burn_step << std::endl;
    std::cout << "  最大燃耗步: " << _max_burn_steps << std::endl;

    // 创建本地副本用于传递给Fortran
    int burn_step_copy = static_cast<int>(_burn_step);
    int max_steps_copy = static_cast<int>(_max_burn_steps);

    // 调用Fortran接口（只在 rank 0 执行）
    update_burnup_step(burn_step_copy, max_steps_copy);

    std::cout << "FortranInterface: Fortran程序燃耗步信息更新成功" << std::endl;
    
    // 更新全局状态
    ReactorGlobalState::instance().setFortranUpdated(true);
  }
  
  // 同步所有进程，确保 rank 0 完成后其他进程再继续
  _comm.barrier();
  // ==========================================
}
```

### 更新构造 FortranInterface 的调用

在 `ReactorCouplingUserObject` 构造函数中：

```cpp
// 修改前：
_fortran_interface = std::make_unique<FortranInterface>(_burn_step, _max_burn_steps);

// 修改后：
_fortran_interface = std::make_unique<FortranInterface>(
    _burn_step, 
    _max_burn_steps, 
    _fe_problem.comm());  // 添加 communicator 参数
```

## 步骤 3：在 MultiApp 中读取全局状态

### 修改 NeutronicsMultiApp

在 `src/multiapps/NeutronicsMultiApp.C` 中：

```cpp
#include "NeutronicsMultiApp.h"
#include "ReactorGlobalState.h"  // 添加这一行
// ... 其他 includes

void
NeutronicsMultiApp::initialSetup()
{
  MultiApp::initialSetup();
  
  // ===== 新增：从全局状态读取 =====
  unsigned int burn_step = ReactorGlobalState::instance().getBurnStep();
  unsigned int max_steps = ReactorGlobalState::instance().getMaxBurnSteps();
  unsigned int calc_type = ReactorGlobalState::instance().getCalcType();
  
  std::cout << "\n====== NeutronicsMultiApp: 读取全局状态 ======" << std::endl;
  std::cout << "  当前燃耗步: " << burn_step << std::endl;
  std::cout << "  最大燃耗步: " << max_steps << std::endl;
  std::cout << "  计算类型: " << calc_type << std::endl;
  std::cout << "=============================================\n" << std::endl;
  // ===================================
  
  // ... 原有的初始化代码
}

bool
NeutronicsMultiApp::solveStep(Real dt, Real target_time, bool auto_advance)
{
  // ===== 新增：在每次求解前检查全局状态 =====
  unsigned int current_burn_step = ReactorGlobalState::instance().getBurnStep();
  bool fortran_updated = ReactorGlobalState::instance().isFortranUpdated();
  
  std::cout << "NeutronicsMultiApp: 求解燃耗步 " << current_burn_step;
  if (fortran_updated)
  {
    std::cout << " (Fortran 已更新)";
  }
  std::cout << std::endl;
  // ==========================================
  
  // 继续原有的求解流程
  return MultiApp::solveStep(dt, target_time, auto_advance);
}
```

## 步骤 4：确保 CMakeLists.txt 包含新文件

检查 `CMakeLists.txt` 是否自动包含了新文件，或手动添加：

```cmake
# 如果使用 GLOB（根据你的 memory），应该会自动包含
file(GLOB_RECURSE src_files src/*.C)

# 或者手动添加
set(src_files
    src/base/ReactorGlobalState.C
    # ... 其他文件
)
```

## 优势总结

使用 `ReactorGlobalState` 后：

1. ✅ **避免重复调用 Fortran**：只在 rank 0 调用，其他进程只读取状态
2. ✅ **集中状态管理**：所有共享状态在一个地方，易于维护
3. ✅ **线程安全**：使用互斥锁保护
4. ✅ **解耦合**：MultiApp 不需要通过复杂的参数传递就能获取状态
5. ✅ **调试方便**：可以随时调用 `printState()` 查看当前状态

## 执行流程

```
时间步开始
  ↓
UserObject::execute() 被调用（所有进程）
  ↓
if (rank == 0) {
    更新 ReactorGlobalState
    调用 Fortran 程序（写文件）
    设置 FortranUpdated = true
}
  ↓
barrier() - 等待 rank 0 完成
  ↓
执行计算模块
  ↓
MultiApp::solve() 被调用
  ↓
MultiApp 从 ReactorGlobalState 读取状态
  ↓
MultiApp 从 Fortran 文件读取数据
  ↓
计算完成
```






