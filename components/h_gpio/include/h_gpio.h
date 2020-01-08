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


//���밴������
#define GPIO_INPUT_IO_0				GPIO_NUM_39//WIFI�򿪰���
#define GPIO_INPUT_PIN_SEL			((((uint64_t)1)<<GPIO_INPUT_IO_0))
#define BTN1_GetStatus				gpio_get_level(GPIO_INPUT_IO_0)
/*
//���밴������2
#define GPIO_INPUT_IO_2				GPIO_NUM_34//����1
#define GPIO_INPUT_PIN_SEL2			((((uint64_t)1)<<GPIO_INPUT_IO_2))
#define BTN2_GetStatus				gpio_get_level(GPIO_INPUT_IO_2)

//���밴������3
#define GPIO_INPUT_IO_3				GPIO_NUM_35//��ť2
#define GPIO_INPUT_PIN_SEL3			((((uint64_t)1)<<GPIO_INPUT_IO_3))
#define BTN3_GetStatus				gpio_get_level(GPIO_INPUT_IO_3)
*/
//�ص�
typedef void PF_GPIO_STATUS (int gpio_num, int is_press);
void h_gpio_init(PF_GPIO_STATUS* pf);
void task_gpio_thread();


#ifdef __cplusplus
}
#endif

#endif /*#ifndef __HAL_GPIO_IN_OUT_H__*/