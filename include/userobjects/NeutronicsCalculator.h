/****************************************************************/
/* NeutronicsCalculator.h                                       */
/* 中子学计算模块                                                 */
/*                                                              */
/* 负责处理反应堆中子学计算，包括首次计算和后续燃耗步计算          */
/*                                                              */
/* Created: Oct 13, 2025                                        */
/****************************************************************/

#pragma once

#include "FEProblem.h"
#include <string>

/**
 * 中子学计算模块
 * 封装了中子学计算的具体实现
 */
class NeutronicsCalculator
{
public:
  /**
   * 构造函数
   * @param fe_problem FE问题引用
   * @param neutronics_app_name 中子学多应用程序名称
   * @param burn_step 当前燃耗步
   */
  NeutronicsCalculator(FEProblemBase & fe_problem,
                       const std::string & neutronics_app_name,
                       unsigned int & burn_step);

  /**
   * 执行首次中子学计算
   */
  void executeFirst();

  /**
   * 执行后续燃耗步的中子学计算
   */
  void executeSubsequent();

protected:
  /// FE问题引用
  FEProblemBase & _fe_problem;

  /// 中子学多应用程序名称
  const std::string & _neutronics_app_name;

  /// 当前燃耗步引用
  unsigned int & _burn_step;
};

