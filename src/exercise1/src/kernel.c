#include "peripherials/base.h"
#include "common.h"
#include "mini_uart.h"


void kernel_main() {

  uart_init();
  uart_write_array("Hello bare metal world\n");


  while (1) {
    uart_write(uart_read());

  } //End while

}//End void