###fortran
tran_heat_srcfiles     = $(shell find $(TRAN_HEAT_DIR) -maxdepth 1  -name *.f90)

ADDITIONAL_APP_OBJECTS += $(patsubst %.f90, %.$(obj-suffix), $(tran_heat_srcfiles))

APP_INCLUDES           += $(shell find $(TRAN_HEAT_DIR) -maxdepth 1  -name *.mod)

ADDITIONAL_APP_DEPS    += $(patsubst %.f90, %.$(obj-suffix).d, $(tran_heat_srcfiles))