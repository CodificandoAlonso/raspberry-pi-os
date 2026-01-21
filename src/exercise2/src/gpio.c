#include "gpio.h"
#include "common.h"
#include "peripherials/gpio.h"

#include "utils.h"
#include <stdint.h>



void gpio_pin_set_func(byte pinNum, GpioFunc func) {
  byte which_gpiofunc = pinNum / 10;
  byte which_bit_pin_func = (pinNum % 10) * 3;

  //Clear bits. AND with (000) [~(7[111])] So clear the 3 bits for the new value
  GPIO_REGS->func_selec[which_gpiofunc] &= ~(7 << which_bit_pin_func);

  //Or with the new val.
  GPIO_REGS->func_selec[which_gpiofunc] |= func << which_bit_pin_func;
}



void gpio_pin_set(byte pinNum, Gpio_pu_pd mode) {
  //Divide into 16 to map the reg32 corresponding
  byte which_regpin = pinNum /16;
  byte which_bit_pin_enable = (pinNum %16) * 2;


  //Clear Val
  GPIO_REGS->pu_pd_reg[which_regpin] &= ~(3 << which_bit_pin_enable);

  //OR with the new val
  GPIO_REGS->pu_pd_reg[which_regpin] |= (mode << which_bit_pin_enable);

}



void gpio_pin_setzero(byte pinNum) {
  //Divide into 16 to map the reg32 corresponding
  byte which_regpin = pinNum /16;
  byte which_bit_pin_enable = (pinNum %16) * 2;

  //Simply clear val to match 00.
  GPIO_REGS->pu_pd_reg[which_regpin] &= ~(3 << which_bit_pin_enable);
}

