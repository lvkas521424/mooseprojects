#!/bin/bash
# 快速测试 OpenMP 配置

echo "========================================"
echo "  快速 OpenMP 并行测试"
echo "========================================"

# 测试 2 MPI × 4 OpenMP
export OMP_NUM_THREADS=4
echo ""
echo ">>> 配置: 2 MPI 进程 × 4 OpenMP 线程 = 8核心"
echo ">>> 命令: mpirun -n 2 OMP_NUM_THREADS=4 ./mooseprojects-opt -i neutronic.i"
echo ""

timeout 60 mpirun -n 2 ./mooseprojects-opt -i neutronic.i --allow-unused 2>&1 | \
  grep -A 8 "OpenMP 并行配置" | head -10

echo ""
echo "========================================"
echo "✅ 测试完成！"
echo ""
echo "提示："
echo "  - 修改线程数: export OMP_NUM_THREADS=N"
echo "  - 查看文档: cat contrib/OPENMP_README.md"
echo "========================================"



