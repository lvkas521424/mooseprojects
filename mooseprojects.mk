TRAN_HEAT_DIR       ?= $(APPLICATION_DIR)/contrib/tran_heat
ADDITIONAL_CPPFLAGS += -DTRAN_HEAT_ENABLED

# app
APP_INCLUDES        += -I$(TRAN_HEAT_DIR)/..
include $(APPLICATION_DIR)/contrib/tran_heat.mk