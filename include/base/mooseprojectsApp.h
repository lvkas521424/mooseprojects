/****************************************************************/
/* mooseprojectsApp.h                                           */
/* MOOSE Application Interface                                  */
/*                                                              */
/* Defines the main application class for reactor simulation    */
/* with registration of all components.                         */
/*                                                              */
/* Created: Mar 19, 2025                                        */
/* Last Modified: Mar 19, 2025                                  */
/****************************************************************/

#pragma once

#include "MooseApp.h"
#include "mooseprojectsApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"
#include "MooseSyntax.h"

class mooseprojectsApp : public MooseApp
{
public:
  static InputParameters validParams();

  mooseprojectsApp(InputParameters parameters);
  virtual ~mooseprojectsApp();

  static void registerApps();
  static void registerAll(Factory &f, ActionFactory &af, Syntax &s);
};
