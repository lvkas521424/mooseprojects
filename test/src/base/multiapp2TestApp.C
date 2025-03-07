//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html
#include "multiapp2TestApp.h"
#include "multiapp2App.h"
#include "Moose.h"
#include "AppFactory.h"
#include "MooseSyntax.h"

InputParameters
multiapp2TestApp::validParams()
{
  InputParameters params = multiapp2App::validParams();
  params.set<bool>("use_legacy_material_output") = false;
  params.set<bool>("use_legacy_initial_residual_evaluation_behavior") = false;
  return params;
}

multiapp2TestApp::multiapp2TestApp(InputParameters parameters) : MooseApp(parameters)
{
  multiapp2TestApp::registerAll(
      _factory, _action_factory, _syntax, getParam<bool>("allow_test_objects"));
}

multiapp2TestApp::~multiapp2TestApp() {}

void
multiapp2TestApp::registerAll(Factory & f, ActionFactory & af, Syntax & s, bool use_test_objs)
{
  multiapp2App::registerAll(f, af, s);
  if (use_test_objs)
  {
    Registry::registerObjectsTo(f, {"multiapp2TestApp"});
    Registry::registerActionsTo(af, {"multiapp2TestApp"});
  }
}

void
multiapp2TestApp::registerApps()
{
  registerApp(multiapp2App);
  registerApp(multiapp2TestApp);
}

/***************************************************************************************************
 *********************** Dynamic Library Entry Points - DO NOT MODIFY ******************************
 **************************************************************************************************/
// External entry point for dynamic application loading
extern "C" void
multiapp2TestApp__registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  multiapp2TestApp::registerAll(f, af, s);
}
extern "C" void
multiapp2TestApp__registerApps()
{
  multiapp2TestApp::registerApps();
}
