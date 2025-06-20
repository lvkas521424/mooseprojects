/****************************************************************/
/* NeutronicsMultiApp.C                                         */
/* Neutronics Calculation Implementation                        */
/*                                                              */
/* Implements neutron transport and diffusion calculations      */
/* for reactor physics simulations.                             */
/*                                                              */
/* Created: Mar 19, 2025                                        */
/* Last Modified: Mar 19, 2025                                  */
/****************************************************************/

#include "NeutronicsMultiApp.h"
#include "FEProblem.h"
#include "MooseVariableFE.h"
#include "SystemBase.h"
#include "MooseMesh.h"
#include "LevelSetTypes.h"

registerMooseObject("mooseprojectsApp", NeutronicsMultiApp);

InputParameters
NeutronicsMultiApp::validParams()
{
    InputParameters params = FullSolveMultiApp::validParams();
    params.addClassDescription("中子学多应用，使用b1_execute作为求解核心");
    
    params.addRequiredParam<std::vector<int>>("mesh_dims", "网格维度 (3个整数)");
    params.addParam<std::string>("power_var_name1", "power1", "第一个功率场变量名");
    params.addParam<std::string>("power_var_name2", "power2", "第二个功率场变量名");
    params.addParam<std::string>("temperature_var_name", "temperature", "温度场变量名");

    ExecFlagEnum & exec = params.set<ExecFlagEnum>("execute_on");
    exec.addAvailableFlags(LevelSet::EXEC_NEUTRONIC);
    exec.addAvailableFlags(LevelSet::EXEC_PRENEUTRONIC);
    exec.addAvailableFlags(LevelSet::EXEC_CORNEUTRONIC);
    exec = {LevelSet::EXEC_NEUTRONIC, LevelSet::EXEC_PRENEUTRONIC, LevelSet::EXEC_CORNEUTRONIC};
    
    return params;
}

NeutronicsMultiApp::NeutronicsMultiApp(const InputParameters & parameters)
    : FullSolveMultiApp(parameters),
      _mesh_dims(getParam<std::vector<int>>("mesh_dims")),
      _power_var_name1(getParam<std::string>("power_var_name1")),
      _power_var_name2(getParam<std::string>("power_var_name2")),
      _temperature_var_name(getParam<std::string>("temperature_var_name"))
{
    if (_mesh_dims.size() != 3)
        mooseError("NeutronicsMultiApp: mesh_dims必须包含3个整数值");
}

void
NeutronicsMultiApp::executeB1Solver()
{
    if (!hasLocalApp(0))
        return;

    auto & app = appProblemBase(0);
    
    // 检查变量是否存在
    if (!app.hasVariable(_power_var_name1) || !app.hasVariable(_power_var_name2) || 
        !app.hasVariable(_temperature_var_name))
        return;

    // 获取功率变量1
    auto & power_var1 = app.getVariable(0, _power_var_name1);
    auto & power_var2 = app.getVariable(0, _power_var_name2);
    
    if(&power_var1 == &power_var2)
    {
        mooseInfo("power_var1 和 power_var2 是同一个变量");
        if(power_var1.number() != power_var2.number())
        {
            mooseInfo("power_var1 和 power_var2 的编号不同");
        }
        else
        {
            mooseInfo("power_var1 和 power_var2 的编号相同");
        }
    }
    
    if(&power_var1.sys() == &power_var2.sys())
    {
        mooseInfo("power_var1 和 power_var2 的系统相同");
    }
    else
    {
        mooseInfo("power_var1 和 power_var2 的系统不同");
    }

    auto & power_sys = power_var1.sys();
    auto & power_solution = power_sys.solution();

    unsigned int power1_offset = power_var1.number();  // 获取变量在系统中的偏移
    unsigned int power2_offset = power_var2.number();  // 获取变量在系统中的偏移   

    // 获取温度变量
    auto & temp_var = app.getVariable(0, _temperature_var_name);
    auto & temp_sys = temp_var.sys();
    auto & temp_solution = temp_sys.solution();
        
    std::cout << "power_solution.size() = " << power_solution.size() << std::endl;
    std::cout << "temp_solution.size() = " << temp_solution.size() << std::endl;
    std::cout << "power 1变量编号: " << power_var1.number() << std::endl;
    std::cout << "power 2变量编号: " << power_var2.number() << std::endl;
    std::cout << "temp 变量编号: " << temp_var.number() << std::endl;

    
    // 验证变量是否真的不同
    std::cout << "power_var1 地址: " << &power_var1 << std::endl;
    std::cout << "power_var2 地址: " << &power_var2 << std::endl;
    std::cout << "temp_var   地址: " << &temp_var << std::endl;
    
    // 检查解向量的实际结构
    std::cout << "解向量总大小            : " << power_solution.size() << std::endl;
    std::cout << "power 1每个变量的DOF数量: " << power_var1.dofIndices().size() << std::endl;
    std::cout << "power 2每个变量的DOF数量: " << power_var2.dofIndices().size() << std::endl;  
    std::cout << "temp 每个变量的DOF数量  : " << temp_var.dofIndices().size() << std::endl;  
    
    // 检查变量的DOF索引
    const auto & dof_indices1 = power_var1.dofIndices();
    const auto & dof_indices2 = power_var2.dofIndices();
    
    std::cout << "power1 DOF索引数量: " << dof_indices1.size() << std::endl;
    std::cout << "power2 DOF索引数量: " << dof_indices2.size() << std::endl;
    
    
    // 获取数据场大小
    int field_size = _mesh_dims[0] * _mesh_dims[1] * _mesh_dims[2];
    std::cout << "field_size = " << field_size << std::endl;
    // 调整数据数组大小
    _power_data1.resize(field_size, 0.0);
    _power_data2.resize(field_size, 0.0);
    _temperature_data.resize(field_size, 0.0);
    
    // 从MOOSE复制温度场数据
    for (int i = 0; i < field_size; ++i)
        _temperature_data[i] = temp_solution(i);
    
    // 创建网格维度的可修改副本
    std::vector<int> mesh_dims_copy = _mesh_dims;
    
    // 调用Fortran的b1_execute子程序
    b1_execute_3d(mesh_dims_copy.data(), 
                  _power_data1.data(), 
                  _power_data2.data(),
                  _temperature_data.data(), 
                  field_size);

    for (int i = 0; i < field_size; i=i+1)
    {
        std::cout << "写入 power_density_1[" << std::setw(4) << i   <<
         "] = " << _power_data1[i] << std::endl;
    }

    for (int i = 0; i < field_size; i=i+1)
    {
        std::cout << "写入 power_density_2[" << std::setw(4) << i   <<
         "] = " << _power_data2[i] << std::endl;
    }

    
    // 将结果写回MOOSE变量
    for (int i = 0; i < field_size; i=i+1)
    {
        power_solution.set( i *2,     _power_data1[i]);
        power_solution.set( i *2 + 1, _power_data2[i]);
    }




    //power_var1.sys().update();

}
bool
NeutronicsMultiApp::solveStep(Real dt, Real target_time, bool auto_advance)
{
  // 直接使用b1_execute作为求解核心
  executeB1Solver();
  return true; // 始终返回成功
}  