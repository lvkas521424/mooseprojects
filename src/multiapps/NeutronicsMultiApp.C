/****************************************************************/
/* NeutronicsMultiApp.C                                         */
/* Neutronics Calculation Implementation                        */
/*                                                              */
/* Implements neutron transport and diffusion calculations      */
/* for reactor physics simulations.                             */
/*                                                              */
/* Created: Mar 19, 2025                                        */
/* Last Modified: Mar 19, 2025                                  */
/****************************************************************/

#include "NeutronicsMultiApp.h"
#include "FEProblem.h"
#include "MooseVariableFE.h"
#include "SystemBase.h"
#include "MooseMesh.h"
#include "LevelSetTypes.h"

registerMooseObject("mooseprojectsApp", NeutronicsMultiApp);

InputParameters
NeutronicsMultiApp::validParams()
{
  InputParameters params = FullSolveMultiApp::validParams();
  params.addClassDescription("使用b1_execute作为求解核心的中子学多应用程序");
  
  params.addRequiredParam<std::vector<int>>("mesh_dims", "网格维度 (3个整数，表示x、y、z方向上的节点数)");
  params.addParam<std::string>("power_var_name", "power", "功率场变量名");
  params.addParam<std::string>("temperature_var_name", "temperature", "温度场变量名");

  ExecFlagEnum & exec = params.set<ExecFlagEnum>("execute_on");
  exec.addAvailableFlags(LevelSet::EXEC_NEUTRONIC);
  exec.addAvailableFlags(LevelSet::EXEC_PRENEUTRONIC);
  exec.addAvailableFlags(LevelSet::EXEC_CORNEUTRONIC);
  //exec.addAvailableFlags(LevelSet::EXEC_FROM_THERMAL);
  //exec.addAvailableFlags(LevelSet::EXEC_TO_THERMAL);
  exec = {LevelSet::EXEC_NEUTRONIC, LevelSet::EXEC_PRENEUTRONIC, LevelSet::EXEC_CORNEUTRONIC};

  //exec = {LevelSet::EXEC_FROM_THERMAL};
  //exec = {LevelSet::EXEC_TO_THERMAL};
  
  
  return params;
}

NeutronicsMultiApp::NeutronicsMultiApp(const InputParameters & parameters)
  : FullSolveMultiApp(parameters),
    _mesh_dims(getParam<std::vector<int>>("mesh_dims")),
    _power_var_name(getParam<std::string>("power_var_name")),
    _temperature_var_name(getParam<std::string>("temperature_var_name"))
{
  // 验证网格维度是否有3个元素
  if (_mesh_dims.size() != 3)
    mooseError("NeutronicsMultiApp: mesh_dims 必须包含3个整数值");
}

void
NeutronicsMultiApp::executeB1Solver()
{
  if (!hasLocalApp(0))
    return;

  auto & app = appProblemBase(0);
  // 修复错误1: 使用正确的方法获取变量
  auto & power_var = app.getVariable(0, _power_var_name);
  auto & power_sys = power_var.sys();
  auto & power_solution = power_sys.solution();
  
  // 修复错误2: 同样修正温度变量获取方法
  auto & temp_var = app.getVariable(0, _temperature_var_name);
  auto & temp_sys = temp_var.sys();
  auto & temp_solution = temp_sys.solution();
  
  // 获取数据场大小
  unsigned int field_size = power_solution.size();
  
  // 验证温度场和功率场大小一致
  if (temp_solution.size() != field_size)
    mooseError("NeutronicsMultiApp: 功率场和温度场大小不匹配");
  
  // 创建数据数组
  std::vector<double> power_data(field_size);
  std::vector<double> temperature_data(field_size);
  
  // 从MOOSE复制功率场数据
  for (unsigned int i = 0; i < field_size; ++i)
    power_data[i] = power_solution(i);
  
  // 修复错误3: 处理const问题，使用临时非const数组
  std::vector<int> mesh_dims_copy = _mesh_dims; 
  
  // 调用 Fortran 的 b1_execute 子程序
  b1_execute(mesh_dims_copy.data(), power_data.data(), temperature_data.data(), field_size);
  
  // 将计算结果写回MOOSE的温度场
  for (unsigned int i = 0; i < field_size; ++i)
    temp_solution.set(i, temperature_data[i]);
  
  // 更新温度场系统
  temp_sys.update();

}

bool
NeutronicsMultiApp::solveStep(Real dt, Real target_time, bool auto_advance)
{
  // 直接使用 b1_execute 作为求解核心
  std::cout << "NeutronicsMultiApp::solveStep" << std::endl;
  executeB1Solver();
  std::cout << "NeutronicsMultiApp::solveStep end" << std::endl;
  return true; // 始终返回成功

} 