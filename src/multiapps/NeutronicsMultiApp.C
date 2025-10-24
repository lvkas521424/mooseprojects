/****************************************************************/
/* NeutronicsMultiApp.C                                         */
/* Neutronics Calculation Implementation                        */
/*                                                              */
/* Implements neutron transport and diffusion calculations      */
/* for reactor physics simulations.                             */
/*                                                              */
/* (简化版：直接使用MOOSE的网格分解)                                */
/*                                                              */
/* Created: Mar 19, 2025                                        */
/* Last Modified: Jun 30, 2025                                  */
/****************************************************************/

#include "NeutronicsMultiApp.h"
#include "FEProblem.h"
#include "MooseVariableFE.h"
#include "SystemBase.h"
#include "MooseMesh.h"
#include "LevelSetTypes.h"
#include "ReactorCouplingUserObject.h"
#include "mpi.h"

registerMooseObject("mooseprojectsApp", NeutronicsMultiApp);

InputParameters
NeutronicsMultiApp::validParams()
{
    InputParameters params = FullSolveMultiApp::validParams();
    params.addClassDescription("中子学多应用，使用b1_execute作为求解核心，直接使用MOOSE的网格分解。");

    params.addRequiredParam<std::vector<int>>("mesh_dims", "网格维度 (3个整数)");
    params.addParam<std::string>("power_var_name1", "power1", "第一个功率场变量名");
    params.addParam<std::string>("power_var_name2", "power2", "第二个功率场变量名");
    params.addParam<std::string>("temperature_var_name", "temperature", "温度场变量名");

    // 添加计算类型参数
    MooseEnum calc_types("NEUTRONICS=1 COUPLED=2", "COUPLED");
    params.addParam<MooseEnum>("calc_type", calc_types, "计算类型：1-仅中子学, 2-耦合计算");

    // 添加UserObject引用参数（可选方法）
    params.addParam<std::string>("coupling_userobject", "", "ReactorCouplingUserObject的名称");

    ExecFlagEnum &exec = params.set<ExecFlagEnum>("execute_on");
    exec.addAvailableFlags(LevelSet::EXEC_NEUTRONIC);
    exec.addAvailableFlags(LevelSet::EXEC_PRENEUTRONIC);
    exec.addAvailableFlags(LevelSet::EXEC_CORNEUTRONIC);
    exec = {LevelSet::EXEC_NEUTRONIC, LevelSet::EXEC_PRENEUTRONIC, LevelSet::EXEC_CORNEUTRONIC};

    return params;
}

NeutronicsMultiApp::NeutronicsMultiApp(const InputParameters &parameters)
    : FullSolveMultiApp(parameters),
      _mesh_dims(getParam<std::vector<int>>("mesh_dims")),
      _power_var_name1(getParam<std::string>("power_var_name1")),
      _power_var_name2(getParam<std::string>("power_var_name2")),
      _temperature_var_name(getParam<std::string>("temperature_var_name")),
      _calc_type(getParam<MooseEnum>("calc_type")),
      _coupling_userobject_name(getParam<std::string>("coupling_userobject"))
{
    if (_mesh_dims.size() != 3)
        mooseError("NeutronicsMultiApp: mesh_dims必须包含3个整数值");
}

void NeutronicsMultiApp::executeB1Solver()
{
    if (!hasLocalApp(0))
        return;

    auto &app = appProblemBase(0);
    const Parallel::Communicator &comm = app.comm();

    // 检查变量是否存在
    if (!app.hasVariable(_power_var_name1) || !app.hasVariable(_power_var_name2) ||
        !app.hasVariable(_temperature_var_name))
        return;

    // 获取MOOSE变量及其解向量
    auto &power_var = app.getVariable(0, _power_var_name1);
    auto &temp_var = app.getVariable(0, _temperature_var_name);

    auto &power_solution = power_var.sys().solution();
    auto &temp_solution = temp_var.sys().solution();
    
    // 获取时间步长和时间步数信息
    Real current_time = _fe_problem.time();
    Real time_step_size = _fe_problem.dt();
    int time_step_number = _fe_problem.timeStep();
    
    // 从 ReactorCouplingUserObject 获取计算参数
    unsigned int burn_step = time_step_number;
    if (!_coupling_userobject_name.empty() && _fe_problem.hasUserObject(_coupling_userobject_name))
    {
        const auto& coupling_uo = _fe_problem.getUserObject<ReactorCouplingUserObject>(_coupling_userobject_name);
        burn_step = coupling_uo.getBurnStep();
    }
    
    if (comm.rank() == 0)
        std::cout << "  Solving neutronics (MPI=" << comm.size() << ")..." << std::endl;

    // 获取本地数据信息
    unsigned int power_local_size = power_solution.local_size();
    unsigned int power_start_idx = power_solution.first_local_index();
    unsigned int temp_local_size = temp_solution.local_size();
    unsigned int temp_start_idx = temp_solution.first_local_index();

    const unsigned int num_variables = 2;

    // 保存原始值用于写回
    unsigned int original_power_local_size = power_local_size;
    unsigned int original_power_start_idx = power_start_idx;
    
    // 计算每个变量的实际大小
    power_local_size = power_local_size / num_variables;
    power_start_idx = power_start_idx / num_variables;

    // 准备本地数据数组
    std::vector<double> local_power_data1(power_local_size, 0.0);
    std::vector<double> local_power_data2(power_local_size, 0.0);
    std::vector<double> local_temperature_data(temp_local_size);

    // 读取本地温度数据
    for (unsigned int i = 0; i < temp_local_size; ++i)
        local_temperature_data[i] = temp_solution(temp_start_idx + i);

    // 调用Fortran并行计算
    int fortran_data_size = static_cast<int>(power_local_size);
    std::vector<int> mesh_dims_copy = _mesh_dims;

    MPI_Barrier(MPI_COMM_WORLD);

    b1_execute_3d_mpi(
        mesh_dims_copy.data(),
        local_power_data1.data(),
        local_power_data2.data(),
        local_temperature_data.data(),
        fortran_data_size);

    MPI_Barrier(MPI_COMM_WORLD);

    // 写回MOOSE解向量
    unsigned int data1_idx = 0;
    unsigned int data2_idx = 0;

    for (unsigned int i = 0; i < original_power_local_size; ++i)
    {
        unsigned int global_idx = original_power_start_idx + i;
        unsigned int sort_idx = global_idx % num_variables;

        if (sort_idx == 0) // power1 (偶数索引)
        {
            if (data1_idx < local_power_data1.size())
            {
                power_solution.set(global_idx, local_power_data1[data1_idx]);
                data1_idx++;
            }
        }
        else if (sort_idx == 1) // power2 (奇数索引)
        {
            if (data2_idx < local_power_data2.size())
            {
                power_solution.set(global_idx, local_power_data2[data2_idx]);
                data2_idx++;
            }
        }
    }

    power_solution.close();
}

bool NeutronicsMultiApp::solveStep(Real dt, Real target_time, bool auto_advance)
{
    executeB1Solver();
    return true;
}