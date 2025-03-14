# 中子学子应用输入卡

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

[Executioner]
  type = Steady
[]

[Outputs]
  exodus = true
[]