[Mesh]
  # Generate a 2D mesh for the reactor model
  # 为反应堆模型生成二维网格
  type = GeneratedMesh
  dim = 2
  nx = 20
  ny = 20
  xmax = 100
  ymax = 100
[]

[Variables]
  [flux]
    # Neutron flux variable
    # 中子通量变量
    order = FIRST
    family = LAGRANGE
    initial_condition = 1.0e14  # Initial neutron flux (n/cm²·s)
                                # 初始中子通量 (n/cm²·s)
  []
  [power]
    # Power density variable
    # 功率密度变量
    order = FIRST
    family = LAGRANGE
    initial_condition = 1.0e6   # Initial power density (W/cm³)
                                # 初始功率密度 (W/cm³)
  []
  [temperature]
    # Temperature variable
    # 温度变量
    order = FIRST
    family = LAGRANGE
    initial_condition = 600.0   # Initial temperature (K)
                                # 初始温度 (K)
  []
[]

[AuxVariables]
  [burnup]
    # Burnup tracking variable
    # 燃耗跟踪变量
    order = FIRST
    family = LAGRANGE
    initial_condition = 0.0     # Initial burnup (MWd/kgU)
                                # 初始燃耗 (MWd/kgU)
  []
  [k_eff]
    # Effective multiplication factor
    # 有效增殖因子
    order = CONSTANT
    family = MONOMIAL
    initial_condition = 1.0     # Initial k-effective value
                                # 初始有效增殖因子值
  []
  [doppler]
    # Doppler reactivity coefficient
    # 多普勒反应性系数
    order = FIRST
    family = LAGRANGE
    initial_condition = 0.0     # Initial Doppler
  []
[] 