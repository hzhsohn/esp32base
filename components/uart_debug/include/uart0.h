/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "driver/uart.h"


/**
 * - Port: UART1
 * - Receive (Rx) buffer: on
 * - Transmit (Tx) buffer: off
 * - Flow control: off
 * - Event queue: off
 * - Pin assignment: see defines below
 */
#define ECHO_TEST_RXD0  (GPIO_NUM_34)
#define ECHO_TEST_TXD0  (GPIO_NUM_35)
#define ECHO_TEST_RTS0  (UART_PIN_NO_CHANGE)
#define ECHO_TEST_CTS0  (UART_PIN_NO_CHANGE)

#define BUF_SIZE0 (1024)

typedef void PF_UART_DEBUG_READ (const char *buf, int len);

//-----------------------------------------------------------------
int uart_debug_send( const char * data, int len);
void uart_debug_init(PF_UART_DEBUG_READ*pf);
void task_uart_debug();