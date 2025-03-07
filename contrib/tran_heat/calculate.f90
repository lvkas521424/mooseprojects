! ģ�飺�������ɺʹ��ȼ���
module heat_module
    implicit none
    real, parameter :: tolerance = 1.0e-6  ! �����ݲ�
contains
    ! Main heat calculation subroutine
    ! Performs heat transfer calculation on a 2D grid
    ! Parameters:
    !   nx, ny - grid dimensions
    !   k - thermal conductivity coefficient
    !   dx, dy - grid spacing in x and y directions
    !   temp - temperature field (2D array, modified in-place)
    subroutine heat_calculation(nx, ny, k, dx, dy, temp)
        integer, intent(in) :: nx, ny
        real, intent(in) :: k, dx, dy
        real, intent(inout) :: temp(nx, ny)
        
        ! Local variables
        integer :: i, j, iter
        integer, parameter :: max_iter = 1000   ! Maximum iterations
        real, parameter :: tol = 1.0e-6         ! Convergence tolerance
        real :: error, old_val
        real :: temp_new(nx, ny)                ! Temporary array for new values
        
        ! Initialize error
        error = 1.0
        
        ! Iterative solution
        do iter = 1, max_iter
            ! Exit if converged
            if (error < tol) then
                print *, "Converged after", iter, "iterations"
                exit
            end if
            
            error = 0.0
            
            ! Update interior points using finite difference method
            do j = 2, ny-1
                do i = 2, nx-1
                    ! Store old value for error calculation
                    old_val = temp(i,j)
                    
                    ! Discretized heat equation: central difference for second derivatives
                    temp_new(i,j) = 0.25 * ( &
                        temp(i+1,j) + temp(i-1,j) + &
                        temp(i,j+1) + temp(i,j-1))
                    
                    ! Update maximum error
                    error = max(error, abs(temp_new(i,j) - old_val))
                end do
            end do
            
            ! Update temperature field
            do j = 2, ny-1
                do i = 2, nx-1
                    temp(i,j) = temp_new(i,j)
                end do
            end do
        end do
        
        ! Check if maximum iterations reached without convergence
        if (iter > max_iter) then
            print *, "Warning: Maximum iterations reached without convergence"
            print *, "Final error =", error
        end if
        
    end subroutine heat_calculation
end module heat_module
