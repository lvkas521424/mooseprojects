#!/bin/bash
# MPI编译脚本 for reactor_interfaces.f90

# 设置MPI编译器
export FC=mpif90
export CC=mpicc
export CXX=mpicxx

# 编译选项
FFLAGS="-fPIC -shared -O2 -g"

# 编译Fortran模块
echo "编译Fortran MPI模块..."
$FC $FFLAGS -o reactor_interfaces.so reactor_interfaces.f90

if [ $? -eq 0 ]; then
    echo "编译成功!"
    echo "生成文件: reactor_interfaces.so"
else
    echo "编译失败!"
    exit 1
fi

# 检查MPI符号
echo "检查MPI符号..."
nm -D reactor_interfaces.so | grep MPI

echo "编译完成!" 