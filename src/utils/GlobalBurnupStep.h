/****************************************************************/
/* GlobalBurnupStep.h                                           */
/* Global variable for burnup step access                       */
/*                                                              */
/* Provides global access to burnup step information            */
/* across different MOOSE objects.                              */
/*                                                              */
/* Created: Mar 19, 2025                                        */
/* Last Modified: Mar 19, 2025                                  */
/****************************************************************/

#pragma once

// 全局变量声明
extern unsigned int g_current_burn_step;
extern unsigned int g_max_burn_steps; 