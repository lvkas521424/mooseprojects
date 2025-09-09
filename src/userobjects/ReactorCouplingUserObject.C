/****************************************************************/
/* ReactorCouplingUserObject                                    */
/* A Reactor Neutronics-Thermal Coupling UserObject Module      */
/*                                                              */
/* This module controls the neutronics-thermal coupling         */
/* calculation process and manages burnup step iterations in    */
/* reactor simulations.                                         */
/*                                                              */
/* Created: Mar 19, 2025                                        */
/* Last Modified: Mar 19, 2025                                  */
/****************************************************************/

#include "ReactorCouplingUserObject.h"
#include "FEProblem.h"
#include "MultiApp.h"
#include "MultiAppTransfer.h"
#include "mooseprojectsApp.h"
#include "LevelSetTypes.h"
#include "ReactorTransfer.h"
#include "NeutronicsMultiApp.h"

registerMooseObject("mooseprojectsApp", ReactorCouplingUserObject);

InputParameters
ReactorCouplingUserObject::validParams()
{
  InputParameters params = GeneralUserObject::validParams();

  params.addClassDescription("Reactor Neutronics-Thermal Coupling UserObject Module, manages burnup step iterations and calculation types");

  MooseEnum calc_types("NEUTRONICS=1 COUPLED=2", "COUPLED");
  params.addParam<MooseEnum>("calc_type", calc_types, "Calculation type");

  params.addParam<unsigned int>("burn_step", 1, "Initial burnup step");
  params.addParam<unsigned int>("max_burn_steps", 10, "Maximum burnup steps");

  params.addParam<unsigned int>("max_coupling_iterations", 5, "Maximum coupling iterations");
  params.addParam<Real>("coupling_tolerance", 1e-4, "Coupling convergence tolerance");

  params.addParam<std::string>("neutronics_app", "neutronics", "Neutronics multiapp name");
  params.addParam<std::string>("thermal_app", "thermal", "Thermal multiapp name");

  params.addParam<unsigned int>("fixed_point_max_its", 5, "Maximum number of fixed point iterations");
  params.addParam<unsigned int>("fixed_point_min_its", 1, "Minimum number of fixed point iterations");
  params.addParam<Real>("fixed_point_tol", 1e-6, "Fixed point convergence tolerance");
  params.addParam<bool>("accept_on_max_iteration", true, "Whether to accept the solution if max iteration is reached");

  return params;
}

ReactorCouplingUserObject::ReactorCouplingUserObject(const InputParameters &parameters)
    : GeneralUserObject(parameters),
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
      _accept_on_max_iteration(getParam<bool>("accept_on_max_iteration")),
      _fe_problem(*getCheckedPointerParam<FEProblemBase *>("_fe_problem_base"))
{
  // 验证燃耗步不超出最大步数
  if (_burn_step > _max_burn_steps)
    mooseError("ReactorCouplingUserObject: INITIAL BURNUP STEP (", _burn_step,
               ") CANNOT BE GREATER THAN OR EQUAL TO MAX BURNUP STEPS (", _max_burn_steps, ")");
}

void ReactorCouplingUserObject::initialize()
{
  // 初始化工作，如果需要
  std::cout << "ReactorCouplingUserObject: INITIALIZE METHOD CALLED" << std::endl;

  // 确认多应用存在
  if (!_fe_problem.hasMultiApp(_neutronics_app_name) || !_fe_problem.hasMultiApp(_thermal_app_name))
  {
    std::cout << "WARNING: ReactorCouplingUserObject: 必要的多应用不存在" << std::endl;
  }

  std::cout << "ReactorCouplingUserObject: INITIALIZE METHOD CALLED, NEUTRONICS APP EXISTS: " << _fe_problem.hasMultiApp(_neutronics_app_name) << std::endl;
  std::cout << "ReactorCouplingUserObject: INITIALIZE METHOD CALLED, THERMAL    APP EXISTS: " << _fe_problem.hasMultiApp(_thermal_app_name) << std::endl;
}

void ReactorCouplingUserObject::execute()
{
  Real time = _fe_problem.time();
  unsigned int current_step = std::floor(time + 0.0001);

  std::cout << "ReactorCouplingUserObject: EXECUTE METHOD CALLED, TIME=" << time << ", BURNUP STEP=" << _burn_step << std::endl;

  bool success = false;

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
}

// 执行第一个燃耗步
bool ReactorCouplingUserObject::executeFirstStep()
{
  std::cout << "ReactorCouplingUserObject: EXECUTE FIRST BURNUP STEP" << std::endl;
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

// 执行后续燃耗步
bool ReactorCouplingUserObject::executeSubsequentStep()
{
  std::cout << "ReactorCouplingUserObject: EXECUTE SUBSEQUENT BURNUP STEP" << std::endl;

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

// 执行第一次中子学计算
void ReactorCouplingUserObject::executefirstNeutronics()
{
  // 检查中子学多应用程序是否存在
  if (!_fe_problem.hasMultiApp(_neutronics_app_name))
  {
    std::cout << "WARNING: ReactorCouplingUserObject: Can't find neutronics multiapp '" << _neutronics_app_name << "'" << std::endl;
    return;
  }

  std::cout << "ReactorCouplingUserObject: EXECUTE NEUTRONICS, CURRENT BURNUP STEP=" << _burn_step << std::endl;

  // 准备中子学应用程序列表，用于选择性执行
  std::vector<std::string> neutronics_apps = {_neutronics_app_name};

  // 更新Fortran程序中的燃耗步信息
  updateFortranBurnupStep();

  std::cout << "UserObject进程 " << processor_id() << " 执行中子学计算" << std::endl;
  // 执行中子学应用
  _fe_problem.execMultiApps(LevelSet::EXEC_NEUTRONIC);

  // 触发后处理器计算和输出
  //_fe_problem.execute(EXEC_TIMESTEP_END);
  //_fe_problem.outputStep(EXEC_TIMESTEP_END);
}

// 执行后续中子学计算
void ReactorCouplingUserObject::executesubsquentNeutronics()
{
  // 检查中子学多应用程序是否存在
  if (!_fe_problem.hasMultiApp(_neutronics_app_name))
  {
    std::cout << "WARNING: ReactorCouplingUserObject: Can't find neutronics multiapp '" << _neutronics_app_name << "'" << std::endl;
    return;
  }

  std::cout << "ReactorCouplingUserObject: EXECUTE NEUTRONICS, CURRENT BURNUP STEP=" << _burn_step << std::endl;

  // 准备中子学应用程序列表，用于选择性执行
  std::vector<std::string> neutronics_apps = {_neutronics_app_name};

  // 更新Fortran程序中的燃耗步信息
  updateFortranBurnupStep();

  std::cout << "UserObject进程 " << processor_id() << " 执行中子学计算" << std::endl;
  // 执行中子学应用
  //_fe_problem.execMultiApps(LevelSet::EXEC_PRENEUTRONIC);

  _fe_problem.execMultiApps(LevelSet::EXEC_CORNEUTRONIC);
}

// 执行第一次核热耦合计算
bool ReactorCouplingUserObject::executefirstCoupled()
{

  std::cout << "\n====== 开始固定点迭代 ======" << std::endl;
  std::cout << "最大迭代次数: " << _fixed_point_max_its << std::endl;
  std::cout << "最小迭代次数: " << _fixed_point_min_its << std::endl;
  std::cout << "收敛容差: " << _fixed_point_tol << std::endl;

  _fe_problem.execMultiApps(LevelSet::EXEC_CORNEUTRONIC);
  std::cout << "ReactorCouplingUserObject: EXECUTE Fixed Point Iteration..." << std::endl;
  _fe_problem.execMultiApps(LevelSet::EXEC_THERMAL);

  std::cout << "====== 固定点迭代完成 ======\n"
            << std::endl;

  return true;
}

// 执行后续核热耦合计算
bool ReactorCouplingUserObject::executesubsquentCoupled()
{
  // 检查多应用程序是否存在
  bool has_neutronics = _fe_problem.hasMultiApp(_neutronics_app_name);
  bool has_thermal = _fe_problem.hasMultiApp(_thermal_app_name);

  if (!has_neutronics || !has_thermal)
  {
    std::cout << "WARNING: ReactorCouplingUserObject: Can't find necessary multiapps" << std::endl;
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
    std::cout << "ReactorCouplingUserObject: 耦合迭代次数=" << iter << std::endl;

    std::cout << "EXECUTE NEUTRONICS..." << std::endl;
    bool neutronics_success = _fe_problem.execMultiApps(LevelSet::EXEC_NEUTRONIC);
    if (!neutronics_success)
    {
      std::cout << "NEUTRONICS EXECUTION FAILED!" << std::endl;
      return false;
    }

    std::cout << "EXECUTE THERMAL..." << std::endl;
    bool thermal_success = _fe_problem.execMultiApps(LevelSet::EXEC_THERMAL);
    if (!thermal_success)
    {
      std::cout << "THERMAL EXECUTION FAILED!" << std::endl;
      return false;
    }
  }

  // 判断是否达到最大迭代次数
  if (iter >= _max_coupling_iterations)
  {
    std::cout << "ReactorCouplingUserObject: MAX ITERATIONS REACHED (" << _max_coupling_iterations << "), BUT CONVERGENCE NOT REACHED (current: " << temp_convergence << ", target: " << _coupling_tolerance << ")" << std::endl;
    return true;
  }

  return true;
}

// 更新Fortran程序中的燃耗步信息
void ReactorCouplingUserObject::updateFortranBurnupStep()
{
  std::cout << "Passing burnup step information to Fortran program" << std::endl;

  // 创建本地副本
  int burn_step_copy = static_cast<int>(_burn_step);
  int max_steps_copy = static_cast<int>(_max_burn_steps);

  // 调用Fortran接口，使用本地变量
  update_burnup_step(burn_step_copy, max_steps_copy);

  std::cout << "Fortran program burnup step updated successfully" << std::endl;
}
