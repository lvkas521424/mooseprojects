/****************************************************************/
/* ConditionalMultiAppInterpolationTransfer - Interpolation     */
/*                                                              */
/*  PostProcessor-controlled MultiApp interpolation transfer    */
/*  条件多应用插值传输 - 由后处理器控制的数据插值传输           */
/****************************************************************/

#include "ConditionalMultiAppInterpolationTransfer.h"
#include "MooseVariableFE.h"
#include "MooseMesh.h"

registerMooseObject("mooseprojectsApp", ConditionalMultiAppInterpolationTransfer);

InputParameters
ConditionalMultiAppInterpolationTransfer::validParams()
{
  InputParameters params = ConditionalMultiAppTransfer::validParams();
  
  params.addClassDescription("Transfers variables between MultiApps using interpolation, only when enabled by PostProcessor");
  // 在由后处理器启用时，使用插值在多应用之间传输变量
  
  // Add parameters for source and target variable names
  // 添加源变量和目标变量名称的参数
  params.addRequiredParam<VariableName>("source_variable", 
                         "The name of the source variable");
                         // 源变量的名称
  params.addRequiredParam<VariableName>("target_variable", 
                         "The name of the target variable");
                         // 目标变量的名称
  
  // Add parameters for interpolation options
  // 添加插值选项参数
  params.addParam<bool>("use_linear_interpolation", true,
                        "Whether to use linear interpolation (vs. constant)");
                        // 是否使用线性插值(对比常数插值)
  
  return params;
}

ConditionalMultiAppInterpolationTransfer::ConditionalMultiAppInterpolationTransfer(const InputParameters & parameters) :
  ConditionalMultiAppTransfer(parameters),
  _source_var_name(getParam<VariableName>("source_variable")),
  _target_var_name(getParam<VariableName>("target_variable")),
  _use_linear_interpolation(getParam<bool>("use_linear_interpolation"))
{
  // Initialize
  // 初始化
  if (_communicator.rank() == 0) {
    _console << "ConditionalMultiAppInterpolationTransfer '" << name() << "' initialized" << std::endl;
    _console << "  Source variable: " << _source_var_name << std::endl;
    _console << "  Target variable: " << _target_var_name << std::endl;
    _console << "  Interpolation method: " << (_use_linear_interpolation ? "Linear" : "Constant") << std::endl;
  }
}

void
ConditionalMultiAppInterpolationTransfer::execute()
{
  // Check if transfer should execute
  // 检查是否应执行传输
  if (!shouldTransfer()) {
    _console << "Skipping transfer '" << name() << "'" << std::endl;
    return;
  }
  
  // Execute the transfer based on the direction
  // 根据传输方向执行传输
  if (_current_direction == TO_MULTIAPP) {
    _console << "Executing transfer '" << name() << "' TO_MULTIAPP" << std::endl;
    transferToMultiApp();
  } else {
    _console << "Executing transfer '" << name() << "' FROM_MULTIAPP" << std::endl;
    transferFromMultiApp();
  }
}

void
ConditionalMultiAppInterpolationTransfer::transferFromMultiApp()
{
  // Get source and target variables
  // 获取源变量和目标变量
  for (unsigned int i = 0; i < _multi_app->numGlobalApps(); i++) {
    if (_multi_app->hasLocalApp(i)) {
      auto & from_app = _multi_app->appProblem(i);
      auto & to_app = _fe_problem;
      
      auto & from_var = from_app.getVariable(0, _source_var_name);
      auto & to_var = to_app.getVariable(0, _target_var_name);
      
      // Copy solution with interpolation
      // This is a simplified implementation - real applications would need more complex interpolation
      // 使用插值复制解
      // 这是一个简化的实现 - 实际应用需要更复杂的插值
      auto & from_solution = from_var.solution();
      auto & to_solution = to_var.solution();
      
      // For this simplified example, we do a direct copy if sizes match
      // In a real implementation, spatial interpolation would be performed
      // 对于这个简化的例子，如果大小匹配，我们做直接复制
      // 在实际实现中，会执行空间插值
      if (from_solution.size() == to_solution.size()) {
        for (unsigned int j = 0; j < from_solution.size(); j++) {
          to_solution.set(j, from_solution(j));
        }
        
        to_solution.close();
        to_var.sys().update();
        
        _console << "  Interpolation complete (simplified direct copy)" << std::endl;
        // 插值完成（简化的直接复制）
      } else {
        _console << "  Warning: Solution size mismatch, using simple averaging" << std::endl;
        // 警告：解大小不匹配，使用简单平均
        
        // Simple averaging if sizes don't match
        // 如果大小不匹配，使用简单平均
        Real avg_value = 0.0;
        for (unsigned int j = 0; j < from_solution.size(); j++) {
          avg_value += from_solution(j);
        }
        avg_value /= from_solution.size();
        
        for (unsigned int j = 0; j < to_solution.size(); j++) {
          to_solution.set(j, avg_value);
        }
        
        to_solution.close();
        to_var.sys().update();
        
        _console << "  Average value set: " << avg_value << std::endl;
        // 已设置平均值：
      }
    }
  }
}

void
ConditionalMultiAppInterpolationTransfer::transferToMultiApp()
{
  // Get source and target variables
  // 获取源变量和目标变量
  for (unsigned int i = 0; i < _multi_app->numGlobalApps(); i++) {
    if (_multi_app->hasLocalApp(i)) {
      auto & from_app = _fe_problem;
      auto & to_app = _multi_app->appProblem(i);
      
      auto & from_var = from_app.getVariable(0, _source_var_name);
      auto & to_var = to_app.getVariable(0, _target_var_name);
      
      // Copy solution with interpolation
      // This is a simplified implementation - real applications would need more complex interpolation
      // 使用插值复制解
      // 这是一个简化的实现 - 实际应用需要更复杂的插值
      auto & from_solution = from_var.solution();
      auto & to_solution = to_var.solution();
      
      // For this simplified example, we do a direct copy if sizes match
      // In a real implementation, spatial interpolation would be performed
      // 对于这个简化的例子，如果大小匹配，我们做直接复制
      // 在实际实现中，会执行空间插值
      if (from_solution.size() == to_solution.size()) {
        for (unsigned int j = 0; j < from_solution.size(); j++) {
          to_solution.set(j, from_solution(j));
        }
        
        to_solution.close();
        to_var.sys().update();
        
        _console << "  Interpolation complete (simplified direct copy)" << std::endl;
        // 插值完成（简化的直接复制）
      } else {
        _console << "  Warning: Solution size mismatch, using simple averaging" << std::endl;
        // 警告：解大小不匹配，使用简单平均
        
        // Simple averaging if sizes don't match
        // 如果大小不匹配，使用简单平均
        Real avg_value = 0.0;
        for (unsigned int j = 0; j < from_solution.size(); j++) {
          avg_value += from_solution(j);
        }
        avg_value /= from_solution.size();
        
        for (unsigned int j = 0; j < to_solution.size(); j++) {
          to_solution.set(j, avg_value);
        }
        
        to_solution.close();
        to_var.sys().update();
        
        _console << "  Average value set: " << avg_value << std::endl;
        // 已设置平均值：
      }
    }
  }
} 