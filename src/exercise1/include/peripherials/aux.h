#pragma once

#include "../common.h"
#include "../utils.h"
#define AUX_BASE 0x7E215000;


struct AuxRegs {
  reg32 irq_status;
  reg32 enables;
  reg32 reserved[14];
  reg32 mu_io;
  reg32 mu_ier;
  reg32 mu_iir;
  reg32 mu_lcr;
  reg32 mu_mcr;
  reg32 mu_lsr;
  reg32 mu_msr;
  reg32 mu_scratch;
  reg32 mu_control;
  reg32 mu_status;
  reg32 mu_baudrate;
};

#define AUX_REGS ((struct AuxRegs *)(AUX_BASE + 0x4))

