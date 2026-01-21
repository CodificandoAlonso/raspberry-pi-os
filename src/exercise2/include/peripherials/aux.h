#pragma once

#include "../common.h"
#include "../utils.h"
#define AUX_BASE 0xFE215000


struct AuxRegs {
  reg32 irq_status;
  reg32 enables;
  reg32 reserved[14];
  reg32 mu_io;   //IO data
  reg32 mu_ier;  //Interrupt enable
  reg32 mu_iir;  //Interrupt identify
  reg32 mu_lcr;  //Line control
  reg32 mu_mcr;  //Modem conttrol
  reg32 mu_lsr;  //Line status
  reg32 mu_msr;  //Modem status
  reg32 mu_scratch; //Scratch
  reg32 mu_control; //Extra control
  reg32 mu_status;  //extra status
  reg32 mu_baudrate; //BAud rate
};

#define AUX_REGS ((struct AuxRegs *)(AUX_BASE))

