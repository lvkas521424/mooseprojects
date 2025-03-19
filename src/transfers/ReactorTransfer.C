/****************************************************************/
/* ReactorTransfer.C                                            */
/* Reactor Data Transfer Implementation                         */
/*                                                              */
/* Implements data transfer between neutronics and thermal      */
/* applications with support for custom execution flags.        */
/*                                                              */
/* Created: Mar 19, 2025                                        */
/* Last Modified: Mar 19, 2025                                  */
/****************************************************************/

#include "ReactorTransfer.h"
#include "FEProblem.h"
#include "LevelSetTypes.h"

registerMooseObject("mooseprojectsApp", ReactorTransfer);

InputParameters
ReactorTransfer::validParams()
{
  InputParameters params = MultiAppCopyTransfer::validParams();
  
  params.addClassDescription("支持自定义执行标志的反应堆数据传输");
  
  params.addParam<std::string>("transfer_group", "", "传输组名称，用于在代码中分组执行传输");
  
  // 添加自定义执行标志
  ExecFlagEnum & exec_enum = params.set<ExecFlagEnum>("execute_on");
  exec_enum.addAvailableFlags(LevelSet::EXEC_FROM_NEUTRONIC);
  exec_enum.addAvailableFlags(LevelSet::EXEC_FROM_THERMAL);
  exec_enum.addAvailableFlags(LevelSet::EXEC_TO_NEUTRONIC);
  exec_enum.addAvailableFlags(LevelSet::EXEC_TO_THERMAL);
  
  return params;
}

ReactorTransfer::ReactorTransfer(const InputParameters & parameters)
  : MultiAppCopyTransfer(parameters),
    _transfer_group(getParam<std::string>("transfer_group"))
{
}

void
ReactorTransfer::initialSetup()
{
  MultiAppCopyTransfer::initialSetup();
  
  // 可以在这里添加额外的初始化逻辑
  
  // 输出注册信息
  if (!_transfer_group.empty())
    mooseInfo("ReactorTransfer: 已注册传输组 '", _transfer_group, "'");
}

void
ReactorTransfer::execute()
{
  // 调用父类的execute方法执行实际的数据传输
  MultiAppCopyTransfer::execute();
} 