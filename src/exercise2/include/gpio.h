#pragma once



#include "peripherials/base.h"

#include "common.h"


typedef enum _GpioFunc {
  GFInput = 0,
  GFOutput = 1,
  GFAlt0 = 4,
  GFAlt1 = 5,
  GFAlt2 = 6,
  GFAlt3 = 7,
  GFAlt4 = 3,
  GFAlt5 = 2
} GpioFunc;


typedef enum _Gpio_pu_pd{
  PullNone = 0,  // 00
  PullUp   = 1,  // 01
  PullDown = 2,  // 10
  Reserved = 3   // 11
} Gpio_pu_pd;

void gpio_pin_set_func(byte pinNum, GpioFunc func);


void gpio_pin_set(byte pinNum, Gpio_pu_pd mode);


void gpio_pin_setzero(byte pinNum);