/****************************************************************/
/*        ConditionalMultiAppTransfer - Conditional Base        */
/*                                                              */
/*        PostProcessor-controlled MultiApp transfer base       */
/*  条件多应用传输 - 由后处理器控制的多应用程序传输基类         */
/****************************************************************/

#pragma once

#include "MultiAppTransfer.h"
#include "PostprocessorInterface.h"

/**
 * Base class for PostProcessor-controlled MultiApp data transfer
 * Transfers only execute when control PostProcessor returns 1
 * 后处理器控制的多应用数据传输基类
 * 仅当控制后处理器返回1时才执行传输
 */
class ConditionalMultiAppTransfer : 
  public MultiAppTransfer,
  public PostprocessorInterface
{
public:
  static InputParameters validParams();
  
  ConditionalMultiAppTransfer(const InputParameters & parameters);
  
  /**
   * Check if transfer should execute
   * @return Whether to execute transfer
   * 检查是否应该执行传输
   * @return 是否执行传输
   */
  virtual bool shouldTransfer();

protected:
  /// Control PostProcessor name
  /// 控制后处理器名称
  const PostprocessorName & _control_pp_name;
  
  /// Console output stream
  /// 控制台输出流
  ConsoleStream _console;
}; 