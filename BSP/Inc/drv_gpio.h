#ifndef __DRV_GPIO_H__
#define __DRV_GPIO_H__

#ifdef __cplusplus
extern "C" {    
}
#endif
#include "stm32f4xx_hal.h"

typedef struct{
    GPIO_TypeDef  *port;              // GPIOx端口
    uint16_t       pin;               // GPIOx引脚
    uint8_t        mode;              // 模式
    uint8_t        pull;              // 上拉/下拉 0：无 1：上拉 2：下拉
    uint8_t        speed;             // 速度
    uint8_t        init_state;             // 初始状态: 0：开 1：关
} drv_gpio_type_t; 

void drv_gpio_init(drv_gpio_type_t *gpio);
void drv_gpio_set_level(drv_gpio_type_t *gpio, GPIO_PinState level);
int8_t drv_gpio_get_level(drv_gpio_type_t *gpio);
void drv_gpio_toggle(drv_gpio_type_t *gpio);
void drv_gpio_on(drv_gpio_type_t *gpio);
void drv_gpio_off(drv_gpio_type_t *gpio);
#endif