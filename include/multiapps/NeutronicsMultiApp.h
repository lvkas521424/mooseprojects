/*
 * @Author      : lvjiahui eba424@163.com
 * @Date        : Do not edit
 * @LastEditors : lvjiahui eba424@163.com
 * @LastEditTime: 2025-09-16 14:31:01
 * @FilePath    : Do not edit
 * @Description : 
 * 
 * Copyright (c) 2025 by ${git_name_email}, All Rights Reserved. 
 */
/*
 * @Author: lvjiahui eba424@163.com
 * @Date: 2025-09-11 11:32:37
 * @LastEditors: lvjiahui eba424@163.com
 * @LastEditTime: 2025-09-16 14:30:32
 * @FilePath: /mooseprojects/include/multiapps/NeutronicsMultiApp.h
 * @Description: 
 * 
 * Copyright (c) 2025 by ${git_name_email}, All Rights Reserved. 
 */


#pragma once

#include "FullSolveMultiApp.h"
// 添加MPI支持
#include "libmesh/parallel.h"
#include <mpi.h>
#include <vector>

// 声明Fortran接口
extern "C"
{
    void b1_execute_3d_mpi(
        // MPI_Comm comm,
        int *mesh_dims,
        double *power_data1,
        double *power_data2,
        double *temperature_data,
        int field_size);
}

/**
 * 中子学多应用类，简化版
 * 用于调用外部b1_execute计算程序
 * 支持MPI并行计算
 */
class NeutronicsMultiApp : public FullSolveMultiApp
{
public:
    NeutronicsMultiApp(const InputParameters &parameters);
    static InputParameters validParams();

protected:
    virtual bool solveStep(Real dt, Real target_time, bool auto_advance) override;
    void executeB1Solver();

    // 网格维度参数
    const std::vector<int> _mesh_dims;

    // 变量名称参数
    const std::string _power_var_name1; // 第一个功率变量名
    const std::string _power_var_name2; // 第二个功率变量名
    const std::string _temperature_var_name;

    // 计算类型参数
    const unsigned int _calc_type;

    // UserObject引用（可选方法）
    const std::string _coupling_userobject_name;

    // 数据存储
    std::vector<double> _power_data1;
    std::vector<double> _power_data2;
    std::vector<double> _temperature_data;
};