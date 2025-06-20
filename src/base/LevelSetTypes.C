/****************************************************************/
/* LevelSetTypes.C                                              */
/* Custom Execution Flags for Reactor Simulations               */
/*                                                              */
/* Defines execution flags for neutronics and thermal           */
/* calculations in multi-physics reactor simulations.           */
/*                                                              */
/* Created: July 15, 2024                                       */
/* Last Modified: July 15, 2024                                 */
/****************************************************************/
// Level set includes
#include "LevelSetTypes.h"

// MOOSE includes
#include "ExecFlagRegistry.h"

// neutronics execute flags
const ExecFlagType LevelSet::EXEC_NEUTRONIC    = registerExecFlag("NEUTRONIC");
const ExecFlagType LevelSet::EXEC_PRENEUTRONIC = registerExecFlag("PRENEUTRONIC");
const ExecFlagType LevelSet::EXEC_CORNEUTRONIC = registerExecFlag("CORNEUTRONIC");

// thermal execute flags
const ExecFlagType LevelSet::EXEC_THERMAL        = registerExecFlag("THERMAL");

// transfer execute flags
const ExecFlagType LevelSet::EXEC_FROM_NEUTRONIC = registerExecFlag("FROM_NEUTRONIC");
const ExecFlagType LevelSet::EXEC_FROM_THERMAL   = registerExecFlag("FROM_THERMAL");
const ExecFlagType LevelSet::EXEC_TO_NEUTRONIC   = registerExecFlag("TO_NEUTRONIC");
const ExecFlagType LevelSet::EXEC_TO_THERMAL     = registerExecFlag("TO_THERMAL");
