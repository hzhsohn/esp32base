#include "h_gpio.h"

PF_GPIO_STATUS* g_pf_h_gpio;

typedef void PF_UART_READ (const char *buf, int len);

void task_gpio_thread()
{
	//闪烁
    while (1) {
	  //------------------------------------
	  //按钮1
      if(0==BTN1_GetStatus)
	  {
		  //按下
		  //printf("BTN1 down\n");
		 if(g_pf_h_gpio)
		 {
			g_pf_h_gpio(GPIO_INPUT_IO_0,1);
		 }
	  }
	  else
	  {
		  //没按
		  //printf("BTN1 up\n");
		  if(g_pf_h_gpio)
		 {
			g_pf_h_gpio(GPIO_INPUT_IO_0,0);
		 }
	  }
	  /*//------------------------------------
	  //按钮2
      if(0==BTN2_GetStatus)
	  {
		  //按下
		  //printf("BTN2 down\n");
		 if(g_pf_h_gpio)
		 {
			g_pf_h_gpio(GPIO_INPUT_IO_2,1);
		 }
	  }
	  else
	  {
		  //没按
		  //printf("BTN2 up\n");
		  if(g_pf_h_gpio)
		 {
			g_pf_h_gpio(GPIO_INPUT_IO_2,0);
		 }
	  }
	  //------------------------------------
	  //按钮3
      if(0==BTN3_GetStatus)
	  {
		  //按下
		  //printf("BTN3 down\n");
		 if(g_pf_h_gpio)
		 {
			g_pf_h_gpio(GPIO_INPUT_IO_3,1);
		 }
	  }
	  else
	  {
		  //没按
		  //printf("BTN3 up\n");
		  if(g_pf_h_gpio)
		 {
			g_pf_h_gpio(GPIO_INPUT_IO_3,0);
		 }
	  }
	  */
	  //------------------------------------
	  vTaskDelay(100 / portTICK_PERIOD_MS);
    }

	vTaskDelete(NULL); 
}


void h_gpio_init(PF_GPIO_STATUS* pf)
{
	g_pf_h_gpio=pf;
	//---
    gpio_config_t io_conf;
	//输入-----------------------------------------------------
    io_conf.intr_type = GPIO_PIN_INTR_POSEDGE;
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en =0;
	io_conf.pull_down_en = 0;
    gpio_config(&io_conf);
	/*
	//输入-----------------------------------------------------
    io_conf.intr_type = GPIO_PIN_INTR_POSEDGE;
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL2;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en =0;
	io_conf.pull_down_en = 0;
    gpio_config(&io_conf);

	//输入-----------------------------------------------------
    io_conf.intr_type = GPIO_PIN_INTR_POSEDGE;
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL3;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en =0;
	io_conf.pull_down_en = 0;
    gpio_config(&io_conf);
	*/
}
