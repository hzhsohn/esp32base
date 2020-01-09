#ifndef __HAL_GPIO_IN_OUT_H__
#define __HAL_GPIO_IN_OUT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

/*
  2018/7/9
  Copyright (C) Han.zhihong
	
	
例子:
	
	EzhKeyEvent ev;
	EzhKeyState btn1;//一个GPIO对应一个EzhKeyState对象
	EzhKeyState btn2;

	zhSCM_initKey(&btn1,GPIO_NUM_39);
	zhSCM_initKey(&btn2,GPIO_NUM_36);

   
//按键
void task_gpio_thread()
{
    while (1) 
	{	  
		//-------------------
		//长按出厂设置
		//initFactoryCfgFlash();
		//esp_restart();

		//出厂设置
		ev=zhSCM_keyState(&btnFactory,GPIO_NUM_39);
		switch(ev)
		{
				case ZH_KEY_EVENT_NONE:
					break;
				case ZH_KEY_EVENT_DOWN:
					break;
				case ZH_KEY_EVENT_PRESS:
					break;
				case ZH_KEY_EVENT_UP:					
					printf("GPIO_NUM_39 ZH_KEY_EVENT_UP\n");
				break;
		}
		//按键1
		ev=zhSCM_keyState(&btn1,GPIO_NUM_34);
		switch(ev)
		{
				case ZH_KEY_EVENT_NONE:
					break;
				case ZH_KEY_EVENT_DOWN:
					break;
				case ZH_KEY_EVENT_PRESS:
					break;
				case ZH_KEY_EVENT_UP:
					printf("GPIO_NUM_34 ZH_KEY_EVENT_UP\n");
					break;
		}
		vTaskDelay(20 / portTICK_PERIOD_MS);
    }

	vTaskDelete(NULL); 
}
	
*/


/*******************************************************************************
* 类型         : 按键
* 描述          : 按键设置的状态和事件标识
*******************************************************************************/
typedef enum _EzhKeyState{
   ZH_KEY_STATE_0,
   ZH_KEY_STATE_1,
   ZH_KEY_STATE_2,
   ZH_KEY_STATE_3,
}EzhKeyState;
typedef enum _EzhKeyEvent{
   ZH_KEY_EVENT_NONE,
   ZH_KEY_EVENT_DOWN,
   ZH_KEY_EVENT_PRESS,
   ZH_KEY_EVENT_UP
}EzhKeyEvent;


void zhSCM_initKey(EzhKeyState *status, gpio_num_t pin);
EzhKeyEvent zhSCM_keyState(EzhKeyState *status,gpio_num_t pin);


#ifdef __cplusplus
}
#endif

#endif /*#ifndef __HAL_GPIO_IN_OUT_H__*/