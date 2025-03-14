[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = ${nx}
  ny = ${ny}
  nz = ${nz}
[]

[Variables]
  [temperature]
    order = FIRST
    family = LAGRANGE
    initial_condition = 565.0  # 初始温度(K)
  []
  [power_density]
    order = FIRST
    family = LAGRANGE
    initial_condition = 0.0
  []
[]

[Kernels]
  [heat_conduction]
    type = HeatConduction
    variable = temperature
  []
  [heat_source]
    type = CoupledForce
    variable = temperature
    v = power_density
    coef = 1.0
  []
[]

[Materials]
  [thermal_conductivity]
    type = GenericConstantMaterial
    prop_names = 'thermal_conductivity'
    prop_values = 30.0  # W/(m·K)
  []
[]

[BCs]
  [temp_inlet]
    type = DirichletBC
    variable = temperature
    boundary = bottom
    value = ${inlet_temperature}
  []
  [temp_outlet]
    type = OutflowBC
    variable = temperature
    boundary = top
  []
[]

[Executioner]
  type = Steady
  solve_type = PJFNK
  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre boomeramg'
  nl_rel_tol = 1e-6
  nl_abs_tol = 1e-8
[]

[Outputs]
  exodus = true
  console = true
[] 