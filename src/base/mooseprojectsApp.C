#include "mooseprojectsApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"
#include "MooseSyntax.h"

InputParameters
mooseprojectsApp::validParams()
{
  InputParameters params = MooseApp::validParams();
  params.set<bool>("use_legacy_material_output") = false;
  params.set<bool>("use_legacy_initial_residual_evaluation_behavior") = false;
  return params;
}

mooseprojectsApp::mooseprojectsApp(InputParameters parameters) : MooseApp(parameters)
{
  mooseprojectsApp::registerAll(_factory, _action_factory, _syntax);
}

mooseprojectsApp::~mooseprojectsApp() {}

void
mooseprojectsApp::registerAll(Factory & f, ActionFactory & af, Syntax & syntax)
{
  ModulesApp::registerAllObjects<mooseprojectsApp>(f, af, syntax);
  Registry::registerObjectsTo(f, {"mooseprojectsApp"});
  Registry::registerActionsTo(af, {"mooseprojectsApp"});

  /* register custom execute flags, action syntax, etc. here */
}

void
mooseprojectsApp::registerApps()
{
  registerApp(mooseprojectsApp);
}

/***************************************************************************************************
 *********************** Dynamic Library Entry Points - DO NOT MODIFY ******************************
 **************************************************************************************************/
extern "C" void
mooseprojectsApp__registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  mooseprojectsApp::registerAll(f, af, s);
}
extern "C" void
mooseprojectsApp__registerApps()
{
  mooseprojectsApp::registerApps();
}
