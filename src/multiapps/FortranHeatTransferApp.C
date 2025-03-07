#include "FortranHeatTransferApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "MultiApp.h"

// External Fortran function declaration
extern "C" 
{
  // 名称必须与Fortran中的bind(C, name="run_heat_transfer")匹配
  void run_heat_transfer(int* nx, int* ny, double* k, double* dx, double* dy, double* temperature);
}

registerMooseObject("multiapp2App", FortranHeatTransferApp);

InputParameters
FortranHeatTransferApp::validParams()
{
  InputParameters params = MultiApp::validParams();
  params.addClassDescription("用于集成Fortran热传导求解器的MultiApp");
  
  // 添加应用程序特定参数
  params.addParam<int>("nx", 10, "x方向网格点数");
  params.addParam<int>("ny", 10, "y方向网格点数");
  params.addParam<Real>("thermal_conductivity", 1.0, "热传导系数");
  params.addParam<Real>("dx", 0.1, "x方向网格间距");
  params.addParam<Real>("dy", 0.1, "y方向网格间距");
  
  return params;
}

FortranHeatTransferApp::FortranHeatTransferApp(const InputParameters & parameters) 
  : MultiApp(parameters),
    _nx(getParam<int>("nx")),
    _ny(getParam<int>("ny")),
    _k(getParam<Real>("thermal_conductivity")),
    _dx(getParam<Real>("dx")),
    _dy(getParam<Real>("dy"))
{
  // 初始化温度场向量，确保大小正确
  _temperature.resize(_nx * _ny, 0.0);
  
  // 输出信息
  mooseInfo("FortranHeatTransferApp initialized, grid size: ", _nx, "x", _ny);
}

FortranHeatTransferApp::~FortranHeatTransferApp() 
{
  // 只输出日志，不执行任何清理操作
  // std::vector会自动清理自己
  mooseInfo("Destroying FortranHeatTransferApp, resources will be cleaned automatically");
  
  // 不要添加任何资源清理代码！
  // _temperature会自动清理
  // 不要手动delete或free任何内容
}

bool 
FortranHeatTransferApp::solveStep(Real dt, Real target_time, bool auto_advance)
{
  // 调用Fortran热传导计算
  callHeatCalculation();
  return true;
}

void 
FortranHeatTransferApp::callHeatCalculation()
{
  // 问题点1: 确保参数有效性检查
  if (_nx <= 0 || _ny <= 0 || _temperature.size() != static_cast<size_t>(_nx * _ny))
  {
    mooseError("Invalid parameters: nx=", _nx, " ny=", _ny, 
               " temperature array size=", _temperature.size());
    return;
  }
  
  // 问题点2: 使用作用域限制临时变量生命周期
  {
    // 创建临时数组
    std::vector<double> temp_fortran(_nx * _ny, 0.0);
    
    // 问题点3: 确保循环边界正确
    for (int i = 0; i < _nx * _ny; ++i)
    {
      // 安全地复制数据
      temp_fortran[i] = static_cast<double>(_temperature[i]);
    }
    
    // 问题点4: 是否需要转换参数类型?
    int nx_int = static_cast<int>(_nx);
    int ny_int = static_cast<int>(_ny);
    double k_double = static_cast<double>(_k);
    double dx_double = static_cast<double>(_dx);
    double dy_double = static_cast<double>(_dy);
    
    // 问题点5: 调用前记录信息
    mooseInfo("Calling Fortran: nx=", nx_int, " ny=", ny_int, " array size=", nx_int * ny_int);
    
    // 问题点6: 检查Fortran函数声明是否存在
    // 确保外部函数声明位于CPP文件顶部，并且名称与Fortran绑定一致
    run_heat_transfer(&nx_int, &ny_int, &k_double, &dx_double, &dy_double, temp_fortran.data());
    
    // 问题点7: 安全地复制结果回来
    for (int i = 0; i < _nx * _ny; ++i)
    {
      _temperature[i] = static_cast<Real>(temp_fortran[i]);
    }
    
    // temp_fortran会在此作用域结束时自动释放
  }
}

std::vector<Real>
FortranHeatTransferApp::getTemperatureField() const
{
  return _temperature;
}

void
FortranHeatTransferApp::setTemperatureField(const std::vector<Real> & temperatures)
{
  if (temperatures.size() != _temperature.size())
  {
    mooseError("Temperature field size mismatch: expected ", _temperature.size(), ", got ", temperatures.size());
  }
  
  _temperature = temperatures;
}