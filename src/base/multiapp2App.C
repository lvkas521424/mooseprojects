#include "multiapp2App.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"
#include "MooseSyntax.h"

InputParameters
multiapp2App::validParams()
{
  InputParameters params = MooseApp::validParams();
  params.set<bool>("use_legacy_material_output") = false;
  params.set<bool>("use_legacy_initial_residual_evaluation_behavior") = false;
  return params;
}

multiapp2App::multiapp2App(InputParameters parameters) : MooseApp(parameters)
{
  multiapp2App::registerAll(_factory, _action_factory, _syntax);
}

multiapp2App::~multiapp2App() {}

void
multiapp2App::registerAll(Factory & f, ActionFactory & af, Syntax & syntax)
{
  ModulesApp::registerAllObjects<multiapp2App>(f, af, syntax);
  Registry::registerObjectsTo(f, {"multiapp2App"});
  Registry::registerActionsTo(af, {"multiapp2App"});

  /* register custom execute flags, action syntax, etc. here */
}

void
multiapp2App::registerApps()
{
  registerApp(multiapp2App);
}

/***************************************************************************************************
 *********************** Dynamic Library Entry Points - DO NOT MODIFY ******************************
 **************************************************************************************************/
extern "C" void
multiapp2App__registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  multiapp2App::registerAll(f, af, s);
}
extern "C" void
multiapp2App__registerApps()
{
  multiapp2App::registerApps();
}
