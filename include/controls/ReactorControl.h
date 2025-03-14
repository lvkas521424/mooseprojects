/// 是否预留燃料棒接口
const bool _reserve_fuel_rod_interface;
  
/// 是否预留水化学接口
const bool _reserve_water_chemistry_interface; 

/****************************************************************/
/*            ReactorControl - Reactor Physics Controller       */
/*                                                              */
/*          Controls Coupled Reactor Physics Calculations       */
/*        反应堆控制器 - 控制耦合的堆物理计算流程               */
/****************************************************************/

#pragma once

#include "Control.h"

/**
 * Controls the execution flow of reactor physics calculations
 * Supports different calculation modes and burnup steps
 * 控制堆物理计算的执行流程，支持不同计算模式和燃耗步骤
 */
class ReactorControl : public Control
{
public:
  static InputParameters validParams();
  
  ReactorControl(const InputParameters & parameters);
  
  virtual void execute() override;

protected:
  /**
   * Execute neutronics calculation only
   * 仅执行中子学计算
   */
  void executeNeutronics();
  
  /**
   * Execute coupled neutronics-thermal calculation
   * 执行耦合的中子学-热工计算
   */
  void executeCoupled();
  
  /**
   * Execute first burnup step (special handling)
   * 执行第一个燃耗步骤（特殊处理）
   */
  void executeFirstStep();
  
  /**
   * Execute subsequent burnup steps
   * 执行后续燃耗步骤
   */
  void executeSubsequentStep();
  
  /**
   * Set transfer activation status
   * @param pp_name PostProcessor name
   * @param active Whether to activate
   * 设置传输激活状态
   */
  void setTransferActive(const PostprocessorName & pp_name, bool active);
  
  /**
   * Reset all transfer states
   * 重置所有传输状态
   */
  void resetAllTransferControls();

private:
  /// Calculation type: 1=b1 only, 2=b2-b1 coupling
  /// 计算类型：1=仅b1，2=b2-b1耦合
  const int _calc_type;
  
  /// Total burnup steps
  /// 总燃耗步数
  const unsigned int _total_steps;
  
  /// Current burnup step
  /// 当前燃耗步
  unsigned int _current_step;
  
  /// Mesh dimensions
  /// 网格维度
  const std::vector<int> _mesh_dims;
  
  /// Temperature transfer control PostProcessor name
  /// 温度传输控制后处理器名称
  const PostprocessorName _temp_to_b1_control_pp_name;
  
  /// Power transfer control PostProcessor name
  /// 功率传输控制后处理器名称
  const PostprocessorName _power_to_b2_control_pp_name;
  
  /// Temperature convergence check PostProcessor name
  /// 温度收敛检查后处理器名称
  const PostprocessorName _temp_convergence_pp_name;
  
  /// Power transfer from neutronics to main app control PostProcessor name
  /// 从中子学到主应用程序的功率传输控制后处理器名称
  const PostprocessorName _power_to_main_control_pp_name;
  
  /// Maximum iterations
  /// 最大迭代次数
  const unsigned int _max_iterations;
  
  /// Convergence tolerance
  /// 收敛容差
  const Real _convergence_tol;
  
  /// Whether to use initial temperature field in first step
  /// 是否在第一步中使用初始温度场
  const bool _use_initial_temperature;
  
  /// Console output stream
  /// 控制台输出流
  ConsoleStream _console;
}; 