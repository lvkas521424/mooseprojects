#include "IterativeSteadyExecutioner.h"
#include "FEProblemBase.h"
#include "MooseApp.h"

registerMooseObject("multiapp2App", IterativeSteadyExecutioner);

InputParameters
IterativeSteadyExecutioner::validParams()
{
  InputParameters params = Steady::validParams();
  params.addClassDescription("执行多次稳态求解，可配置迭代次数和计算类型");
  
  params.addParam<unsigned int>("max_iterations", 1, "最大迭代次数");
  params.addParam<int>("calc_type", 1, "计算类型(记录用，不影响MultiApp存在)");
  
  return params;
}

IterativeSteadyExecutioner::IterativeSteadyExecutioner(const InputParameters & parameters)
  : Steady(parameters),
    _max_iterations(getParam<unsigned int>("max_iterations")),
    _calc_type(getParam<int>("calc_type")),
    _current_iteration(0)
{
  // 输出配置信息
  mooseInfo("迭代稳态执行器：最大迭代次数 = ", _max_iterations);
  mooseInfo("计算类型 = ", _calc_type, "（此值仅作记录，MultiApp始终存在）");
}

void
IterativeSteadyExecutioner::execute()
{
  mooseInfo("开始执行迭代稳态求解，共", _max_iterations, "次迭代");
  
  bool all_success = true;
  
  for (_current_iteration = 0; _current_iteration < _max_iterations; ++_current_iteration)
  {
    mooseInfo("执行迭代 ", _current_iteration + 1, "/", _max_iterations);
    
    try
    {
      // 执行一次稳态求解
      Steady::execute();
      mooseInfo("迭代 ", _current_iteration + 1, " 完成");
    }
    catch (const std::exception & e)
    {
      // 捕获可能的异常
      mooseError("迭代 ", _current_iteration + 1, " 求解失败: ", e.what());
      all_success = false;
      break;
    }
    catch (...)
    {
      // 捕获任何其他类型的异常
      mooseError("迭代 ", _current_iteration + 1, " 求解失败，发生未知错误");
      all_success = false;
      break;
    }
  }
  
  if (all_success)
    mooseInfo("所有", _max_iterations, "次迭代均已成功完成。");
  else
    mooseInfo("迭代在第", _current_iteration + 1, "次时失败。");
} 