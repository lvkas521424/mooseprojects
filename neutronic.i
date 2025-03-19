[Controls]
  [coupling_control]
    type = ReactorCouplingControl
    
    calc_type = NEUTRONICS
    
    burn_step = 1            # 更新为1，与代码默认值一致
    max_burn_steps = 2      # 减少为2，只执行一次燃耗步
    
    neutronics_app = neutronics 
    thermal_app = thermal
    #from_neutronics_transfers = from_neutronics
    execute_on = 'timestep_begin'   # 在每个时间步开始时执行Control
  []
[]

[MultiApps]
  [neutronics]
    type = NeutronicsMultiApp
    app_type = MooseprojectsApp
    input_files = 'scapn_input.i'
    mesh_dims = '10 10 10'           # 要与 ReactorCouplingControl 一致
    power_var_name = 'power_density'  # 要与变量名一致
    temperature_var_name = 'temperature'  # 要与变量名一致
    execute_on = 'NEUTRONIC PRENEUTRONIC CORNEUTRONIC'     # 每个时间步开始时执行
  []
  
  [thermal]
    type = ThermalMultiApp
    app_type = MooseprojectsApp
    input_files = 'thermal_input.i'
    mesh_dims = '10 10 10'           # 保持一致
    power_var_name = 'power_density'  # 要与变量名一致
    temperature_var_name = 'temperature'  # 要与变量名一致
    execute_on = 'THERMAL'            # 使用标准执行标志
  []
[]

# [Transfers]
#   [from_neutronics]
#     type = ReactorTransfer
#     from_multi_app = neutronics
#     source_variable = 'power_density'
#     variable = 'power_density'
#     execute_on = 'timestep_end'       # 每个时间步结束时执行
#     transfer_group = 'from_neutronics'    # 分组名称，用于代码中引用
#   []
# []

# 基本网格和材料设置
[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 10
  ny = 10
  nz = 10
[]

[Variables]
  [dummy]
    family = LAGRANGE
    order = FIRST
  []
[]

[Kernels]
  [dummy]
    type = Diffusion
    variable = dummy
  []
[]

[Problem]
  type = FEProblem
  verbose_multiapps = true
  solve = False
  kernel_coverage_check = false
  material_coverage_check = false
[]

# 用于数据传输的辅助变量
[AuxVariables]
  [temperature]
    family = MONOMIAL
    order = CONSTANT
    initial_condition = 600.0
  []
  [power_density]
    family = MONOMIAL
    order = CONSTANT
    initial_condition = 0.0
  []
[]

[Executioner]
  type = Transient
  
  # 每个时间步对应一个燃耗步
  start_time = 0
  end_time = 1            # 最大燃耗步数
  dt = 1.0                # 固定时间步长
  
  num_steps = 5           # 最大步数，对应最大燃耗步数
  
  # 求解设置
  solve_type = 'PJFNK'
  nl_abs_tol = 1e-6
  nl_rel_tol = 1e-6
  l_tol = 1e-6
  l_max_its = 100
  nl_max_its = 50
[]

[Outputs]
  exodus = true
  console = true
[]