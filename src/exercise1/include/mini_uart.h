#pragma once



/*@brief
 * Function to init the service
 * @param NOTHING YOU BITCH
 * @retval NOTHING YOU BITCH
 */
void uart_init();


char uart_read();


void uart_write(char c);


void uart_write_array(char *str);