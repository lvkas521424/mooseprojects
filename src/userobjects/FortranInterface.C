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
  int burn_step_copy = static_cast<int>(_burn_step);
  int max_steps_copy = static_cast<int>(_max_burn_steps);
  update_burnup_step(burn_step_copy, max_steps_copy);
}

