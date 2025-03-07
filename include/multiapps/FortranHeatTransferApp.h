//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "MultiApp.h"

/**
 * FortranHeatTransferApp类
 * 此类实现一个MOOSE multiapp，用于集成Fortran热传导求解器。
 * 它直接调用现有Fortran代码中的热计算函数。
 */
class FortranHeatTransferApp : public MultiApp
{
public:
  /**
   * 构造此应用程序默认参数的方法
   */
  static InputParameters validParams();

  /**
   * 构造函数
   * @param parameters 应用程序的输入参数
   */
  FortranHeatTransferApp(const InputParameters & parameters);

  /**
   * 析构函数
   */
  virtual ~FortranHeatTransferApp();

  /**
   * 执行求解步骤
   * @param dt 时间步长
   * @param target_time 目标时间
   * @param auto_advance 是否自动推进时间
   * @return 求解是否成功
   */
  virtual bool solveStep(Real dt, Real target_time, bool auto_advance = true) override;

  /**
   * 获取当前温度场
   * @return 温度场向量的副本
   */
  std::vector<Real> getTemperatureField() const;

  /**
   * 设置温度场
   * @param temperatures 新的温度场向量
   */
  void setTemperatureField(const std::vector<Real> & temperatures);

protected:
  /**
   * 调用Fortran热计算函数
   */
  void callHeatCalculation();

  /// x方向网格大小
  int _nx;

  /// y方向网格大小
  int _ny;

  /// 热传导系数
  Real _k;

  /// x方向网格间距
  Real _dx;

  /// y方向网格间距
  Real _dy;

  /// 温度场存储（展平为1D向量）
  std::vector<Real> _temperature;
};