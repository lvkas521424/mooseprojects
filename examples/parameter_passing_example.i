# =====================================================
# 参数传递示例配置文件
# 展示如何从 ReactorCouplingUserObject 传递参数到 NeutronicsMultiApp
# =====================================================

[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 10
  ny = 10
  nz = 10
[]

[Variables]
  [power1]
  []
  [power2]
  []
  [temperature]
  []
[]

[AuxVariables]
  [dummy]
  []
[]

[Kernels]
  [time_derivative]
    type = TimeDerivative
    variable = dummy
  []
  [diffusion]
    type = Diffusion
    variable = dummy
  []
[]

[BCs]
  [all]
    type = DirichletBC
    variable = dummy
    boundary = 'left right top bottom front back'
    value = 0
  []
[]

[Executioner]
  type = Transient
  num_steps = 5
  dt = 1.0
  solve_type = PJFNKMP
  petsc_options_iname = '-pc_type'
  petsc_options_value = 'lu'
[]

# =====================================================
# UserObject: 耦合控制对象，管理计算类型和燃耗步
# =====================================================
[UserObjects]
  [coupling_control]
    type = ReactorCouplingUserObject
    calc_type = COUPLED           # 计算类型: NEUTRONICS=1, COUPLED=2
    burn_step = 1                 # 初始燃耗步
    max_burn_steps = 5            # 最大燃耗步数
    max_coupling_iterations = 3   # 最大耦合迭代次数
    coupling_tolerance = 1e-4     # 耦合收敛容差
    neutronics_app = 'neutronics' # 中子学多应用名称
    thermal_app = 'thermal'       # 热工多应用名称
    execute_on = 'TIMESTEP_BEGIN'
  []
[]

# =====================================================
# MultiApp: 中子学多应用
# 关键点：通过 coupling_userobject 参数引用 UserObject
# =====================================================
[MultiApps]
  [neutronics]
    type = NeutronicsMultiApp
    input_files = 'neutronics_sub.i'
    mesh_dims = '10 10 10'
    
    # 方法1: 可以在这里直接设置 calc_type（静态）
    # calc_type = COUPLED
    
    # 方法2: 通过 UserObject 动态获取参数（推荐）
    coupling_userobject = 'coupling_control'  # 引用上面定义的 UserObject
    
    power_var_name1 = 'power1'
    power_var_name2 = 'power2'
    temperature_var_name = 'temperature'
    
    execute_on = 'EXEC_NEUTRONIC'
  []
  
  [thermal]
    type = ThermalMultiApp
    input_files = 'thermal_sub.i'
    execute_on = 'EXEC_THERMAL'
  []
[]

[Outputs]
  exodus = true
  print_linear_residuals = false
[]

