/****************************************************************/
/* CoupledCalculator.C                                          */
/* 耦合计算模块实现                                              */
/*                                                              */
/* 负责处理反应堆中子学-热工耦合计算，包括固定点迭代              */
/*                                                              */
/* Created: Oct 13, 2025                                        */
/****************************************************************/

#include "CoupledCalculator.h"
#include "LevelSetTypes.h"
#include <iostream>

CoupledCalculator::CoupledCalculator(FEProblemBase & fe_problem,
                                     const std::string & neutronics_app_name,
                                     const std::string & thermal_app_name,
                                     unsigned int & burn_step,
                                     unsigned int max_coupling_iterations,
                                     Real coupling_tolerance,
                                     unsigned int fixed_point_max_its,
                                     unsigned int fixed_point_min_its,
                                     Real fixed_point_tol,
                                     bool accept_on_max_iteration)
  : _fe_problem(fe_problem),
    _neutronics_app_name(neutronics_app_name),
    _thermal_app_name(thermal_app_name),
    _burn_step(burn_step),
    _max_coupling_iterations(max_coupling_iterations),
    _coupling_tolerance(coupling_tolerance),
    _fixed_point_max_its(fixed_point_max_its),
    _fixed_point_min_its(fixed_point_min_its),
    _fixed_point_tol(fixed_point_tol),
    _accept_on_max_iteration(accept_on_max_iteration)
{
}

bool
CoupledCalculator::executeFirst()
{
  _fe_problem.execMultiApps(LevelSet::EXEC_CORNEUTRONIC);
  _fe_problem.execMultiApps(LevelSet::EXEC_THERMAL);
  return true;
}

bool
CoupledCalculator::executeSubsequent()
{
  Real temp_convergence = 1.0;
  
  for (unsigned int iter = 1; iter <= _max_coupling_iterations && temp_convergence > _coupling_tolerance; ++iter)
  {
    if (!_fe_problem.execMultiApps(LevelSet::EXEC_NEUTRONIC))
      return false;
    if (!_fe_problem.execMultiApps(LevelSet::EXEC_THERMAL))
      return false;
  }

  return true;
}

