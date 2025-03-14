#!/usr/bin/env python3

import sys
import numpy as np
from exodus3 import exodus

def check_neutronics_results(filename):
    """检查中子学计算结果"""
    e = exodus(filename)
    
    # 检查功率分布
    power = e.get_node_variable_values("power_density", 1)
    assert np.all(power >= 0), "功率密度不能为负"
    assert np.max(power) < 1e6, "功率密度超出物理范围"
    
    # 检查中子通量
    flux = e.get_node_variable_values("flux", 1)
    assert np.all(flux >= 0), "中子通量不能为负"
    
    e.close()
    return True

def check_thermal_results(filename):
    """检查热工计算结果"""
    e = exodus(filename)
    
    # 检查温度分布
    temp = e.get_node_variable_values("temperature", 1)
    assert np.all(temp >= 500), "温度低于物理范围"
    assert np.all(temp <= 1000), "温度超出物理范围"
    
    e.close()
    return True

def main():
    # 检查独立模式结果
    try:
        neutronics_ok = check_neutronics_results("neutronics_out.e")
        print("中子学计算结果检查：", "通过" if neutronics_ok else "失败")
    except Exception as e:
        print("中子学结果检查失败：", str(e))
        
    # 检查耦合模式结果
    try:
        thermal_ok = check_thermal_results("coupled_out.e")
        print("热工计算结果检查：", "通过" if thermal_ok else "失败")
    except Exception as e:
        print("热工结果检查失败：", str(e))

if __name__ == "__main__":
    main() 