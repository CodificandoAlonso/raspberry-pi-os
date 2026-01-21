#include "mm.h"
#include "common.h"
#include "peripherials/base.h"
#include "peripherials/aux.h"
#include "utils.h"
#include "mini_uart.h"
#include "peripherials/gpio.h"
#include "gpio.h"
#include <stdbool.h>

#define BAUD_RATE 0x21E

void uart_init() {

  //set aux registers to MUART mode



  //1. Activate enables for uart config
  AUX_REGS->enables &= ~(1 << 0);
  AUX_REGS->enables |= 1 << 0;

  //2. Clear possible RX TX activations on AUX_mu_CNTL

  AUX_REGS->mu_control &= ~(3 << 0);





  //3. Clear possible interrupts by setting to 0 the int handlers

    //Set to 0 the enable receiver and enable transmitter (Bits 1 and 0);
  AUX_REGS->mu_ier &= ~(3 << 0);



  //4. Set data format: 8 bits. Clear DLAB on line status control reg
    //For that, as always, clean it up.

    //4.1 Clear and set 0 on DLAB
    AUX_REGS->mu_lcr &= ~(1 << 7);

    //4.2 Clear and set 1 on DATA SIZE bit (0)
    AUX_REGS->mu_lcr &= ~(1 << 0);
    AUX_REGS->mu_lcr |= 1 << 0;



  //5. Clear and set baud rate. Can be redundant if = directly, pero me gusta coño
  AUX_REGS->mu_baudrate &= ~(65535 << 0);
  AUX_REGS->mu_baudrate |= BAUD_RATE;


  //CONFIGURE GPIO PINS

  gpio_pin_set(14, PullNone);
  gpio_pin_set(15, PullNone);

  //Set GPIO 14 15 to alt5 (UART1 === Mini UART)
  gpio_pin_set_func(14, GFAlt5);
  gpio_pin_set_func(15, GFAlt5);


  /*It specifies that the control bits should be set after the GPIO pins were
  *being set up
  *
  */
  //FINAL STEP. PUT AUX_MU_CNTRL ON UART MODE
    //Set 1 in bit 0 and 1, no clear because done in step 2
    AUX_REGS->mu_control |= 3 << 0;


}


/*
Purpose: Wait for a character to arrive, then return it.
Steps:

Loop: read status register, check "data ready" bit
When ready, read from data register
Return the byte

What to find:

Which register has the status flags?
Which bit means "receive FIFO has data"?
Which register holds received data?
*/


char uart_read() {

    bool cond = (AUX_REGS->mu_status & (1 << 0)) != 0;

    while (cond != true){
        cond = (AUX_REGS->mu_status & (1 << 0)) != 0;
    }

    //Byte available to read.

    char ret_val = (AUX_REGS->mu_io >> 0) & ((1 << 8) -1);
    return ret_val;
}



/*
*Purpose: Wait until UART can accept a byte, then send it.
Steps:

Loop: read status register, check "transmitter ready" bit
When ready, write character to data register

What to find:

Which bit means "transmitter empty/ready"?
Same data register as read, or different?
*/


void uart_write(char c) {


    //Wait until acceptance of symbol
    bool cond = (AUX_REGS->mu_status & (1 << 1)) != 0;

    while (cond != true){
        cond = (AUX_REGS->mu_status & (1 << 1)) != 0;
    }

    //Byte available to read.
    AUX_REGS->mu_io = c;
}



/*
*Purpose: Send a null-terminated string.
Steps:

While current character is not '\0'
Call uart_write with current character
Move to next character

*/


void uart_write_array(char *str) {

  byte str_ctr = 0;
  while (str[str_ctr] != '\0') {
    uart_write(str[str_ctr++]);
  }

}