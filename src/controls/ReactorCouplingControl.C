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

// fixed point iteration
#include "FixedPointSolve.h"


registerMooseObject("mooseprojectsApp", ReactorCouplingControl);

InputParameters
ReactorCouplingControl::validParams()
{
  InputParameters params = Control::validParams();
  
  params.addClassDescription("反应堆核热耦合计算控制模块，管理燃耗步迭代和计算类型");
  
  MooseEnum calc_types("NEUTRONICS=1 COUPLED=2", "COUPLED");
  params.addParam<MooseEnum>("calc_type", calc_types, "计算类型");
  
  params.addParam<unsigned int>("burn_step", 1, "初始燃耗步");
  params.addParam<unsigned int>("max_burn_steps", 10, "最大燃耗步数");
  
  params.addParam<unsigned int>("max_coupling_iterations", 5, "最大耦合迭代次数");
  params.addParam<Real>("coupling_tolerance", 1e-4, "耦合收敛容差");
  
  params.addParam<std::string>("neutronics_app", "neutronics", "中子学多应用程序名称");
  params.addParam<std::string>("thermal_app", "thermal", "热工多应用程序名称");
  
  // params.addParam<std::string>("to_neutronics_transfers", "to_neutronics", "向中子学应用传输数据的传输组");
  // params.addParam<std::string>("from_neutronics_transfers", "from_neutronics", "从中子学应用接收数据的传输组");
  // params.addParam<std::string>("to_thermal_transfers", "to_thermal", "向热工应用传输数据的传输组");
  // params.addParam<std::string>("from_thermal_transfers", "from_thermal", "从热工应用接收数据的传输组");
  
  params.addParam<std::string>("temp_conv_pp", "temp_convergence", "温度收敛判断后处理器名称");

  
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
    // _to_neutronics_transfers(getParam<std::string>("to_neutronics_transfers")),
    // _from_neutronics_transfers(getParam<std::string>("from_neutronics_transfers")),
    // _to_thermal_transfers(getParam<std::string>("to_thermal_transfers")),
    // _from_thermal_transfers(getParam<std::string>("from_thermal_transfers")),
    _temp_conv_pp_name(getParam<std::string>("temp_conv_pp"))
{
  // 验证燃耗步不超出最大步数
  if (_burn_step >= _max_burn_steps)
    mooseError("ReactorCouplingControl: 初始燃耗步 (", _burn_step, 
              ") 不能大于或等于最大燃耗步数 (", _max_burn_steps, ")");
}


// Execute the first burnup step
// 执行第一个燃耗步
bool
ReactorCouplingControl::executeFirstStep()
{
  std::cout << "ReactorCouplingControl: 执行第一个燃耗步特殊处理" << std::endl;
  
  // 第一步特殊逻辑
  // 例如：初始化计算、使用初始燃料组成等
  std::cout << "计算类型：" << _calc_type << std::endl;
  std::cout << "ReactorCouplingControl: 执行完第一个燃耗步特殊处理" << std::endl;

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
  std::cout << "ReactorCouplingControl: 执行后续燃耗步标准处理" << std::endl;
  
  // 后续步骤标准逻辑
  // 例如：更新燃料组成、考虑燃料燃耗效应等
  
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

// Main execution method
// 主执行方法
void
ReactorCouplingControl::execute()
{
  // 获取当前时间步，并将其设置为当前燃耗步
  Real time = _fe_problem.time();        // 获取当前时间
  unsigned int current_step = std::floor(time + 0.1); // 转换为整数燃耗步（添加0.1避免浮点误差）
  
  // 更新燃耗步
  if (current_step < _max_burn_steps)
  {
    if (current_step != _burn_step)
    {
      std::cout << "ReactorCouplingControl: 根据时间步更新燃耗步，时间=" << time
                << "，旧燃耗步=" << _burn_step << "，新燃耗步=" << current_step << std::endl;
      _burn_step = current_step;
    }
  }

  _burn_step = current_step;

  std::cout << "ReactorCouplingControl: execute方法被调用，当前时间=" << time << "，当前燃耗步=" << _burn_step << std::endl;
  
  // 只执行当前燃耗步
  bool success = false;
  
  if (_burn_step == 0)
  {
    // 对于第0步，进行初始化操作
    std::cout << "ReactorCouplingControl: 进行第0燃耗步初始化操作" << std::endl;
    success = true;
  }
  else if (_burn_step == 1)
  {
    success = executeFirstStep();
  }
  else if (_burn_step >= 2)
  {
    success = executeSubsequentStep();
  }
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

// Set burnup step
// 设置燃耗步
void
ReactorCouplingControl::setBurnupStep(unsigned int step)
{
  if (step >= _max_burn_steps)
    mooseError("ReactorCouplingControl: 燃耗步 (", step, 
              ") 不能大于或等于最大燃耗步数 (", _max_burn_steps, ")");
  
  _burn_step = step;
}

// Execute first neutronics calculation
// 执行第一次中子学计算
void
ReactorCouplingControl::executefirstNeutronics()
{
  // 检查中子学多应用程序是否存在
  if (!_fe_problem.hasMultiApp(_neutronics_app_name))
  {
    mooseWarning("ReactorCouplingControl: 找不到中子学多应用程序 '", _neutronics_app_name, "'");
    return;
  }
  
  std::cout << "ReactorCouplingControl: 执行中子学计算，当前燃耗步=" << _burn_step << std::endl;
  
  // 准备中子学应用程序列表，用于选择性执行
  std::vector<std::string> neutronics_apps = {_neutronics_app_name};
  
  // 更新Fortran程序中的燃耗步信息
  updateFortranBurnupStep();
  
  // 执行中子学应用
  _fe_problem.execMultiApps(LevelSet::EXEC_NEUTRONIC, &neutronics_apps);
}

// Execute subsequent neutronics calculations
// 执行后续中子学计算
void
ReactorCouplingControl::executesubsquentNeutronics()
{
  // 检查中子学多应用程序是否存在
  if (!_fe_problem.hasMultiApp(_neutronics_app_name))
  {
    mooseWarning("ReactorCouplingControl: 找不到中子学多应用程序 '", _neutronics_app_name, "'");
    return;
  }
  
  std::cout << "ReactorCouplingControl: 执行中子学计算，当前燃耗步=" << _burn_step << std::endl;
  
  // 准备中子学应用程序列表，用于选择性执行
  std::vector<std::string> neutronics_apps = {_neutronics_app_name};
  
  // 更新Fortran程序中的燃耗步信息
  updateFortranBurnupStep();
  
  // 执行中子学应用
  _fe_problem.execMultiApps(LevelSet::EXEC_PRENEUTRONIC, &neutronics_apps);
  _fe_problem.execMultiApps(LevelSet::EXEC_CORNEUTRONIC, &neutronics_apps);
}

// Execute first coupled neutronics-thermal calculation
// 执行第一次核热耦合计算
bool
ReactorCouplingControl::executefirstCoupled()
{
  // 检查多应用程序是否存在
  bool has_neutronics = _fe_problem.hasMultiApp(_neutronics_app_name);
  bool has_thermal = _fe_problem.hasMultiApp(_thermal_app_name);
  
  if (!has_neutronics || !has_thermal)
  {
    mooseWarning("ReactorCouplingControl: 找不到必要的多应用程序");
    return false;
  }
  // 执行固定点迭代求解
    
  
  
  // 准备特定多应用程序名称列表，用于选择性执行
  std::vector<std::string> neutronics_apps = {_neutronics_app_name};
  std::vector<std::string> thermal_apps = {_thermal_app_name};

    // 启用固定点迭代

  
  // 耦合迭代
  Real temp_convergence = 1.0;
  unsigned int iter = 0;
  // while (iter < _max_coupling_iterations && temp_convergence > _coupling_tolerance)
  while (iter < _max_coupling_iterations)
  {
    // 记录迭代次数
    iter++;

    std::cout << "ReactorCouplingControl: 耦合迭代次数=" << iter << std::endl;

    // 执行中子学应用
    _fe_problem.execMultiApps(LevelSet::EXEC_NEUTRONIC, &neutronics_apps);

    // 执行热学应用
    _fe_problem.execMultiApps(LevelSet::EXEC_THERMAL, &thermal_apps);
    
    // if (_fe_problem.hasPostprocessor(_temp_conv_pp_name))
    // {
    //   temp_convergence = _fe_problem.getPostprocessorValueByName(_temp_conv_pp_name);
    //   if (temp_convergence <= _coupling_tolerance)
    //   {
    //     // 达到收敛条件
    //     return true;
    //   }
    // }
    // else
    // {
    //   mooseWarning("ReactorCouplingControl: 找不到温度收敛判断后处理器 '", _temp_conv_pp_name, "'");
    //   // 如果没有找到后处理器，假设已经收敛
    //   return true;
    // }
  }
  
  // 判断是否达到最大迭代次数
  if (iter >= _max_coupling_iterations)
  {
    std::cout << "ReactorCouplingControl: 达到最大耦合迭代次数 ("<< _max_coupling_iterations <<
                "), 但未达到收敛条件 (current: " << temp_convergence <<
                ", target: " <<  _coupling_tolerance <<  ")" << std::endl;
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
    mooseWarning("ReactorCouplingControl: 找不到必要的多应用程序");
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
    
    // if (_fe_problem.hasPostprocessor(_temp_conv_pp_name))
    // {
    //   temp_convergence = _fe_problem.getPostprocessorValueByName(_temp_conv_pp_name);
    //   if (temp_convergence <= _coupling_tolerance)
    //   {
    //     // 达到收敛条件
    //     return true;
    //   }
    // }
    // else
    // {
    //   mooseWarning("ReactorCouplingControl: 找不到温度收敛判断后处理器 '", _temp_conv_pp_name, "'");
    //   // 如果没有找到后处理器，假设已经收敛
    //   return true;
    // }
  }
  
  // 判断是否达到最大迭代次数
  if (iter >= _max_coupling_iterations)
  {
    std::cout << "ReactorCouplingControl: 达到最大耦合迭代次数 ("<< _max_coupling_iterations <<
                "), 但未达到收敛条件 (current: " << temp_convergence <<
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
  
  // 调用Fortran接口函数，传递当前燃耗步和最大燃耗步数
  update_burnup_step(_burn_step, _max_burn_steps);
  
  std::cout << "Fortran program burnup step updated" << std::endl;
} 