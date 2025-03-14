#pragma once

#include "ReactorApp.h"

class ReactorTestApp : public ReactorApp
{
public:
  static InputParameters validParams();
  ReactorTestApp(InputParameters parameters);
  virtual ~ReactorTestApp();
}; 