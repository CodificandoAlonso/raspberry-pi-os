# Exercise 2.1: Processor Initialization — Switching to EL1

## Objective

Modify your kernel to switch from Exception Level 3 (EL3) to Exception Level 1 (EL1) before executing `kernel_main`.

---

## Background

ARM processors supporting ARMv8 have 4 exception levels:

| Level | Purpose |
|-------|---------|
| EL0 | User applications |
| EL1 | OS kernel |
| EL2 | Hypervisor |
| EL3 | Secure monitor |

The Raspberry Pi boots at **EL3**. Your OS should run at **EL1**.

---

## What You Need to Understand

1. Why can't a program increase its own exception level?
2. How do `ELR_ELn` and `SPSR_ELn` registers work during exceptions?
3. What does the `eret` instruction do?
4. How can you exploit writable exception registers to switch levels?

---

## Tasks

### Task 1: Read Current Exception Level

Create a function `get_el()` that returns the current exception level as an integer.

**Hint:** Look up the `CurrentEL` system register.

### Task 2: Configure System Registers

Before switching to EL1, you must configure several system registers. Find out what each does and what values they need:

| Register | Purpose to Research |
|----------|---------------------|
| `SCTLR_EL1` | How should EL1 behave? (MMU, caches, endianness) |
| `HCR_EL2` | What execution state at EL1? |
| `SCR_EL3` | What execution state at EL2? Secure or non-secure? |
| `SPSR_EL3` | What processor state after switch? (interrupts, target mode) |
| `ELR_EL3` | Where to continue execution? |

### Task 3: Implement the Switch

Modify `boot.S` to:
1. Configure the registers above
2. Execute `eret` to switch to EL1
3. Continue with BSS clearing, stack setup, and kernel_main

### Task 4: Verify

Print the exception level from your kernel. Confirm it shows EL1 instead of EL3.

---

## Reference Documentation

- AArch64 Reference Manual (search for each register name)
- BCM2711 ARM Peripherals (for any Pi-specific details)

---

## Deliverables

- [ ] `get_el()` function that reads current exception level
- [ ] Boot code that switches from EL3 to EL1
- [ ] Kernel prints "Exception level: 1"

---

## Questions to Answer From the Datasheet

1. Which bits in `CurrentEL` contain the exception level value?
2. Which bits in `SCTLR_EL1` are marked RES1 (must be set to 1)?
3. What value in `SPSR_EL3` bits 3:0 means "EL1 with dedicated stack pointer"?
4. Which bit in `HCR_EL2` controls AArch64 vs AArch32 execution at EL1?
5. Which bits in `SCR_EL3` control execution state and security for lower levels?