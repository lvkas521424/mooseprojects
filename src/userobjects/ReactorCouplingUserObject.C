/*
 * @Author      : lvjiahui eba424@163.com
 * @Date        : Do not edit
 * @LastEditors : lvjiahui eba424@163.com
 * @LastEditTime: 2025-10-14 14:24:06
 * @FilePath    : Do not edit
 * @Description : 
 * 
 * Copyright (c) 2025 by ${git_name_email}, All Rights Reserved. 
 */
/****************************************************************/
/* ReactorCouplingUserObject                                    */
/* A Reactor Neutronics-Thermal Coupling UserObject Module      */
/*                                                              */
/* This module controls the neutronics-thermal coupling         */
/* calculation process and manages burnup step iterations in    */
/* reactor simulations.                                         */
/*                                                              */
/* Created: Mar 19, 2025                                        */
/* Last Modified: Oct 13, 2025 - 模块化重构                      */
/****************************************************************/

#include "ReactorCouplingUserObject.h"
#include "FEProblem.h"
#include "MultiApp.h"
#include "MultiAppTransfer.h"
#include "mooseprojectsApp.h"
#include "LevelSetTypes.h"
#include "ReactorTransfer.h"
#include "NeutronicsMultiApp.h"
#include <memory>


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

  // 初始化计算模块（组合模式）
  _neutronics_calculator = std::make_unique<NeutronicsCalculator>(
      _fe_problem, _neutronics_app_name, _burn_step);

  _coupled_calculator = std::make_unique<CoupledCalculator>(_fe_problem,
                                                             _neutronics_app_name,
                                                             _thermal_app_name,
                                                             _burn_step,
                                                             _max_coupling_iterations,
                                                             _coupling_tolerance,
                                                             _fixed_point_max_its,
                                                             _fixed_point_min_its,
                                                             _fixed_point_tol,
                                                             _accept_on_max_iteration);

  _fortran_interface = std::make_unique<FortranInterface>(_burn_step, _max_burn_steps);

  std::cout << "\n====== ReactorCouplingUserObject: 模块化构造完成 ======" << std::endl;
  std::cout << "已创建模块: " << std::endl;
  std::cout << "  - 中子学计算模块 (NeutronicsCalculator)" << std::endl;
  std::cout << "  - 耦合计算模块 (CoupledCalculator)" << std::endl;
  std::cout << "  - Fortran接口模块 (FortranInterface)" << std::endl;
  std::cout << "====================================================\n" << std::endl;
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

// 执行第一个燃耗步（调用模块）
bool ReactorCouplingUserObject::executeFirstStep()
{
  std::cout << "\n====== ReactorCouplingUserObject: 执行首次燃耗步 ======" << std::endl;
  std::cout << "计算类型: " << _calc_type << std::endl;

  // 更新Fortran燃耗步信息
  _fortran_interface->updateBurnupStep();

  // 根据计算类型执行对应的计算
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

// 执行后续燃耗步（调用模块）
bool ReactorCouplingUserObject::executeSubsequentStep()
{
  std::cout << "\n====== ReactorCouplingUserObject: 执行后续燃耗步 ======" << std::endl;

  // 更新Fortran燃耗步信息
  _fortran_interface->updateBurnupStep();

  // 根据计算类型执行对应的计算
  if (_calc_type == 1) // 仅中子学
  {
    std::cout << "调用中子学计算模块..." << std::endl;
    _neutronics_calculator->executeSubsequent();
    return true;
  }
  else if (_calc_type == 2) // 耦合计算
  {
    std::cout << "调用耦合计算模块..." << std::endl;
    return _coupled_calculator->executeSubsequent();
  }

  return false;
}

