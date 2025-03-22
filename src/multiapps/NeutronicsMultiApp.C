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
  params.addClassDescription("Neutronics multiapp using b1_execute as the solver core");
  
  params.addRequiredParam<std::vector<int>>("mesh_dims", "Mesh dimensions (3 integers, representing the number of nodes in the x, y, z directions)");
  params.addParam<std::string>("power_var_name", "power", "Power field variable name");
  params.addParam<std::string>("temperature_var_name", "temperature", "Temperature field variable name");

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
    mooseError("NeutronicsMultiApp: mesh_dims MUST contain 3 integer values");
}

void
NeutronicsMultiApp::executeB1Solver()
{
  if (!hasLocalApp(0))
    return;

  auto & app = appProblemBase(0);
  
  // check if the variable is in the nonlinear system
  // 检查变量是否存在
  if (!app.hasVariable(_power_var_name) && !app.hasVariable(_temperature_var_name))
    return;

  // get the variable from the nonlinear system
  // 获取功率变量
  auto & power_var = app.getVariable(0, _power_var_name);
  auto & power_sys = power_var.sys();
  auto & power_solution = power_sys.solution();

  // get the variable from the auxiliary system
  // 获取温度变量
  auto & temp_var = app.getVariable(0, _temperature_var_name);
  auto & temp_sys = temp_var.sys();
  auto & temp_solution = temp_sys.solution();
  
  // get the field size
  // 获取数据场大小并验证
  unsigned int field_size = power_solution.size();
  if (temp_solution.size() != field_size || field_size == 0)
    return;
  
  // establish the data arrays
  // 创建数据数组
  std::vector<double> power_data(field_size, 0.0);
  std::vector<double> temperature_data(field_size, 0.0);
  //std::vector<double> power_density(field_size, 1.0);

  
  // copy the power field data
  // 从MOOSE复制温度场数据
  for (unsigned int i = 0; i < field_size; ++i)
    temperature_data[i] = temp_solution(i);
  
  // copy the mesh dimensions
  // 创建网格维度的可修改副本
  std::vector<int> mesh_dims_copy = _mesh_dims;
  
  // 调用Fortran的b1_execute子程序
  b1_execute(mesh_dims_copy.data(), power_data.data(), temperature_data.data(), field_size);
  
  for (unsigned int i = 0; i < field_size; ++i)
    power_solution.set(i, power_data[i]);
  
  
  //power_sys.update();

  //for (unsigned int i = 0; i < field_size; ++i)
  //{
  //  power_density[i] = power_solution(i);
  //  std::cout << "#" << "power_density[" << i << "] = " << power_density[i] << std::endl;
  //}
 // 
  //return;
}

bool
NeutronicsMultiApp::solveStep(Real dt, Real target_time, bool auto_advance)
{
  // 直接使用b1_execute作为求解核心
  executeB1Solver();
  return true; // 始终返回成功
} 