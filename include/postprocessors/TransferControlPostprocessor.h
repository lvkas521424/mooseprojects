/****************************************************************/
/*     TransferControlPostprocessor - Transfer Controller       */
/*                                                              */
/*            Controls Data Transfer Enable/Disable             */
/*      传输控制后处理器 - 控制数据传输的启用/禁用状态          */
/****************************************************************/

#pragma once

#include "GeneralPostprocessor.h"

/**
 * PostProcessor controlling data transfer enable/disable state
 * 控制数据传输启用/禁用状态的后处理器
 */
class TransferControlPostprocessor : public GeneralPostprocessor
{
public:
  static InputParameters validParams();
  
  TransferControlPostprocessor(const InputParameters & parameters);
  
  virtual void initialize() override;
  virtual void execute() override;
  virtual Real getValue() override;
  
  /**
   * Set transfer mode
   * @param mode 0=disable transfer, 1=enable transfer
   * 设置传输模式
   * @param mode 0=禁用传输, 1=启用传输
   */
  void setTransferMode(int mode);

protected:
  /// Transfer mode: 0=disabled, 1=enabled
  /// 传输模式: 0=禁用, 1=启用
  int _transfer_mode;
  
  /// Console output stream
  /// 控制台输出流
  ConsoleStream _console;
}; 