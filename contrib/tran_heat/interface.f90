module fortran_interface
    use heat_module, only: heat_calculation
    use iso_c_binding, only: c_int, c_double
    implicit none
contains
    ! C interface subroutine for heat transfer calculation
    ! This is the main entry point called from C++
    ! Parameters are passed by reference to match C calling convention
    subroutine run_heat_transfer(nx, ny, k, dx, dy, temp) bind(C, name="run_heat_transfer")
        ! Parameter declaration - using pass-by-reference for C compatibility
        integer(c_int), intent(in) :: nx, ny
        real(c_double), intent(in) :: k, dx, dy
        real(c_double), intent(inout) :: temp(*)   ! Using assumed-size array
        
        ! Local variable declaration
        integer :: i, j, idx
        real(c_double) :: temp_2d(nx, ny)
        
        ! Debug information
        print *, "Fortran: Received parameters nx=", nx, " ny=", ny, " array size=", nx*ny
        
        ! Parameter validation
        if (nx <= 0 .or. ny <= 0 .or. nx*ny <= 0) then
            print *, "Fortran: Invalid array dimensions! nx=", nx, " ny=", ny
            return
        end if
        
        ! Convert 1D array to 2D array (row-major to column-major conversion)
        do j = 1, ny
            do i = 1, nx
                idx = (j-1)*nx + i
                if (idx <= nx*ny) then  ! 边界检查
                    temp_2d(i, j) = temp(idx)
                else
                    print *, "Index out of bounds: ", idx
                    temp_2d(i, j) = 0.0
                end if
            end do
        end do
        
        ! Call the core heat transfer calculation routine
        call heat_calculation(nx, ny, k, dx, dy, temp_2d)
        
        ! Copy results back to 1D array (column-major to row-major conversion)
        do j = 1, ny
            do i = 1, nx
                idx = (j-1)*nx + i
                if (idx <= nx*ny) then  ! 边界检查
                    temp(idx) = temp_2d(i, j)
                end if
            end do
        end do
        
    end subroutine run_heat_transfer
end module fortran_interface