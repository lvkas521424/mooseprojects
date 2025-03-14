/****************************************************************/
/*            ReactorControl - Reactor Physics Controller       */
/*                                                              */
/*          Controls Coupled Reactor Physics Calculations       */
/*        反应堆控制器 - 控制耦合的堆物理计算流程               */
/****************************************************************/

#include "ReactorControl.h"
#include "TransferControlPostprocessor.h"
#include "TemperatureConvergencePostprocessor.h"

registerMooseObject("mooseprojectsApp", ReactorControl);

// Fortran interface declarations
// Fortran接口声明
extern "C" {
  // External interface declarations (simplified)
  // 外部接口声明(简化版)
  void a1_preprocess(int* comm, int* step_number);
  void c1_postprocess(int* comm);
}

InputParameters
ReactorControl::validParams()
{
  InputParameters params = Control::validParams();
  
  // Add class description
  // 添加类描述
  params.addClassDescription("Controller for reactor physics calculation flow with burnup cycling support");
  // 支持燃耗循环的堆物理计算流程控制器
  
  // Add parameters
  // 添加参数
  params.addParam<int>("calc_type", 1, 
                       "Calculation type: 1=neutronics only(b1), 2=neutronics-thermal coupling(b2-b1)");
                     // 计算类型：1=仅中子学(b1)，2=中子学-热工耦合(b2-b1)
  params.addParam<unsigned int>("total_steps", 1,
                               "Total number of burnup steps to execute");
                               // 要执行的燃耗步数总数
  params.addParam<std::vector<int>>("mesh_dims", {10,10,10}, 
                                    "Mesh dimensions (nx,ny,nz)");
                                    // 网格维度(nx,ny,nz)
  
  params.addParam<PostprocessorName>("temp_to_b1_control_pp", "temp_to_b1_control", 
                                     "PostProcessor controlling temperature field transfer");
                                     // 控制温度场传输的后处理器
  params.addParam<PostprocessorName>("power_to_b2_control_pp", "power_to_b2_control", 
                                     "PostProcessor controlling power distribution transfer");
                                     // 控制功率分布传输的后处理器
  params.addParam<PostprocessorName>("temp_convergence_pp", "temperature_convergence", 
                                     "Temperature convergence PostProcessor");
                                     // 温度收敛后处理器
  params.addParam<PostprocessorName>("power_to_main_control_pp", "power_to_main_control", 
                                     "PostProcessor controlling power distribution transfer from neutronics to main app");
                                     // 控制从中子学到主应用程序的功率分布传输的后处理器
  params.addParam<bool>("use_initial_temperature", false,
                       "Whether to use initial temperature field in first step");
                       // 是否在第一步中使用初始温度场
  
  params.addParam<unsigned int>("max_iterations", 20, 
                               "Maximum coupling iterations");
                               // 最大耦合迭代次数
  params.addParam<Real>("convergence_tol", 1e-4, 
                       "Temperature field convergence criterion");
                       // 温度场收敛标准
  
  return params;
}

ReactorControl::ReactorControl(const InputParameters & parameters) :
  Control(parameters),
  _calc_type(getParam<int>("calc_type")),
  _total_steps(getParam<unsigned int>("total_steps")),
  _current_step(0),
  _mesh_dims(getParam<std::vector<int>>("mesh_dims")),
  _temp_to_b1_control_pp_name(getParam<PostprocessorName>("temp_to_b1_control_pp")),
  _power_to_b2_control_pp_name(getParam<PostprocessorName>("power_to_b2_control_pp")),
  _temp_convergence_pp_name(getParam<PostprocessorName>("temp_convergence_pp")),
  _power_to_main_control_pp_name(getParam<PostprocessorName>("power_to_main_control_pp")),
  _max_iterations(getParam<unsigned int>("max_iterations")),
  _convergence_tol(getParam<Real>("convergence_tol")),
  _use_initial_temperature(getParam<bool>("use_initial_temperature")),
  _console(_communicator.rank() == 0 ? Moose::out : Moose::err)
{
  if (_communicator.rank() == 0) {
    _console << "ReactorControl initialized with:" << std::endl;
    _console << "  Calculation type: " << (_calc_type == 1 ? "Neutronics only (b1)" : "Coupled neutronics-thermal (b2-b1)") << std::endl;
    _console << "  Total burnup steps: " << _total_steps << std::endl;
    _console << "  Use initial temperature in first step: " << (_use_initial_temperature ? "Yes" : "No") << std::endl;
  }
}

void
ReactorControl::execute()
{
  // Reset all transfer control states
  // 重置所有传输控制状态
  resetAllTransferControls();
  
  _console << "\n=============================================================" << std::endl;
  _console << "Executing burnup step " << _current_step + 1 << " of " << _total_steps << std::endl; 
  // 执行燃耗步骤 x/y
  _console << "=============================================================" << std::endl;
  
  // Execute preprocessing module for current step
  // 为当前步骤执行预处理模块
  int step = _current_step;
  a1_preprocess(&_communicator.get(), &step);
  
  // Special handling for first burnup step vs subsequent steps
  // 对第一个燃耗步骤与后续步骤进行特殊处理
  if (_current_step == 0) {
    executeFirstStep();
  } else {
    executeSubsequentStep();
  }
  
  // Execute postprocessing
  // 执行后处理
  c1_postprocess(&_communicator.get());
  
  // Increment current step
  // 增加当前步骤
  _current_step++;
  
  _console << "Burnup step completed" << std::endl; // 燃耗步骤完成
  _console << "=============================================================" << std::endl;
}

void
ReactorControl::executeFirstStep()
{
  _console << "Executing first burnup step with special handling" << std::endl;
  // 使用特殊处理执行第一个燃耗步骤
  
  // Based on calculation type
  // 基于计算类型
  if (_calc_type == 1) {
    // Neutronics only mode - first step
    // 仅中子学模式 - 第一步
    executeNeutronics();
  }
  else if (_calc_type == 2) {
    // Coupled mode - first step (no previous temperature data)
    // 耦合模式 - 第一步（没有之前的温度数据）
    
    // In first step, we might start with default temperature 
    // or transfer initial temperature if available and enabled
    // 在第一步中，我们可能从默认温度开始
    // 或者如果可用且启用，则传输初始温度
    if (_use_initial_temperature) {
      _console << "  Using initial temperature field for first step" << std::endl;
      // 对第一步使用初始温度场
      setTransferActive(_temp_to_b1_control_pp_name, true);
      _problem.execTransfers("to_neutronics", MultiAppTransfer::TO_MULTIAPP);
    }
    
    executeCoupled();
  }
}

void
ReactorControl::executeSubsequentStep()
{
  _console << "Executing subsequent burnup step" << std::endl;
  // 执行后续燃耗步骤
  
  // Based on calculation type
  // 基于计算类型
  if (_calc_type == 1) {
    // Neutronics only mode - subsequent step
    // 仅中子学模式 - 后续步骤
    executeNeutronics();
  }
  else if (_calc_type == 2) {
    // Coupled mode - subsequent step (may have previous temperature data)
    // 耦合模式 - 后续步骤（可能有之前的温度数据）
    executeCoupled();
  }
}

void
ReactorControl::executeNeutronics()
{
  _console << "Executing neutronics (b1) calculation..." << std::endl; // 执行中子学(b1)计算...
  
  // Execute neutronics calculation
  // 执行中子学计算
  _problem.execMultiApp("neutronics");
  
  // Transfer power from neutronics to main app for output
  // 将功率从中子学传输到主应用程序用于输出
  _console << "Transferring power distribution from neutronics to main app..." << std::endl;
  // 将功率分布从中子学传输到主应用程序...
  setTransferActive(_power_to_main_control_pp_name, true);
  _problem.execTransfers("from_neutronics", MultiAppTransfer::FROM_MULTIAPP);
  
  _console << "Neutronics calculation completed" << std::endl; // 中子学计算完成
}

void
ReactorControl::executeCoupled()
{
  _console << "Executing coupled neutronics-thermal (b2-b1) calculation..." << std::endl; // 执行耦合的中子学-热工(b2-b1)计算...
  
  // Initialize iteration counter
  // 初始化迭代计数器
  unsigned int iter = 0;
  bool converged = false;
  
  while (iter < _max_iterations && !converged) {
    _console << "  Coupling iteration " << iter + 1 << " of " << _max_iterations << std::endl; // 耦合迭代 x/y
    
    // Reset temperature convergence for new iteration
    // 为新迭代重置温度收敛
    auto* temp_conv_pp = &_problem.getUserObject<TemperatureConvergencePostprocessor>(_temp_convergence_pp_name);
    temp_conv_pp->reset();
    
    // Step 1: Execute thermal calculation (b2)
    // 步骤1：执行热工计算(b2)
    _console << "    Executing thermal calculation (b2)..." << std::endl; // 执行热工计算(b2)...
    
    // For the first iteration, we need power distribution 
    // 对于第一次迭代，我们需要功率分布
    if (iter == 0) {
      // Execute neutronics first to get initial power distribution
      // 首先执行中子学以获取初始功率分布
      _console << "    Executing initial neutronics calculation (b1) for power..." << std::endl; // 执行初始中子学计算(b1)以获取功率...
      _problem.execMultiApp("neutronics");
      
      // Transfer power to thermal calculation
      // 将功率传输到热工计算
      setTransferActive(_power_to_b2_control_pp_name, true);
      _problem.execTransfers("to_thermal", MultiAppTransfer::TO_MULTIAPP);
      
      // Also transfer power to main app for output
      // 同时将功率传输到主应用程序用于输出
      setTransferActive(_power_to_main_control_pp_name, true);
      _problem.execTransfers("from_neutronics", MultiAppTransfer::FROM_MULTIAPP);
    }
    
    // Execute thermal calculation
    // 执行热工计算
    _problem.execMultiApp("thermal");
    
    // Step 2: Transfer temperature field to neutronics
    // 步骤2：将温度场传输到中子学
    setTransferActive(_temp_to_b1_control_pp_name, true);
    _problem.execTransfers("to_neutronics", MultiAppTransfer::TO_MULTIAPP);
    
    // Step 3: Execute neutronics calculation (b1)
    // 步骤3：执行中子学计算(b1)
    _console << "    Executing neutronics calculation (b1)..." << std::endl; // 执行中子学计算(b1)...
    _problem.execMultiApp("neutronics");
    
    // Step 4: Transfer power distribution to thermal calculation
    // 步骤4：将功率分布传输到热工计算
    setTransferActive(_power_to_b2_control_pp_name, true);
    _problem.execTransfers("to_thermal", MultiAppTransfer::TO_MULTIAPP);
    
    // Also transfer power to main app for output
    // 同时将功率传输到主应用程序用于输出
    setTransferActive(_power_to_main_control_pp_name, true);
    _problem.execTransfers("from_neutronics", MultiAppTransfer::FROM_MULTIAPP);
    
    // Check convergence
    // 检查收敛性
    Real temp_change = temp_conv_pp->getValue();
    _console << "    Temperature field relative change: " << temp_change << std::endl; // 温度场相对变化：x
    
    if (temp_change < _convergence_tol) {
      converged = true;
      _console << "  Coupling converged after " << iter + 1 << " iterations" << std::endl; // 耦合在x次迭代后收敛
    }
    
    // Increment iteration counter
    // 增加迭代计数器
    iter++;
  }
  
  if (!converged) {
    _console << "  Warning: Coupling did not converge within " << _max_iterations << " iterations" << std::endl;
    // 警告：耦合在x次迭代内未收敛
  }
}

void
ReactorControl::setTransferActive(const PostprocessorName & pp_name, bool active)
{
  // Set transfer control PostProcessor mode
  // 设置传输控制后处理器模式
  auto* control_pp = &_problem.getUserObject<TransferControlPostprocessor>(pp_name);
  control_pp->setTransferMode(active ? 1 : 0);
}

void
ReactorControl::resetAllTransferControls()
{
  // Reset all transfer controls to inactive state
  // 将所有传输控制重置为非激活状态
  setTransferActive(_temp_to_b1_control_pp_name, false);
  setTransferActive(_power_to_b2_control_pp_name, false);
  setTransferActive(_power_to_main_control_pp_name, false);
  
  // Reset temperature convergence
  // 重置温度收敛
  auto* temp_conv_pp = &_problem.getUserObject<TemperatureConvergencePostprocessor>(_temp_convergence_pp_name);
  temp_conv_pp->reset();
} 
} 