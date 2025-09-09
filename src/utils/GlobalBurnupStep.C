/****************************************************************/
/* GlobalBurnupStep.C                                           */
/* Global variable implementation for burnup step access         */
/*                                                              */
/* Implements global variables for burnup step information      */
/* across different MOOSE objects.                              */
/*                                                              */
/* Created: Mar 19, 2025                                        */
/* Last Modified: Mar 19, 2025                                  */
/****************************************************************/

#include "GlobalBurnupStep.h"

// 全局变量定义
unsigned int g_current_burn_step = 1;
unsigned int g_max_burn_steps = 10; 