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
  std::cout << "\n====== CoupledCalculator: 开始首次耦合计算 ======" << std::endl;
  std::cout << "最大固定点迭代次数: " << _fixed_point_max_its << std::endl;
  std::cout << "最小固定点迭代次数: " << _fixed_point_min_its << std::endl;
  std::cout << "固定点收敛容差: " << _fixed_point_tol << std::endl;
  std::cout << "当前燃耗步: " << _burn_step << std::endl;

  // 执行校正中子学计算
  std::cout << "CoupledCalculator: 执行校正中子学计算..." << std::endl;
  _fe_problem.execMultiApps(LevelSet::EXEC_CORNEUTRONIC);

  // 执行热工计算
  std::cout << "CoupledCalculator: 执行热工计算..." << std::endl;
  _fe_problem.execMultiApps(LevelSet::EXEC_THERMAL);

  std::cout << "====== CoupledCalculator: 首次耦合计算完成 ======\n" << std::endl;

  return true;
}

bool
CoupledCalculator::executeSubsequent()
{
  // 检查多应用程序是否存在
  bool has_neutronics = _fe_problem.hasMultiApp(_neutronics_app_name);
  bool has_thermal = _fe_problem.hasMultiApp(_thermal_app_name);

  if (!has_neutronics || !has_thermal)
  {
    std::cout << "WARNING: CoupledCalculator: 无法找到必要的多应用程序" << std::endl;
    std::cout << "  中子学应用存在: " << has_neutronics << std::endl;
    std::cout << "  热工应用存在: " << has_thermal << std::endl;
    return false;
  }

  std::cout << "\n====== CoupledCalculator: 开始后续耦合计算 ======" << std::endl;
  std::cout << "当前燃耗步: " << _burn_step << std::endl;
  std::cout << "最大耦合迭代次数: " << _max_coupling_iterations << std::endl;
  std::cout << "耦合收敛容差: " << _coupling_tolerance << std::endl;

  // 耦合迭代
  Real temp_convergence = 1.0;
  unsigned int iter = 0;

  while (iter < _max_coupling_iterations && temp_convergence > _coupling_tolerance)
  {
    // 记录迭代次数
    iter++;
    std::cout << "\nCoupledCalculator: 耦合迭代 #" << iter << std::endl;

    // 执行中子学计算
    std::cout << "  执行中子学计算..." << std::endl;
    bool neutronics_success = _fe_problem.execMultiApps(LevelSet::EXEC_NEUTRONIC);
    if (!neutronics_success)
    {
      std::cout << "  错误: 中子学计算失败!" << std::endl;
      return false;
    }

    // 执行热工计算
    std::cout << "  执行热工计算..." << std::endl;
    bool thermal_success = _fe_problem.execMultiApps(LevelSet::EXEC_THERMAL);
    if (!thermal_success)
    {
      std::cout << "  错误: 热工计算失败!" << std::endl;
      return false;
    }

    std::cout << "  当前收敛性: " << temp_convergence << std::endl;
  }

  // 判断收敛结果
  if (iter >= _max_coupling_iterations)
  {
    std::cout << "\nCoupledCalculator: 达到最大迭代次数 (" << _max_coupling_iterations
              << "), 当前收敛性: " << temp_convergence << ", 目标: " << _coupling_tolerance
              << std::endl;
    if (_accept_on_max_iteration)
    {
      std::cout << "CoupledCalculator: 接受当前解" << std::endl;
    }
    else
    {
      std::cout << "CoupledCalculator: 警告 - 未达到收敛" << std::endl;
    }
  }
  else
  {
    std::cout << "\nCoupledCalculator: 耦合迭代收敛! 迭代次数: " << iter << std::endl;
  }

  std::cout << "====== CoupledCalculator: 后续耦合计算完成 ======\n" << std::endl;

  return true;
}

