#include "uart0.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "esp_log.h"
#include "driver/uart.h"

bool isUart0Enable=false;
PF_UART_DEBUG_READ* g_pfUartDebugRead=NULL;

void task_uart_debug()
{
	printf("task_uart_debug thread run...\n");
    
    // Configure a temporary buffer for the incoming data
    uint8_t *data = (uint8_t *) malloc(BUF_SIZE0);

    while (1) {
        // Read data from the UART
        int len = uart_read_bytes(UART_NUM_0, data, BUF_SIZE0, 20 / portTICK_RATE_MS);
		if(len>0)
		{
			if(g_pfUartDebugRead)
			{
				g_pfUartDebugRead((const char*)data, len);
			}
		}
    }

	free(data);
	vTaskDelete(NULL);
}

int uart_debug_send( const char * data, int len)
{	
	if(isUart0Enable)
	{
		// Write data back to the UART
		return uart_write_bytes(UART_NUM_0, (const char *) data, len);
	}
	return 0;
}
void uart_debug_init(PF_UART_DEBUG_READ*pf)
{
	g_pfUartDebugRead=pf;

    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    uart_param_config(UART_NUM_0, &uart_config);
    uart_set_pin(UART_NUM_0, ECHO_TEST_TXD0, ECHO_TEST_RXD0, ECHO_TEST_RTS0, ECHO_TEST_CTS0);
    uart_driver_install(UART_NUM_0, BUF_SIZE0 , 0, 0, NULL, 0);

	isUart0Enable=true;
}