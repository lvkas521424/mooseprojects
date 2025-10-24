#!/bin/bash
# 简单的 OpenMP 测试

echo "测试 OpenMP 环境变量传递"
echo "======================================"

# 设置不同的线程数
for threads in 1 2 4 8; do
    echo ""
    echo "设置 OMP_NUM_THREADS=${threads}"
    export OMP_NUM_THREADS=${threads}
    
    # 在当前 shell 中检查
    echo "当前 shell 中: OMP_NUM_THREADS=$OMP_NUM_THREADS"
    
    # 通过 mpirun 传递（尝试不同方法）
    echo "--- 方法1: export + mpirun ---"
    mpirun -n 1 sh -c 'echo "MPI进程中: OMP_NUM_THREADS=$OMP_NUM_THREADS"'
    
    echo "--- 方法2: mpirun -genv ---"
    mpirun -n 1 -genv OMP_NUM_THREADS ${threads} sh -c 'echo "MPI进程中(genv): OMP_NUM_THREADS=$OMP_NUM_THREADS"'
done

