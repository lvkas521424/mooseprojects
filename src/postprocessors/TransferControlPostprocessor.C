/****************************************************************/
/*     TransferControlPostprocessor - Transfer Controller       */
/*                                                              */
/*            Controls Data Transfer Enable/Disable             */
/*      传输控制后处理器 - 控制数据传输的启用/禁用状态          */
/****************************************************************/

#include "TransferControlPostprocessor.h"

registerMooseObject("mooseprojectsApp", TransferControlPostprocessor);

InputParameters
TransferControlPostprocessor::validParams()
{
  InputParameters params = GeneralPostprocessor::validParams();
  
  params.addClassDescription("PostProcessor that controls whether data transfer should be executed");
  // 控制是否执行数据传输的后处理器
  
  // Add parameter for initial transfer mode
  // 添加初始传输模式参数
  params.addParam<bool>("initially_active", false, 
                        "Whether transfer should be initially active");
                        // 传输是否应该初始状态为激活
  
  // Add verbose parameter
  // 添加详细输出参数
  params.addParam<bool>("verbose", true, 
                        "Whether to output transfer status messages");
                        // 是否输出传输状态消息
  
  return params;
}

TransferControlPostprocessor::TransferControlPostprocessor(const InputParameters & parameters) :
  GeneralPostprocessor(parameters),
  _transfer_mode(parameters.get<bool>("initially_active") ? 1 : 0),
  _console(_communicator.rank() == 0 && parameters.get<bool>("verbose") 
           ? Moose::out : Moose::err)
{
  // Initial state message
  // 初始状态消息
  if (_communicator.rank() == 0 && parameters.get<bool>("verbose")) {
    _console << "Transfer control '" << name() << "' initialized as "
             << (_transfer_mode ? "ACTIVE" : "INACTIVE") << std::endl;
  }
}

void
TransferControlPostprocessor::initialize()
{
  // Nothing to do here
  // 此处无需操作
}

void
TransferControlPostprocessor::execute()
{
  // Nothing to do here
  // 此处无需操作
}

Real
TransferControlPostprocessor::getValue()
{
  // Return the current transfer mode (0=inactive, 1=active)
  // 返回当前传输模式 (0=非活动, 1=活动)
  return static_cast<Real>(_transfer_mode);
}

void
TransferControlPostprocessor::setTransferMode(int mode)
{
  // Set new transfer mode
  // 设置新的传输模式
  bool changed = (_transfer_mode != mode);
  _transfer_mode = mode;
  
  // Output status change
  // 输出状态变化
  if (changed && _communicator.rank() == 0 && isParamValid("verbose") && getParam<bool>("verbose")) {
    _console << "Transfer control '" << name() << "' set to "
             << (_transfer_mode ? "ACTIVE" : "INACTIVE") << std::endl;
  }
} 