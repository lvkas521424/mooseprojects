# Fortran编译方式选择指南

你的项目现在支持**两种编译方式**，可以随时切换！

---

## 📊 两种方式对比

| 特性 | CMake编译 | 直接编译（自动依赖） |
|------|-----------|-------------------|
| **模块依赖处理** | CMake自动处理 | Python脚本自动分析 |
| **适用场景** | 已有CMakeLists.txt | 简单项目或需要精细控制 |
| **配置复杂度** | 中等（需要CMakeLists.txt） | 低（自动生成） |
| **编译速度** | 快（CMake并行优化） | 快（Make并行） |
| **灵活性** | 中等 | 高（可手动调整） |
| **推荐** | ✅ 大型已有项目 | ✅ 新项目或小项目 |

---

## 🔧 当前配置

**默认使用：CMake编译**（因为你的项目已有 `CMakeLists.txt`）

查看当前配置：
```bash
grep "USE_CMAKE_FOR_FORTRAN" mooseprojects.mk
```

---

## 🚀 使用方式

### 方式1：CMake编译（当前默认）

**编译：**
```bash
make -j8
```

**工作流程：**
```
make -j8
  ↓
自动调用 CMake
  ↓
CMake分析依赖关系
  ↓
编译生成 libtran_heat.a
  ↓
链接到主程序
  ↓
完成！
```

**优点：**
- ✅ CMake自动处理模块依赖
- ✅ 不需要运行额外的依赖分析命令
- ✅ 支持复杂的构建配置
- ✅ 跨平台兼容性好

**清理：**
```bash
make clean-tran_heat
```

---

### 方式2：直接编译（自动依赖分析）

**切换方法：**

编辑 `mooseprojects.mk`，修改：
```makefile
# 从
USE_CMAKE_FOR_FORTRAN ?= yes

# 改为
USE_CMAKE_FOR_FORTRAN ?= no
```

**或者在命令行临时切换：**
```bash
make USE_CMAKE_FOR_FORTRAN=no -j8
```

**编译：**
```bash
# 首次或源码修改后
make update-fortran-deps

# 正常编译
make -j8
```

**优点：**
- ✅ 不需要维护CMakeLists.txt
- ✅ 完全透明的依赖关系
- ✅ 可以手动微调编译顺序
- ✅ 学习Makefile的好机会

**清理：**
```bash
make clean-tran_heat
```

---

## 🔄 切换方式

### 永久切换

**切换到CMake编译：**
```bash
# 编辑 mooseprojects.mk
USE_CMAKE_FOR_FORTRAN ?= yes
```

**切换到直接编译：**
```bash
# 编辑 mooseprojects.mk
USE_CMAKE_FOR_FORTRAN ?= no

# 然后生成依赖配置
make update-fortran-deps
```

### 临时切换（一次性）

**临时使用CMake：**
```bash
make USE_CMAKE_FOR_FORTRAN=yes -j8
```

**临时使用直接编译：**
```bash
make USE_CMAKE_FOR_FORTRAN=no update-fortran-deps
make USE_CMAKE_FOR_FORTRAN=no -j8
```

---

## 💡 推荐配置

### 如果你的项目已经有完善的CMakeLists.txt

**推荐：CMake编译**

```makefile
# mooseprojects.mk
USE_CMAKE_FOR_FORTRAN ?= yes
```

**工作流程：**
```bash
# 添加新文件后，只需要
make -j8
# CMake会自动检测和编译
```

---

### 如果你想要更精细的控制

**推荐：直接编译**

```makefile
# mooseprojects.mk
USE_CMAKE_FOR_FORTRAN ?= no
```

**工作流程：**
```bash
# 添加新文件后
make update-fortran-deps
make -j8
```

---

## 📂 文件组织

### CMake模式生成的文件：
```
contrib/tran_heat/
├── build/
│   ├── libtran_heat.a          # 静态库
│   ├── CMakeCache.txt           # CMake配置
│   └── ...                      # CMake中间文件
└── modules/
    └── *.mod                    # 模块文件（CMake生成）
```

### 直接编译模式生成的文件：
```
contrib/tran_heat/
├── build/
│   └── *.o                      # 对象文件
├── modules/
│   └── *.mod                    # 模块文件
└── tran_heat_auto.mk            # 自动生成的依赖配置
```

---

## 🔍 实际测试

### 测试CMake模式

```bash
# 1. 确保使用CMake
echo "USE_CMAKE_FOR_FORTRAN ?= yes" | grep yes

# 2. 清理
make clean-tran_heat

# 3. 编译
make -j8

# 4. 检查生成的库
ls -lh contrib/tran_heat/build/libtran_heat.a
```

### 测试直接编译模式

```bash
# 1. 切换到直接编译
# 编辑 mooseprojects.mk: USE_CMAKE_FOR_FORTRAN ?= no

# 2. 清理
make clean-tran_heat

# 3. 生成依赖
make update-fortran-deps

# 4. 编译
make -j8

# 5. 检查对象文件
ls -lh contrib/tran_heat/build/*.o
```

---

## 🐛 故障排查

### CMake编译失败

**症状：** CMake报错

**解决：**
```bash
# 1. 清理CMake缓存
rm -rf contrib/tran_heat/build

# 2. 检查CMakeLists.txt是否正确
cat contrib/tran_heat/CMakeLists.txt

# 3. 手动运行CMake测试
cd contrib/tran_heat
cmake -B build -S .
cmake --build build
```

### 直接编译找不到.mod文件

**症状：** `Can't open module file 'xxx.mod'`

**解决：**
```bash
# 1. 确保生成了依赖配置
make update-fortran-deps

# 2. 检查配置
make debug-tran_heat

# 3. 清理重建
make clean-tran_heat
make -j2
```

---

## 📋 完整工作流程示例

### 场景：使用CMake编译（推荐）

```bash
# ============ 首次设置 ============
cd /home/lvkas/projects/mooseprojects

# 确保使用CMake（默认已经是）
# USE_CMAKE_FOR_FORTRAN ?= yes (在mooseprojects.mk中)

# 直接编译
make -j8

# ============ 添加新文件 ============
# 创建新的Fortran文件
vim contrib/tran_heat/src/new_module.f90

# CMake会自动检测，直接编译即可
make -j8

# ============ 完成！ ============
```

### 场景：使用自动依赖分析

```bash
# ============ 首次设置 ============
cd /home/lvkas/projects/mooseprojects

# 切换到直接编译模式
# 编辑 mooseprojects.mk: USE_CMAKE_FOR_FORTRAN ?= no

# 生成依赖配置
make update-fortran-deps

# 编译
make -j8

# ============ 添加新文件 ============
# 创建新的Fortran文件
vim contrib/tran_heat/src/new_module.f90

# 更新依赖配置
make update-fortran-deps

# 编译
make -j8

# ============ 完成！ ============
```

---

## 🎯 快速决策指南

**使用CMake，如果：**
- ✅ 项目已有完善的 `CMakeLists.txt`
- ✅ 需要跨平台编译
- ✅ Fortran代码频繁变动
- ✅ 团队熟悉CMake

**使用直接编译，如果：**
- ✅ 想要学习Makefile
- ✅ 需要精细控制编译过程
- ✅ 项目较小，不需要CMake的复杂性
- ✅ 想要完全透明的依赖关系

---

## ⚙️ 高级配置

### 自定义CMake参数

编辑 `mooseprojects.mk`：

```makefile
$(TRAN_HEAT_DIR)/build/libtran_heat.a: ...
	@cd $(TRAN_HEAT_DIR) && \
	cmake -B build -S . \
		-DCMAKE_BUILD_TYPE=$(METHOD) \
		-DCMAKE_Fortran_COMPILER=$(FC) \
		-DCMAKE_Fortran_FLAGS="-O3 -march=native" \  # 添加自定义标志
		-DENABLE_OPENMP=ON \                          # 启用OpenMP
		... && \
	cmake --build build --config $(METHOD) --parallel
```

### 自定义直接编译参数

编辑 `contrib/tran_heat.mk`：

```makefile
$(TRAN_HEAT_DIR)/build/%.$(obj-suffix) : $(TRAN_HEAT_DIR)/src/%.f90
	@$(libmesh_F90) \
	  -O3 -march=native \              # 添加优化标志
	  -fopenmp \                        # 启用OpenMP
	  -I$(TRAN_HEAT_DIR)/modules \
	  -J$(TRAN_HEAT_DIR)/modules \
	  -c $< -o $@
```

---

## ✅ 总结

你现在有**两个强大的工具**可以选择：

| | CMake | 自动依赖分析 |
|---|---|---|
| **命令** | `make -j8` | `make update-fortran-deps && make -j8` |
| **适合** | 大型已有项目 | 新项目或需要控制 |
| **默认** | ✅ 是 | 否 |

**当前配置：** 使用CMake（因为你已有CMakeLists.txt）

**想切换？** 修改 `mooseprojects.mk` 中的 `USE_CMAKE_FOR_FORTRAN`

两种方式都经过测试，选择你喜欢的即可！🎉

