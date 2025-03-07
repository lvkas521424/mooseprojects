! ģ�飺�ļ��������
module io_module
    implicit none
contains
    ! Read input parameters from file
    ! Parameters:
    !   filename - input file name
    !   nx, ny - grid dimensions (output)
    !   k - thermal conductivity (output)
    !   dx, dy - grid spacing (output)
    !   temp - initial temperature field (allocated and initialized)
    subroutine read_input(filename, nx, ny, k, dx, dy, temp)
        character(len=*), intent(in) :: filename
        integer, intent(out) :: nx, ny
        real, intent(out) :: k, dx, dy
        real, allocatable, intent(out) :: temp(:,:)
        
        ! Local variables
        integer :: i, j, io_status, file_unit
        
        ! Open input file
        open(newunit=file_unit, file=trim(filename), status='old', action='read', iostat=io_status)
        
        ! Check if file opened successfully
        if (io_status /= 0) then
            print *, "Error: Cannot open input file: ", trim(filename)
            stop
        end if
        
        ! Read grid dimensions and parameters
        read(file_unit, *) nx, ny
        read(file_unit, *) k
        read(file_unit, *) dx, dy
        
        ! Allocate temperature array
        allocate(temp(nx, ny))
        
        ! Read initial temperature field
        do j = 1, ny
            read(file_unit, *) (temp(i,j), i=1,nx)
        end do
        
        ! Close file
        close(file_unit)
        
        print *, "Input parameters successfully read from file:", trim(filename)
        print *, "Grid dimensions: ", nx, " x ", ny
        print *, "Thermal conductivity: ", k
        print *, "Grid spacing: dx=", dx, " dy=", dy
        
    end subroutine read_input

    ! Write output results to file
    ! Parameters:
    !   filename - output file name
    !   nx, ny - grid dimensions
    !   temp - temperature field to output
    subroutine write_output(filename, nx, ny, temp)
        character(len=*), intent(in) :: filename
        integer, intent(in) :: nx, ny
        real, intent(in) :: temp(nx, ny)
        
        ! Local variables
        integer :: i, j, io_status, file_unit
        
        ! Open output file
        open(newunit=file_unit, file=trim(filename), status='replace', action='write', iostat=io_status)
        
        ! Check if file opened successfully
        if (io_status /= 0) then
            print *, "Error: Cannot open output file: ", trim(filename)
            return
        end if
        
        ! Write grid dimensions as header
        write(file_unit, *) "# Grid dimensions: ", nx, " x ", ny
        
        ! Write temperature field
        do j = 1, ny
            write(file_unit, *) (temp(i,j), i=1,nx)
        end do
        
        ! Close file
        close(file_unit)
        
        print *, "Results successfully written to file:", trim(filename)
        
    end subroutine write_output
end module io_module
