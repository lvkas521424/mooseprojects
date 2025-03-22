/****************************************************************/
/* ReactorCouplingControl.h                                     */
/* Header for Reactor Neutronics-Thermal Coupling Control       */
/*                                                              */
/* Defines interface for neutronics-thermal coupling control    */
/* and burnup step management in reactor simulations.           */
/*                                                              */
/* Created: Mar 19, 2025                                        */
/* Last Modified: Mar 19, 2025                                  */
/****************************************************************/

#pragma once

#include "Control.h"
#include "FEProblem.h"
#include "MultiApp.h"
#include "MultiAppTransfer.h"
#include "PostprocessorInterface.h"

   extern "C" {
     void update_burnup_step(int step, int max_steps);
   }

class ReactorCouplingControl : public Control
{
public:
  static InputParameters validParams();
  
  ReactorCouplingControl(const InputParameters & parameters);
  
  /**
   * 执行控制逻辑
   */
  virtual void execute() override;
  
  /**
   * 获取计算类型
   * @return 计算类型: 1-仅中子学, 2-耦合计算
   */
  unsigned int getCalculationType() const { return _calc_type; }
  
  /**
   * 获取当前燃耗步
   * @return 当前燃耗步
   */
  unsigned int getBurnupStep() const { return _burn_step; }
  
  /**
   * 设置计算类型
   * @param type 计算类型
   */
  void setCalculationType(unsigned int type);
  
  /**
   * 设置燃耗步
   * @param step 燃耗步
   */
  void setBurnupStep(unsigned int step);
  
  /**
   * 执行中子学计算
   */
  void executefirstNeutronics();

    /**
   * 执行中子学计算
   */
  void executesubsquentNeutronics();

  /**
   * 执行耦合计算
   * @return 是否收敛
   */
  bool executefirstCoupled();
  
  /**
   * 执行耦合计算
   * @return 是否收敛
   */
  bool executesubsquentCoupled();
  
  
  /**
   * 执行完整的燃耗计算
   * 先循环燃耗步，再根据燃耗步编号和计算类型执行不同的计算
   */
  void executeBurnupCalculation();
  
  /**
   * 执行第一个燃耗步
   * @return 计算是否成功
   */
  bool executeFirstStep();
  
  /**
   * 执行第二个燃耗步
   * @return 计算是否成功
   */
  bool executeSecondStep();
  
  /**
   * 执行后续燃耗步
   * @return 计算是否成功
   */
  bool executeSubsequentStep();
  
  /**
   * 更新Fortran程序中的燃耗步
   * Update burnup step in Fortran program
   * 将当前燃耗步传递给外部Fortran程序
   * Passes current burnup step to external Fortran program
   */
  void updateFortranBurnupStep();
  
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
  
  /// 向中子学应用传输数据的传输组名称
  const std::string _to_neutronics_transfers;
  
  /// 从中子学应用接收数据的传输组名称
  const std::string _from_neutronics_transfers;
  
  /// 向热工应用传输数据的传输组名称
  const std::string _to_thermal_transfers;
  
  /// 从热工应用接收数据的传输组名称
  const std::string _from_thermal_transfers;
  
  /// 温度收敛判断后处理器名称
  const std::string _temp_conv_pp_name;
  
  /// 累积燃耗值 (MWd/tU)
  /// Accumulated burnup value (MWd/tU)
  std::vector<Real> _burnup_values;
  
  /// 功率水平 (MW)
  /// Power level (MW)
  Real _power_level;
  
  /// 时间步长 (天)
  /// Time step (days)
  Real _time_step;
}; 