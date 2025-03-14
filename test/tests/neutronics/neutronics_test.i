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
  [b1_standalone]
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
  [power_from_b1_standalone]
    type = ConditionalMultiAppCopyTransfer
    multi_app = b1_standalone
    direction = from_multiapp
    source_var = power_density
    target_var = power_density
    control_pp = power_to_b2_control
  []
[]

[Postprocessors]
  [power_to_b2_control]
    type = TransferControlPostprocessor
  []
[]

[Controls]
  [reactor_control]
    type = ReactorControl
    calc_type = 1  # 独立中子学模式
    total_steps = 5
    max_iterations = 20
    convergence_tol = 1e-4
    mesh_dims = '10 10 10'
    steady_state = true
    burnup_increment = 2000.0
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