/****************************************************************/
/*               mooseprojectsApp - Reactor Physics App         */
/*                                                              */
/*                 Main Application Class                       */
/* mooseprojectsApp - 堆物理应用程序的主应用类，用于管理所有组件 */
/****************************************************************/

#include "mooseprojectsApp.h"
#include "Moose.h"
#include "AppFactory.h"

// PostProcessors
// 后处理器
#include "TransferControlPostprocessor.h"
#include "TemperatureConvergencePostprocessor.h"

// Transfers
// 传输类
#include "ConditionalMultiAppTransfer.h"
#include "ConditionalMultiAppCopyTransfer.h"
#include "ConditionalMultiAppInterpolationTransfer.h"

// MultiApps
// 多应用程序类
#include "NeutronicsMultiApp.h"
#include "ThermalMultiApp.h"

// Controls
// 控制器类
#include "ReactorControl.h"

registerMooseApp(mooseprojectsApp);

InputParameters
mooseprojectsApp::validParams()
{
  InputParameters params = MooseApp::validParams();
  
  params.addClassDescription("Reactor physics calculation application");
  // 堆物理计算应用程序
  
  // Add application-specific parameters
  // 添加应用程序特定参数
  params.addParam<bool>("enable_debug_output", false,
    "Enable debug output"); // 启用调试输出
  
  return params;
}

mooseprojectsApp::mooseprojectsApp(InputParameters parameters) : MooseApp(parameters)
{
  mooseprojectsApp::registerAll(
    _factory, _action_factory, _syntax);
    
  // Output application information
  // 输出应用程序信息
  if (parameters.get<bool>("enable_debug_output"))
  {
    _console << "\n=== Reactor Physics Application Initialized ===" << std::endl;
    _console << "All components registered" << std::endl;
    _console << "================================================\n" << std::endl;
  }
}

void
mooseprojectsApp::registerApps()
{
  registerApp(mooseprojectsApp);
}

void
mooseprojectsApp::registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  Registry::registerObjectsTo(f, {"mooseprojectsApp"});
  Registry::registerActionsTo(af, {"mooseprojectsApp"});
  
  mooseprojectsApp::registerAppsDepends();
  mooseprojectsApp::registerObjects(f);
  mooseprojectsApp::registerObjectDepends(f);
  mooseprojectsApp::associateSyntax(s, af);
}

void
mooseprojectsApp::registerObjects(Factory & factory)
{
  // Register all components
  // 注册所有组件
  registerPostprocessors(factory);
  registerTransfers(factory);
  registerMultiApps(factory);
  registerControls(factory);
}

void
mooseprojectsApp::registerObjectDepends(Factory & /*factory*/)
{
  // Register dependencies
  // 注册依赖项
}

void
mooseprojectsApp::registerAppsDepends()
{
  // Register application dependencies
  // 注册应用程序依赖项
}

void
mooseprojectsApp::associateSyntax(Syntax & /*syntax*/, ActionFactory & /*action_factory*/)
{
  // Associate syntax and actions
  // 关联语法和动作
}

void
mooseprojectsApp::associateSyntaxDepends(Syntax & /*syntax*/, ActionFactory & /*action_factory*/)
{
  // Associate syntax and action dependencies
  // 关联语法和动作依赖项
}

void
mooseprojectsApp::registerPostprocessors(Factory & factory)
{
  // Register PostProcessor objects
  // 注册后处理器对象
  registerMooseObject("mooseprojectsApp", TransferControlPostprocessor);
  registerMooseObject("mooseprojectsApp", TemperatureConvergencePostprocessor);
}

void
mooseprojectsApp::registerTransfers(Factory & factory)
{
  // Register Transfer objects
  // 注册传输对象
  registerMooseObject("mooseprojectsApp", ConditionalMultiAppTransfer);
  registerMooseObject("mooseprojectsApp", ConditionalMultiAppCopyTransfer);
  registerMooseObject("mooseprojectsApp", ConditionalMultiAppInterpolationTransfer);
}

void
mooseprojectsApp::registerMultiApps(Factory & factory)
{
  // Register MultiApp objects
  // 注册多应用程序对象
  registerMooseObject("mooseprojectsApp", NeutronicsMultiApp);
  registerMooseObject("mooseprojectsApp", ThermalMultiApp);
}

void
mooseprojectsApp::registerControls(Factory & factory)
{
  // Register Control objects
  // 注册控制对象
  registerMooseObject("mooseprojectsApp", ReactorControl);
} 