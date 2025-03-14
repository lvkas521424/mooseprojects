#include "ReactorTestApp.h"
#include "Moose.h"
#include "AppFactory.h"

registerApp(ReactorTestApp);

InputParameters
ReactorTestApp::validParams()
{
  InputParameters params = ReactorApp::validParams();
  return params;
}

ReactorTestApp::ReactorTestApp(InputParameters parameters) :
  ReactorApp(parameters)
{
  Moose::registerObjects(_factory);
  Moose::associateSyntax(_syntax, _action_factory);
}

ReactorTestApp::~ReactorTestApp() {} 