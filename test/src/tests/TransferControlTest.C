#include "gtest/gtest.h"
#include "TransferControlPostprocessor.h"
#include "FEProblem.h"
#include "MooseTest.h"

class TransferControlTest : public MooseTest
{
protected:
  void SetUp() override
  {
    InputParameters params = validParams<TransferControlPostprocessor>();
    _pp = std::make_shared<TransferControlPostprocessor>(params);
  }
  
  std::shared_ptr<TransferControlPostprocessor> _pp;
};

TEST_F(TransferControlTest, InitialState)
{
  // 检查初始状态
  EXPECT_EQ(_pp->getValue(), 0.0);
}

TEST_F(TransferControlTest, SetMode)
{
  // 测试设置模式
  _pp->setTransferMode(1);
  EXPECT_EQ(_pp->getValue(), 1.0);
  
  _pp->setTransferMode(0);
  EXPECT_EQ(_pp->getValue(), 0.0);
  
  // 测试无效模式
  _pp->setTransferMode(2);
  EXPECT_EQ(_pp->getValue(), 0.0);
} 