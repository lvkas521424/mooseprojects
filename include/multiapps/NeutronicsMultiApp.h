/****************************************************************/
/* NeutronicsMultiApp.h                                         */
/* 简化版中子学多应用头文件                                         */
/****************************************************************/

#pragma once

#include "FullSolveMultiApp.h"

// 声明Fortran接口
extern "C" {
  void b1_execute_3d(int* mesh_dims, double* power_data1, double* power_data2, double* temperature_data, int field_size);
}

/**
 * 中子学多应用类，简化版
 * 用于调用外部b1_execute计算程序
 */
class NeutronicsMultiApp : public FullSolveMultiApp
{
public:
    static InputParameters validParams();
    NeutronicsMultiApp(const InputParameters & parameters);

protected:
    virtual bool solveStep(Real dt, Real target_time, bool auto_advance) override;
    void executeB1Solver();

    // 网格维度参数
    const std::vector<int> _mesh_dims;
    
    // 变量名称参数
    const std::string _power_var_name1;  // 第一个功率变量名
    const std::string _power_var_name2;  // 第二个功率变量名
    const std::string _temperature_var_name;

    // 数据存储
    std::vector<double> _power_data1;
    std::vector<double> _power_data2;
    std::vector<double> _temperature_data;
}; 