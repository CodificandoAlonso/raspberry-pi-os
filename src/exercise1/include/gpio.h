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


void gpio_pin_set_func(byte pinNum, GpioFunc func);

void gpio_pin_enable(byte pinNum);