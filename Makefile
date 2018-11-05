##########################################################################################################################
# Cloud_STM32F429IGTx_FIRE GCC compiler Makefile
##########################################################################################################################

# ------------------------------------------------
# Generic Makefile (based on gcc)
# ------------------------------------------------
include config.mk
include prune.mk
######################################
# target
######################################
TARGET = Huawei_LiteOS
######################################
# building variables
######################################
# debug build?
DEBUG = 1
# optimization
OPT = -Og


USE_OTA := no
OTA_IS_NEED_DTLS := no
ifeq ($(USE_FOTA), yes)
	USE_OTA = yes
endif
ifeq ($(USE_SOTA), yes)
	USE_OTA = yes
endif
ifeq ($(USE_OTA), yes)
    ifneq ($(OTA_PACK_CHECKSUM), NO_CHECKSUM)
	OTA_IS_NEED_DTLS = yes
    endif
endif


#######################################
# binaries
#######################################
PREFIX    =
CC        = $(PREFIX)gcc
AS        = $(PREFIX)gcc -x assembler-with-cpp
OBJCOPY   = $(PREFIX)objcopy
OBJDUMP   = $(PREFIX)objdump
AR        = $(PREFIX)ar
SZ        = $(PREFIX)size
LD        = $(PREFIX)ld
HEX       = $(OBJCOPY) -O ihex
BIN       = $(OBJCOPY) -O binary -S


PROJECTBASE = $(PWD)
override PROJECTBASE    := $(abspath $(PROJECTBASE))
TOP_DIR = $(PROJECTBASE)/


#######################################
# paths
#######################################
# firmware library path
PERIFLIB_PATH =

# Build path
BUILD_DIR = build

######################################
# source
######################################
# C sources



ifneq ($(OTA_IS_NEED_DTLS)_$(WITH_DTLS), no_no)
MBEDTLS_SRC = \
        ${wildcard $(TOP_DIR)/mbedtls/mbedtls-2.6.0/library/*.c}
        C_SOURCES += $(MBEDTLS_SRC)

MBEDTLS_PORT_SRC = \
        ${wildcard $(TOP_DIR)/mbedtls/mbedtls_port/*.c}
        C_SOURCES += $(MBEDTLS_PORT_SRC)
endif

ifeq ($(WITH_LWM2M), yes)
ER_COAP_SRC = \
        ${wildcard $(TOP_DIR)/lwm2m/core/er-coap-13/er-coap-13.c}
        C_SOURCES += $(ER_COAP_SRC)

LWM2M_SRC = \
        ${wildcard $(TOP_DIR)/lwm2m/core/*.c} \
	${wildcard $(TOP_DIR)/lwm2m/examples/shared/*.c}
        C_SOURCES += $(LWM2M_SRC)

ATINY_TINY_SRC = \
        ${wildcard $(TOP_DIR)/agent_tiny/atiny_lwm2m/*.c}

ifeq ($(USE_FOTA), yes)
	C_SOURCES += $(ATINY_TINY_SRC)
else
	ATINY_TINY_SRC_NO_FOTA = \
			$(filter-out $(TOP_DIR)/agent_tiny/atiny_lwm2m/atiny_fota_manager.c \
			$(TOP_DIR)/agent_tiny/atiny_lwm2m/atiny_fota_state.c \
			$(TOP_DIR)/agent_tiny/atiny_lwm2m/firmware_update.c, $(ATINY_TINY_SRC))
	C_SOURCES += $(ATINY_TINY_SRC_NO_FOTA)
endif
		
AGENT_DEMO_SRC = \
        ${wildcard $(TOP_DIR)/demos/agenttiny_lwm2m/*.c}
        C_SOURCES += $(AGENT_DEMO_SRC)

endif



OS_DEPENDS_SRC = \
        ${wildcard $(TOP_DIR)/osdepends/liteos/*.c}
        C_SOURCES += $(OS_DEPENDS_SRC)

SOCKET_ADAPTER_SRC = \
        ${wildcard $(TOP_DIR)/net/sal/*.c}
        C_SOURCES += $(SOCKET_ADAPTER_SRC)

NEWLIB_SRC = \
        ${wildcard $(TOP_DIR)/components/lib/libc/malloc.c}
        C_SOURCES += $(NEWLIB_SRC)
		

ATINY_LOG = \
        ${wildcard $(TOP_DIR)/log/*.c}
        C_SOURCES += $(ATINY_LOG)






USER_SRC =  \
        ${wildcard $(TOP_DIR)/agenttiny_lwm2m/*.c} \
        ${wildcard $(TOP_DIR)/main.c}
        C_SOURCES += $(USER_SRC)









######################################
# firmware library
######################################
PERIFLIB_SOURCES =


#######################################
# CFLAGS
#######################################
# cpu
CPU =
# fpu
FPU =
# float-abi
FLOAT-ABI =
# mcu
MCU = $(CPU) $(FPU) $(FLOAT-ABI)

# macros for gcc
# AS defines
AS_DEFS =

# C defines
C_DEFS =  \
        -D NDEBUG

ifeq ($(WITH_LWM2M), yes)
		C_DEFS += \
			-D LWM2M_LITTLE_ENDIAN \
			-D LWM2M_CLIENT_MODE
endif
ifneq ($(OTA_IS_NEED_DTLS)_$(WITH_DTLS), no_no)
		C_DEFS += -D MBEDTLS_CONFIG_FILE=\"los_mbedtls_config.h\"
	ifeq ($(USE_MBEDTLS_DEBUG_C), yes)
		C_DEFS += -DMBEDTLS_DEBUG_C
	endif

	ifeq ($(USE_MBEDTLS_AES_ROM_TABLES), yes)
		C_DEFS += -DMBEDTLS_AES_ROM_TABLES
	endif
endif
ifeq ($(WITH_DTLS), yes)
		C_DEFS += -DWITH_DTLS
endif
ifeq ($(WITH_LINUX), yes)
		C_DEFS += -DWITH_LINUX
endif





ifeq ($(LWM2M_BOOTSTRAP), yes)
		C_DEFS += -DLWM2M_BOOTSTRAP
endif


ifeq ($(LWM2M_WITH_LOGS), yes)
		C_DEFS += -DLWM2M_WITH_LOGS
endif

ifeq ($(ATINY_DEBUG), yes)
		C_DEFS += -DATINY_DEBUG
endif





# AS includes
AS_INCLUDES =

# C includes
INCLUDE_INC = \
        -I $(TOP_DIR)/include
        C_INCLUDES += $(INCLUDE_INC)

USER_INC =
        C_INCLUDES += $(USER_INC)



ARCH_INC = \
        -I $(TOP_DIR)/arch/arm/arm-m/include
        C_INCLUDES += $(ARCH_INC)

	

ifneq ($(OTA_IS_NEED_DTLS)_$(WITH_DTLS), no_no)
MBEDTLS_INC = \
        -I $(TOP_DIR)/mbedtls/mbedtls-2.6.0/include
        C_INCLUDES += $(MBEDTLS_INC)

MBEDTLS_PORT_INC = \
        -I $(TOP_DIR)/mbedtls/mbedtls_port
        C_INCLUDES += $(MBEDTLS_PORT_INC)
endif

ifeq ($(WITH_LWM2M), yes)
ER_COAP_INC = \
        -I $(TOP_DIR)/lwm2m/core/er-coap-13
        C_INCLUDES += $(ER_COAP_INC)
LWM2M_INC = \
        -I $(TOP_DIR)/lwm2m/core \
        -I $(TOP_DIR)/lwm2m/examples/shared
        C_INCLUDES += $(LWM2M_INC)
endif




DEMO_INC = \
        -I $(TOP_DIR)/demos
        C_INCLUDES += $(DEMO_INC)








# compile gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif


# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$@"


#######################################
# LDFLAGS
#######################################
# link script


# libraries
LIBS = -lc -lm -lpthread
LIBDIR = -L$(PROJECTBASE)/../Lib
LDFLAGS = $(MCU)  $(LIBDIR) $(LIBS) -Wl,--gc-sections

# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin

#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile config.mk prune.mk | $(BUILD_DIR)
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile config.mk prune.mk | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

ifeq ($(USE_BOOTLOADER), no)
$(BUILD_DIR)/%.o: %.S Makefile config.mk prune.mk | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@
endif

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile config.mk prune.mk
	echo $(USER_SRC)
	echo $(C_SOURCES)
	echo $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@

$(BUILD_DIR):
	mkdir $@

#######################################
# clean up
#######################################
clean:
	-rm -fR .dep $(BUILD_DIR)

#######################################
# dependencies
#######################################
#-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)
-include $(wildcard $(BUILD_DIR)/*.d)

# *** EOF ***
