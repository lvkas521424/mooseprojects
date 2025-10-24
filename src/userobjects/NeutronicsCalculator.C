/****************************************************************/
/* NeutronicsCalculator.C                                       */
/* 中子学计算模块实现                                             */
/*                                                              */
/* 负责处理反应堆中子学计算，包括首次计算和后续燃耗步计算          */
/*                                                              */
/* Created: Oct 13, 2025                                        */
/****************************************************************/

#include "NeutronicsCalculator.h"
#include "LevelSetTypes.h"
#include <iostream>

NeutronicsCalculator::NeutronicsCalculator(FEProblemBase & fe_problem,
                                           const std::string & neutronics_app_name,
                                           unsigned int & burn_step)
  : _fe_problem(fe_problem), _neutronics_app_name(neutronics_app_name), _burn_step(burn_step)
{
}

void
NeutronicsCalculator::executeFirst()
{
  if (_fe_problem.hasMultiApp(_neutronics_app_name))
    _fe_problem.execMultiApps(LevelSet::EXEC_NEUTRONIC);
}

void
NeutronicsCalculator::executeSubsequent()
{
  if (_fe_problem.hasMultiApp(_neutronics_app_name))
    _fe_problem.execMultiApps(LevelSet::EXEC_CORNEUTRONIC);
}

