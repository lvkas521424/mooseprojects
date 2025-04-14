/****************************************************************/
/* ReactorCouplingControl                                       */
/* A Reactor Neutronics-Thermal Coupling Control Module         */
/*                                                              */
/* This module controls the neutronics-thermal coupling         */
/* calculation process and manages burnup step iterations in    */
/* reactor simulations.                                         */
/*                                                              */
/* Created: Mar 19, 2025                                        */
/* Last Modified: Mar 19, 2025                                  */
/****************************************************************/

#include "ReactorCouplingControl.h"
#include "FEProblem.h"
#include "MultiApp.h"
#include "MultiAppTransfer.h"
#include "mooseprojectsApp.h"
#include "LevelSetTypes.h"
#include "ReactorTransfer.h"
#include "LevelSetTypes.h"
#include "NeutronicsMultiApp.h"

// fixed point iteration
#include "FixedPointSolve.h"

// #include "Transient.h"
#include "ReactorTransfer.h"

   #include "MooseVariableField.h"  // 添加这个
   #include "MooseVariableFV.h"     // 如果使用有限体积法，添加这个
   #include "NonlinearSystemBase.h" // 添加这个以获取系统解


registerMooseObject("mooseprojectsApp", ReactorCouplingControl);

InputParameters
ReactorCouplingControl::validParams()
{
  InputParameters params = Control::validParams();
  
  params.addClassDescription("Reactor Neutronics-Thermal Coupling Control Module, manages burnup step iterations and calculation types");
  
  MooseEnum calc_types("NEUTRONICS=1 COUPLED=2", "COUPLED");
  params.addParam<MooseEnum>("calc_type", calc_types, "Calculation type");
  
  params.addParam<unsigned int>("burn_step", 1, "Initial burnup step");
  params.addParam<unsigned int>("max_burn_steps", 10, "Maximum burnup steps");
  
  params.addParam<unsigned int>("max_coupling_iterations", 5, "Maximum coupling iterations");
  params.addParam<Real>("coupling_tolerance", 1e-4, "Coupling convergence tolerance");
  
  params.addParam<std::string>("neutronics_app", "neutronics", "Neutronics multiapp name");
  params.addParam<std::string>("thermal_app", "thermal", "Thermal multiapp name");
  params.addParam<unsigned int>("max_iterations", 10, "Maximum number of fixed point iterations");
  params.addParam<Real>("fixed_point_tol", 1e-6, "Fixed point convergence tolerance");

  params.addParam<unsigned int>("fixed_point_max_its", 5, "Maximum number of fixed point iterations");
  params.addParam<unsigned int>("fixed_point_min_its", 1, "Minimum number of fixed point iterations");
  params.addParam<Real>("fixed_point_tol", 1e-6, "Fixed point convergence tolerance");
  params.addParam<bool>("accept_on_max_iteration", true, "Whether to accept the solution if max iteration is reached");
  
  
  // params.addParam<std::string>("to_neutronics_transfers", "to_neutronics", "向中子学应用传输数据的传输组");
  // params.addParam<std::string>("from_neutronics_transfers", "from_neutronics", "从中子学应用接收数据的传输组");
  // params.addParam<std::string>("to_thermal_transfers", "to_thermal", "向热工应用传输数据的传输组");
  // params.addParam<std::string>("from_thermal_transfers", "from_thermal", "从热工应用接收数据的传输组");
  
  // params.addParam<std::string>("temp_conv_pp", "temp_convergence", "Temperature convergence postprocessor name");

  
  return params;
}

ReactorCouplingControl::ReactorCouplingControl(const InputParameters & parameters)
  : Control(parameters),
    _calc_type(getParam<MooseEnum>("calc_type")),
    _burn_step(getParam<unsigned int>("burn_step")),
    _max_burn_steps(getParam<unsigned int>("max_burn_steps")),
    _max_coupling_iterations(getParam<unsigned int>("max_coupling_iterations")),
    _coupling_tolerance(getParam<Real>("coupling_tolerance")),
    _neutronics_app_name(getParam<std::string>("neutronics_app")),
    _thermal_app_name(getParam<std::string>("thermal_app")),
    _fixed_point_max_its(getParam<unsigned int>("fixed_point_max_its")),
    _fixed_point_min_its(getParam<unsigned int>("fixed_point_min_its")),
    _fixed_point_tol(getParam<Real>("fixed_point_tol")),
    _accept_on_max_iteration(getParam<bool>("accept_on_max_iteration"))

    // _to_neutronics_transfers(getParam<std::string>("to_neutronics_transfers")),
    // _from_neutronics_transfers(getParam<std::string>("from_neutronics_transfers")),
    // _to_thermal_transfers(getParam<std::string>("to_thermal_transfers")),
    // _from_thermal_transfers(getParam<std::string>("from_thermal_transfers")),
    //_temp_conv_pp_name(getParam<std::string>("temp_conv_pp"))
{
  // 验证燃耗步不超出最大步数
  if (_burn_step > _max_burn_steps)
    mooseError("ReactorCouplingControl: INITIAL BURNUP STEP (", _burn_step, 
              ") CANNOT BE GREATER THAN OR EQUAL TO MAX BURNUP STEPS (", _max_burn_steps, ")");
}

// Main execution method
// 主执行方法
void
ReactorCouplingControl::execute()
{
  // 获取当前时间步，并将其设置为当前燃耗步
  Real time = _fe_problem.time();        // 获取当前时间
  unsigned int current_step = std::floor(time + 0.0001); // 转换为整数燃耗步（添加0.1避免浮点误差）
  
  // 更新燃耗步
  // if (current_step < _max_burn_steps)
  // {
  //   if (current_step != _burn_step)
  //   {
  //     std::cout << "ReactorCouplingControl: 根据时间步更新燃耗步，时间=" << time
  //               << "，旧燃耗步=" << _burn_step << "，新燃耗步=" << current_step << std::endl;
  //     _burn_step = current_step;
  //   }
  // }


  std::cout << "ReactorCouplingControl: EXECUTE METHOD CALLED, CURRENT TIME=" << time << ", CURRENT BURNUP STEP=" << _burn_step << std::endl;
  std::cout << "ReactorCouplingControl: _burn_step=" << _burn_step << std::endl;
  // 只执行当前燃耗步
  bool success = false;
  
  if (_burn_step == 1)
  {
    success = executeFirstStep();
  }
  else if (_burn_step >= 2)
  {
    success = executeSubsequentStep();
  }

  //if (_fe_problem.hasVariable("power_density"))
  //{
  //  // 使用 MooseVariableFieldBase 类型
  //  MooseVariableFieldBase & power_var = _fe_problem.getVariable(0, "power_density");
  //  
  //  auto & sys = power_var.sys();
  //  
  //  auto & solution = sys.solution();
  //  
  //  std::cout << "主应用功率密度字段大小: " << solution.size() << std::endl;
  //  std::cout << "solution = " << solution << std::endl;
  //  // 检查是否有模式
  //}
  _burn_step ++;
}

// Execute the first burnup step
// 执行第一个燃耗步
bool
ReactorCouplingControl::executeFirstStep()
{
  std::cout << "ReactorCouplingControl: EXECUTE FIRST BURNUP STEP" << std::endl;
  
  std::cout << "CALCULATION TYPE: " << _calc_type << std::endl;

  // 根据计算类型执行对应的计算
  if (_calc_type == 1) // 仅中子学
  {
    executefirstNeutronics();

    return true;
  }
  else if (_calc_type == 2) // 耦合计算
  {
    return executefirstCoupled();
  }
  
  return false;
}

// Execute subsequent burnup steps
// 执行后续燃耗步
bool
ReactorCouplingControl::executeSubsequentStep()
{
  std::cout << "ReactorCouplingControl: EXECUTE SUBSEQUENT BURNUP STEP" << std::endl;
  
  // 根据计算类型执行对应的计算
  if (_calc_type == 1) // 仅中子学
  {
    executesubsquentNeutronics();
    return true;
  }
  else if (_calc_type == 2) // 耦合计算
  {
    return executesubsquentCoupled();
  }

  
  return false;
}



// Set calculation type
// 设置计算类型
void
ReactorCouplingControl::setCalculationType(unsigned int type)
{
  if (type < 1 || type > 2)
    mooseError("ReactorCouplingControl: 无效的计算类型: ", type);
  
  _calc_type = type;
}


// Execute first neutronics calculation
// 执行第一次中子学计算
void
ReactorCouplingControl::executefirstNeutronics()
{
  // 检查中子学多应用程序是否存在
  if (!_fe_problem.hasMultiApp(_neutronics_app_name))
  {
    std::cout << "WARNING: ReactorCouplingControl: Can't find neutronics multiapp '" << _neutronics_app_name << "'" << std::endl;
    return;
  }
  
  std::cout << "ReactorCouplingControl: EXECUTE NEUTRONICS, CURRENT BURNUP STEP=" << _burn_step << std::endl;
  
  // 准备中子学应用程序列表，用于选择性执行
  std::vector<std::string> neutronics_apps = {_neutronics_app_name};
  
  // 更新Fortran程序中的燃耗步信息
  updateFortranBurnupStep();
  
  // 执行中子学应用
  _fe_problem.execMultiApps(LevelSet::EXEC_NEUTRONIC);
  // 如果有定义传输组名称，使用传输组



  // if (_fe_problem.hasVariable("power_density"))
  // {
  //   // 使用 MooseVariableFieldBase 类型
  //   MooseVariableFieldBase & power_var = _fe_problem.getVariable(0, "power_density");
  //   
  //   auto & sys = power_var.sys();
  //   
  //   auto & solution = sys.solution();
  //   
  //   std::cout << "主应用功率密度字段大小: " << solution.size() << std::endl;
  //   std::cout << "solution = " << solution << std::endl;
  //   // 检查是否有模式
  // }

  // 执行从中子学应用到主应用的数据传输 
  std::cout << "ReactorCouplingControl: 传输功率密度场到主应用..." << std::endl;
  // _fe_problem.execTransfers(LevelSet::EXEC_FROM_NEUTRONIC);

 //  // 验证传输成功
 // if (_fe_problem.hasVariable("power_density"))
 // {
 //   // 使用 MooseVariableFieldBase 类型
 //   MooseVariableFieldBase & power_var = _fe_problem.getVariable(0, "power_density");
 //   
 //   auto & sys = power_var.sys();
 //   
 //   auto & solution = sys.solution();
 //   
 //   std::cout << "主应用功率密度字段大小: " << solution.size() << std::endl;
 //   std::cout << "solution = " << solution << std::endl;
 //   // 检查是否有模式
 //   }

  // 触发后处理器计算和输出
  _fe_problem.execute(EXEC_TIMESTEP_END);
  _fe_problem.outputStep(EXEC_TIMESTEP_END);

}

// Execute subsequent neutronics calculations
// 执行后续中子学计算
void
ReactorCouplingControl::executesubsquentNeutronics()
{
  // 检查中子学多应用程序是否存在
  if (!_fe_problem.hasMultiApp(_neutronics_app_name))
  {
    std::cout << "WARNING: ReactorCouplingControl: Can't find neutronics multiapp '" << _neutronics_app_name << "'" << std::endl;
    return;
  }
  
  std::cout << "ReactorCouplingControl: EXECUTE NEUTRONICS, CURRENT BURNUP STEP=" << _burn_step << std::endl;
  
  // 准备中子学应用程序列表，用于选择性执行
  std::vector<std::string> neutronics_apps = {_neutronics_app_name};
  
  // 更新Fortran程序中的燃耗步信息
  updateFortranBurnupStep();
  
  // 执行中子学应用
  _fe_problem.execMultiApps(LevelSet::EXEC_PRENEUTRONIC, 
                          neutronics_apps.empty() ? nullptr : &neutronics_apps);

  _fe_problem.execMultiApps(LevelSet::EXEC_CORNEUTRONIC, 
                          neutronics_apps.empty() ? nullptr : &neutronics_apps);
}

// Execute first coupled neutronics-thermal calculation
// 执行第一次核热耦合计算
bool
ReactorCouplingControl::executefirstCoupled()
{
  try // 添加异常捕获
  {
    
    Real time = _fe_problem.time();
    int time_step = _fe_problem.timeStep();

    std::cout << "\n====== Starting Fixed Point Iteration at t = " << time 
             << ", step = " << time_step << " ======\n";


    // 获取Executioner参数
    const unsigned int max_iterations = getParam<unsigned int>("max_iterations");
    const Real fixed_point_tol = getParam<Real>("fixed_point_tol");
    
    std::cout << "Fixed Point Settings (from input):" << std::endl;
    std::cout << "  Max iterations: " << max_iterations << std::endl;
    std::cout << "  Convergence tolerance: " << fixed_point_tol << std::endl;
  
    // 开始固定点迭代
    std::cout << "Starting fixed point iteration..." << std::endl;

    _fe_problem.execMultiApps(LevelSet::EXEC_CORNEUTRONIC);


  

    std::cout << "ReactorCouplingControl: EXECUTE Fixed Point Iteration..." << std::endl;

    _fe_problem.execMultiApps(LevelSet::EXEC_THERMAL);

    std::cout << "====== Fixed Point Iteration Completed ======\n\n";
    
  }
  catch (const std::exception& e)
  {
    std::cout << "COUPLING ITERATION PROCESS EXCEPTION: " << e.what() << std::endl;
    return false;
  }
  catch (...)
  {
    std::cout << "COUPLING ITERATION PROCESS UNKNOWN EXCEPTION" << std::endl;
    return false;
  }
  
  return true;
}

// Execute subsequent coupled neutronics-thermal calculations
// 执行后续核热耦合计算
bool
ReactorCouplingControl::executesubsquentCoupled()
{
  // 检查多应用程序是否存在
  bool has_neutronics = _fe_problem.hasMultiApp(_neutronics_app_name);
  bool has_thermal = _fe_problem.hasMultiApp(_thermal_app_name);
  
  if (!has_neutronics || !has_thermal)
  {
    std::cout << "WARNING: ReactorCouplingControl: Can't find necessary multiapps" << std::endl;
    return false;
  }
  
  // 准备特定多应用程序名称列表，用于选择性执行
  std::vector<std::string> neutronics_apps = {_neutronics_app_name};
  std::vector<std::string> thermal_apps = {_thermal_app_name};
  
  // 耦合迭代
  Real temp_convergence = 1.0;
  unsigned int iter = 0;
  
  while (iter < _max_coupling_iterations && temp_convergence > _coupling_tolerance)
  {
    // 记录迭代次数
    iter++;
    std::cout << "ReactorCouplingControl: 耦合迭代次数=" << iter << std::endl;
    
    std::cout << "EXECUTE NEUTRONICS..." << std::endl;
    bool neutronics_success = _fe_problem.execMultiApps(LevelSet::EXEC_NEUTRONIC, 
                        neutronics_apps.empty() ? nullptr : &neutronics_apps);
    if (!neutronics_success) {
      std::cout << "NEUTRONICS EXECUTION FAILED!" << std::endl;
      return false;
    }
    
    std::cout << "EXECUTE THERMAL..." << std::endl;
    bool thermal_success = _fe_problem.execMultiApps(LevelSet::EXEC_THERMAL, 
                        thermal_apps.empty() ? nullptr : &thermal_apps);
    if (!thermal_success) {
      std::cout << "THERMAL EXECUTION FAILED!" << std::endl;
      return false;
    }
  }
  
  // 判断是否达到最大迭代次数
  if (iter >= _max_coupling_iterations)
  {
    std::cout << "ReactorCouplingControl:   MAX ITERATIONS REACHED ("<< _max_coupling_iterations <<
                "), BUT CONVERGENCE NOT REACHED (current: " << temp_convergence <<
                ", target: " <<  _coupling_tolerance <<  ")" << std::endl;
    return false;
  }
  
  return true;
}

// Update burnup step information in Fortran program
// 更新Fortran程序中的燃耗步信息
void
ReactorCouplingControl::updateFortranBurnupStep()
{
 
  std::cout << "Passing burnup step information to Fortran program" << std::endl;
  
  // 创建本地副本
  int burn_step_copy = static_cast<int>(_burn_step); 
  int max_steps_copy = static_cast<int>(_max_burn_steps);
  
  // 调用Fortran接口，使用本地变量
  update_burnup_step(burn_step_copy, max_steps_copy);
  
  std::cout << "Fortran program burnup step updated successfully" << std::endl;

} 