# 极简版 SCAPN 输入文件 - 用于外部 b1_execute 中子计算

[Mesh]
  [gmg]
    type = GeneratedMeshGenerator  
    dim = 3
    nx = 3
    ny = 4
    nz = 5
  []
[]

[Variables]
  [temperature]
    #family = MONOMIAL
    #order = CONSTANT
    #initial_condition = 300.0
  []
[]
[AuxVariables]
  [power_density]  # 添加这个
    family = MONOMIAL
    order = CONSTANT
    #initial_condition = 0.0
  []
  # [power_density2]  # 添加这个
  #   family = MONOMIAL
  #   order = CONSTANT
  #   #initial_condition = 0.0
  # []
[]

[Problem]
  type = FEProblem
  solve = false  
[]

[Executioner]
  type = Transient
  num_steps = 1  
  dt = 1.0
[]

[Outputs]
  exodus = true
  console = true
[]