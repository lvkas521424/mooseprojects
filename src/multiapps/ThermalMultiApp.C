/****************************************************************/
/* ThermalMultiApp.C                                            */
/* Thermal-Hydraulics Multi-Application Implementation          */
/*                                                              */
/* Implements heat transfer and fluid flow calculations         */
/* for thermal-hydraulic analysis of nuclear reactors.          */
/*                                                              */
/* Created: Mar 19, 2025                                        */
/* Last Modified: Mar 19, 2025                                  */
/****************************************************************/

#include "ThermalMultiApp.h"
#include "FEProblem.h"
#include "MooseVariableFE.h"
#include "SystemBase.h"
#include "LevelSetTypes.h"

registerMooseObject("mooseprojectsApp", ThermalMultiApp);

InputParameters
ThermalMultiApp::validParams()
{
  InputParameters params = FullSolveMultiApp::validParams();
  params.addClassDescription("Thermal multiapp using thermal_execute as the solver core");
  
  params.addRequiredParam<std::vector<int>>("mesh_dims", "Mesh dimensions (3 integers, representing the number of nodes in the x, y, z directions)");
  params.addParam<std::string>("power_var_name", "power", "Power field variable name");
  params.addParam<std::string>("temperature_var_name", "temperature", "Temperature field variable name");

  ExecFlagEnum & exec = params.set<ExecFlagEnum>("execute_on");
  exec.addAvailableFlags(LevelSet::EXEC_THERMAL);
  //exec.addAvailableFlags(LevelSet::EXEC_FROM_THERMAL);
  //exec.addAvailableFlags(LevelSet::EXEC_TO_THERMAL);
  exec = {LevelSet::EXEC_THERMAL};
  //exec = {LevelSet::EXEC_FROM_THERMAL};
  //exec = {LevelSet::EXEC_TO_THERMAL};

  return params;
}

ThermalMultiApp::ThermalMultiApp(const InputParameters & parameters)
  : FullSolveMultiApp(parameters),
    _mesh_dims(getParam<std::vector<int>>("mesh_dims")),
    _power_var_name(getParam<std::string>("power_var_name")),
    _temperature_var_name(getParam<std::string>("temperature_var_name"))
{
  // 验证网格维度是否有3个元素
  if (_mesh_dims.size() != 3)
    mooseError("ThermalMultiApp: mesh_dims must contain 3 integer values");
}

void
ThermalMultiApp::executeThermalSolver()
{
  if (!hasLocalApp(0))
    return;

  auto & app = appProblemBase(0);
  
  // 获取功率场变量
  auto & power_var = app.getVariable(0, _power_var_name);
  auto & power_sys = power_var.sys();
  auto & power_solution = power_sys.solution();
  
  // 获取温度场变量
  auto & temp_var = app.getVariable(0, _temperature_var_name);
  auto & temp_sys = temp_var.sys();
  auto & temp_solution = temp_sys.solution();
  
  // 获取数据场大小
  unsigned int field_size = power_solution.size();
  
  // 验证温度场和功率场大小一致
  if (temp_solution.size() != field_size)
    mooseError("ThermalMultiApp: Power and temperature field sizes do not match");
  
  // 创建数据数组
  std::vector<double> power_data(field_size);
  std::vector<double> temperature_data(field_size);
  
  // 从MOOSE复制功率场数据
  for (unsigned int i = 0; i < field_size; ++i)
    power_data[i] = power_solution(i);
  
  // 创建网格维度的可修改副本
  std::vector<int> mesh_dims_copy = _mesh_dims;
  
  // 调用 Fortran 的热工计算函数
  thermal_execute(mesh_dims_copy.data(), power_data.data(), temperature_data.data(), field_size);
  
  // 将计算结果写回MOOSE的温度场
  for (unsigned int i = 0; i < field_size; ++i)
    temp_solution.set(i, temperature_data[i]);
  
  // 更新温度场系统
  //temp_sys.update();
}

bool
ThermalMultiApp::solveStep(Real dt, Real target_time, bool auto_advance)
{
  // 直接使用热工计算作为求解核心
  executeThermalSolver();
  
  return true; // 始终返回成功
} 