!
! Author: lvjiahui eba424@163.com
! Date: 2025-09-11 11:32:37
! LastEditors: lvjiahui eba424@163.com
! LastEditTime: 2025-10-24 13:41:39
! FilePath: /mooseprojects/contrib/tran_heat/src/reactor_interfaces.f90
! Description: 
!

module reactor_interfaces
  use iso_c_binding
  use omp_lib
  implicit none
  
  ! 包含MPI头文件，避免模块冲突
  include 'mpif.h'
  
  INTEGER, SAVE :: times  = 1 ! 时间步计数器0
  INTEGER, SAVE :: times0 = 1  ! 时间步计数器
  LOGICAL, SAVE :: omp_initialized = .FALSE.  ! OpenMP初始化标志
  
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
      
      ! 安全检查
      if (field_size <= 0 .or. field_size > 100000) return
      
      ! 简单串行计算
      do i = 1, field_size
        call random_number(random_value)
        power_field(i) = 300.0_c_double + 10.0_c_double * random_value
      end do
      
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

      ! 安全检查
      if (field_size <= 0 .or. field_size > 100000) return
          
      ! 简单串行计算
      do i = 1, field_size
        call random_number(random_value)
        temperature_field(i) = 300.0 + 0.01 * power_field(i) * random_value
      end do
      
    end subroutine

    subroutine update_burnup_step(step, max_steps) bind(C, name="update_burnup_step")
        use iso_c_binding, only: c_int
        
        integer(c_int), intent(in), value :: step
        integer(c_int), intent(in), value :: max_steps

    end subroutine update_burnup_step
    
    subroutine update_burnup_detailed(step, max_steps, burnup_value, power, time_step) bind(C, name="update_burnup_detailed")
        use iso_c_binding, only: c_int, c_double
        
        integer(c_int), intent(in), value :: step
        integer(c_int), intent(in), value :: max_steps
        real(c_double), intent(in), value :: burnup_value
        real(c_double), intent(in), value :: power
        real(c_double), intent(in), value :: time_step
        
        ! 仅在特定步骤输出（例如每100步）
        if (MOD(step, 100) == 0 .or. step == max_steps) then
          write(*,'(A,I0,A,I0,A,F8.2,A,F8.2)') "Fortran燃耗: 步骤=", step, "/", max_steps, &
                ", 燃耗=", burnup_value, ", 功率=", power
        endif
        
    end subroutine update_burnup_detailed

    ! 简化的MPI并行版本（已废弃，建议使用 b1_execute_3d_mpi）
    subroutine b1_execute_3d(mesh_dims, power_field1, power_field2, temperature_field, field_size) &
            bind(C, name="b1_execute_3d")
      use iso_c_binding, only: c_int, c_double
      implicit none
    
      integer(c_int), intent(in) :: mesh_dims(3)
      real(c_double), intent(inout) :: power_field1(*)    
      real(c_double), intent(inout) :: power_field2(*)    
      real(c_double), intent(in) :: temperature_field(*)
      integer(c_int), intent(in), value :: field_size
      
      integer :: rank, size, ierr, i
      real(8) :: random
      
      ! 获取MPI信息
      call MPI_COMM_RANK(MPI_COMM_WORLD, rank, ierr)
      if (ierr /= MPI_SUCCESS) rank = 0
      
      call MPI_COMM_SIZE(MPI_COMM_WORLD, size, ierr)
      if (ierr /= MPI_SUCCESS) size = 1
      
      ! 安全检查
      if (field_size <= 0 .or. field_size > 100000) then
        if (rank == 0) write(*,*) "ERROR: Invalid field_size"
        return
      endif
      
      ! 串行计算
      do i = 1, field_size
        call random_number(random)
        power_field1(i) = 100.0D0 + 10 * random + 10 * times + rank * 5.0D0
        call random_number(random)
        power_field2(i) = 500.0D0 + 10 * random + 10 * times + rank * 15.0D0
      end do
      
      ! MPI同步
      if (size > 1) call MPI_BARRIER(MPI_COMM_WORLD, ierr)
    
      times = times + 1
    end subroutine b1_execute_3d

    ! 推荐的MPI+OpenMP混合并行版本
    ! MOOSE已经完成数据分割，直接处理传入的局部数据
    subroutine b1_execute_3d_mpi(mesh_dims, power_field1, power_field2, temperature_field, field_size) &
              bind(C, name="b1_execute_3d_mpi")
      use iso_c_binding, only: c_int, c_double, C_INTPTR_T
      implicit none
      
      ! Arguments
      integer(c_int), intent(in)            :: mesh_dims(3)
      real(c_double), intent(inout)         :: power_field1(*)
      real(c_double), intent(inout)         :: power_field2(*)
      real(c_double), intent(in)            :: temperature_field(*)
      integer(c_int), intent(in), value     :: field_size
  
      ! MPI variables
      integer :: rank, size, ierr
      integer :: comm_f
      integer :: i
      integer :: num_threads, thread_id
      character(len=32) :: omp_env
      
      ! 简化：直接使用MPI_COMM_WORLD
      comm_f = MPI_COMM_WORLD
      
      ! 获取MPI信息
      call MPI_COMM_RANK(comm_f, rank, ierr)
      if (ierr /= MPI_SUCCESS) then
         write(*,*) "ERROR: MPI_COMM_RANK 失败!"
         return
      endif
      
      call MPI_COMM_SIZE(comm_f, size, ierr)
      if (ierr /= MPI_SUCCESS) then
         write(*,*) "ERROR: MPI_COMM_SIZE 失败!"
         return
      endif
      
      ! OpenMP初始化检查（仅第一次执行时输出）
      if (.NOT. omp_initialized) then
         ! 从环境变量读取线程数并强制设置
         call get_environment_variable("OMP_NUM_THREADS", omp_env)
         
         if (LEN_TRIM(omp_env) > 0) then
            read(omp_env, *) num_threads
            ! 强制设置线程数（覆盖 MPI 的限制）
            call omp_set_num_threads(num_threads)
         endif
         
         !$OMP PARALLEL DEFAULT(SHARED) PRIVATE(thread_id)
         thread_id = omp_get_thread_num()
         !$OMP MASTER
         num_threads = omp_get_num_threads()
         if (rank == 0) then
            write(*,'(A)') "========================================="
            write(*,'(A)') "  Fortran OpenMP 并行配置验证"
            write(*,'(A)') "========================================="
            write(*,'(A,I0)') "  MPI 进程数: ", size
            write(*,'(A,I0)') "  OpenMP 实际线程数: ", num_threads
            if (LEN_TRIM(omp_env) > 0) then
               write(*,'(A,A)') "  OMP_NUM_THREADS 环境变量: ", TRIM(omp_env)
            else
               write(*,'(A)') "  OMP_NUM_THREADS 环境变量: (未设置)"
            endif
            if (num_threads > 1) then
               write(*,'(A)') " OpenMP 并行已启用"
            else
               write(*,'(A)') " OpenMP 未启用 (单线程模式)"
               write(*,'(A)') "  提示: 运行前设置 export OMP_NUM_THREADS=N"
            endif
            write(*,'(A)') "========================================="
         endif
         !$OMP END MASTER
         !$OMP END PARALLEL
         omp_initialized = .TRUE.
      endif
      
      ! 安全检查
      if (field_size <= 0) then
        if (rank == 0) write(*,*) "ERROR: Invalid field_size"
        return
      endif

      ! OpenMP并行计算
      !$OMP PARALLEL DO DEFAULT(SHARED) PRIVATE(i) SCHEDULE(STATIC)
      do i = 1, field_size
         power_field1(i) = 100.0D0 + i * 10.0D0 + rank * 1000.0D0 + temperature_field(i)
         power_field2(i) = 500.0D0 + i * 10.0D0 + rank * 1000.0D0 + temperature_field(i)
      end do
      !$OMP END PARALLEL DO
      
      ! MPI同步
      call MPI_BARRIER(comm_f, ierr)
      
      ! 简洁的进度输出（仅rank 0，每10步输出一次）
      if (rank == 0 .AND. MOD(times, 10) == 1) then
        write(*,'(A,I0,A,I0,A)') "Fortran计算: 时间步=", times, ", 数据点=", field_size, " ✓"
      endif
      
      ! 增加时间步计数器
      times = times + 1
      
    end subroutine b1_execute_3d_mpi

end module reactor_interfaces