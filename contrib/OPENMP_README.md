# Fortran OpenMP 并行支持说明

## ✅ 功能状态

Fortran 代码已成功集成 OpenMP 并行支持！

## 📋 功能特性

### 1. **MPI + OpenMP 混合并行**
- **MPI 进程级并行**：跨节点/核心分配数据
- **OpenMP 线程级并行**：每个 MPI 进程内部多线程计算
- **自动验证**：首次运行时输出并行配置信息

### 2. **自动初始化**
- 从 `OMP_NUM_THREADS` 环境变量读取线程数
- 自动覆盖 MPI 的线程限制
- 仅首次调用时输出验证信息

### 3. **简洁输出**
- 删除了冗余的调试输出
- 每10步输出一次进度信息
- 清晰的并行配置验证输出

## 🚀 使用方法

### 基本用法

```bash
# 设置 OpenMP 线程数
export OMP_NUM_THREADS=4

# 运行程序（2个 MPI 进程，每个进程4个 OpenMP 线程）
mpirun -n 2 ./mooseprojects-opt -i neutronic.i --allow-unused
```

### 推荐配置

**总核心数 = MPI进程数 × OpenMP线程数**

示例（8核心机器）：
```bash
# 方案1: 2 MPI × 4 OpenMP = 8 核心
export OMP_NUM_THREADS=4
mpirun -n 2 ./mooseprojects-opt -i input.i

# 方案2: 4 MPI × 2 OpenMP = 8 核心
export OMP_NUM_THREADS=2
mpirun -n 4 ./mooseprojects-opt -i input.i

# 方案3: 纯 MPI（适合小规模问题）
mpirun -n 8 ./mooseprojects-opt -i input.i
```

### 验证输出示例

```
=========================================
  Fortran OpenMP 并行配置验证
=========================================
  MPI 进程数: 2
  OpenMP 实际线程数: 4
  OMP_NUM_THREADS 环境变量: 4
  ✅ OpenMP 并行已启用
=========================================
```

## 📊 性能建议

### 1. **选择合适的并行策略**

| 问题规模 | 推荐配置 | 原因 |
|---------|---------|------|
| 小规模 (<1000个元素) | 纯 MPI | OpenMP 开销大于收益 |
| 中等规模 | MPI + OpenMP | 平衡负载，减少通信 |
| 大规模 (>10000个元素) | MPI + OpenMP | 充分利用计算资源 |

### 2. **线程数建议**

- **OpenMP 线程数** ≤ **物理核心数/MPI进程数**
- 避免超线程（Hyper-Threading）过度使用
- 典型配置：2-4个 OpenMP 线程/MPI 进程

### 3. **NUMA 优化**

```bash
# 绑定进程到特定 NUMA 节点
export OMP_PROC_BIND=true
export OMP_PLACES=cores
mpirun -n 2 ./mooseprojects-opt -i input.i
```

## 🔧 编译配置

### 编译标志

Fortran 代码使用以下 OpenMP 编译选项：

**编译时**：
- `-fopenmp`：启用 OpenMP 支持
- `-ffree-line-length-none`：允许长行

**链接时**：
- `-fopenmp`：链接 libgomp 运行时库

### 修改的文件

1. **`contrib/tran_heat/src/reactor_interfaces.f90`**
   - 添加 `use omp_lib`
   - 实现 OpenMP 并行循环
   - 添加自动初始化和验证

2. **`contrib/tran_heat.mk`**
   - 添加 `-fopenmp` 编译标志

3. **`mooseprojects.mk`**
   - 添加 `-fopenmp` 链接标志

## 📝 代码示例

### OpenMP 并行循环

```fortran
! OpenMP并行计算
!$OMP PARALLEL DO DEFAULT(SHARED) PRIVATE(i) SCHEDULE(STATIC)
do i = 1, field_size
   power_field1(i) = 100.0D0 + i * 10.0D0 + rank * 1000.0D0 + temperature_field(i)
   power_field2(i) = 500.0D0 + i * 10.0D0 + rank * 1000.0D0 + temperature_field(i)
end do
!$OMP END PARALLEL DO
```

### 调度策略

| 策略 | 适用场景 | 说明 |
|------|---------|------|
| `STATIC` | 负载均衡 | 预先均匀分配迭代（当前使用）|
| `DYNAMIC` | 负载不均 | 动态分配，适应性强 |
| `GUIDED` | 负载递减 | 初始大块，逐渐减小 |

## ⚠️ 注意事项

1. **线程安全**
   - MPI 通信已正确同步
   - 避免在并行区域内调用 MPI 函数

2. **内存访问**
   - 注意 false sharing（伪共享）
   - 大数组使用 `SCHEDULE(STATIC)` 效果更好

3. **调试模式**
   - 调试时建议使用单线程：`export OMP_NUM_THREADS=1`
   - 更容易重现问题

## 🧪 性能测试

```bash
# 测试脚本
for np in 1 2 4; do
  for nt in 1 2 4; do
    echo "MPI=$np, OMP=$nt"
    export OMP_NUM_THREADS=$nt
    time mpirun -n $np ./mooseprojects-opt -i input.i
  done
done
```

## 📚 参考资料

- [OpenMP 官方文档](https://www.openmp.org/)
- [MPICH 文档](https://www.mpich.org/)
- [混合 MPI/OpenMP 编程](https://www.mcs.anl.gov/research/projects/mpi/tutorial/gropp/talk.html)

## ✨ 更新日志

**2025-10-24**
- ✅ 添加 OpenMP 并行支持
- ✅ 实现自动初始化和验证
- ✅ 清理冗余输出
- ✅ 测试验证所有线程数配置

---

**编译命令**：`make -j8`  
**清理命令**：`make clean-tran_heat`  
**测试命令**：`./test_openmp.sh`

