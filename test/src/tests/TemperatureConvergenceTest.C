#include "gtest/gtest.h"
#include "TemperatureConvergencePostprocessor.h"
#include "FEProblem.h"
#include "MooseTest.h"

class TemperatureConvergenceTest : public MooseTest
{
protected:
  void SetUp() override
  {
    InputParameters params = validParams<TemperatureConvergencePostprocessor>();
    params.set<VariableName>("temperature_var") = "temperature";
    _pp = std::make_shared<TemperatureConvergencePostprocessor>(params);
  }
  
  std::shared_ptr<TemperatureConvergencePostprocessor> _pp;
};

TEST_F(TemperatureConvergenceTest, Convergence)
{
  // 初始化测试数据
  std::vector<Real> temp1 = {565.0, 570.0, 575.0};
  std::vector<Real> temp2 = {565.1, 570.1, 575.1};
  
  // 设置温度场
  _pp->setTemperatureField(temp1);
  _pp->execute();
  
  // 更新温度场
  _pp->setTemperatureField(temp2);
  Real rel_change = _pp->getValue();
  
  // 检查相对变化
  EXPECT_NEAR(rel_change, 0.000174, 1e-6);
} 