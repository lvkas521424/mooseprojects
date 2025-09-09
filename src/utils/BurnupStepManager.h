/****************************************************************/
/* BurnupStepManager.h                                          */
/* Burnup step manager for cross-object communication           */
/*                                                              */
/* Provides static methods to access burnup step information   */
/* across different MOOSE objects without circular dependencies.*/
/*                                                              */
/* Created: Mar 19, 2025                                        */
/* Last Modified: Mar 19, 2025                                  */
/****************************************************************/

#pragma once

class BurnupStepManager
{
public:
    // 设置当前燃耗步
    static void setCurrentBurnStep(unsigned int step);
    
    // 获取当前燃耗步
    static unsigned int getCurrentBurnStep();
    
    // 设置最大燃耗步
    static void setMaxBurnSteps(unsigned int max_steps);
    
    // 获取最大燃耗步
    static unsigned int getMaxBurnSteps();

private:
    static unsigned int _current_burn_step;
    static unsigned int _max_burn_steps;
}; 