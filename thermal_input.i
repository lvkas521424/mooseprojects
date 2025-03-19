# 极简版 SCAPN 输入文件 - 用于外部 b1_execute 中子计算

[Mesh]
  [generated]
    type = GeneratedMeshGenerator
    dim = 3
    nx = 10
    ny = 10
    nz = 10
  []
[]

[AuxVariables]
  [power_density]
    family = MONOMIAL
    order = CONSTANT
    initial_condition = 0.0
  []
  
  [temperature]
    family = MONOMIAL
    order = CONSTANT
    initial_condition = 300.0
  []
[]

[Problem]
  type = FEProblem
  solve = false
[]

[Executioner]
  type = Transient
  num_steps = 1  # 可以减少到1步，因为实际计算在外部完成
  dt = 1.0
[]

[Outputs]
  exodus = true
  console = true
[]