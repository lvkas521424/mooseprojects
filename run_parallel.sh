#!/bin/bash

echo "=== 测试不同并行度的性能 ==="

echo "1 进程 (串行):"
time mpirun -np 1 ./mooseprojects-opt -i neutronic.i > serial_output.log 2>&1

echo "2 进程:"  
time mpirun -np 2 ./mooseprojects-opt -i neutronic.i > parallel_2_output.log 2>&1

echo "4 进程:"
time mpirun -np 4 ./mooseprojects-opt -i neutronic.i > parallel_4_output.log 2>&1

echo "性能测试完成，查看各个日志文件了解详细输出" 