#!/bin/bash
# OpenMP 并行测试脚本

echo "========================================="
echo "  测试 Fortran OpenMP 并行支持"
echo "========================================="
echo ""

# 测试不同的线程数
for threads in 1 2 4; do
    echo ">>> 测试: OMP_NUM_THREADS=${threads}"
    echo "----------------------------------------"
    export OMP_NUM_THREADS=${threads}
    mpirun -n 2 ./mooseprojects-opt -i neutronic.i --allow-unused 2>&1 | grep -E "OpenMP|线程|计算:|MPI 进程"
    echo ""
done

echo "========================================="
echo "  测试完成"
echo "========================================="

