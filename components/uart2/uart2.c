#include "uart2.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "esp_log.h"
#include "driver/uart.h"

bool isUart2Enable=false;
PF_UART_READ2* g_pfUart2Read=NULL;

void task_uart2()
{
	printf("task_uart2 thread run...\n");

    // Configure a temporary buffer for the incoming data
    uint8_t *data = (uint8_t *) malloc(BUF_SIZE2);

    while (isUart2Enable) {
        // Read data from the UART
        int len = uart_read_bytes(UART_NUM_2, data, BUF_SIZE2, 20 / portTICK_RATE_MS);
		if(len>0)
		{
			if(g_pfUart2Read)
			{
				g_pfUart2Read((const char*)data, len);
			}
		}
    }

	free(data);
	vTaskDelete(NULL);
	printf("task_uart2 thread destory...\n");
}

int uart2_send( const char * data, int len)
{	
	if(isUart2Enable)
	{
		// Write data back to the UART
		return uart_write_bytes(UART_NUM_2, (const char *) data, len);
	}
	return 0;
}

void uart2_init(PF_UART_READ2*pf , int baud_rate)
{
	if(baud_rate<=0)
	{
		return;
	}

	g_pfUart2Read=pf;

    uart_config_t uart_config = {
        .baud_rate = baud_rate,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    uart_param_config(UART_NUM_2, &uart_config);
    uart_set_pin(UART_NUM_2, ECHO_TEST_TXD2, ECHO_TEST_RXD2, ECHO_TEST_RTS2, ECHO_TEST_CTS2);
    uart_driver_install(UART_NUM_2, BUF_SIZE2 , 0, 0, NULL, 0);

	isUart2Enable=true;
}