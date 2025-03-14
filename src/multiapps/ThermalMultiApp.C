/****************************************************************/
/*            ThermalMultiApp - Thermal Application             */
/*                                                              */
/*              Manages thermal calculations                    */
/*      热工多应用程序 - 管理热工计算并与外部Fortran代码交互     */
/****************************************************************/

#include "ThermalMultiApp.h"
#include "MooseVariable.h"
#include "MooseMesh.h"
#include "FEProblem.h"

// Fortran interface declarations
// Fortran接口声明
extern "C" {
  void b2_execute(int* comm, int* mesh_dims, double* power_field, 
                 double* temperature_field, int* field_size);
}

registerMooseObject("mooseprojectsApp", ThermalMultiApp);

InputParameters
ThermalMultiApp::validParams()
{
  InputParameters params = FullSolveMultiApp::validParams();
  
  params.addClassDescription("MultiApp responsible for thermal calculations (b2)");
  // 负责热工计算(b2)的多应用程序
  
  // Only keep essential parameters
  // 只保留必要参数
  params.addParam<std::vector<int>>("mesh_dims", {10,10,10},
    "Mesh dimensions (nx,ny,nz)"); // 网格维度(nx,ny,nz)
  
  return params;
}

ThermalMultiApp::ThermalMultiApp(const InputParameters & parameters) :
  FullSolveMultiApp(parameters),
  _mesh_dims(getParam<std::vector<int>>("mesh_dims")),
  _console(_communicator.rank() == 0 ? Moose::out : Moose::err)
{
  if (_communicator.rank() == 0) {
    _console << "ThermalMultiApp initialized" << std::endl; // 热工多应用程序初始化完成
    _console << "  Using external Fortran solver with MOOSE mesh" << std::endl; // 使用带MOOSE网格的外部Fortran求解器
  }
}

bool
ThermalMultiApp::solveStep(Real dt, Real target_time, bool auto_advance)
{
  if (_communicator.rank() == 0) {
    _console << "ThermalMultiApp executing..." << std::endl; // 热工多应用程序执行中...
  }
  
  // Get power distribution from MOOSE system
  // 从MOOSE系统获取功率分布
  std::vector<Real> power_field = getPowerDistribution();
  
  // Calculate field size
  // 计算场大小
  int field_size = _mesh_dims[0] * _mesh_dims[1] * _mesh_dims[2];
  
  // Create array for temperature field
  // 创建温度场数组
  std::vector<double> temperature_field(field_size, 300.0); // Initial temperature (K) // 初始温度(K)
  
  if (_communicator.rank() == 0) {
    _console << "  Field size: " << field_size << " nodes" << std::endl; // 场大小：x个节点
    _console << "  Calling Fortran b2_execute..." << std::endl; // 调用Fortran b2_execute...
  }
  
  // Call Fortran b2_execute function with MOOSE mesh dimensions
  // 使用MOOSE网格维度调用Fortran b2_execute函数
  b2_execute(&_communicator.get(), _mesh_dims.data(), power_field.data(), 
             temperature_field.data(), &field_size);
  
  // Set temperature field in MOOSE system
  // 在MOOSE系统中设置温度场
  setTemperatureField(temperature_field);
  
  if (_communicator.rank() == 0) {
    _console << "ThermalMultiApp execution completed" << std::endl; // 热工多应用程序执行完成
  }
  
  // 成功完成计算，返回true
  return true;
}

std::vector<Real>
ThermalMultiApp::getPowerDistribution() const
{
  // Get power distribution from variables
  // 从变量获取功率分布
  std::vector<Real> power_field;
  
  if (hasLocalApp()) {
    auto & app = appProblem();
    
    // Get power variable
    // 获取功率变量
    auto & var = app.getVariable(0, "power");
    auto & solution = var.solution();
    
    // Copy solution to power field
    // 将解复制到功率场
    power_field.resize(solution.size());
    for (unsigned int i = 0; i < solution.size(); i++) {
      power_field[i] = solution(i);
    }
  }
  
  // Initialize with default power if empty
  // 如果为空，用默认功率初始化
  if (power_field.empty()) {
    int field_size = _mesh_dims[0] * _mesh_dims[1] * _mesh_dims[2];
    power_field.resize(field_size, 0.0); // Default power // 默认功率
  }
  
  return power_field;
}

void
ThermalMultiApp::setTemperatureField(const std::vector<Real> & temperature_data)
{
  if (hasLocalApp()) {
    auto & app = appProblem();
    
    // Get temperature variable
    // 获取温度变量
    auto & var = app.getVariable(0, "temperature");
    auto & solution = var.solution();
    
    // Check size
    // 检查大小
    if (temperature_data.size() != solution.size()) {
      mooseError("ThermalMultiApp: Temperature data size mismatch"); // 热工多应用程序：温度数据大小不匹配
    }
    
    // Copy temperature data to solution
    // 将温度数据复制到解
    for (unsigned int i = 0; i < solution.size(); i++) {
      solution.set(i, temperature_data[i]);
    }
    
    solution.close();
    var.sys().update();
  }
} 