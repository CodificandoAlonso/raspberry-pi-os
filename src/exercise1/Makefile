RPI_V ?= 4

COMPILER ?= aarch64-linux-gnu

BOOTLOC ?= /home/hectoralv22/Desktop


C_FLAGS = -DRPI_VERSION=$(RPI_V) -Wall -Wextra -nostdlib -nostartfiles -ffreestanding \
			-Iinclude -mgeneral-regs-only

ASM_OPS = -Iinclude


BUILD_DIR = build


SRC_DIR = src


all: kernel8.img


clean:
	rm -rf $(BUILD_DIR) *.img


$(BUILD_DIR)/%_c.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(COMPILER)-gcc $(C_FLAGS) -MMD -c $< -o $@

$(BUILD_DIR)/%_s.o: $(SRC_DIR)/%.S
	mkdir -p $(@D)
	$(COMPILER)-gcc $(C_FLAGS) -MMD -c $< -o $@



C_FILES = $(wildcard $(SRC_DIR)/*.c)
ASM_FILES = $(wildcard $(SRC_DIR)/*.S)
OBJ_FILES = $(C_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%_c.o)
OBJ_FILES += $(ASM_FILES:$(SRC_DIR)/%.S=$(BUILD_DIR)/%_s.o)

DEP_FILES = $(OBJ_FILES:%.o=%.d)
-include $(DEP_FILES)



kernel8.img: $(SRC_DIR)/linker.ld $(OBJ_FILES)
	@echo "BUILDING FOR RASPBERRY PI V. $(value RPI_V)"
	@echo "DEPLOY TO $(value BUILD_DIR)"
	$(COMPILER)-ld -T $(SRC_DIR)/linker.ld -o $(BUILD_DIR)/kernel8.elf $(OBJ_FILES)
	$(COMPILER)-objcopy $(BUILD_DIR)/kernel8.elf -O binary kernel8.img
ifeq ($(RPI_V), 4)
	cp kernel8.img $(BOOTLOC)/kernel8-rpi4.img
else
	cp kernel8.img $(BOOTLOC)/
endif
	cp config.txt $(BOOTLOC)/
	sync