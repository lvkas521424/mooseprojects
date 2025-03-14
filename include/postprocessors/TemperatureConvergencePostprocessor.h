/****************************************************************/
/*  TemperatureConvergencePostprocessor - Convergence Checker   */
/*                                                              */
/*          Checks Temperature Field Convergence                */
/*      温度收敛后处理器 - 检查温度场的收敛状态                 */
/****************************************************************/

#pragma once

#include "GeneralPostprocessor.h"

/**
 * PostProcessor checking temperature field convergence
 * 检查温度场收敛性的后处理器
 */
class TemperatureConvergencePostprocessor : public GeneralPostprocessor
{
public:
  static InputParameters validParams();
  
  TemperatureConvergencePostprocessor(const InputParameters & parameters);
  
  virtual void initialize() override;
  virtual void execute() override;
  virtual Real getValue() override;
  
  /**
   * Reset convergence check
   * 重置收敛检查
   */
  void reset();
  
  /**
   * Set temperature field (primarily for testing)
   * @param temp Temperature field data
   * 设置温度场(主要用于测试)
   * @param temp 温度场数据
   */
  void setTemperatureField(const std::vector<Real> & temp);

protected:
  /// Previous iteration temperature field
  /// 上一次迭代的温度场
  std::vector<Real> _old_temperature;
  
  /// Current iteration temperature field
  /// 当前迭代的温度场
  std::vector<Real> _current_temperature;
  
  /// Whether it's the first iteration
  /// 是否是第一次迭代
  bool _is_first_iteration;
  
  /// Temperature field variable name
  /// 温度场变量名称
  const VariableName & _temperature_var;
  
  /// Console output stream
  /// 控制台输出流
  ConsoleStream _console;
  
  /**
   * Calculate temperature field relative change
   * @return Relative change value
   * 计算温度场相对变化值
   * @return 相对变化值
   */
  Real computeRelativeChange();
}; 