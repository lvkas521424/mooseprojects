# 子应用输入文件 - 仅作为占位符

[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 20
  ny = 20
[]

[Variables]
  [temperature]
    order = FIRST
    family = LAGRANGE
  []
[]

[Kernels]
  [null]
    type = NullKernel
    variable = temperature
  []
[]

[Executioner]
  type = Steady
  solve_type = PJFNK
[]

[Outputs]
  console = false
[]