PROJECT = wl_lcd-f103

DISPLAY = ILI9320
DISPVAR = 8BIT

# Lowercase argument
lc = $(shell echo $1 | tr '[:upper:]' '[:lower:]')

TARGET = $(call lc, $(PROJECT)_$(DISPLAY)_$(DISPVAR))

C_DEFS = -DUSE_FULL_LL_DRIVER -DSTM32F103xB

DISP_HI_BYTE = YES
DISP_LO_BYTE = NO

ifeq "$(DISP_HI_BYTE)" "YES"
  C_DEFS += -D_DISP_HI_BYTE
endif
ifeq "$(DISP_LO_BYTE)" "YES"
  C_DEFS += -D_DISP_LO_BYTE
endif

C_DEFS += -D_DISP_READ_ENABLED
C_DEFS += -D_DISP_RST_ENABLED

C_SOURCES = main.c

C_SOURCES += pins.c
C_SOURCES += usart.c

# Display source files
C_SOURCES += $(wildcard display/fonts/font*.c)
C_SOURCES += $(wildcard display/icons/icon*.c)

ifneq (,$(filter $(DISPLAY), \
  ILI9163   \
  S6D0144   \
  ST7735    \
))
  DISPSIZE = 160x128
endif

ifneq (,$(filter $(DISPLAY), \
  L2F50126  \
  LPH9157   \
  LS020     \
  SSD1286A  \
))
  DISPSIZE = 176x132
endif

ifneq (,$(filter $(DISPLAY), \
  HX8340    \
  ILI9225   \
  LGDP4524  \
  S6D0164   \
))
  DISPSIZE = 220x176
endif

ifneq (,$(filter $(DISPLAY), \
  HX8347A   \
  HX8347D   \
  ILI9320   \
  ILI9341   \
  MC2PA8201 \
  S6D0129   \
  S6D0139   \
  SPFD5408  \
  SSD1289   \
  SSD2119   \
))
  DISPSIZE = 320x240
endif

ifneq (,$(filter $(DISPLAY), \
  ILI9327   \
  S6D04D1   \
  ST7793    \
))
  DISPSIZE = 400x240
endif

ifneq (,$(filter $(DISPLAY), \
  ILI9481   \
  ILI9486   \
  R61581    \
))
  DISPSIZE = 480x320
endif

C_SOURCES += display/gc$(DISPSIZE)/$(call lc,$(DISPLAY)).c
C_SOURCES += display/dispdrv.c
C_SOURCES += display/glcd.c
C_DEFS += -D_$(DISPLAY)
C_DEFS += -D_DISP_$(DISPVAR)
C_DEFS += -D_DISP_$(DISPSIZE)

C_SOURCES += \
  drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_dma.c \
  drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_exti.c \
  drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_gpio.c \
  drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_pwr.c \
  drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_rcc.c \
  drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_rtc.c \
  drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_spi.c \
  drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_tim.c \
  drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_usart.c \
  drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_utils.c \
  system/system_stm32f1xx.c

C_INCLUDES = \
  -Idrivers/STM32F1xx_HAL_Driver/Inc \
  -Idrivers/CMSIS/Device/ST/STM32F1xx/Include \
  -Idrivers/CMSIS/Include

AS_DEFS =

ASM_SOURCES = \
  system/startup_stm32f103xb.s

# Build directory
BUILD_DIR = build

DEBUG = 1

# Compiler
FPU =
FLOAT-ABI =
MCU = -mcpu=cortex-m3 -mthumb $(FPU) $(FLOAT-ABI)
OPT = -Os -fshort-enums -ffunction-sections -fdata-sections
WARN = -Wall -Werror
CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) $(WARN)
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) $(WARN)
ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif
# Dependency information
CFLAGS += -MMD -MP -MT $(BUILD_DIR)/$(*F).o -MF $(BUILD_DIR)/$(*D)/$(*F).d

LDSCRIPT = system/stm32f103c8tx_flash.ld
LIBS = -lc -lm -lnosys
LIBDIR =
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections
#LDFLAGS = $(WARN) -mmcu=$(MCU) -Wl,--gc-sections -Wl,--relax

# Main definitions
PREFIX = arm-none-eabi-
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
AR = $(PREFIX)ar
SZ = $(PREFIX)size

OPENOCD := openocd
OPENOCD_CFG := system/stm32f103cb_openocd.cfg

OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

ELF = $(BUILD_DIR)/$(TARGET).elf
BIN = flash/$(TARGET).bin

all: $(BIN) size

$(BIN): $(ELF)
	$(CP) -O binary $(ELF) $(BIN)

$(ELF): $(OBJECTS)
	@mkdir -p flash
	$(CC) $(LDFLAGS) -o $(ELF) $(OBJECTS)

size: $(ELF)
	$(SZ) $(ELF)

$(BUILD_DIR)/%.o: %.c Makefile
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) $(C_DEFS) -o $@ $<

$(BUILD_DIR)/%.o: %.s Makefile
	@mkdir -p $(dir $@)
	$(AS) -c $(CFLAGS) $(AS_DEFS) -o $@ $<

clean:
	rm -rf $(BUILD_DIR)

clean_disp:
	@rm -f $(patsubst %.c,$(BUILD_DIR)/%.o, $(notdir $(wildcard display/gc*/*.c)))
	@rm -f $(BUILD_DIR)/dispdrv.o $(BUILD_DIR)/pins.o

.PHONY: flash
flash: $(BIN)
	$(OPENOCD) -f $(OPENOCD_CFG) -c "stm_flash $(BIN)" -c shutdown

# Other dependencies
-include $(OBJECTS:.o=.d)
