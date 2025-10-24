# Fortran依赖自动化工作流程

## ✨ 完全自动化方案

不需要手动复制粘贴代码，一条命令搞定！

---

## 🚀 快速开始（3步）

### 步骤1：更新依赖配置

```bash
cd /home/lvkas/projects/mooseprojects
make update-fortran-deps
```

**输出示例：**
```
==========================================
分析Fortran模块依赖关系...
==========================================

分析目录: contrib/tran_heat/src
找到 4 个 .f90 文件

文件: constants.f90
  定义模块: constants

文件: physics.f90
  定义模块: physics
  使用模块: constants

正确的编译顺序:
------------------------------------------------------------
1. constants.f90
2. physics.f90
...

✅ Makefile已生成: contrib/tran_heat/tran_heat_auto.mk
   在主Makefile中使用: include contrib/tran_heat/tran_heat_auto.mk

✅ 依赖配置已更新！
   下次编译会自动使用正确的编译顺序
```

### 步骤2：正常编译

```bash
make -j8
```

### 步骤3：完成！

就这么简单！不需要任何手动操作。

---

## 📋 工作原理

```mermaid
graph LR
    A[运行 make update-fortran-deps] --> B[Python脚本分析源码]
    B --> C[生成 tran_heat_auto.mk]
    C --> D[主Makefile自动include]
    D --> E[使用正确的编译顺序]
    E --> F[编译成功！]
```

### 自动生成的文件

**`contrib/tran_heat/tran_heat_auto.mk`** 内容示例：
```makefile
# ======================================================================
# 自动生成的Fortran模块依赖配置
# 由 analyze_fortran_deps.py 自动生成
# 请勿手动编辑此文件！
# ======================================================================

# 按依赖顺序列出的源文件
tran_heat_f90srcfiles_ordered := \
    $(TRAN_HEAT_DIR)/src/constants.f90 \
    $(TRAN_HEAT_DIR)/src/types.f90 \
    $(TRAN_HEAT_DIR)/src/physics.f90 \
    $(TRAN_HEAT_DIR)/src/reactor_interfaces.f90 \

tran_heat_f90srcfiles := $(tran_heat_f90srcfiles_ordered)

# 显式依赖规则（确保模块编译顺序）
$(TRAN_HEAT_DIR)/build/constants.$(obj-suffix) : $(TRAN_HEAT_DIR)/src/constants.f90

$(TRAN_HEAT_DIR)/build/types.$(obj-suffix) : $(TRAN_HEAT_DIR)/src/types.f90 \
    $(TRAN_HEAT_DIR)/build/constants.$(obj-suffix) \

$(TRAN_HEAT_DIR)/build/physics.$(obj-suffix) : $(TRAN_HEAT_DIR)/src/physics.f90 \
    $(TRAN_HEAT_DIR)/build/types.$(obj-suffix) \

$(TRAN_HEAT_DIR)/build/reactor_interfaces.$(obj-suffix) : $(TRAN_HEAT_DIR)/src/reactor_interfaces.f90 \
    $(TRAN_HEAT_DIR)/build/physics.$(obj-suffix) \
```

### 主Makefile如何使用

**`contrib/tran_heat.mk`** 自动引入：
```makefile
# 如果 tran_heat_auto.mk 存在，自动include
-include $(TRAN_HEAT_DIR)/tran_heat_auto.mk

# 如果不存在，使用备用方案（自动查找）
ifndef tran_heat_f90srcfiles
    tran_heat_f90srcfiles := $(shell find $(TRAN_HEAT_DIR)/src -name "*.f90")
endif
```

---

## 🛠️ 所有可用命令

### 1. 更新依赖配置
```bash
make update-fortran-deps
```
**何时使用：**
- ✅ 首次设置项目
- ✅ 添加新的Fortran源文件
- ✅ 修改模块依赖关系（添加/删除`use`语句）
- ✅ 重命名Fortran文件

### 2. 检查依赖关系（不生成文件）
```bash
make check-fortran-deps
```
**何时使用：**
- 🔍 只想查看依赖关系，不修改配置
- 🔍 调试依赖问题

### 3. 查看当前配置
```bash
make debug-tran_heat
```
**输出示例：**
```
========================================
Fortran编译配置调试信息
========================================
TRAN_HEAT_DIR = /home/lvkas/projects/mooseprojects/contrib/tran_heat

依赖模式:
  ✅ 自动模式（使用 tran_heat_auto.mk）

F90源文件:
  contrib/tran_heat/src/constants.f90
  contrib/tran_heat/src/physics.f90
  ...

模块文件:
  constants.mod
  physics.mod
  ...
========================================
```

### 4. 清理构建产物
```bash
make clean-tran_heat
```

### 5. 正常编译
```bash
make -j8
```

---

## 🔄 完整工作流程示例

### 场景：添加新的Fortran模块

**1. 创建新文件**
```bash
cat > contrib/tran_heat/src/new_module.f90 << 'EOF'
module new_module
    use constants
    use types
    implicit none
    ! ...
end module new_module
EOF
```

**2. 更新依赖**
```bash
make update-fortran-deps
```

**3. 编译**
```bash
make -j8
```

**完成！** 新模块会自动被加入到正确的编译顺序中。

---

## ⚙️ 高级选项

### 手动运行Python脚本

#### 只查看分析结果
```bash
python3 contrib/analyze_fortran_deps.py contrib/tran_heat/src
```

#### 生成到指定文件
```bash
python3 contrib/analyze_fortran_deps.py contrib/tran_heat/src \
    -o contrib/my_custom.mk
```

#### 静默模式（只生成文件）
```bash
python3 contrib/analyze_fortran_deps.py contrib/tran_heat/src \
    -o contrib/tran_heat_auto.mk -q
```

#### 查看帮助
```bash
python3 contrib/analyze_fortran_deps.py --help
```

---

## 🐛 故障排查

### 问题1：`make update-fortran-deps` 失败

**症状：** Python脚本报错

**解决：**
```bash
# 检查Python版本
python3 --version  # 需要 >= 3.6

# 手动运行脚本查看详细错误
python3 contrib/analyze_fortran_deps.py contrib/tran_heat/src
```

### 问题2：编译时仍然找不到 .mod 文件

**症状：** `Fatal Error: Can't open module file 'xxx.mod'`

**解决步骤：**
```bash
# 1. 清理旧文件
make clean-tran_heat

# 2. 重新生成依赖
make update-fortran-deps

# 3. 检查配置
make debug-tran_heat

# 4. 查看生成的依赖文件
cat contrib/tran_heat/tran_heat_auto.mk

# 5. 重新编译（减少并行数）
make -j2
```

### 问题3：自动生成的文件不存在

**症状：** 编译时警告 `⚠️ tran_heat_auto.mk 不存在`

**解决：**
```bash
# 运行更新命令
make update-fortran-deps

# 或者手动生成
python3 contrib/analyze_fortran_deps.py contrib/tran_heat/src \
    -o contrib/tran_heat/tran_heat_auto.mk
```

### 问题4：循环依赖

**症状：** Python脚本警告循环依赖

**原因：** 模块A依赖模块B，模块B又依赖模块A

**解决：**
- 重构代码打破循环
- 或将循环依赖的模块合并到一个文件

---

## 📊 与传统方法对比

| 特性 | 传统手动方式 | 自动化方式 |
|------|------------|-----------|
| **初次设置** | 分析依赖、手动编辑Makefile | `make update-fortran-deps` |
| **添加新文件** | 重新分析、手动添加 | `make update-fortran-deps` |
| **修改依赖** | 手动调整顺序 | `make update-fortran-deps` |
| **出错几率** | 高（容易遗漏） | 低（自动计算） |
| **维护成本** | 高 | 极低 |
| **大型项目** | 非常困难 | 轻松应对 |

---

## 💡 最佳实践

### 1. 版本控制

**建议提交到Git：**
```bash
git add contrib/tran_heat/tran_heat_auto.mk
git commit -m "Update Fortran dependencies"
```

**原因：**
- ✅ 团队成员clone后可以直接编译
- ✅ 可追踪依赖关系变化
- ✅ CI/CD可以直接使用

### 2. 持续集成

**在CI脚本中：**
```bash
# .github/workflows/build.yml
steps:
  - name: Update Fortran dependencies
    run: make update-fortran-deps
  
  - name: Build
    run: make -j8
```

### 3. 定期更新

修改Fortran源码后记得运行：
```bash
make update-fortran-deps
```

可以考虑添加到`pre-commit` hook：
```bash
# .git/hooks/pre-commit
#!/bin/bash
if git diff --cached --name-only | grep -q "contrib/tran_heat/src/.*\.f90"; then
    echo "检测到Fortran源码修改，更新依赖配置..."
    make update-fortran-deps
    git add contrib/tran_heat/tran_heat_auto.mk
fi
```

---

## 📚 相关文档

- **完整指南：** `contrib/FORTRAN_DEPENDENCY_GUIDE.md`
- **快速开始：** `contrib/QUICK_START.md`
- **Python脚本源码：** `contrib/analyze_fortran_deps.py`

---

## ✅ 总结

使用自动化工作流程，你只需要记住一件事：

**修改Fortran源码后运行：**
```bash
make update-fortran-deps
```

其余的交给自动化工具处理！🎉

