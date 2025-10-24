/****************************************************************/
/* ReactorCouplingUserObject.h                                  */
/* Header for Reactor Neutronics-Thermal Coupling UserObject    */
/*                                                              */
/* Defines interface for neutronics-thermal coupling control    */
/* and burnup step management in reactor simulations.           */
/*                                                              */
/* Created: Mar 19, 2025                                        */
/* Last Modified: Oct 13, 2025 - 模块化重构                      */
/****************************************************************/

#pragma once

#include "GeneralUserObject.h"
#include "FEProblem.h"
#include "MultiApp.h"
#include "MultiAppTransfer.h"
#include "NeutronicsCalculator.h"
#include "CoupledCalculator.h"
#include "FortranInterface.h"
#include <memory>

class ReactorCouplingUserObject : public GeneralUserObject
{
public:
  static InputParameters validParams();

  ReactorCouplingUserObject(const InputParameters &parameters);

  /**
   * 必需的UserObject接口方法
   */
  virtual void initialize() override;
  virtual void execute() override;
  virtual void finalize() override {};

  /**
   * 公共访问接口：获取计算类型
   */
  unsigned int getCalcType() const { return _calc_type; }
  
  /**
   * 公共访问接口：获取当前燃耗步
   */
  unsigned int getBurnStep() const { return _burn_step; }

protected:
  /**
   * 执行第一个燃耗步
   * @return 计算是否成功
   */
  bool executeFirstStep();

  /**
   * 执行后续燃耗步
   * @return 计算是否成功
   */
  bool executeSubsequentStep();

protected:
  /// 计算类型: 1-仅中子学, 2-耦合计算
  unsigned int _calc_type;

  /// 当前燃耗步
  unsigned int _burn_step;

  /// 最大燃耗步数
  const unsigned int _max_burn_steps;

  /// 最大耦合迭代次数
  const unsigned int _max_coupling_iterations;

  /// 耦合收敛容差
  const Real _coupling_tolerance;

  /// 中子学多应用程序名称
  const std::string _neutronics_app_name;

  /// 热工多应用程序名称
  const std::string _thermal_app_name;

  /// 固定点迭代参数
  const unsigned int _fixed_point_max_its;
  const unsigned int _fixed_point_min_its;
  const Real _fixed_point_tol;
  const bool _accept_on_max_iteration;

  /// FE问题引用
  FEProblemBase &_fe_problem;

  /// 计算模块（采用组合模式）
  std::unique_ptr<NeutronicsCalculator> _neutronics_calculator;
  std::unique_ptr<CoupledCalculator> _coupled_calculator;
  std::unique_ptr<FortranInterface> _fortran_interface;
};