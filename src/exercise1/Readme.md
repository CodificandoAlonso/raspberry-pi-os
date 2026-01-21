# Exercise 1: Hello World — Bare Metal UART Communication

## Objective

Create a minimal bare metal kernel for Raspberry Pi 4 that prints "Hello World" over serial UART. No operating system, no libraries — just your code talking directly to hardware.

---

## Background

### What is Bare Metal?

Bare metal programming means running code directly on hardware without an operating system. You control everything: memory, peripherals, CPU initialization.

### Why UART First?

You have no screen output, no debugger, no filesystem. UART (serial communication) is the simplest way to get output from your kernel. It's your `printf` for kernel development.

### The Boot Process

When Raspberry Pi 4 powers on:

1. GPU (VideoCore) boots first, runs `bootcode.bin`
2. GPU loads `start4.elf`
3. GPU reads `config.txt` for settings
4. GPU loads `kernel8.img` to address `0x80000`
5. GPU releases ARM cores from reset
6. CPU starts executing at `0x80000`

---

## Hardware Setup

### Required Components

| Component | Purpose |
|-----------|---------|
| Raspberry Pi 4 | Target hardware |
| MicroSD card (FAT32) | Boot media |
| USB-to-TTL serial adapter | Serial communication |
| Dupont jumper wires (F-F) | Connections |
| USB-C power supply (5V 3A) | Power |

### Wiring Diagram

```
USB-to-TTL Adapter          Raspberry Pi 4
┌─────────────────┐         ┌─────────────────┐
│                 │         │                 │
│  RX  ──────────────────────►  GPIO 14 (Pin 8)  TX
│                 │         │                 │
│  TX  ◄──────────────────────  GPIO 15 (Pin 10) RX
│                 │         │                 │
│  GND ──────────────────────►  GND (Pin 6)
│                 │         │                 │
│  VCC            │         │  (NOT CONNECTED)
│                 │         │                 │
└─────────────────┘         └─────────────────┘
```

**Critical:** Cross TX/RX (Pi TX → Adapter RX). Do NOT connect VCC.

---

## Development Environment

### Toolchain (Ubuntu)

Install the cross-compiler for ARM 64-bit:

```
sudo apt install gcc-aarch64-linux-gnu make git screen
```

### Serial Terminal

```
screen /dev/ttyUSB0 115200
```

---

## SD Card Contents

Your FAT32-formatted SD card needs:

| File | Source | Purpose |
|------|--------|---------|
| `bootcode.bin` | Raspberry Pi firmware repo | First-stage bootloader |
| `start4.elf` | Raspberry Pi firmware repo | GPU firmware (Pi 4) |
| `fixup4.dat` | Raspberry Pi firmware repo | Memory configuration |
| `config.txt` | You create | Boot configuration |
| `kernel8.img` | You compile | Your kernel |

### config.txt Contents

```
arm_64bit=1
enable_uart=1
core_freq=500
```

| Setting | Purpose |
|---------|---------|
| `arm_64bit=1` | Boot in 64-bit mode (AArch64) |
| `enable_uart=1` | Enable UART hardware |
| `core_freq=500` | Fix clock at 500 MHz (for baud rate calculation) |

---

## Your Task

Create a bare metal kernel with the following files:

```
project/
├── src/
│   ├── boot.S          # Assembly entry point
│   ├── kernel.c        # Main kernel code
│   ├── gpio.c          # GPIO configuration
│   ├── mini_uart.c     # UART driver
│   ├── mm.S            # Memory utilities (memzero)
│   └── linker.ld       # Memory layout
├── include/
│   ├── common.h        # Type definitions
│   ├── gpio.h          # GPIO function declarations
│   ├── mini_uart.h     # UART function declarations
│   ├── mm.h            # Memory constants
│   └── peripherals/
│       ├── base.h      # Peripheral base address
│       ├── gpio.h      # GPIO register definitions
│       └── aux.h       # AUX/UART register definitions
└── Makefile
```

---

## Part 1: Understanding Addresses

### BCM2711 Address Translation

The datasheet uses **bus addresses** (GPU perspective). The ARM CPU uses **physical addresses**.

| Peripheral | Datasheet (Bus) | ARM Physical (Pi 4) |
|------------|-----------------|---------------------|
| GPIO | 0x7E200000 | 0xFE200000 |
| AUX (UART) | 0x7E215000 | 0xFE215000 |

**Rule:** Replace `0x7E` with `0xFE` for Pi 4 ARM addresses.

### Questions to Answer

1. What is the peripheral base address for Pi 4?
2. What offset from base gives GPIO registers?
3. What offset from base gives AUX registers?

---

## Part 2: GPIO Configuration

### Background

GPIO pins can serve multiple functions. Pins 14 and 15 can be:
- Regular input/output
- UART (what we need)
- Other alternate functions (SPI, I2C, etc.)

### Register: GPFSELn (Function Select)

Each pin needs 3 bits to select its function:

| Value | Function |
|-------|----------|
| 000 | Input |
| 001 | Output |
| 010 | Alt5 |
| 011 | Alt4 |
| 100 | Alt0 |
| 101 | Alt1 |
| 110 | Alt2 |
| 111 | Alt3 |

**Organization:** 10 pins per GPFSEL register, 3 bits per pin.

### Register: GPIO_PUP_PDN_CNTRL_REGn (Pull-up/Pull-down)

Each pin needs 2 bits:

| Value | Setting |
|-------|---------|
| 00 | No resistor |
| 01 | Pull-up |
| 10 | Pull-down |
| 11 | Reserved |

**Organization:** 16 pins per register, 2 bits per pin.

### Questions to Answer (BCM2711 Chapter 5)

1. Which GPFSEL register controls pins 14 and 15?
2. Which bit positions within that register?
3. What alternate function number connects pins 14/15 to mini UART?
4. Which GPIO_PUP_PDN register controls pins 14 and 15?

### Functions to Implement

```
gpio_pin_set_func(pin_number, function)
    - Calculate which GPFSEL register (pin / 10)
    - Calculate bit position ((pin % 10) * 3)
    - Clear the 3 bits for this pin
    - Set the new function value

gpio_pin_set(pin_number, pull_mode)
    - Calculate which PUP_PDN register (pin / 16)
    - Calculate bit position ((pin % 16) * 2)
    - Clear the 2 bits for this pin
    - Set the new pull mode value
```

---

## Part 3: Mini UART Configuration

### Background

The BCM2711 has two UARTs:
- **PL011 (UART0):** Full-featured, used by Bluetooth on Pi 4
- **Mini UART (UART1):** Simpler, available on GPIO 14/15

We use mini UART because it's simpler and available.

### AUX Registers Structure

The mini UART is part of the AUX peripheral. Key registers:

| Register | Offset | Purpose |
|----------|--------|---------|
| AUX_IRQ | 0x00 | Interrupt status |
| AUX_ENABLES | 0x04 | Enable peripherals |
| AUX_MU_IO | 0x40 | Data read/write |
| AUX_MU_IER | 0x44 | Interrupt enable |
| AUX_MU_IIR | 0x48 | Interrupt identify |
| AUX_MU_LCR | 0x4C | Line control |
| AUX_MU_MCR | 0x50 | Modem control |
| AUX_MU_LSR | 0x54 | Line status |
| AUX_MU_MSR | 0x58 | Modem status |
| AUX_MU_SCRATCH | 0x5C | Scratch |
| AUX_MU_CNTL | 0x60 | Extra control |
| AUX_MU_STAT | 0x64 | Extra status |
| AUX_MU_BAUD | 0x68 | Baud rate |

### Baud Rate Calculation

```
baudrate_reg = (system_clock / (8 * desired_baud)) - 1
```

For 115200 baud with 500 MHz clock:
```
baudrate_reg = (500000000 / (8 * 115200)) - 1 = ?
```

Calculate this value and convert to hex.

### Questions to Answer (BCM2711 Chapter 2)

1. Which bit in AUX_ENABLES enables the mini UART?
2. Which bits in AUX_MU_CNTL enable TX and RX?
3. Which bit in AUX_MU_LCR sets 8-bit mode?
4. Which bit in AUX_MU_STAT indicates "receive FIFO has data"?
5. Which bit in AUX_MU_STAT indicates "transmitter ready"?

### uart_init() Sequence

**Important:** Order matters!

```
1. Enable mini UART in AUX_ENABLES
   (Without this, you can't access other registers!)

2. Disable TX/RX in AUX_MU_CNTL
   (Don't receive garbage during configuration)

3. Disable interrupts in AUX_MU_IER
   (We'll poll, not use interrupts)

4. Set 8-bit mode in AUX_MU_LCR

5. Set baud rate in AUX_MU_BAUD

6. Configure GPIO pins 14/15
   - Set to Alt5 function
   - Disable pull resistors

7. Enable TX/RX in AUX_MU_CNTL
   (Now UART is operational)
```

### uart_write(char c) Logic

```
1. Wait until transmitter is ready
   - Read status register
   - Check "transmitter ready" bit
   - Loop while not ready

2. Write character to data register
```

### uart_read() Logic

```
1. Wait until data is available
   - Read status register
   - Check "data ready" bit
   - Loop while no data

2. Read character from data register

3. Return the character
```

### uart_write_array(char *str) Logic

```
1. While current character is not '\0':
   - Call uart_write with current character
   - Move to next character
```

---

## Part 4: Boot Assembly

### Entry Point (_start)

When the kernel loads at 0x80000, execution begins at `_start`.

**Tasks:**

1. **Check core ID:** Pi 4 has 4 cores. Only core 0 should boot, others should hang.
    - Read `MPIDR_EL1` system register
    - Mask to get core ID (bits 0-1)
    - If not 0, branch to infinite loop

2. **Clear BSS:** Uninitialized global variables must be zero.
    - Get BSS start and end addresses from linker
    - Call memzero to clear the region

3. **Set stack pointer:** C code needs a stack.
    - Load stack address
    - Set SP register

4. **Call kernel:** Branch to kernel_main

### memzero Function

```
Inputs: x0 = start address, x1 = size in bytes
Action: Write zeros from start to start+size
Use: str xzr (store zero register) in a loop
```

### Questions to Answer

1. Which register holds the core ID?
2. What's a safe stack address? (Hint: below kernel code at 0x80000)
3. How do you get BSS boundaries from the linker script?

---

## Part 5: Linker Script

### Purpose

Tells the linker:
- Where to place code in memory
- How to order sections
- What symbols to export

### Required Elements

```
Start address: 0x80000 (where Pi loads kernel)

Sections (in order):
    .text.boot  - Entry point (must be first!)
    .text       - Code
    .rodata     - Read-only data (strings)
    .data       - Initialized data
    .bss        - Uninitialized data

Symbols to export:
    bss_begin   - Start of BSS (for zeroing)
    bss_end     - End of BSS
```

### Questions to Answer

1. Why must `.text.boot` come before `.text`?
2. Why align BSS to 8 bytes?
3. What does `. = 0x80000;` do?

---

## Part 6: Build System

### Makefile Requirements

1. Cross-compile C files with `aarch64-linux-gnu-gcc`
2. Assemble .S files with same compiler
3. Link with custom linker script
4. Convert ELF to raw binary (`objcopy -O binary`)
5. Output: `kernel8.img`

### Compiler Flags

| Flag | Purpose |
|------|---------|
| `-nostdlib` | No standard library |
| `-nostartfiles` | No default startup code |
| `-ffreestanding` | Freestanding environment |
| `-mgeneral-regs-only` | Don't use FPU registers |
| `-Wall -Wextra` | Enable warnings |

---

## Testing Procedure

### Build

```
make clean
make
```

### Deploy

1. Mount SD card
2. Copy firmware files (bootcode.bin, start4.elf, fixup4.dat)
3. Copy config.txt
4. Copy kernel8.img
5. Unmount: `sync && sudo umount /path/to/sd`

### Connect

1. Wire USB-to-TTL adapter to Pi (TX↔RX crossed, GND connected)
2. Connect adapter to PC via USB
3. Open terminal: `screen /dev/ttyUSB0 115200`

### Boot

1. Insert SD card into Pi (contacts facing PCB)
2. Power on Pi
3. Watch terminal for "Hello World"

---

## Debugging

### Nothing on screen?

- Check SD card contents (all files present?)
- Check config.txt settings
- Verify kernel8.img was copied
- Check wiring (TX/RX crossed?)

### Garbled characters?

- Baud rate mismatch
- Verify `core_freq=500` in config.txt
- Recalculate baud rate register value
- Check both ends use 115200 baud

### Pi doesn't boot at all?

- Red LED on? (Power OK)
- Green LED blinks? (SD activity)
- Try reformatting SD card as FAT32
- Re-download firmware files

---

## Reference Documentation

| Document | Content |
|----------|---------|
| BCM2711 ARM Peripherals | GPIO (Ch 5), AUX/UART (Ch 2) |
| ARM Cortex-A72 TRM | Core features |
| ARMv8-A Architecture Reference | Assembly instructions |
| Raspberry Pi Documentation | Boot process, config.txt |

---

## Deliverables Checklist

- [ ] GPIO driver that configures pin functions and pull resistors
- [ ] Mini UART driver with init, read, write, write_array
- [ ] Boot assembly that initializes processor and calls C
- [ ] Linker script placing code at 0x80000
- [ ] Makefile that produces kernel8.img
- [ ] Working "Hello World" output over serial

---

## Extension Challenges

1. **Echo:** Read characters and echo them back
2. **Line editing:** Handle backspace, echo typed characters
3. **Hex dump:** Print memory contents in hex format
4. **LED blink:** Toggle GPIO pin connected to LED
5. **printf:** Implement basic printf with %d, %x, %s