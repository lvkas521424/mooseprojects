# ======================================================================
# 自动生成的Fortran模块依赖配置
# 由 analyze_fortran_deps.py 自动生成
# 请勿手动编辑此文件！
# ======================================================================

# 按依赖顺序列出的源文件
tran_heat_f90srcfiles_ordered := \
    $(TRAN_HEAT_DIR)/src/reactor_interfaces.f90 \

tran_heat_f90srcfiles := $(tran_heat_f90srcfiles_ordered)

# 显式依赖规则（确保模块编译顺序）
$(TRAN_HEAT_DIR)/build/reactor_interfaces.$(obj-suffix) : $(TRAN_HEAT_DIR)/src/reactor_interfaces.f90