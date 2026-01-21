#pragma once



/*@brief
 * Function to init the service
 * @param NOTHING YOU BITCH
 * @retval NOTHING YOU BITCH
 */
void uart_init(void);


char uart_read(void);


void uart_write(char c);


void uart_write_array(char *str);


