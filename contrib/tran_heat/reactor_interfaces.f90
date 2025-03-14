!****************************************************************!
!*             reactor_interfaces - Reactor Interface Module    *!
!*                                                              *!
!*              Interface to External Fortran Programs          *!
!*     reactor_interfaces - 堆接口模块，与外部Fortran程序交互   *!
!****************************************************************!

module reactor_interfaces
  use mpi
  implicit none

  ! Declare external interfaces
  ! 声明外部接口
  interface
    ! a0 initialization interface
    ! a0初始化接口
    subroutine external_a0_init(comm, burnup_steps, burnup_days)
      integer, intent(in) :: comm
      integer, intent(out) :: burnup_steps
      integer, intent(out), pointer :: burnup_days(:)
    end subroutine external_a0_init
    
    ! a1 preprocessing interface
    ! a1预处理接口
    subroutine external_a1_preprocess(comm, burnup_days)
      integer, intent(in) :: comm
      integer, intent(in) :: burnup_days
    end subroutine external_a1_preprocess
    
    ! a2 preprocessing interface
    subroutine external_a2_preprocess(comm, burnup_days)
      integer, intent(in) :: comm
      integer, intent(in) :: burnup_days
    end subroutine external_a2_preprocess
    
    ! a3 preprocessing interface
    subroutine external_a3_preprocess(comm)
      integer, intent(in) :: comm
    end subroutine external_a3_preprocess
    
    ! b1 neutronics calculation interface
    subroutine external_b1_execute(comm, mesh_dims, temperature_field, power_field, field_size)
      integer, intent(in) :: comm
      integer, intent(in) :: mesh_dims(3)
      real(8), intent(in) :: temperature_field(*)
      real(8), intent(out) :: power_field(*)
      integer, intent(in) :: field_size
    end subroutine external_b1_execute
    
    ! b2 thermal calculation interface
    subroutine external_b2_execute(comm, mesh_dims, power_field, temperature_field, field_size)
      integer, intent(in) :: comm
      integer, intent(in) :: mesh_dims(3)
      real(8), intent(in) :: power_field(*)
      real(8), intent(out) :: temperature_field(*)
      integer, intent(in) :: field_size
    end subroutine external_b2_execute
    
    ! c1 postprocessing interface
    subroutine external_c1_postprocess(comm)
      integer, intent(in) :: comm
    end subroutine external_c1_postprocess
  end interface

contains

  !
  ! Initialization interface
  ! 初始化接口
  !
  subroutine a0_init(comm, burnup_steps, burnup_days)
    integer, intent(in) :: comm                  ! MPI communicator 通信器
    integer, intent(out) :: burnup_steps         ! Burnup steps 燃耗步数
    integer, intent(out), pointer :: burnup_days(:) ! Days for each burnup step 每步燃耗的天数
    
    ! Local variables
    ! 局部变量
    integer :: ierr, rank, size
    
    ! Get MPI information
    ! 获取MPI信息
    call MPI_COMM_RANK(comm, rank, ierr)
    call MPI_COMM_SIZE(comm, size, ierr)
    
    ! Initialize system
    ! 初始化系统
    if (rank == 0) then
      write(*,*) "Executing reactor system initialization..." ! 执行堆系统初始化...
    endif
    
    ! Call external a0 module to get burnup information
    ! 调用外部a0模块获取燃耗信息
    call external_a0_init(comm, burnup_steps, burnup_days)
    
    if (rank == 0) then
      write(*,*) "Burnup steps: ", burnup_steps  ! 燃耗步数：
      if (associated(burnup_days)) then
        write(*,*) "Days per step: ", burnup_days(1:burnup_steps)  ! 每步天数：
      else
        write(*,*) "Warning: burnup_days not successfully allocated"  ! 警告：燃耗天数未成功分配
      endif
      write(*,*) "Initialization complete"  ! 初始化完成
    endif
  end subroutine a0_init

  !
  ! a1 preprocessing interface
  !
  subroutine a1_preprocess(comm, step_number)
    integer, intent(in) :: comm        ! MPI communicator
    integer, intent(in) :: step_number ! Days for current burnup step
    
    ! Local variables
    integer :: ierr, rank, size
    
    ! Get MPI information
    call MPI_COMM_RANK(comm, rank, ierr)
    call MPI_COMM_SIZE(comm, size, ierr)
    
    ! Execute preprocessing
    if (rank == 0) then
      write(*,*) "Executing a1 preprocessing for ", step_number, " days..."
    endif
    
    ! Call external a1 module
    call external_a1_preprocess(comm, step_number)
    
    if (rank == 0) then
      write(*,*) "a1 preprocessing complete"
    endif
  end subroutine a1_preprocess

  !
  ! a2 preprocessing interface
  !
  subroutine a2_preprocess(comm, burnup_days)
    integer, intent(in) :: comm        ! MPI communicator
    integer, intent(in) :: burnup_days ! Days for current burnup step
    
    ! Local variables
    integer :: ierr, rank, size
    
    ! Get MPI information
    call MPI_COMM_RANK(comm, rank, ierr)
    call MPI_COMM_SIZE(comm, size, ierr)
    
    ! Execute preprocessing
    if (rank == 0) then
      write(*,*) "Executing a2 preprocessing for ", burnup_days, " days..."
    endif
    
    ! Call external a2 module
    call external_a2_preprocess(comm, burnup_days)
    
    if (rank == 0) then
      write(*,*) "a2 preprocessing complete"
    endif
  end subroutine a2_preprocess

  !
  ! a3 preprocessing interface
  !
  subroutine a3_preprocess(comm)
    integer, intent(in) :: comm ! MPI communicator
    
    ! Local variables
    integer :: ierr, rank, size
    
    ! Get MPI information
    call MPI_COMM_RANK(comm, rank, ierr)
    call MPI_COMM_SIZE(comm, size, ierr)
    
    ! Execute preprocessing
    if (rank == 0) then
      write(*,*) "Executing a3 preprocessing..."
    endif
    
    ! Call external a3 module
    call external_a3_preprocess(comm)
    
    if (rank == 0) then
      write(*,*) "a3 preprocessing complete"
    endif
  end subroutine a3_preprocess

  !
  ! c1 postprocessing interface
  !
  subroutine c1_postprocess(comm)
    integer, intent(in) :: comm ! MPI communicator
    
    ! Local variables
    integer :: ierr, rank, size
    
    ! Get MPI information
    call MPI_COMM_RANK(comm, rank, ierr)
    call MPI_COMM_SIZE(comm, size, ierr)
    
    ! Execute postprocessing
    if (rank == 0) then
      write(*,*) "Executing c1 postprocessing..."
    endif
    
    ! Call external c1 module
    call external_c1_postprocess(comm)
    
    if (rank == 0) then
      write(*,*) "c1 postprocessing complete"
    endif
  end subroutine c1_postprocess

  !
  ! b1 neutronics calculation interface
  !
  subroutine b1_execute(comm, mesh_dims, temperature_field, power_field, field_size)
    integer, intent(in) :: comm              ! MPI communicator
    integer, intent(in) :: mesh_dims(3)      ! Mesh dimensions
    real(8), intent(in) :: temperature_field(*) ! Temperature field
    real(8), intent(out) :: power_field(*)   ! Power distribution
    integer, intent(in) :: field_size        ! Field array size
    
    ! Local variables
    integer :: ierr, rank, size
    
    ! Get MPI information
    call MPI_COMM_RANK(comm, rank, ierr)
    call MPI_COMM_SIZE(comm, size, ierr)
    
    ! Execute neutronics calculation
    if (rank == 0) then
      write(*,*) "Executing b1 neutronics calculation..."
      write(*,*) "  Mesh dimensions: ", mesh_dims
      write(*,*) "  Field size: ", field_size
    endif
    
    ! Call external b1 module
    call external_b1_execute(comm, mesh_dims, temperature_field, power_field, field_size)
    
    if (rank == 0) then
      write(*,*) "b1 neutronics calculation complete"
    endif
  end subroutine b1_execute

  !
  ! b2 thermal calculation interface
  !
  subroutine b2_execute(comm, mesh_dims, power_field, temperature_field, field_size)
    integer, intent(in) :: comm              ! MPI communicator
    integer, intent(in) :: mesh_dims(3)      ! Mesh dimensions
    real(8), intent(in) :: power_field(*)    ! Power distribution
    real(8), intent(out) :: temperature_field(*) ! Temperature field
    integer, intent(in) :: field_size        ! Field array size
    
    ! Local variables
    integer :: ierr, rank, size
    
    ! Get MPI information
    call MPI_COMM_RANK(comm, rank, ierr)
    call MPI_COMM_SIZE(comm, size, ierr)
    
    ! Execute thermal calculation
    if (rank == 0) then
      write(*,*) "Executing b2 thermal calculation..."
      write(*,*) "  Mesh dimensions: ", mesh_dims
      write(*,*) "  Field size: ", field_size
    endif
    
    ! Call external b2 module
    call external_b2_execute(comm, mesh_dims, power_field, temperature_field, field_size)
    
    if (rank == 0) then
      write(*,*) "b2 thermal calculation complete"
    endif
  end subroutine b2_execute

  !
  ! Set calculation mode
  !
  subroutine set_calculation_mode(steady_flag)
    integer, intent(in) :: steady_flag ! 1=steady-state, 0=transient
    
    ! Local variables
    character(len=20) :: mode
    
    ! Set mode
    if (steady_flag == 1) then
      mode = "Steady-state"
    else
      mode = "Transient"
    endif
    
    write(*,*) "Setting calculation mode to ", trim(mode)
    
    ! Note: In a real implementation, this would set parameters in the
    ! external modules to use the specified calculation mode
  end subroutine set_calculation_mode

  !
  ! Free burnup days array allocated in Fortran
  !
  subroutine free_burnup_days(burnup_days)
    integer, intent(inout), pointer :: burnup_days(:)
    
    ! Deallocate the array if associated
    if (associated(burnup_days)) then
      deallocate(burnup_days)
      nullify(burnup_days)
    endif
  end subroutine free_burnup_days

end module reactor_interfaces

!
! 以下是C++可以直接调用的接口
!

!
! Initialization interface
!
subroutine a0_init(comm, burnup_steps, burnup_days) bind(C, name="a0_init")
  use iso_c_binding, only: c_int, c_ptr, c_loc, c_null_ptr, c_associated
  
  integer(c_int), intent(in) :: comm
  integer(c_int), intent(out) :: burnup_steps
  type(c_ptr), intent(out) :: burnup_days
  
  integer, pointer :: days(:)
  
  call reactor_interfaces_a0_init(comm, burnup_steps, days)
  
  if (associated(days)) then
    burnup_days = c_loc(days(1))
  else
    burnup_days = c_null_ptr
  endif
  
  contains
    ! 内部实现，避免名称冲突
    subroutine reactor_interfaces_a0_init(comm, burnup_steps, burnup_days)
      integer, intent(in) :: comm
      integer, intent(out) :: burnup_steps
      integer, pointer, intent(out) :: burnup_days(:)
      
      ! 调用外部模块的a0_init实现
      call external_a0_init(comm, burnup_steps, burnup_days)
      
      ! 执行其他与原a0_init相同的操作
      ! 获取MPI信息
      integer :: ierr, rank, size
      call MPI_COMM_RANK(comm, rank, ierr)
      call MPI_COMM_SIZE(comm, size, ierr)
      
      if (rank == 0) then
        write(*,*) "Executing reactor system initialization..." ! 执行堆系统初始化...
        write(*,*) "Burnup steps: ", burnup_steps  ! 燃耗步数：
        if (associated(burnup_days)) then
          write(*,*) "Days per step: ", burnup_days(1:burnup_steps)  ! 每步天数：
        else
          write(*,*) "Warning: burnup_days not successfully allocated"  ! 警告：燃耗天数未成功分配
        endif
        write(*,*) "Initialization complete"  ! 初始化完成
      endif
    end subroutine
end subroutine

!
! Preprocessing a1
!
subroutine a1_preprocess(comm, step_number) bind(C, name="a1_preprocess")
  use iso_c_binding, only: c_int
  
  integer(c_int), intent(in) :: comm
  integer(c_int), intent(in) :: step_number
  
  integer :: ierr, rank, size
  
  call MPI_COMM_RANK(comm, rank, ierr)
  call MPI_COMM_SIZE(comm, size, ierr)
  
  if (rank == 0) then
    write(*,*) "Executing a1 preprocessing for ", step_number, " days..."
  endif
  
  call external_a1_preprocess(comm, step_number)
  
  if (rank == 0) then
    write(*,*) "a1 preprocessing complete"
  endif
end subroutine

!
! Preprocessing a2
!
subroutine a2_preprocess(comm, burnup_days) bind(C, name="a2_preprocess")
  use iso_c_binding, only: c_int
  
  integer(c_int), intent(in) :: comm
  integer(c_int), intent(in) :: burnup_days
  
  integer :: ierr, rank, size
  
  call MPI_COMM_RANK(comm, rank, ierr)
  call MPI_COMM_SIZE(comm, size, ierr)
  
  if (rank == 0) then
    write(*,*) "Executing a2 preprocessing for ", burnup_days, " days..."
  endif
  
  call external_a2_preprocess(comm, burnup_days)
  
  if (rank == 0) then
    write(*,*) "a2 preprocessing complete"
  endif
end subroutine

!
! Preprocessing a3
!
subroutine a3_preprocess(comm) bind(C, name="a3_preprocess")
  use iso_c_binding, only: c_int
  
  integer(c_int), intent(in) :: comm
  
  integer :: ierr, rank, size
  
  call MPI_COMM_RANK(comm, rank, ierr)
  call MPI_COMM_SIZE(comm, size, ierr)
  
  if (rank == 0) then
    write(*,*) "Executing a3 preprocessing..."
  endif
  
  call external_a3_preprocess(comm)
  
  if (rank == 0) then
    write(*,*) "a3 preprocessing complete"
  endif
end subroutine

!
! Postprocessing c1
!
subroutine c1_postprocess(comm) bind(C, name="c1_postprocess")
  use iso_c_binding, only: c_int
  
  integer(c_int), intent(in) :: comm
  
  integer :: ierr, rank, size
  
  call MPI_COMM_RANK(comm, rank, ierr)
  call MPI_COMM_SIZE(comm, size, ierr)
  
  if (rank == 0) then
    write(*,*) "Executing c1 postprocessing..."
  endif
  
  call external_c1_postprocess(comm)
  
  if (rank == 0) then
    write(*,*) "c1 postprocessing complete"
  endif
end subroutine

!
! Set calculation mode
!
subroutine set_calculation_mode(steady_flag) bind(C, name="set_calculation_mode")
  use iso_c_binding, only: c_int
  
  integer(c_int), intent(in) :: steady_flag
  
  character(len=20) :: mode
  
  if (steady_flag == 1) then
    mode = "Steady-state"
  else
    mode = "Transient"
  endif
  
  write(*,*) "Setting calculation mode to ", trim(mode)
end subroutine

!
! Free burnup days array memory
!
subroutine free_burnup_days(burnup_days) bind(C, name="free_burnup_days")
  use iso_c_binding, only: c_ptr, c_f_pointer, c_null_ptr, c_associated
  
  type(c_ptr), intent(inout) :: burnup_days
  
  integer, pointer :: days(:)
  
  if (c_associated(burnup_days)) then
    call c_f_pointer(burnup_days, days, [5])  ! Use known length or dynamic storage length
    
    if (associated(days)) then
      deallocate(days)
      nullify(days)
    endif
    
    burnup_days = c_null_ptr
  endif
end subroutine

!
! b1 neutron calculation interface
!
subroutine b1_execute(comm, mesh_dims, temperature_field, power_field, field_size) &
          bind(C, name="b1_execute")
  use iso_c_binding, only: c_int, c_double
  
  integer(c_int), intent(in) :: comm
  integer(c_int), intent(in) :: mesh_dims(3)
  real(c_double), intent(in) :: temperature_field(*)
  real(c_double), intent(out) :: power_field(*)
  integer(c_int), intent(in) :: field_size
  
  integer :: ierr, rank, size
  
  call MPI_COMM_RANK(comm, rank, ierr)
  call MPI_COMM_SIZE(comm, size, ierr)
  
  if (rank == 0) then
    write(*,*) "Executing b1 neutronics calculation..."
    write(*,*) "  Mesh dimensions: ", mesh_dims
    write(*,*) "  Field size: ", field_size
  endif
  
  call external_b1_execute(comm, mesh_dims, temperature_field, power_field, field_size)
  
  if (rank == 0) then
    write(*,*) "b1 neutronics calculation complete"
  endif
end subroutine

!
! b2 thermal calculation interface
!
subroutine b2_execute(comm, mesh_dims, power_field, temperature_field, field_size) &
          bind(C, name="b2_execute")
  use iso_c_binding, only: c_int, c_double
  
  integer(c_int), intent(in) :: comm
  integer(c_int), intent(in) :: mesh_dims(3)
  real(c_double), intent(in) :: power_field(*)
  real(c_double), intent(out) :: temperature_field(*)
  integer(c_int), intent(in) :: field_size
  
  integer :: ierr, rank, size
  
  call MPI_COMM_RANK(comm, rank, ierr)
  call MPI_COMM_SIZE(comm, size, ierr)
  
  if (rank == 0) then
    write(*,*) "Executing b2 thermal calculation..."
    write(*,*) "  Mesh dimensions: ", mesh_dims
    write(*,*) "  Field size: ", field_size
  endif
  
  call external_b2_execute(comm, mesh_dims, power_field, temperature_field, field_size)
  
  if (rank == 0) then
    write(*,*) "b2 thermal calculation complete"
  endif
end subroutine 