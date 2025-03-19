/****************************************************************/
/* ReactorTransfer.h                                            */
/* Reactor Data Transfer Interface                              */
/*                                                              */
/* Defines interface for data transfer between neutronics       */
/* and thermal applications with custom execution flags.        */
/*                                                              */
/* Created: Mar 19, 2025                                        */
/* Last Modified: Mar 19, 2025                                  */
/****************************************************************/

#pragma once

#include "MultiAppCopyTransfer.h"

/**
 * 反应堆数据传输类，用于支持自定义执行标志的数据传输
 * 继承自MultiAppCopyTransfer，增加了对自定义执行标志的支持
 */
class ReactorTransfer : public MultiAppCopyTransfer
{
public:
  static InputParameters validParams();
  
  ReactorTransfer(const InputParameters & parameters);
  
  /**
   * 执行传输
   * @param exec_flag 执行标志类型
   */
  virtual void execute() override;
  
  /**
   * 初始化传输
   */
  virtual void initialSetup() override;
  
protected:
  /// 传输组名称
  const std::string _transfer_group;
}; 