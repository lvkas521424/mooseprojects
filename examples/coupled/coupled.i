# 耦合计算模式输入卡 (calc_type=2)

[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 10
  ny = 10
  nz = 10
[]

[Variables]
  [power]
    order = FIRST
    family = LAGRANGE
  []
  [temperature]
    order = FIRST
    family = LAGRANGE
    initial_condition = 300
  []
[]

[Controls]
  [reactor_control]
    type = ReactorControl
    calc_type = 2
    mesh_dims = '10 10 10'
    total_steps = 3
    temp_to_b1_control_pp = temp_to_b1_control
    power_to_b2_control_pp = power_to_b2_control
    power_to_main_control_pp = power_to_main_control
    temp_convergence_pp = temperature_convergence
    use_initial_temperature = true
    max_iterations = 20
    convergence_tol = 1e-4
    execute_on = 'TIMESTEP_END'
  []
[]

[MultiApps]
  [neutronics]
    type = NeutronicsMultiApp
    input_files = neutronics_sub.i
    mesh_dims = '10 10 10'
    execute_on = 'TIMESTEP_END'
  []
  
  [thermal]
    type = ThermalMultiApp
    input_files = thermal_sub.i
    mesh_dims = '10 10 10'
    execute_on = 'TIMESTEP_END'
  []
[]

[Transfers]
  [power_from_neutronics]
    type = ConditionalMultiAppCopyTransfer
    direction = from_multiapp
    multi_app = neutronics
    source_variable = power
    target_variable = power
    control_pp = power_to_main_control
  []
  
  [temp_to_neutronics]
    type = ConditionalMultiAppCopyTransfer
    direction = to_multiapp
    multi_app = neutronics
    source_variable = temperature
    target_variable = temperature
    control_pp = temp_to_b1_control
  []
  
  [power_to_thermal]
    type = ConditionalMultiAppCopyTransfer
    direction = to_multiapp
    multi_app = thermal
    source_variable = power
    target_variable = power
    control_pp = power_to_b2_control
  []
[]

[Postprocessors]
  [temp_to_b1_control]
    type = TransferControlPostprocessor
    initially_active = false
  []
  
  [power_to_b2_control]
    type = TransferControlPostprocessor
    initially_active = false
  []
  
  [power_to_main_control]
    type = TransferControlPostprocessor
    initially_active = false
  []
  
  [temperature_convergence]
    type = TemperatureConvergencePostprocessor
    temperature = temperature
  []
  
  # 添加功率的平均值和最大值后处理器
  [average_power]
    type = ElementAverageValue
    variable = power
    execute_on = 'TIMESTEP_END'
  []
  
  [max_power]
    type = ElementExtremeValue
    variable = power
    value_type = max
    execute_on = 'TIMESTEP_END'
  []
  
  # 添加温度的平均值和最大值后处理器
  [average_temp]
    type = ElementAverageValue
    variable = temperature
    execute_on = 'TIMESTEP_END'
  []
  
  [max_temp]
    type = ElementExtremeValue
    variable = temperature
    value_type = max
    execute_on = 'TIMESTEP_END'
  []
[]

[VectorPostprocessors]
  [power_sample]
    type = ElementValueSampler
    variable = power
    sort_by = id
    execute_on = 'TIMESTEP_END'
  []
  
  [temp_sample]
    type = ElementValueSampler
    variable = temperature
    sort_by = id
    execute_on = 'TIMESTEP_END'
  []
[]

[Executioner]
  type = Transient
  num_steps = 3
  dt = 1.0
[]

[Outputs]
  exodus = true
  csv = true
  
  # 每个燃耗步保存一个单独的输出文件
  [out]
    type = Exodus
    file_base = coupled_step
    sequence = true  # 创建序列文件，每步一个
    output_timesteps = true
  []
  
  # 检查点以便恢复任何燃耗步
  [checkpoint]
    type = Checkpoint
    num_files = 4
    interval = 1
  []
[]