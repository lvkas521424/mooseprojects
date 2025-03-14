/****************************************************************/
/*        ConditionalMultiAppTransfer - Conditional Base        */
/*                                                              */
/*        PostProcessor-controlled MultiApp transfer base       */
/*  条件多应用传输 - 由后处理器控制的多应用程序传输基类         */
/****************************************************************/

#include "ConditionalMultiAppTransfer.h"
#include "TransferControlPostprocessor.h"

registerMooseObject("mooseprojectsApp", ConditionalMultiAppTransfer);

InputParameters
ConditionalMultiAppTransfer::validParams()
{
  InputParameters params = MultiAppTransfer::validParams();
  
  params.addClassDescription("Base class for conditional MultiApp transfers controlled by a PostProcessor");
  // 由后处理器控制的条件多应用传输基类
  
  // Add parameter for the control PostProcessor
  // 添加控制后处理器参数
  params.addRequiredParam<PostprocessorName>("control_pp", 
                         "PostProcessor that controls whether the transfer executes");
                         // 控制传输是否执行的后处理器
  
  return params;
}

ConditionalMultiAppTransfer::ConditionalMultiAppTransfer(const InputParameters & parameters) :
  MultiAppTransfer(parameters),
  PostprocessorInterface(this),
  _control_pp_name(getParam<PostprocessorName>("control_pp")),
  _console(_communicator.rank() == 0 ? Moose::out : Moose::err)
{
  // Initialize
  // 初始化
  if (_communicator.rank() == 0) {
    _console << "ConditionalMultiAppTransfer '" << name() << "' initialized" << std::endl;
    _console << "  Controlled by PostProcessor: " << _control_pp_name << std::endl;
  }
}

bool
ConditionalMultiAppTransfer::shouldTransfer()
{
  // Get the control PostProcessor value
  // 获取控制后处理器的值
  Real pp_value = getPostprocessorValue(_control_pp_name);
  
  // Transfer is active if control PostProcessor returns 1
  // 如果控制后处理器返回1，则传输活动
  bool active = (pp_value > 0.5);
  
  if (_communicator.rank() == 0) {
    _console << "Transfer '" << name() << "' status: " 
             << (active ? "ACTIVE" : "INACTIVE") << std::endl;
  }
  
  return active;
} 