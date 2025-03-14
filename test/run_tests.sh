#!/bin/bash

# 编译测试程序
make -j4

# 运行单元测试
./run_tests --gtest_output="xml:test_results.xml"

# 运行独立中子学模式测试
./reactor-opt -i test/tests/neutronics/neutronics_test.i

# 运行耦合模式测试
./reactor-opt -i test/tests/coupled/coupled_test.i

# 检查结果
echo "检查测试结果..."
python scripts/check_results.py 