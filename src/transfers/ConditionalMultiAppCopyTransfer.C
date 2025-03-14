/****************************************************************/
/*   ConditionalMultiAppCopyTransfer - Conditional Copy Transfer*/
/*                                                              */
/*    PostProcessor-controlled MultiApp copy data transfer      */
/*    条件多应用复制传输 - 由后处理器控制的数据复制传输         */
/****************************************************************/

#include "ConditionalMultiAppCopyTransfer.h"
#include "MooseVariableFE.h"
#include "MultiApp.h"

registerMooseObject("mooseprojectsApp", ConditionalMultiAppCopyTransfer);

InputParameters
ConditionalMultiAppCopyTransfer::validParams()
{
  InputParameters params = ConditionalMultiAppTransfer::validParams();
  
  params.addClassDescription("Transfers variables between MultiApps using direct copy, only when enabled by PostProcessor");
  // 在由后处理器启用时，使用直接复制在多应用之间传输变量
  
  // Add parameters for source and target variable names
  // 添加源变量和目标变量名称的参数
  params.addRequiredParam<VariableName>("source_variable", 
                         "The name of the source variable");
                         // 源变量的名称
  params.addRequiredParam<VariableName>("target_variable", 
                         "The name of the target variable");
                         // 目标变量的名称
  
  return params;
}

ConditionalMultiAppCopyTransfer::ConditionalMultiAppCopyTransfer(const InputParameters & parameters) :
  ConditionalMultiAppTransfer(parameters),
  _source_var_name(getParam<VariableName>("source_variable")),
  _target_var_name(getParam<VariableName>("target_variable"))
{
  // Initialize
  // 初始化
  if (_communicator.rank() == 0) {
    _console << "ConditionalMultiAppCopyTransfer '" << name() << "' initialized" << std::endl;
    _console << "  Source variable: " << _source_var_name << std::endl;
    _console << "  Target variable: " << _target_var_name << std::endl;
  }
}

void
ConditionalMultiAppCopyTransfer::execute()
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
ConditionalMultiAppCopyTransfer::transferFromMultiApp()
{
  // Get source and target variables
  // 获取源变量和目标变量
  for (unsigned int i = 0; i < _multi_app->numGlobalApps(); i++) {
    if (_multi_app->hasLocalApp(i)) {
      auto & from_app = _multi_app->appProblem(i);
      auto & to_app = _fe_problem;
      
      auto & from_var = from_app.getVariable(0, _source_var_name);
      auto & to_var = to_app.getVariable(0, _target_var_name);
      
      // Copy solution
      // 复制解
      auto & from_solution = from_var.solution();
      auto & to_solution = to_var.solution();
      
      // This is a simplified copy - would need more robust approach for real applications
      // 这是一个简化的复制 - 实际应用需要更健壮的方法
      if (from_solution.size() == to_solution.size()) {
        for (unsigned int j = 0; j < from_solution.size(); j++) {
          to_solution.set(j, from_solution(j));
        }
        
        to_solution.close();
        to_var.sys().update();
      } else {
        mooseError("ConditionalMultiAppCopyTransfer: Solution size mismatch in FROM_MULTIAPP direction");
        // 条件多应用复制传输：FROM_MULTIAPP方向上的解大小不匹配
      }
    }
  }
}

void
ConditionalMultiAppCopyTransfer::transferToMultiApp()
{
  // Get source and target variables
  // 获取源变量和目标变量
  for (unsigned int i = 0; i < _multi_app->numGlobalApps(); i++) {
    if (_multi_app->hasLocalApp(i)) {
      auto & from_app = _fe_problem;
      auto & to_app = _multi_app->appProblem(i);
      
      auto & from_var = from_app.getVariable(0, _source_var_name);
      auto & to_var = to_app.getVariable(0, _target_var_name);
      
      // Copy solution
      // 复制解
      auto & from_solution = from_var.solution();
      auto & to_solution = to_var.solution();
      
      // This is a simplified copy - would need more robust approach for real applications
      // 这是一个简化的复制 - 实际应用需要更健壮的方法
      if (from_solution.size() == to_solution.size()) {
        for (unsigned int j = 0; j < from_solution.size(); j++) {
          to_solution.set(j, from_solution(j));
        }
        
        to_solution.close();
        to_var.sys().update();
      } else {
        mooseError("ConditionalMultiAppCopyTransfer: Solution size mismatch in TO_MULTIAPP direction");
        // 条件多应用复制传输：TO_MULTIAPP方向上的解大小不匹配
      }
    }
  }
} 