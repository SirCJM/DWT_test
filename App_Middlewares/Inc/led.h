#ifndef  __LED_H__
#define  __LED_H__ 
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif  
#include "drv_gpio.h"
/*  led函数映射 */
#define  drv_led_init               drv_gpio_init          
#define  drv_led_on                 drv_gpio_on            
#define  drv_led_off                drv_gpio_off           
#define  drv_led_set_level          drv_gpio_set_level     
#define  drv_led_toggle             drv_gpio_toggle        



// extern drv_gpio_type_t led_green;


#ifdef __cplusplus
}
#endif
#endif /* __LED_H__ */