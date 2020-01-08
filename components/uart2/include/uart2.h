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
 * - Port: UART2
 * - Receive (Rx) buffer: on
 * - Transmit (Tx) buffer: off
 * - Flow control: off
 * - Event queue: off
 * - Pin assignment: see defines below
 */
#define ECHO_TEST_RXD2  (GPIO_NUM_14)
#define ECHO_TEST_TXD2  (GPIO_NUM_15)
#define ECHO_TEST_RTS2  (UART_PIN_NO_CHANGE)
#define ECHO_TEST_CTS2  (UART_PIN_NO_CHANGE)

#define BUF_SIZE2 (1024)

typedef void PF_UART_READ2 (const char *buf, int len);

//-----------------------------------------------------------------
int uart2_send( const char * data, int len);
void uart2_init(PF_UART_READ2*pf , int baud_rate);
void task_uart2();