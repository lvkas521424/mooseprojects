/****************************************************************/
/*               mooseprojectsApp - Reactor Physics App         */
/*                                                              */
/*                 Main Application Class                       */
/* mooseprojectsApp - 堆物理应用程序的主应用类，用于管理所有组件 */
/****************************************************************/

#pragma once

#include "MooseApp.h"

class mooseprojectsApp : public MooseApp
{
public:
  static InputParameters validParams();
  
  mooseprojectsApp(InputParameters parameters);
  
  static void registerApps();
  static void registerAll(Factory & f, ActionFactory & af, Syntax & s);
  
protected:
  virtual void registerObjects(Factory & factory) override;
  virtual void registerObjectDepends(Factory & factory);
  
  virtual void registerAppsDepends();
  
  virtual void associateSyntax(Syntax & syntax, ActionFactory & action_factory);
  virtual void associateSyntaxDepends(Syntax & syntax, ActionFactory & action_factory);
  
  /**
   * Register all PostProcessor objects
   * @param factory Object factory
   * 注册所有后处理器对象
   */
  void registerPostprocessors(Factory & factory);
  
  /**
   * Register all Transfer objects
   * @param factory Object factory
   * 注册所有传输对象
   */
  void registerTransfers(Factory & factory);
  
  /**
   * Register all MultiApp objects
   * @param factory Object factory
   * 注册所有多应用程序对象
   */
  void registerMultiApps(Factory & factory);
  
  /**
   * Register all Control objects
   * @param factory Object factory
   * 注册所有控制对象
   */
  void registerControls(Factory & factory);
}; 