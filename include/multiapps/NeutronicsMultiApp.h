/****************************************************************/
/*           NeutronicsMultiApp - Neutronics Application        */
/*                                                              */
/*              Manages neutronics calculations                 */
/* 中子学多应用程序 - 管理中子物理计算并与外部Fortran代码交互    */
/****************************************************************/

#pragma once

#include "FullSolveMultiApp.h"

/**
 * Manages neutronics calculations in multi-application framework
 * Uses external Fortran codes for initialization and calculation
 * 管理中子学计算的多应用程序框架，使用外部Fortran代码进行初始化和计算
 */
class NeutronicsMultiApp : public FullSolveMultiApp
{
public:
  static InputParameters validParams();
  
  NeutronicsMultiApp(const InputParameters & parameters);
  
  /**
   * Execute neutronics calculation
   * Calls external Fortran solver
   * 执行中子学计算，调用外部Fortran求解器
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
   * Get temperature field data from MOOSE system
   * @return Temperature field as vector
   * 从MOOSE系统获取温度场数据，返回温度场向量
   */
  std::vector<Real> getTemperatureField() const;
  
  /**
   * Set power distribution data in MOOSE system
   * @param power_data Power distribution data
   * 将功率分布数据设置到MOOSE系统中
   */
  void setPowerDistribution(const std::vector<Real> & power_data);
}; 