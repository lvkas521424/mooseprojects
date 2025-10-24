###############################################################################
# Fortran模块自动依赖管理
# 文档: contrib/编译方式说明.md
###############################################################################

$(shell mkdir -p $(TRAN_HEAT_DIR)/build $(TRAN_HEAT_DIR)/modules)

# ============================================================================
# 自动引入依赖配置（如果存在）
# ============================================================================
-include $(TRAN_HEAT_DIR)/tran_heat_auto.mk

# 如果自动配置不存在，使用查找模式
ifndef tran_heat_f90srcfiles
    $(info ⚠️  tran_heat_auto.mk 不存在，建议运行: make update-fortran-deps)
    tran_heat_f90srcfiles := $(shell find $(TRAN_HEAT_DIR)/src -maxdepth 1 -name "*.f90")
endif

tran_heat_fsrcfiles := $(shell find $(TRAN_HEAT_DIR)/src -maxdepth 1 -name "*.f")

# ============================================================================
# 对象文件和包含路径
# ============================================================================
ADDITIONAL_APP_OBJECTS += $(patsubst $(TRAN_HEAT_DIR)/src/%.f90, $(TRAN_HEAT_DIR)/build/%.$(obj-suffix), $(tran_heat_f90srcfiles))
ADDITIONAL_APP_OBJECTS += $(patsubst $(TRAN_HEAT_DIR)/src/%.f, $(TRAN_HEAT_DIR)/build/%.$(obj-suffix), $(tran_heat_fsrcfiles))

APP_INCLUDES += $(shell find $(TRAN_HEAT_DIR)/modules -maxdepth 1 -name "*.mod" 2>/dev/null || true)

ADDITIONAL_APP_DEPS += $(patsubst $(TRAN_HEAT_DIR)/src/%.f90, $(TRAN_HEAT_DIR)/build/%.$(obj-suffix).d, $(tran_heat_f90srcfiles))
ADDITIONAL_APP_DEPS += $(patsubst $(TRAN_HEAT_DIR)/src/%.f, $(TRAN_HEAT_DIR)/build/%.$(obj-suffix).d, $(tran_heat_fsrcfiles))

# ============================================================================
# 编译规则
# ============================================================================

# Fortran90: 模块文件输出到 modules 目录
$(TRAN_HEAT_DIR)/build/%.$(obj-suffix) : $(TRAN_HEAT_DIR)/src/%.f90
	@echo "Compiling Fortran90 (OpenMP): $<"
	@$(libmesh_LIBTOOL) --tag=FC $(LIBTOOLFLAGS) --mode=compile --quiet \
	  $(libmesh_F90) -ffree-line-length-none -fopenmp $(libmesh_FFLAGS) $(app_INCLUDES) $(libmesh_INCLUDE) \
	  -I$(TRAN_HEAT_DIR)/modules -J$(TRAN_HEAT_DIR)/modules -c $< -o $@

# Fortran77
$(TRAN_HEAT_DIR)/build/%.$(obj-suffix) : $(TRAN_HEAT_DIR)/src/%.f
	@echo "Compiling Fortran77 (OpenMP): $<"
	@$(libmesh_LIBTOOL) --tag=F77 $(LIBTOOLFLAGS) --mode=compile --quiet \
	  $(libmesh_F77) -fopenmp $(libmesh_FFLAGS) $(app_INCLUDES) $(libmesh_INCLUDE) -c $< -o $@

# ============================================================================
# 管理命令
# ============================================================================

.PHONY: update-fortran-deps check-fortran-deps debug-tran_heat clean-tran_heat

# 更新Fortran依赖配置
update-fortran-deps:
	@echo "分析Fortran模块依赖..."
	@python3 contrib/analyze_fortran_deps.py $(TRAN_HEAT_DIR)/src -o $(TRAN_HEAT_DIR)/tran_heat_auto.mk
	@echo "✅ 依赖配置已更新"

# 检查依赖关系（不生成文件）
check-fortran-deps:
	@python3 contrib/analyze_fortran_deps.py $(TRAN_HEAT_DIR)/src

# 调试信息
debug-tran_heat:
	@echo "========================================"
	@echo "Fortran编译配置"
	@echo "========================================"
	@echo "目录: $(TRAN_HEAT_DIR)"
	@echo ""
	@echo "依赖模式:"
	@if [ -f "$(TRAN_HEAT_DIR)/tran_heat_auto.mk" ]; then \
		echo "  ✅ 自动模式（使用 tran_heat_auto.mk）"; \
	else \
		echo "  ⚠️  手动查找模式"; \
	fi
	@echo ""
	@echo "F90源文件 ($(words $(tran_heat_f90srcfiles))个):"
	@echo "$(tran_heat_f90srcfiles)" | tr ' ' '\n' | sed 's/^/  /'
	@echo ""
	@echo "F77源文件 ($(words $(tran_heat_fsrcfiles))个):"
	@echo "$(tran_heat_fsrcfiles)" | tr ' ' '\n' | sed 's/^/  /'
	@echo ""
	@echo "模块文件:"
	@ls -1 $(TRAN_HEAT_DIR)/modules/*.mod 2>/dev/null | sed 's/^/  /' || echo "  (无)"
	@echo "========================================"

# 清理构建产物
clean-tran_heat:
	@echo "清理 tran_heat..."
	@rm -f $(TRAN_HEAT_DIR)/build/*.$(obj-suffix)
	@rm -f $(TRAN_HEAT_DIR)/build/*.lo
	@rm -f $(TRAN_HEAT_DIR)/modules/*.mod
	@echo "✅ 完成"
