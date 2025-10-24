# 基本网格和材料设置
[Mesh]
  [gmg]
    type = GeneratedMeshGenerator  
    dim = 3
    nx = 3
    ny = 4
    nz = 5
    xmin = 0.0
    xmax = 3.0
    ymin = 0.0
    ymax = 4.0
    zmin = 0.0
    zmax = 5.0
  []
[]

[UserObjects]
  [coupling_control]
    type = ReactorCouplingUserObject
    
    calc_type = COUPLED
    
    burn_step = 1            
    max_burn_steps = 3   
    
    neutronics_app = neutronics 
    thermal_app = thermal
        
    execute_on = 'TIMESTEP_BEGIN'
  []
[]

[MultiApps]
  [neutronics]
    type = NeutronicsMultiApp
    app_type = MooseprojectsApp
    input_files = 'scapn_input.i'
    mesh_dims = '3 4 5'          
    power_var_name1 = power_density
    power_var_name2 = tempe_density 
    temperature_var_name = temperature  
    execute_on = 'NEUTRONIC PRENEUTRONIC CORNEUTRONIC'
    
    # MPI进程分配设置
    max_procs_per_app = 20          # 分配给每个子应用的最大进程数
    # min_procs_per_app = 4           # 分配给每个子应用的最小进程数
    
    # execute_on = 'MULTIAPP_FIXED_POINT_BEGIN' 
  []
  
  [thermal]
    type = ThermalMultiApp
    app_type = MooseprojectsApp
    input_files = 'thermal_input.i'
    mesh_dims = '3 4 5'           
    power_var_name = power_density  
    temperature_var_name = temperature  
    execute_on = 'THERMAL'            
    # execute_on = 'MULTIAPP_FIXED_POINT_BEGIN'
    max_procs_per_app = 1          # 分配给每个子应用的最大进程数
    # min_procs_per_app = 1           # 分配给每个子应用的最小进程数
  []
[]

# [Transfers]
#   [from_neutronics]
#     type = ReactorTransfer
#     from_multi_app = neutronics
#     source_variable = power_density
#     variable = power_density
#     #execute_on = 'FROM_NEUTRONIC'       
#   []
# []
# [Transfers]
#   [from_neutronics1]
#     type = MultiAppCopyTransfer
#     from_multi_app = neutronics
#     source_variable = power_density
#     variable = power_density
#     #execute_on = 'FROM_NEUTRONIC'       
#   []
# []

#[Transfers]
#  [from_thermal]
#    type = MultiAppCopyTransfer
#    from_multi_app = thermal
#    source_variable = temperature
#    variable = temperature
#    #execute_on = 'FROM_NEUTRONIC TIMESTEP_END'       
#  []
#[]

# [Transfers]
#   [to_neutronics]
#     type = MultiAppCopyTransfer
#     to_multi_app = neutronics
#     source_variable = temperature
#     variable = temperature
#     #execute_on = 'FROM_NEUTRONIC TIMESTEP_END'       
#   []
# []

[Transfers]
  [from_neutronics]
    type = MultiAppCopyTransfer
    from_multi_app = neutronics
    source_variable = 'power_density tempe_density'
    variable = 'power_density tempe_density'
    # execute_on = 'FROM_NEUTRONIC'
  []
  # [from_neutronics_power2]
  #   type = ReactorTransfer
  #   from_multi_app = neutronics
  #   source_variable = tempe_density
  #   variable = tempe_density
  #   # execute_on = 'FROM_NEUTRONIC'
  # []
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
    family = MONOMIAL
    order = CONSTANT
    # family = LAGRANGE
    # order = FIRST
    # initial_condition = 0.0
  []
  [tempe_density]
    family = MONOMIAL
    order = CONSTANT
    # family = LAGRANGE
    # order = FIRST
    # initial_condition = 0.0
  []
[]
[AuxVariables]
  [temperature]
    # family = MONOMIAL
    # order = CONSTANT
    family = LAGRANGE
    order = FIRST
    # initial_condition = 200.0
  []
[]


[Problem]
  type = FEProblem
  solve = False
[]

[Executioner]
  type = Transient
  
  start_time = 0
  end_time   = 1        
  dt = 1.00          
  
  # num_steps = 10          
  
  # # 求解设置
  # solve_type = 'PJFNK'
  # nl_abs_tol = 1e-6
  # nl_rel_tol = 1e-6
  # l_tol = 1e-6
  # l_max_its = 100
  # nl_max_its = 50

  # fixed_point_max_its = 5
  # fixed_point_min_its = 3
  # fixed_point_abs_tol = 1e-6
  # accept_on_max_fixed_point_iteration = true
  num_threads = 2
[]

[Postprocessors]

  [total_power1]
    type = ElementIntegralVariablePostprocessor
    variable = power_density
  []
  
  [max_power1]
    type = ElementExtremeValue
    variable = power_density
    value_type = max
  []
  
  [min_power1]
    type = ElementExtremeValue
    variable = power_density
    value_type = min
  []

  [total_power2]
    type = ElementIntegralVariablePostprocessor
    variable = tempe_density
  []
  
  [max_power2]
    type = ElementExtremeValue
    variable = tempe_density
    value_type = max
  []
  
    [min_power2]
    type = ElementExtremeValue
    variable = tempe_density
    value_type = min
  []
  
[]


[Outputs]
  exodus = true
  csv = true
#  [vtk]
#    type = VTK
#    #interval = 1
#  []
#  [exodus_out]
#    type = Exodus
#    execute_on = 'TIMESTEP_END'
#  []
[]
