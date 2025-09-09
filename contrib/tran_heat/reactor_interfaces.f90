!****************************************************************!
!*             reactor_interfaces - Reactor Interface Module    *!
!*                                                              *!
!*              Interface to External Fortran Programs          *!
!*     reactor_interfaces - 堆接口模块，与外部Fortran程序交互   *!
!*              简化版本，逐步添加MPI并行支持                    *!
!****************************************************************!

module reactor_interfaces
  use iso_c_binding
  implicit none
  
  ! 包含MPI头文件，避免模块冲突
  include 'mpif.h'
  
  INTEGER, SAVE :: times  = 1
  INTEGER, SAVE :: times0 = 1

  ! Declare external interfaces
  ! 声明外部接口
  CONTAINS

    subroutine b1_execute(mesh_dims, power_field, temperature_field, field_size) &
            bind(C, name="b1_execute")
      use iso_c_binding, only: c_int, c_double
    
      integer(c_int), intent(in) :: mesh_dims(3)
      real(c_double), intent(out) :: power_field(*)    
      real(c_double), intent(in) :: temperature_field(*)
      integer(c_int), intent(in) , value :: field_size
    
      real(c_double) :: random_value
      integer :: i
      
      write(*,*) "Executing b1 calculation (no MPI)..."
      write(*,*) "  Mesh dimensions: ", mesh_dims
      write(*,*) "  Field size: ", field_size
      
      ! 安全检查
      if (field_size <= 0 .or. field_size > 100000) then
        write(*,*) "ERROR: Invalid field_size: ", field_size
        return
      endif
      
      ! 简单串行计算
      do i = 1, field_size
        call random_number(random_value)
        power_field(i) = 300.0_c_double + 10.0_c_double * random_value
      end do
      
      write(*,*) "B1 calculation completed"
      return 
    end subroutine

    subroutine thermal_execute(mesh_dims, power_field, temperature_field, field_size) &
      bind(C, name="thermal_execute")
      use iso_c_binding, only: c_int, c_double
          
      integer(c_int), intent(in)  :: mesh_dims(3)
      real(c_double), intent(in)  :: power_field(*)    
      real(c_double), intent(out) :: temperature_field(*)
      integer(c_int), intent(in) , value :: field_size
      integer :: i
      real(c_double) :: random_value

      write(*,*) "Executing thermal calculation (no MPI)..."
      write(*,*) "  Mesh dimensions: ", mesh_dims
      write(*,*) "  Field size: ", field_size
      
      ! 安全检查
      if (field_size <= 0 .or. field_size > 100000) then
        write(*,*) "ERROR: Invalid field_size: ", field_size
        return
      endif
          
      ! 简单串行计算
      do i = 1, field_size
        call random_number(random_value)
        temperature_field(i) = 300.0 + 0.01 * power_field(i) * random_value
      end do
      
      write(*,*) "Thermal calculation completed"
      return
    end subroutine

    subroutine update_burnup_step(step, max_steps) bind(C, name="update_burnup_step")
        use iso_c_binding, only: c_int
        
        integer(c_int), intent(in), value :: step
        integer(c_int), intent(in), value :: max_steps
        
        ! write(*,*) "Fortran: 更新燃耗步 (Updating burnup step)", step, "/", max_steps
        ! write(*,*) "Fortran: 燃耗步已更新 (Burnup step updated)"
        
    end subroutine update_burnup_step
    
    subroutine update_burnup_detailed(step, max_steps, burnup_value, power, time_step) bind(C, name="update_burnup_detailed")
        use iso_c_binding, only: c_int, c_double
        
        integer(c_int), intent(in), value :: step
        integer(c_int), intent(in), value :: max_steps
        real(c_double), intent(in), value :: burnup_value
        real(c_double), intent(in), value :: power
        real(c_double), intent(in), value :: time_step
        
        write(*,*) "Fortran: 更新详细燃耗信息 (Updating detailed burnup info)"
        write(*,*) "  燃耗值: ", burnup_value
        write(*,*) "  功率: ", power
        write(*,*) "  时间步: ", time_step
        
    end subroutine update_burnup_detailed

    ! 简化的MPI并行版本 - 逐步调试
    subroutine b1_execute_3d(mesh_dims, power_field1, power_field2, temperature_field, field_size) &
            bind(C, name="b1_execute_3d")
      use iso_c_binding, only: c_int, c_double
      implicit none
    
      integer(c_int), intent(in) :: mesh_dims(3)
      real(c_double), intent(inout) :: power_field1(*)    
      real(c_double), intent(inout) :: power_field2(*)    
      real(c_double), intent(in) :: temperature_field(*)
      integer(c_int), intent(in), value :: field_size
      
      ! MPI变量
      integer :: rank, size, ierr
      integer :: i
      real(8) :: random
      
      write(*,*) "Fortran: 函数入口 - 开始获取MPI信息"
      
      ! 第一步：安全地获取MPI信息
      rank = 0
      size = 1
      ierr = 0
      
      ! 小心地调用MPI函数
      call MPI_COMM_RANK(MPI_COMM_WORLD, rank, ierr)
      if (ierr /= MPI_SUCCESS) then
        write(*,*) "WARNING: MPI_COMM_RANK failed, using rank=0"
        rank = 0
      endif
      
      call MPI_COMM_SIZE(MPI_COMM_WORLD, size, ierr)
      if (ierr /= MPI_SUCCESS) then
        write(*,*) "WARNING: MPI_COMM_SIZE failed, using size=1"
        size = 1
      endif
      
      write(*,*) "Fortran: MPI信息获取成功 - rank = ", rank, ", size = ", size
      
      if (rank == 0) then
        write(*,*) "Fortran: 开始简化MPI并行3D计算"
        write(*,*) "  mesh_dims = ", mesh_dims
        write(*,*) "  field_size = ", field_size
        write(*,*) "  expected_size = ", mesh_dims(1) * mesh_dims(2) * mesh_dims(3)
        write(*,*) "  MPI processes = ", size
      endif
      
      ! 安全检查
      if (field_size <= 0) then
        if (rank == 0) write(*,*) "ERROR: Invalid field_size: ", field_size
        return
      endif
      
      if (field_size > 100000) then
        if (rank == 0) write(*,*) "ERROR: field_size too large: ", field_size
        return
      endif
      
      ! 简化版本：暂时使用串行计算，确保基本功能正常
      if (rank == 0) write(*,*) "开始串行计算（简化版本）..."
      
      do i = 1, field_size
        call random_number(random)
        power_field1(i) = 100.0D0 + 10 * random + 10 * times + rank * 5.0D0
        
        call random_number(random)
        power_field2(i) = 500.0D0 + 10 * random + 10 * times + rank * 15.0D0
        
        ! 输出前几个值
        if (rank == 0 .and. i <= 3) then
          write(*,*) "power_field1(", i, ") = ", power_field1(i)
          write(*,*) "power_field2(", i, ") = ", power_field2(i)
        endif
      end do
      
      ! 简单的MPI同步测试
      if (size > 1) then
        if (rank == 0) write(*,*) "进行MPI barrier同步测试..."
        call MPI_BARRIER(MPI_COMM_WORLD, ierr)
        if (ierr == MPI_SUCCESS) then
          if (rank == 0) write(*,*) "MPI barrier同步成功"
        else
          if (rank == 0) write(*,*) "MPI barrier同步失败，ierr = ", ierr
        endif
      endif
      
      if (rank == 0) then
        write(*,*) "Fortran: 简化MPI并行3D计算完成"
        write(*,*) "  power_field1(1) = ", power_field1(1)
        write(*,*) "  power_field2(1) = ", power_field2(1)
        if (field_size > 0) then
          write(*,*) "  power_field1(field_size) = ", power_field1(field_size)
          write(*,*) "  power_field2(field_size) = ", power_field2(field_size)
        endif
      endif
    
      times = times + 1
    end subroutine b1_execute_3d

    ! 推荐的MPI并行版本 - 接收来自C++的Communicator
    ! MOOSE已经完成数据分割，直接处理传入的局部数据
    !subroutine b1_execute_3d_mpi(comm_c, mesh_dims, power_field1, power_field2, temperature_field, field_size) &
    !        bind(C, name="b1_execute_3d_mpi")
    subroutine b1_execute_3d_mpi(mesh_dims, power_field1, power_field2, temperature_field, field_size) &
              bind(C, name="b1_execute_3d_mpi")
      use iso_c_binding, only: c_int, c_double, C_INTPTR_T
      implicit none
      
      ! Arguments
      !integer(C_INTPTR_T), intent(in), value :: comm_c         ! C MPI Communicator handle
      integer(c_int), intent(in)            :: mesh_dims(3)
      real(c_double), intent(inout)         :: power_field1(*)
      real(c_double), intent(inout)         :: power_field2(*)
      real(c_double), intent(in)            :: temperature_field(*)
      integer(c_int), intent(in), value     :: field_size
  
      ! MPI variables
      integer :: rank, size, ierr
      integer :: comm_f  ! Fortran MPI Communicator handle
      integer :: i
      real(8) :: random
      
      ! 简化：直接使用MPI_COMM_WORLD，避免通信器转换问题
      comm_f = MPI_COMM_WORLD
      
      ! 获取MPI信息
      call MPI_COMM_RANK(comm_f, rank, ierr)
      if (ierr /= MPI_SUCCESS) then
         write(*,*) "FATAL ERROR: MPI_COMM_RANK 失败!"
         return
      endif
      
      call MPI_COMM_SIZE(comm_f, size, ierr)
      if (ierr /= MPI_SUCCESS) then
         write(*,*) "FATAL ERROR: MPI_COMM_SIZE 失败!"
         return
      endif
      
      ! 全局同步点1：确保所有进程同时开始计算
      call MPI_BARRIER(comm_f, ierr)
      
      ! 输出当前进程信息
      write(*,*) "Fortran rank ", rank, ": 处理局部数据, field_size=", field_size, ", 时间步=", times
      write(*,*) "Fortran MPI: 使用MPI_COMM_WORLD, rank=", rank, ", size=", size

      ! 安全检查
      if (field_size <= 0) then
        write(*,*) "ERROR rank ", rank, ": Invalid field_size: ", field_size
        return
      endif

      ! 直接处理MOOSE传入的局部数据，无需再次分割
      ! 每个进程处理自己分配到的所有数据点
      do i = 1, field_size
         ! 基于当前进程rank和索引计算功率场
         power_field1(i) = 100.0D0 + i * 10.0D0 + rank * 1000.0D0 +temperature_field(i)
         power_field2(i) = 500.0D0 + i * 10.0D0 + rank * 1000.0D0 +temperature_field(i)
      end do
      
      ! 全局同步点2：确保所有进程完成计算
      call MPI_BARRIER(comm_f, ierr)
      if (ierr /= MPI_SUCCESS) then
         write(*,*) "WARNING rank ", rank, ": MPI_BARRIER失败!"
      endif
      
      ! 输出计算完成信息（每个进程都输出自己的结果）
      if (field_size > 0) then
        write(*,*) "Fortran rank ", rank, " 计算完成: power1[1]=", power_field1(1), &
                   ", power2[1]=", power_field2(1), ", 数据点数=", field_size
      endif
      
      ! 增加时间步计数器
      times = times + 1
      
    end subroutine b1_execute_3d_mpi

end module reactor_interfaces