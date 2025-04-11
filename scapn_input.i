
[Mesh]
  [gmg]
    type = GeneratedMeshGenerator  
    dim = 3
    nx = 4
    ny = 4
    nz = 4
  []
[]


[AuxVariables]
  [temperature]
    #family = MONOMIAL
    #order = CONSTANT
    #initial_condition = 300.0
  []
[]
[Variables]
  [power_density]  
    #family = MONOMIAL
    #order = CONSTANT
    #initial_condition = 0.0
  []
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