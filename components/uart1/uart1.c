#include "uart1.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "esp_log.h"
#include "driver/uart.h"

bool isUart1Enable=false;
PF_UART_READ* g_pfUartRead=NULL;

void task_uart1()
{
	printf("task_uart1 thread run...\n");
    
    // Configure a temporary buffer for the incoming data
    uint8_t *data = (uint8_t *) malloc(BUF_SIZE);

    while (isUart1Enable) {
        // Read data from the UART
        int len = uart_read_bytes(UART_NUM_1, data, BUF_SIZE, 20 / portTICK_RATE_MS);
		if(len>0)
		{
			if(g_pfUartRead)
			{
				g_pfUartRead((const char*)data, len);
			}
		}
    }
		
	free(data);
	vTaskDelete(NULL); 
	printf("task_uart1 thread destory...\n");
}

int uart1_send( const char * data, int len)
{	
	if(isUart1Enable)
	{
		// Write data back to the UART
		return uart_write_bytes(UART_NUM_1, (const char *) data, len);
	}
	return 0;
}
void uart1_init(PF_UART_READ*pf , int baud_rate)
{
	if(baud_rate<=0)
	{
		return;
	}
	g_pfUartRead=pf;

    uart_config_t uart_config = {
        .baud_rate = baud_rate,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS);
    uart_driver_install(UART_NUM_1, BUF_SIZE, 0, 0, NULL, 0);

	isUart1Enable=true;
}