/****************************************************************/
/* ThermalMultiApp.C                                            */
/* Thermal-Hydraulics Multi-Application Implementation          */
/*                                                              */
/* Implements heat transfer and fluid flow calculations         */
/* for thermal-hydraulic analysis of nuclear reactors.          */
/*                                                              */
/* Created: Mar 19, 2025                                        */
/* Last Modified: Mar 19, 2025                                  */
/****************************************************************/

#pragma once

#include "FullSolveMultiApp.h"

// 声明 Fortran 模块中的热工计算函数
extern "C"
{
  void thermal_execute(int *mesh_dims, double *power_field, double *temperature_field, int field_size);
}

class ThermalMultiApp : public FullSolveMultiApp
{
public:
  static InputParameters validParams();

  ThermalMultiApp(const InputParameters &parameters);

protected:
  virtual bool solveStep(Real dt, Real target_time, bool auto_advance = true) override;

  // 执行热工计算
  void executeThermalSolver();

  // 网格维度 (必须是3个整数)
  std::vector<int> _mesh_dims;

  // 功率场变量名
  const std::string _power_var_name;

  // 温度场变量名
  const std::string _temperature_var_name;
};