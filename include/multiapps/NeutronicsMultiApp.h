/****************************************************************/
/* NeutronicsMultiApp.h                                         */
/* Neutronics Multi-Application Interface                       */
/*                                                              */
/* Defines interface for neutron transport and diffusion        */
/* calculations in reactor physics simulations.                 */
/*                                                              */
/* Created: Mar 19, 2025                                        */
/* Last Modified: Mar 19, 2025                                  */
/****************************************************************/

#pragma once

#include "FullSolveMultiApp.h"

// 声明 Fortran 模块中的 b1_execute 函数，注意参数类型匹配
extern "C" {
  void b1_execute(int* mesh_dims, double* power_field, double* temperature_field, int field_size);
}

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
   * 执行中子学计算
   */
  void executeB1();
  
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
  
  // 功率场变量名
  const std::string _power_var_name;
  
  // 温度场变量名
  const std::string _temperature_var_name;
  
  // 执行 b1_execute 计算
  void executeB1Solver();
}; 