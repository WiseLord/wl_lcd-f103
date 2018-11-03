DISPLAY = ILI9320
DISPVAR = 8BIT

TARGET := wl_lcd-f103_$(shell echo $(DISPLAY)_$(DISPVAR) | tr A-Z a-z)

DISP_HI_BYTE = YES

C_DEFS = -DUSE_FULL_LL_DRIVER -DSTM32F103xB
ifeq "$(DISP_HI_BYTE)" "YES"
  C_DEFS += -D_DISP_HI_BYTE
endif

C_SOURCES = main.c

C_SOURCES += pins.c

C_SOURCES += \
system/system_stm32f1xx.c \
drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_exti.c \
drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_rcc.c \
drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_gpio.c \
drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_utils.c \
drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_tim.c \
drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_dma.c \
drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_pwr.c \
drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_rtc.c


ifeq "$(DISPLAY)" "KS0108A"
  C_SOURCES += display/gm128x64/ks0108.c
else ifeq "$(DISPLAY)" "KS0108B"
  C_SOURCES += display/gm128x64/ks0108.c
else ifeq "$(DISPLAY)" "ST7920"
  C_SOURCES += display/gm128x64/st7920.c
else ifeq "$(DISPLAY)" "SSD1306"
  C_SOURCES += display/gm128x64/ssd1306.c
else ifeq "$(DISPLAY)" "ILI9163"
  C_SOURCES += display/gc160x128/ili9163.c
else ifeq "$(DISPLAY)" "ST7735"
  C_SOURCES += display/gc160x128/st7735.c
else ifeq "$(DISPLAY)" "LS020"
  C_SOURCES += display/gc176x132/ls020.c
else ifeq "$(DISPLAY)" "LPH9157"
  C_SOURCES += display/gc176x132/lph9157.c
else ifeq "$(DISPLAY)" "SSD1286A"
  C_SOURCES += display/gc176x132/ssd1286a.c
else ifeq "$(DISPLAY)" "HX8340"
  C_SOURCES += display/gc220x176/hx8340.c
else ifeq "$(DISPLAY)" "ILI9225"
  C_SOURCES += display/gc220x176/ili9225.c
else ifeq "$(DISPLAY)" "ILI9320"
  C_SOURCES += display/gc320x240/ili9320.c
else ifeq "$(DISPLAY)" "ILI9341"
  C_SOURCES += display/gc320x240/ili9341.c
else ifeq "$(DISPLAY)" "S6D0139"
  C_SOURCES += display/gc320x240/s6d0139.c
else ifeq "$(DISPLAY)" "SPFD5408"
  C_SOURCES += display/gc320x240/spfd5408.c
else ifeq "$(DISPLAY)" "MC2PA8201"
  C_SOURCES += display/gc320x240/mc2pa8201.c
else ifeq "$(DISPLAY)" "ILI9327"
  C_SOURCES += display/gc400x240/ili9327.c
else ifeq "$(DISPLAY)" "ST7793"
  C_SOURCES += display/gc400x240/st7793.c
else ifeq "$(DISPLAY)" "ILI9481"
  C_SOURCES += display/gc480x320/ili9481.c
else ifeq "$(DISPLAY)" "R61581"
  C_SOURCES += display/gc480x320/r61581.c
endif
C_SOURCES += display/dispdrv.c
C_SOURCES += display/glcd.c
C_DEFS += -D_$(DISPLAY)
C_DEFS += -D_DISP_$(DISPVAR)


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

.PHONY: flash
flash: $(BIN)
	$(OPENOCD) -f $(OPENOCD_CFG) -c "stm_flash $(BIN)" -c shutdown

# Other dependencies
-include $(OBJECTS:.o=.d)
