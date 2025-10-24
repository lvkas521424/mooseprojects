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
  // 检查中子学多应用程序是否存在
  if (!_fe_problem.hasMultiApp(_neutronics_app_name))
  {
    std::cout << "WARNING: NeutronicsCalculator: Can't find neutronics multiapp '"
              << _neutronics_app_name << "'" << std::endl;
    return;
  }

  std::cout << "NeutronicsCalculator: 执行首次中子学计算, 当前燃耗步=" << _burn_step
            << std::endl;

  // 执行中子学应用
  _fe_problem.execMultiApps(LevelSet::EXEC_NEUTRONIC);

  std::cout << "NeutronicsCalculator: 首次中子学计算完成" << std::endl;
}

void
NeutronicsCalculator::executeSubsequent()
{
  // 检查中子学多应用程序是否存在
  if (!_fe_problem.hasMultiApp(_neutronics_app_name))
  {
    std::cout << "WARNING: NeutronicsCalculator: Can't find neutronics multiapp '"
              << _neutronics_app_name << "'" << std::endl;
    return;
  }

  std::cout << "NeutronicsCalculator: 执行后续中子学计算, 当前燃耗步=" << _burn_step
            << std::endl;

  // 执行中子学应用（校正中子学）
  _fe_problem.execMultiApps(LevelSet::EXEC_CORNEUTRONIC);

  std::cout << "NeutronicsCalculator: 后续中子学计算完成" << std::endl;
}

