/****************************************************************/
/*            ThermalMultiApp - Thermal Application             */
/*                                                              */
/*              Manages thermal calculations                    */
/*      热工多应用程序 - 管理热工计算并与外部Fortran代码交互     */
/****************************************************************/

#pragma once

#include "FullSolveMultiApp.h"

/**
 * Manages thermal calculations in multi-application framework
 * Uses external Fortran code for calculation
 * 管理热工计算的多应用程序框架，使用外部Fortran代码进行计算
 */
class ThermalMultiApp : public FullSolveMultiApp
{
public:
  static InputParameters validParams();
  
  ThermalMultiApp(const InputParameters & parameters);
  
  /**
   * Execute thermal calculation
   * Calls external Fortran solver
   * 执行热工计算，调用外部Fortran求解器
   */
  virtual bool solveStep(Real dt, Real target_time, bool auto_advance = true) override;
  
protected:
  /// Mesh dimensions
  /// 网格维度
  const std::vector<int> _mesh_dims;
  
  /// Console output stream
  /// 控制台输出流
  ConsoleStream _console;
  
  /**
   * Get power distribution data from MOOSE system
   * @return Power distribution as vector
   * 从MOOSE系统获取功率分布数据，返回功率分布向量
   */
  std::vector<Real> getPowerDistribution() const;
  
  /**
   * Set temperature field data in MOOSE system
   * @param temperature_data Temperature field data
   * 将温度场数据设置到MOOSE系统中
   */
  void setTemperatureField(const std::vector<Real> & temperature_data);
}; 