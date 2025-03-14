/****************************************************************/
/*           NeutronicsMultiApp - Neutronics Application        */
/*                                                              */
/*              Manages neutronics calculations                 */
/* 中子学多应用程序 - 管理中子物理计算并与外部Fortran代码交互    */
/****************************************************************/

#include "NeutronicsMultiApp.h"
#include "MooseVariable.h"
#include "MooseMesh.h"
#include "FEProblem.h"

// Fortran interface declarations
// Fortran接口声明
extern "C" {
  void b1_execute(int* comm, int* mesh_dims, double* temperature_field, 
                 double* power_field, int* field_size);
}

registerMooseObject("mooseprojectsApp", NeutronicsMultiApp);

InputParameters
NeutronicsMultiApp::validParams()
{
  InputParameters params = FullSolveMultiApp::validParams();
  
  params.addClassDescription("MultiApp responsible for neutronics calculations (b1)");
  // 负责中子学计算(b1)的多应用程序
  
  // Only keep essential parameters
  // 只保留必要参数
  params.addParam<std::vector<int>>("mesh_dims", {10,10,10},
    "Mesh dimensions (nx,ny,nz)"); // 网格维度(nx,ny,nz)
  
  return params;
}

NeutronicsMultiApp::NeutronicsMultiApp(const InputParameters & parameters) :
  FullSolveMultiApp(parameters),
  _mesh_dims(getParam<std::vector<int>>("mesh_dims")),
  _console(_communicator.rank() == 0 ? Moose::out : Moose::err)
{
  if (_communicator.rank() == 0) {
    _console << "NeutronicsMultiApp initialized" << std::endl; // 中子学多应用程序初始化完成
    _console << "  Using external Fortran solver with MOOSE mesh" << std::endl; // 使用带MOOSE网格的外部Fortran求解器
  }
}

bool
NeutronicsMultiApp::solveStep(Real dt, Real target_time, bool auto_advance)
{
  if (_communicator.rank() == 0) {
    _console << "NeutronicsMultiApp executing..." << std::endl; // 中子学多应用程序执行中...
  }
  
  // Get temperature field from MOOSE system
  // 从MOOSE系统获取温度场
  std::vector<Real> temperature_field = getTemperatureField();
  
  // Calculate field size
  // 计算场大小
  int field_size = _mesh_dims[0] * _mesh_dims[1] * _mesh_dims[2];
  
  // Create array for power distribution
  // 创建功率分布数组
  std::vector<double> power_field(field_size, 0.0);
  
  if (_communicator.rank() == 0) {
    _console << "  Field size: " << field_size << " nodes" << std::endl; // 场大小：x个节点
    _console << "  Calling Fortran b1_execute..." << std::endl; // 调用Fortran b1_execute...
  }
  
  // Call Fortran b1_execute function with MOOSE mesh dimensions
  // 使用MOOSE网格维度调用Fortran b1_execute函数
  b1_execute(&_communicator.get(), _mesh_dims.data(), temperature_field.data(), 
             power_field.data(), &field_size);
  
  // Set power distribution in MOOSE system
  // 在MOOSE系统中设置功率分布
  setPowerDistribution(power_field);
  
  if (_communicator.rank() == 0) {
    _console << "NeutronicsMultiApp execution completed" << std::endl; // 中子学多应用程序执行完成
  }
  
  // 成功完成计算，返回true
  return true;
}

std::vector<Real>
NeutronicsMultiApp::getTemperatureField() const
{
  // Get temperature field from variables
  // 从变量获取温度场
  std::vector<Real> temperature_field;
  
  if (hasLocalApp()) {
    auto & app = appProblem();
    
    // Get temperature variable
    // 获取温度变量
    auto & var = app.getVariable(0, "temperature");
    auto & solution = var.solution();
    
    // Copy solution to temperature field
    // 将解复制到温度场
    temperature_field.resize(solution.size());
    for (unsigned int i = 0; i < solution.size(); i++) {
      temperature_field[i] = solution(i);
    }
  }
  
  // Initialize with default temperature if empty
  // 如果为空，用默认温度初始化
  if (temperature_field.empty()) {
    int field_size = _mesh_dims[0] * _mesh_dims[1] * _mesh_dims[2];
    temperature_field.resize(field_size, 300.0); // Default temperature (K) // 默认温度(K)
  }
  
  return temperature_field;
}

void
NeutronicsMultiApp::setPowerDistribution(const std::vector<Real> & power_data)
{
  if (hasLocalApp()) {
    auto & app = appProblem();
    
    // Get power variable
    // 获取功率变量
    auto & var = app.getVariable(0, "power");
    auto & solution = var.solution();
    
    // Check size
    // 检查大小
    if (power_data.size() != solution.size()) {
      mooseError("NeutronicsMultiApp: Power data size mismatch"); // 中子学多应用程序：功率数据大小不匹配
    }
    
    // Copy power data to solution
    // 将功率数据复制到解
    for (unsigned int i = 0; i < solution.size(); i++) {
      solution.set(i, power_data[i]);
    }
    
    solution.close();
    var.sys().update();
  }
} 