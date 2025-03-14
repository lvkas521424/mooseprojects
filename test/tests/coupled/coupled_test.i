[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 10
  ny = 10
  nz = 10
[]

[Problem]
  type = FEProblem
  solve = false  # 主网格不需要求解
[]

[MultiApps]
  [b2]
    type = ThermalMultiApp
    input_files = thermal_sub.i
    execute_on = 'INITIAL TIMESTEP_END'
    positions = '0 0 0'
    steady_state = true
    mesh_dims = '10 10 10'
    inlet_temperature = 565.0
    system_pressure = 15.5
  []
  
  [b1]
    type = NeutronicsMultiApp
    input_files = neutronics_sub.i
    execute_on = 'INITIAL TIMESTEP_END'
    positions = '0 0 0'
    steady_state = true
    mesh_dims = '10 10 10'
    burnup_step = 2000.0
  []
[]

[Transfers]
  [temp_to_b1]
    type = ConditionalMultiAppCopyTransfer
    multi_app = b2
    direction = from_multiapp
    source_var = temperature
    target_var = temperature
    control_pp = temp_to_b1_control
  []
  
  [power_to_b2]
    type = ConditionalMultiAppCopyTransfer
    multi_app = b1
    direction = from_multiapp
    source_var = power_density
    target_var = power_density
    control_pp = power_to_b2_control
  []
[]

[Postprocessors]
  [temp_to_b1_control]
    type = TransferControlPostprocessor
  []
  
  [power_to_b2_control]
    type = TransferControlPostprocessor
  []
  
  [temperature_convergence]
    type = TemperatureConvergencePostprocessor
    temperature_var = temperature
  []
[]

[Controls]
  [reactor_control]
    type = ReactorControl
    calc_type = 2  # 耦合模式
    total_steps = 5
    max_iterations = 20
    convergence_tol = 1e-4
    mesh_dims = '10 10 10'
    steady_state = true
    burnup_increment = 2000.0
    temp_to_b1_control_pp = temp_to_b1_control
    power_to_b2_control_pp = power_to_b2_control
    temp_convergence_pp = temperature_convergence
    execute_on = 'INITIAL TIMESTEP_END'
  []
[]

[Executioner]
  type = Transient
  num_steps = 5
  dt = 1
[]

[Outputs]
  console = true
  exodus = true
[] 