[Mesh]
  # Generate a 2D mesh for the reactor model
  # 为反应堆模型生成二维网格
  type = GeneratedMesh
  dim = 2
  nx = 20
  ny = 20
  xmax = 100
  ymax = 100
[]

[Variables]
  # Define primary variables for the neutronics calculation
  # 定义中子学计算的主要变量
  [flux]
    # Neutron flux variable
    # 中子通量变量
    order = FIRST
    family = LAGRANGE
    initial_condition = 1.0e14  # Initial neutron flux (n/cm²·s)
                                # 初始中子通量 (n/cm²·s)
  []
  [power]
    # Power density variable
    # 功率密度变量
    order = FIRST
    family = LAGRANGE
    initial_condition = 1.0e6   # Initial power density (W/cm³)
                                # 初始功率密度 (W/cm³)
  []
[]

[AuxVariables]
  # Define auxiliary variables for tracking simulation data
  # 定义用于跟踪模拟数据的辅助变量
  [burnup]
    # Burnup tracking variable
    # 燃耗跟踪变量
    order = FIRST
    family = LAGRANGE
    initial_condition = 0.0     # Initial burnup (MWd/kgU)
                                # 初始燃耗 (MWd/kgU)
  []
  [k_eff]
    # Effective multiplication factor
    # 有效增殖因子
    order = CONSTANT
    family = MONOMIAL
    initial_condition = 1.0     # Initial k-effective value
                                # 初始有效增殖因子值
  []
[]

[Problem]
  # Problem configuration
  # 问题配置
  type = FEProblem
  coord_type = RZ                # Use cylindrical coordinates
                                 # 使用柱坐标系
  kernel_coverage_check = false  # Disable kernel coverage check
                                 # 关闭内核覆盖检查
[]

[Functions]
  # Define functions for initial and boundary conditions
  # 定义用于初始和边界条件的函数
  [power_distribution]
    # Analytical function for initial power distribution
    # 初始功率分布的解析函数
    type = ParsedFunction
    value = '1.0e6*sin(pi*x/100)*sin(pi*y/100)'  # Sinusoidal power profile
                                                 # 正弦功率分布
  []
[]

[ICs]
  # Initial conditions for variables
  # 变量的初始条件
  [power_ic]
    # Initial condition for power density based on function
    # 基于函数的功率密度初始条件
    type = FunctionIC
    variable = power
    function = power_distribution
  []
[]

[MultiApps]
  # Define sub-applications (modules) to be executed
  # 定义要执行的子应用程序（模块）
  [a1]
    # First preprocessing module (always executed)
    # 第一个预处理模块（始终执行）
    type = TransientMultiApp
    input_files = a1_preprocess.i   # Neutronics preprocessing
                                    # 中子学前处理
    execute_on = 'NONE'             # Controlled by SimpleControl
                                    # 由SimpleControl控制
  []
  [a2]
    # Second preprocessing module (subsequent burnup steps only)
    # 第二个预处理模块（仅后续燃耗步）
    type = TransientMultiApp
    input_files = a2_preprocess.i   # Second stage preprocessing
                                    # 第二阶段前处理
    execute_on = 'NONE'
  []
  [a3]
    # Third preprocessing module (subsequent burnup steps only)
    # 第三个预处理模块（仅后续燃耗步）
    type = TransientMultiApp
    input_files = a3_preprocess.i   # Third stage preprocessing
                                    # 第三阶段前处理
    execute_on = 'NONE'
  []
  [b1]
    # Neutronics calculation module
    # 中子学计算模块
    type = TransientMultiApp
    input_files = b1_neutronics.i   # Neutronics calculation
                                    # 中子学计算
    execute_on = 'NONE'
  []
  [c1]
    # Postprocessing module
    # 后处理模块
    type = TransientMultiApp
    input_files = c1_postprocess.i  # Results postprocessing
                                    # 结果后处理
    execute_on = 'NONE'
  []
[]

[Transfers]
  # Define data transfers between main application and sub-applications
  # 定义主应用程序和子应用程序之间的数据传输
  [flux_to_b1]
    # Transfer neutron flux to neutronics module
    # 将中子通量传输到中子学模块
    type = MultiAppCopyTransfer
    to_multi_app = b1
    source_variable = flux
    variable = flux
    execute_on = 'NONE'
  []
  [power_from_b1]
    # Transfer power from neutronics module
    # 从中子学模块传输功率
    type = MultiAppCopyTransfer
    from_multi_app = b1
    source_variable = power
    variable = power
    execute_on = 'NONE'
  []
  [burnup_to_b1]
    # Transfer burnup to neutronics module
    # 将燃耗传输到中子学模块
    type = MultiAppCopyTransfer
    to_multi_app = b1
    source_variable = burnup
    variable = burnup
    execute_on = 'NONE'
  []
[]

[UserObjects]
  [burnup_counter]
    # Counter for tracking burnup steps
    # 用于跟踪燃耗步的计数器
    type = Counter
    initial_value = 0
  []
[]

[Controls]
  [burnup_control]
    # Controller for burnup steps execution
    # 燃耗步执行控制器
    type = SimpleControl
    calc_type = 1                  # 1=neutronics only (仅中子学)
    steps = 3                      # Number of burnup steps (燃耗步数)
    counter = burnup_counter/value
    a1_app = a1
    a2_app = a2
    a3_app = a3
    b1_app = b1
    c1_app = c1
    transfers = 'flux_to_b1 power_from_b1 burnup_to_b1'
    execute_on = 'initial timestep_begin'
  []
[]

[Executioner]
  type = Transient
  num_steps = 3         # Number of time steps = number of burnup steps 
                        # 时间步数与燃耗步数相同
  dt = 1                # Time step size
                        # 时间步长
  solve_type = NEWTON
  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre boomeramg'
[]

[Outputs]
  exodus = true
  csv = true
  console = true
  [console]
    type = Console
    output_screen = true
    output_file = false
    verbose = true      # Output detailed information
                        # 输出详细信息
  []
[] 