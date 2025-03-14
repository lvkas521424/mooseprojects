# 热工子应用输入卡

[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 10
  ny = 10
  nz = 10
[]

[Variables]
  [temperature]
    order = FIRST
    family = LAGRANGE
    initial_condition = 300
  []
  [power]
    order = FIRST
    family = LAGRANGE
  []
[]

[Executioner]
  type = Steady
[]

[Outputs]
  exodus = true
[]