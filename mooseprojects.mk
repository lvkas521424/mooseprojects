###############################################################################
# MOOSE外部依赖配置
###############################################################################

TRAN_HEAT_DIR       ?= $(APPLICATION_DIR)/contrib/tran_heat
ADDITIONAL_CPPFLAGS += -DTRAN_HEAT_ENABLED
APP_INCLUDES        += -I$(TRAN_HEAT_DIR)/..

# ============================================================================
# Fortran编译方式选择
# ============================================================================
# yes = 使用CMake（推荐，如果已有CMakeLists.txt）
# no  = 使用自动依赖分析（需要运行 make update-fortran-deps）
USE_CMAKE_FOR_FORTRAN ?= no

# ============================================================================
# CMake编译模式
# ============================================================================
ifeq ($(USE_CMAKE_FOR_FORTRAN),yes)

  $(TRAN_HEAT_DIR)/build/libtran_heat.a: $(shell find $(TRAN_HEAT_DIR)/src -name "*.f90" -o -name "*.f" 2>/dev/null)
	@echo "使用CMake构建 tran_heat..."
	@cd $(TRAN_HEAT_DIR) && \
	cmake -B build -S . \
		-DCMAKE_BUILD_TYPE=$(METHOD) \
		-DCMAKE_Fortran_COMPILER=$(FC) \
		-DCMAKE_C_COMPILER=$(CC) \
		-DCMAKE_CXX_COMPILER=$(CXX) && \
	cmake --build build --config $(METHOD) --parallel
	@echo "✅ tran_heat 构建完成"

  ADDITIONAL_LIBS += $(TRAN_HEAT_DIR)/build/libtran_heat.a
  APP_INCLUDES += -I$(TRAN_HEAT_DIR)/modules
  $(APPLICATION_NAME)-$(METHOD): $(TRAN_HEAT_DIR)/build/libtran_heat.a

  .PHONY: clean-tran_heat
  clean-tran_heat:
	@echo "清理 tran_heat (CMake)..."
	@rm -rf $(TRAN_HEAT_DIR)/build
	@echo "✅ 完成"

# ============================================================================
# 直接编译模式（自动依赖分析）
# ============================================================================
else
  include $(APPLICATION_DIR)/contrib/tran_heat.mk
  # 添加 OpenMP 链接标志
  ADDITIONAL_LIBS += -fopenmp
endif
