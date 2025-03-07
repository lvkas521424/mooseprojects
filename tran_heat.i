# 使用迭代稳态执行器的Fortran热传导输入文件

[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 20
  ny = 20
[]

[Variables]
  [temperature]
    initial_condition = 300.0
  []
[]

[Kernels]
  [null_kernel]
    type = NullKernel
    variable = temperature
  []
[]

[BCs]
  [all_sides]
    type = NeumannBC
    variable = temperature
    boundary = 'left right top bottom'
    value = 0
  []
[]

[Materials]
  [simple]
    type = GenericConstantMaterial
    prop_names = 'thermal_conductivity'
    prop_values = '1.0'
  []
[]

[MultiApps]
  [fortran_app]
    type = FortranHeatTransferApp
    execute_on = 'final'  # 在每次稳态求解后执行
    positions = '0 0 0'
    input_files = sub_app.i
    
    # Fortran参数
    nx = 20
    ny = 20
    thermal_conductivity = 45.0
    dx = 0.05
    dy = 0.05
  []
[]

[Transfers]
  [to_sub]
    type = MultiAppCopyTransfer
    to_multi_app = fortran_app
    source_variable = temperature
    variable = temperature
    execute_on = 'final'
  []
  [from_sub]
    type = MultiAppCopyTransfer
    from_multi_app = fortran_app
    source_variable = temperature
    variable = temperature
    execute_on = 'final'
  []
[]

# 使用新的迭代稳态执行器
[Executioner]
  type = IterativeSteadyExecutioner
  max_iterations = 15      # 第二个控制参数：最大迭代次数
  calc_type = 1            # 第一个控制参数：计算类型
  
  # 常规稳态求解器设置
  solve_type = PJFNK
  petsc_options_iname = '-pc_type'
  petsc_options_value = 'lu'
  nl_abs_tol = 1e-8
  nl_rel_tol = 1e-6
[]

[Outputs]
  exodus = true
  console = true
  [out]
    type = Console
    execute_on = 'final'
    verbose = true  # 使用verbose参数获取详细输出
  []
[]