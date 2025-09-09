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

  // 获取执行标志枚举
  ExecFlagEnum &exec = params.set<ExecFlagEnum>("execute_on");

  // 保留原有标志，添加新标志
  exec.addAvailableFlags(LevelSet::EXEC_FROM_NEUTRONIC);
  exec.addAvailableFlags(LevelSet::EXEC_FROM_THERMAL);

  // 设置默认执行点（包含标准执行点和新添加的执行点）
  // exec = {LevelSet::EXEC_INITIAL, LevelSet::EXEC_TIMESTEP_BEGIN, LevelSet::EXEC_FROM_NEUTRONIC, LevelSet::EXEC_FROM_THERMAL};

  params.addClassDescription("反应堆耦合数据传输");

  return params;
}

ReactorTransfer::ReactorTransfer(const InputParameters &parameters) : MultiAppCopyTransfer(parameters)
{
}

void ReactorTransfer::execute()
{
  std::cout << "ReactorTransfer开始执行..." << std::endl;
  MultiAppCopyTransfer::execute();
  std::cout << "ReactorTransfer执行完成" << std::endl;
}