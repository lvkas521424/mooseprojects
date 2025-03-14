[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = ${nx}
  ny = ${ny}
  nz = ${nz}
[]

[Variables]
  [flux]
    order = FIRST
    family = LAGRANGE
    initial_condition = 1.0
  []
  [power_density]
    order = FIRST
    family = LAGRANGE
    initial_condition = 0.0
  []
  [temperature]
    order = FIRST
    family = LAGRANGE
    initial_condition = 565.0  # 初始温度(K)
  []
[]

[Kernels]
  [diffusion]
    type = Diffusion
    variable = flux
  []
  [absorption]
    type = CoefDiffusion
    variable = flux
    coef = ${fparse 1.0 + 0.1 * temperature}  # 温度反馈
  []
  [power_calculation]
    type = CoupledForce
    variable = power_density
    v = flux
    coef = 3.204e-11  # 功率系数
  []
[]

[BCs]
  [flux_left]
    type = DirichletBC
    variable = flux
    boundary = left
    value = 0
  []
  [flux_right]
    type = DirichletBC
    variable = flux
    boundary = right
    value = 0
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