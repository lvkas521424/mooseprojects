/****************************************************************/
/*  TemperatureConvergencePostprocessor - Convergence Checker   */
/*                                                              */
/*          Checks Temperature Field Convergence                */
/*      温度收敛后处理器 - 检查温度场的收敛状态                 */
/****************************************************************/

#include "TemperatureConvergencePostprocessor.h"
#include "MooseMesh.h"
#include "MooseVariable.h"

registerMooseObject("mooseprojectsApp", TemperatureConvergencePostprocessor);

InputParameters
TemperatureConvergencePostprocessor::validParams()
{
  InputParameters params = GeneralPostprocessor::validParams();
  
  params.addClassDescription("PostProcessor that computes the relative change in temperature field between coupling iterations");
  // 计算耦合迭代之间温度场相对变化的后处理器
  
  // Add parameter for temperature variable
  // 添加温度变量参数
  params.addRequiredParam<VariableName>("temperature", "The name of the temperature variable");
                                        // 温度变量的名称
  
  return params;
}

TemperatureConvergencePostprocessor::TemperatureConvergencePostprocessor(const InputParameters & parameters) :
  GeneralPostprocessor(parameters),
  _is_first_iteration(true),
  _temperature_var(getParam<VariableName>("temperature")),
  _console(_communicator.rank() == 0 ? Moose::out : Moose::err)
{
  // Initialize vectors
  // 初始化向量
  reset();
}

void
TemperatureConvergencePostprocessor::initialize()
{
  // Store current temperature field for comparison
  // 存储当前温度场以供比较
  if (!_is_first_iteration) {
    // Move current to old
    // 将当前值移动到旧值
    _old_temperature = _current_temperature;
  }
  
  // Get current temperature field from variable
  // 从变量获取当前温度场
  auto & var = _fe_problem.getVariable(0, _temperature_var);
  auto & solution = var.solution();
  
  // Copy solution to temperature field
  // 将解复制到温度场
  _current_temperature.resize(solution.size());
  for (unsigned int i = 0; i < solution.size(); i++) {
    _current_temperature[i] = solution(i);
  }
  
  // Clear first iteration flag
  // 清除第一次迭代标志
  _is_first_iteration = false;
}

void
TemperatureConvergencePostprocessor::execute()
{
  // Nothing to do here
  // 此处无需操作
}

Real
TemperatureConvergencePostprocessor::getValue()
{
  // Return the computed relative change
  // 返回计算的相对变化值
  return computeRelativeChange();
}

void
TemperatureConvergencePostprocessor::reset()
{
  // Reset temperature fields and iteration flag
  // 重置温度场和迭代标志
  _old_temperature.clear();
  _current_temperature.clear();
  _is_first_iteration = true;
  
  if (_communicator.rank() == 0) {
    _console << "Temperature convergence check reset" << std::endl;
    // 温度收敛检查已重置
  }
}

void
TemperatureConvergencePostprocessor::setTemperatureField(const std::vector<Real> & temp)
{
  // Store temperature field (for testing or external setting)
  // 存储温度场（用于测试或外部设置）
  if (_is_first_iteration) {
    _current_temperature = temp;
    _is_first_iteration = false;
  } else {
    _old_temperature = _current_temperature;
    _current_temperature = temp;
  }
}

Real
TemperatureConvergencePostprocessor::computeRelativeChange()
{
  // If this is the first iteration, we don't have a previous solution to compare against
  // 如果这是第一次迭代，我们没有以前的解决方案可以比较
  if (_is_first_iteration || _old_temperature.empty() || _current_temperature.empty()) {
    return 1.0;
  }
  
  // Check that sizes match
  // 检查大小是否匹配
  if (_old_temperature.size() != _current_temperature.size()) {
    mooseError("TemperatureConvergencePostprocessor: Temperature field size mismatch between iterations");
    // 温度收敛后处理器：迭代之间温度场大小不匹配
  }
  
  // Compute relative change: |T_new - T_old| / |T_new|
  // 计算相对变化: |T_new - T_old| / |T_new|
  Real sum_diff = 0.0;
  Real sum_temp = 0.0;
  
  for (unsigned int i = 0; i < _current_temperature.size(); i++) {
    sum_diff += std::abs(_current_temperature[i] - _old_temperature[i]);
    sum_temp += std::abs(_current_temperature[i]);
  }
  
  // Avoid division by zero
  // 避免除以零
  if (sum_temp < 1e-12) {
    return 0.0;
  }
  
  return sum_diff / sum_temp;
} 