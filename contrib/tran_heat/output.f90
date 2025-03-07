module output_module
  use io_module, only: write_output
  implicit none

contains
  ! Advanced output routine with additional statistics
  ! Parameters:
  !   filename - output file name
  !   nx, ny - grid dimensions
  !   temp - temperature field to output
  !   iter - number of iterations performed
  !   error - final convergence error
  subroutine output_results(filename, nx, ny, temp, iter, error)
    character(len=*), intent(in) :: filename
    integer, intent(in) :: nx, ny, iter
    real, intent(in) :: temp(nx, ny), error
    
    ! Local variables
    real :: min_temp, max_temp, avg_temp
    integer :: i, j
    
    ! Calculate statistics
    min_temp = temp(1,1)
    max_temp = temp(1,1)
    avg_temp = 0.0
    
    do j = 1, ny
      do i = 1, nx
        min_temp = min(min_temp, temp(i,j))
        max_temp = max(max_temp, temp(i,j))
        avg_temp = avg_temp + temp(i,j)
      end do
    end do
    
    avg_temp = avg_temp / (nx * ny)
    
    ! Print statistics to console
    print *, "--------- Solution Statistics ---------"
    print *, "Number of iterations: ", iter
    print *, "Final error: ", error
    print *, "Temperature range: [", min_temp, ", ", max_temp, "]"
    print *, "Average temperature: ", avg_temp
    print *, "-------------------------------------"
    
    ! Write results to file using io_module
    call write_output(filename, nx, ny, temp)
    
  end subroutine output_results
end module output_module
