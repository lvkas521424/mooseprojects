/*
 * Author: lvjiahui eba424@163.com
 * Date: 2025-10-13 11:46:32
 * LastEditors: lvjiahui eba424@163.com
 * LastEditTime: 2025-10-14 14:44:27
 * FilePath: /mooseprojects/include/userobjects/CoupledCalculator.h
 * Description: 
 */


#pragma once

#include "FEProblem.h"
#include <string>

/**
 * 耦合计算模块
 * 封装了中子学-热工耦合计算的具体实现
 */
class CoupledCalculator
{
public:
  /**
   * 构造函数
   * @param fe_problem FE问题引用
   * @param neutronics_app_name 中子学多应用程序名称
   * @param thermal_app_name 热工多应用程序名称
   * @param burn_step 当前燃耗步
   * @param max_coupling_iterations 最大耦合迭代次数
   * @param coupling_tolerance 耦合收敛容差
   * @param fixed_point_max_its 固定点迭代最大次数
   * @param fixed_point_min_its 固定点迭代最小次数
   * @param fixed_point_tol 固定点收敛容差
   * @param accept_on_max_iteration 达到最大迭代次数时是否接受解
   */
  CoupledCalculator(FEProblemBase & fe_problem,
                    const std::string & neutronics_app_name,
                    const std::string & thermal_app_name,
                    unsigned int & burn_step,
                    unsigned int max_coupling_iterations,
                    Real coupling_tolerance,
                    unsigned int fixed_point_max_its,
                    unsigned int fixed_point_min_its,
                    Real fixed_point_tol,
                    bool accept_on_max_iteration);

  /**
   * 执行首次耦合计算
   * @return 计算是否成功
   */
  bool executeFirst();

  /**
   * 执行后续燃耗步的耦合计算
   * @return 计算是否成功
   */
  bool executeSubsequent();

protected:
  /// FE问题引用
  FEProblemBase & _fe_problem;

  /// 中子学多应用程序名称
  const std::string & _neutronics_app_name;

  /// 热工多应用程序名称
  const std::string & _thermal_app_name;

  /// 当前燃耗步引用
  unsigned int & _burn_step;

  /// 最大耦合迭代次数
  const unsigned int _max_coupling_iterations;

  /// 耦合收敛容差
  const Real _coupling_tolerance;

  /// 固定点迭代参数
  const unsigned int _fixed_point_max_its;
  const unsigned int _fixed_point_min_its;
  const Real _fixed_point_tol;
  const bool _accept_on_max_iteration;
};

