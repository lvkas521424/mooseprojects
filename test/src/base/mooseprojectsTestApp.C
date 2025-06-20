//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html
#include "mooseprojectsTestApp.h"
#include "mooseprojectsApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "MooseSyntax.h"

InputParameters
mooseprojectsTestApp::validParams()
{
  InputParameters params = mooseprojectsApp::validParams();
  params.set<bool>("use_legacy_material_output") = false;
  params.set<bool>("use_legacy_initial_residual_evaluation_behavior") = false;
  return params;
}

mooseprojectsTestApp::mooseprojectsTestApp(InputParameters parameters) : MooseApp(parameters)
{
  mooseprojectsTestApp::registerAll(
      _factory, _action_factory, _syntax, getParam<bool>("allow_test_objects"));
}

mooseprojectsTestApp::~mooseprojectsTestApp() {}

void
mooseprojectsTestApp::registerAll(Factory & f, ActionFactory & af, Syntax & s, bool use_test_objs)
{
  mooseprojectsApp::registerAll(f, af, s);
  if (use_test_objs)
  {
    Registry::registerObjectsTo(f, {"mooseprojectsTestApp"});
    Registry::registerActionsTo(af, {"mooseprojectsTestApp"});
  }
}

void
mooseprojectsTestApp::registerApps()
{
  registerApp(mooseprojectsApp);
  registerApp(mooseprojectsTestApp);
}

/***************************************************************************************************
 *********************** Dynamic Library Entry Points - DO NOT MODIFY ******************************
 **************************************************************************************************/
// External entry point for dynamic application loading
extern "C" void
mooseprojectsTestApp__registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  mooseprojectsTestApp::registerAll(f, af, s);
}
extern "C" void
mooseprojectsTestApp__registerApps()
{
  mooseprojectsTestApp::registerApps();
}
