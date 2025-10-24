/****************************************************************/
/* FortranInterface.C                                           */
/* Fortran程序接口模块实现                                       */
/*                                                              */
/* 负责与Fortran程序进行数据交互，包括燃耗步信息传递              */
/*                                                              */
/* Created: Oct 13, 2025                                        */
/****************************************************************/

#include "FortranInterface.h"
#include <iostream>

FortranInterface::FortranInterface(unsigned int & burn_step, unsigned int max_burn_steps)
  : _burn_step(burn_step), _max_burn_steps(max_burn_steps)
{
}

void
FortranInterface::updateBurnupStep()
{
  std::cout << "FortranInterface: 传递燃耗步信息到Fortran程序" << std::endl;
  std::cout << "  当前燃耗步: " << _burn_step << std::endl;
  std::cout << "  最大燃耗步: " << _max_burn_steps << std::endl;

  // 创建本地副本用于传递给Fortran
  int burn_step_copy = static_cast<int>(_burn_step);
  int max_steps_copy = static_cast<int>(_max_burn_steps);

  // 调用Fortran接口
  update_burnup_step(burn_step_copy, max_steps_copy);

  std::cout << "FortranInterface: Fortran程序燃耗步信息更新成功" << std::endl;
}

