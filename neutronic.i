# 基本网格和材料设置
[Mesh]
  [gmg]
    type = GeneratedMeshGenerator  
    dim = 3
    nx = 4
    ny = 4
    nz = 4
  []
[]

[Controls]
  [coupling_control]
    type = ReactorCouplingControl
    
    calc_type = COUPLED
    
    burn_step = 1            
    max_burn_steps = 2     
    
    neutronics_app = neutronics 
    thermal_app = thermal
    #from_neutronics_transfers = from_neutronics
    execute_on = 'TIMESTEP_BEGIN'   # 在每个时间步开始时执行Control
  []
[]

[MultiApps]
  [neutronics]
    type = NeutronicsMultiApp
    app_type = MooseprojectsApp
    input_files = 'scapn_input.i'
    mesh_dims = '4 4 4'          
    power_var_name = power_density  
    temperature_var_name = temperature  
    execute_on = 'NEUTRONIC PRENEUTRONIC CORNEUTRONIC MULTIAPP_FIXED_POINT_BEGIN'     
  []
  
  [thermal]
    type = ThermalMultiApp
    app_type = MooseprojectsApp
    input_files = 'thermal_input.i'
    mesh_dims = '4 4 4'           
    power_var_name = power_density  
    temperature_var_name = temperature  
    execute_on = 'THERMAL MULTIAPP_FIXED_POINT_BEGIN'            
  []
[]

[Transfers]
  [from_neutronics]
    type = ReactorTransfer
    from_multi_app = neutronics
    source_variable = power_density
    variable = power_density
    #execute_on = 'FROM_NEUTRONIC'       
  []
[]
# [Transfers]
#   [from_neutronics1]
#     type = MultiAppCopyTransfer
#     from_multi_app = neutronics
#     source_variable = power_density
#     variable = power_density
#     #execute_on = 'FROM_NEUTRONIC'       
#   []
# []

[Transfers]
  [from_thermal]
    type = MultiAppCopyTransfer
    from_multi_app = thermal
    source_variable = temperature
    variable = temperature
    #execute_on = 'FROM_NEUTRONIC TIMESTEP_END'       
  []
[]

[Transfers]
  [to_neutronics]
    type = MultiAppCopyTransfer
    to_multi_app = neutronics
    source_variable = temperature
    variable = temperature
    #execute_on = 'FROM_NEUTRONIC TIMESTEP_END'       
  []
[]

[Transfers]
  [to_thermal]
    type = MultiAppCopyTransfer
    to_multi_app = thermal
    source_variable = power_density
    variable = power_density
    #execute_on = 'FROM_NEUTRONIC TIMESTEP_END'       
  []
[]


[Kernels]
  [dummy]
    type = Diffusion
    variable = dummy
  []
[]

[Variables]
  [dummy]
    family = LAGRANGE
    order = FIRST
  []
[]

[AuxVariables]
  [power_density]
    #order = CONSTANT
    #family = MONOMIAL
  []
[]
[AuxVariables]
  [temperature]
    #order = CONSTANT
    #family = MONOMIAL
  []
[]

[Problem]
  type = FEProblem
  solve = False
[]

[Executioner]
  type = Transient
  
  start_time = 0
  end_time = 2           
  dt = 1.0               
  
  # num_steps = 10          
  
  # # 求解设置
  # solve_type = 'PJFNK'
  # nl_abs_tol = 1e-6
  # nl_rel_tol = 1e-6
  # l_tol = 1e-6
  # l_max_its = 100
  # nl_max_its = 50
[]

[Postprocessors]

  [total_power]
    type = ElementIntegralVariablePostprocessor
    variable = power_density
    execute_on = 'TIMESTEP_END TRANSFER' 
  []
  
  [avg_power]
    type = ElementAverageValue
    variable = power_density
    execute_on = 'TIMESTEP_END TRANSFER'
  []
  
  [max_power]
    type = ElementExtremeValue
    variable = power_density
    value_type = max
    execute_on = 'TIMESTEP_END TRANSFER'
  []
  
  [min_power]
    type = ElementExtremeValue
    variable = power_density
    value_type = min
    execute_on = 'TIMESTEP_END TRANSFER'
  []
  
  # [total_temperature]
  #   type = ElementIntegralVariablePostprocessor
  #   variable = temperature
  #   execute_on = 'TIMESTEP_END TRANSFER' 
  # []
  # 
  # [avg_temperature]
  #   type = ElementAverageValue
  #   variable = temperature
  #   execute_on = 'TIMESTEP_END TRANSFER'
  # []
  # 
  # [max_temperature]
  #   type = ElementExtremeValue
  #   variable = temperature
  #   value_type = max
  #   execute_on = 'TIMESTEP_END TRANSFER'
  # []
  # 
  # [min_temperature]
  #   type = ElementExtremeValue
  #   variable = temperature
  #   value_type = min
  #   execute_on = 'TIMESTEP_END TRANSFER'
  # []
[]

[Outputs]
  console = true
  csv = true
  exodus = true
  
  [console]
    type = Console
    output_screen = true
    execute_postprocessors_on = 'TIMESTEP_END'
  []
  
  [csv_out]
    type = CSV
    execute_on = 'TIMESTEP_END'  
  []
  
  [exodus_out]
    type = Exodus
    execute_on = 'TIMESTEP_END'
  []
[]