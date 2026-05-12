#include "led.h"
// 1. 必须先包含 FreeRTOS 核心头文件，定义 TickType_t
#include "FreeRTOS.h"
// 2. 再包含 projdefs.h（通常 FreeRTOS.h 已经包含它，但显式包含更安全）
#include "projdefs.h"
// 3. 然后是 CMSIS-RTOS API 头文件（提供 osDelay）
#include "cmsis_os.h"
// 4. 最后是其他依赖 FreeRTOS 的头文件，比如 task.h
#include "task.h"

drv_gpio_type_t led_green = {
    .port = GPIOH,
    .pin = GPIO_PIN_12,
    .mode = GPIO_MODE_OUTPUT_PP,
    .pull = GPIO_PULLUP,
    .speed = GPIO_SPEED_FREQ_VERY_HIGH,
    .init_state = 0
};
drv_gpio_type_t led_red = {
    .port = GPIOH,
    .pin = GPIO_PIN_11,
    .mode = GPIO_MODE_OUTPUT_PP,
    .pull = GPIO_PULLUP,
    .speed = GPIO_SPEED_FREQ_VERY_HIGH,
    .init_state = 0
};
drv_gpio_type_t led_blue = {
    .port = GPIOH,
    .pin = GPIO_PIN_10,
    .mode = GPIO_MODE_OUTPUT_PP,
    .pull = GPIO_PULLUP,
    .speed = GPIO_SPEED_FREQ_VERY_HIGH,
    .init_state = 0
};

void led_Task(void *argument) {
    drv_led_init(&led_green);
    drv_led_init(&led_red);
    drv_led_init(&led_blue);
    while(1){
        drv_led_toggle(&led_green);
        osDelay(pdMS_TO_TICKS(100));
        drv_led_toggle(&led_red);
        osDelay(pdMS_TO_TICKS(100));
        drv_led_toggle(&led_blue);
        osDelay(pdMS_TO_TICKS(100));
    } 
}

