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
  
  params.addClassDescription("Reactor data transfer implementation with support for custom execution flags");
  
  params.addParam<std::string>("transfer_group", "", "Transfer group name, used to group execution in code");
  
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
    mooseInfo("ReactorTransfer: Registered transfer group '", _transfer_group, "'");
}

void
ReactorTransfer::execute()
{
  std::cout << "ReactorTransfer::execute()" << std::endl;
  MultiAppCopyTransfer::execute();
} 