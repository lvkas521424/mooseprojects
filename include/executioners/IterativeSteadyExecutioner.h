#pragma once

#include "Steady.h"

/**
 * 迭代稳态执行器
 * 执行多次稳态求解，配置迭代次数
 */
class IterativeSteadyExecutioner : public Steady
{
public:
  static InputParameters validParams();
  IterativeSteadyExecutioner(const InputParameters & parameters);

  virtual void execute() override;

protected:
  /// 最大迭代次数
  const unsigned int _max_iterations;
  
  /// 计算类型控制(仅作记录)
  const int _calc_type;
  
  /// 当前迭代计数
  unsigned int _current_iteration;
}; 