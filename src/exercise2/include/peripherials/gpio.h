#pragma once

#include "../common.h"
#include "base.h"

#define GPIO_BASE 0xFE200000

struct GpioPinData {
  reg32 reserved;
  reg32 data[2];
};

struct GpioRegs {
  reg32 func_selec[6];
  struct GpioPinData output_set;
  struct GpioPinData output_clear;
  struct GpioPinData level;
  struct GpioPinData event_detect_status;
  struct GpioPinData rising_edge_detect_enable;
  struct GpioPinData falling_edge_detect_enable;
  struct GpioPinData high_detect_enable;
  struct GpioPinData low_detect_enable;
  struct GpioPinData async_rising_edge_detect;
  struct GpioPinData async_falling_edge_detect;
  reg32 reserved[21];
  reg32 pu_pd_reg[4];
};


#define GPIO_REGS ((struct GpioRegs *)(GPIO_BASE))