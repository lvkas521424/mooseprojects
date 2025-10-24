/*
 * Author: lvjiahui eba424@163.com
 * Date: 2025-10-13 11:47:13
 * LastEditors: lvjiahui eba424@163.com
 * LastEditTime: 2025-10-14 14:43:00
 * FilePath: /mooseprojects/include/userobjects/FortranInterface.h
 * Description: 
 */
/*
 * Author: lvjiahui eba424@163.com
 * Date: 2025-10-13 11:47:13
 * LastEditors: lvjiahui eba424@163.com
 * LastEditTime: 2025-10-14 14:41:31
 * FilePath: /mooseprojects/include/userobjects/FortranInterface.h
 * Description: 
 */


#pragma once
// weishenmeshizheyangdene 

// 声明Fortran外部函数接口
extern "C"
{
  
  void update_burnup_step(int step, int max_steps);
}

/**
 * Fortran接口模块
 * 封装了与Fortran程序的交互操作
 */
class FortranInterface
{
public:
  /**
   * 构造函数
   * @param burn_step 当前燃耗步引用
   * @param max_burn_steps 最大燃耗步数
   */
  FortranInterface(unsigned int & burn_step, unsigned int max_burn_steps);

  /**
   * 更新Fortran程序中的燃耗步信息
   */
  void updateBurnupStep();

protected:
  /// 当前燃耗步引用
  unsigned int & _burn_step;

  /// 最大燃耗步数
  const unsigned int _max_burn_steps;
};

