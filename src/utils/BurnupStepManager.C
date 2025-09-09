/****************************************************************/
/* BurnupStepManager.C                                          */
/* Burnup step manager implementation                           */
/*                                                              */
/* Implements static methods for burnup step management.        */
/*                                                              */
/* Created: Mar 19, 2025                                        */
/* Last Modified: Mar 19, 2025                                  */
/****************************************************************/

#include "BurnupStepManager.h"

// 静态变量定义
unsigned int BurnupStepManager::_current_burn_step = 1;
unsigned int BurnupStepManager::_max_burn_steps = 10;

void BurnupStepManager::setCurrentBurnStep(unsigned int step)
{
    _current_burn_step = step;
}

unsigned int BurnupStepManager::getCurrentBurnStep()
{
    return _current_burn_step;
}

void BurnupStepManager::setMaxBurnSteps(unsigned int max_steps)
{
    _max_burn_steps = max_steps;
}

unsigned int BurnupStepManager::getMaxBurnSteps()
{
    return _max_burn_steps;
} 