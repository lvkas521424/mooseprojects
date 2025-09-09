/****************************************************************/
/* LevelSetTypes.h                                              */
/* Custom Execution Flags for Reactor Simulations               */
/*                                                              */
/* Declares execution flags for neutronics and thermal          */
/* calculations in multi-physics reactor simulations.           */
/*                                                              */
/* Created: Mar 19, 2025                                        */
/* Last Modified: Mar 19, 2025                                  */
/****************************************************************/
#pragma once

#include "Moose.h"

// 在头文件顶部添加外部函数声明
namespace LevelSet
{
    extern const ExecFlagType EXEC_NEUTRONIC;
    extern const ExecFlagType EXEC_PRENEUTRONIC;
    extern const ExecFlagType EXEC_CORNEUTRONIC;

    extern const ExecFlagType EXEC_THERMAL;
    extern const ExecFlagType EXEC_FROM_NEUTRONIC;
    extern const ExecFlagType EXEC_FROM_THERMAL;
    extern const ExecFlagType EXEC_TO_NEUTRONIC;
    extern const ExecFlagType EXEC_TO_THERMAL;
}