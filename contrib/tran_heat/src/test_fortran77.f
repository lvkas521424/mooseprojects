!
! Author: lvjiahui eba424@163.com
! Date: 2025-09-16 17:01:13
! LastEditors: lvjiahui eba424@163.com
! LastEditTime: 2025-10-14 14:41:04
! FilePath: /mooseprojects/contrib/tran_heat/src/test_fortran77.f
! Description: 
!
C     测试 Fortran77 文件
C     @Author: lvjiahui eba424@163.com
C     @Description: 简单的 Fortran77 测试函数
      
      SUBROUTINE TEST_F77_FUNCTION(N, RESULT)
      IMPLICIT NONE
      INTEGER N, RESULT
      INTEGER I
      
      RESULT = 0
      DO 10 I = 1, N
         RESULT = RESULT + I
   10 CONTINUE
      



      
      RETURN
      END
      
      FUNCTION FACTORIAL(N)
      IMPLICIT NONE
      INTEGER FACTORIAL, N
      INTEGER I
      
      FACTORIAL = 1
      IF (N .LE. 0) RETURN
      
      DO 20 I = 1, N
         FACTORIAL = FACTORIAL * I
   20 CONTINUE
      
      RETURN
      END
