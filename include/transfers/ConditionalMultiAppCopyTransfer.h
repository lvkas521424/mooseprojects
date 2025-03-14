/****************************************************************/
/*   ConditionalMultiAppCopyTransfer - Conditional Copy Transfer*/
/*                                                              */
/*    PostProcessor-controlled MultiApp copy data transfer      */
/*    条件多应用复制传输 - 由后处理器控制的数据复制传输         */
/****************************************************************/

#pragma once

#include "ConditionalMultiAppTransfer.h"

/**
 * Transfers variables between MultiApps using direct copy
 * Only executes when control PostProcessor returns 1
 * 使用直接复制在多应用之间传输变量
 * 仅当控制后处理器返回1时执行
 */
class ConditionalMultiAppCopyTransfer : public ConditionalMultiAppTransfer
{
public:
  static InputParameters validParams();
  
  ConditionalMultiAppCopyTransfer(const InputParameters & parameters);
  
  virtual void execute() override;

protected:
  /**
   * Transfer from MultiApp to parent app
   * 从多应用传输到父应用
   */
  void transferFromMultiApp();
  
  /**
   * Transfer from parent app to MultiApp
   * 从父应用传输到多应用
   */
  void transferToMultiApp();
  
  /// Source variable name
  /// 源变量名称
  const VariableName & _source_var_name;
  
  /// Target variable name
  /// 目标变量名称
  const VariableName & _target_var_name;
}; 