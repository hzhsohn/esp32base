#include "h_gpio.h"

/*******************************************************************************
* 函数名       : zhSCM_initKeyState
* 函数描述      : 初始各GPIO按键状态
* 输入参数      : EzhKeyState的变量
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
void zhSCM_initKey(EzhKeyState *status, gpio_num_t pin)
{
	    gpio_config_t io_conf;

		io_conf.intr_type = GPIO_PIN_INTR_POSEDGE;
		io_conf.pin_bit_mask = ((((uint64_t)1) << pin));
		io_conf.mode = GPIO_MODE_INPUT;
		io_conf.pull_up_en =0;
		io_conf.pull_down_en = 0;
		gpio_config(&io_conf);

		*status=ZH_KEY_STATE_0;
}
/*******************************************************************************
* 函数名       : zhSCM_keyState
* 函数描述      : 检测各GPIO端口按键功能
* 输入参数      : 几个呗
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
EzhKeyEvent zhSCM_keyState(EzhKeyState *status,gpio_num_t pin)
{
		int keyPort=0;
		EzhKeyEvent evnt;
		//读取GPIOA的I/O电平
		keyPort=gpio_get_level(pin); 
		keyPort=!keyPort;
		switch(*status)
		{
				/*使用switch判断检测是否为抖动信号,如果不是便即行操作*/
				case ZH_KEY_STATE_1:
				{
				 //非抖动进行操作
				 if(keyPort)
				 {
						evnt=ZH_KEY_EVENT_DOWN;
						//切换状态
						 *status=ZH_KEY_STATE_2;
				 }
				 else
				 {
						 evnt=ZH_KEY_EVENT_NONE; 
						 *status=ZH_KEY_STATE_0;
				 }
				}
				break;
				/*按压检测*/
				case ZH_KEY_STATE_2:
				{
					if(keyPort)
					{
					 evnt=ZH_KEY_EVENT_PRESS;
				 *status=ZH_KEY_STATE_3;
					}
					else
					{
						 //已经松手,更新状态
						 *status=ZH_KEY_STATE_0;
						 evnt=ZH_KEY_EVENT_UP;
					}
				}
				break;
				/*松手检测*/
				case ZH_KEY_STATE_3:
				{
					if(keyPort)
					{
						evnt=ZH_KEY_EVENT_PRESS;
					}
					else
					{
						 //已经松手,更新状态
						 *status=ZH_KEY_STATE_0;
						 evnt=ZH_KEY_EVENT_UP;
					}
				}
				break;
				default:
				{
							//有电平信号输入
							if(keyPort)
							{
									*status=ZH_KEY_STATE_1; 
							}
							evnt=ZH_KEY_EVENT_NONE;
				}
				break;
		}
		return evnt;
}


