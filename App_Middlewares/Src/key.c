#include "key.h"
#include "FreeRTOS.h"
#include "drv_gpio.h"
#include "projdefs.h"
#include "cmsis_os.h"
#include "task.h"
#include "stm32f4xx_hal.h"
#include <stdint.h>

typedef enum {
    KEY_EVENT_NONE                     = 0x01,                      // 无事件
    KEY_EVENT_PRESSED                  = 0x02,                      // 按键按下事件    
    KEY_EVENT_RELEASED                 = 0x04,                      // 按键释放事件
    KEY_EVENT_SINGLE_CLICK             = 0x08,                      // 单击事件
    KEY_EVENT_DOUBLE_CLICK             = 0x10,                      // 双击事件
    KEY_EVENT_LONG_PRESS               = 0x20,                      // 长按事件
} key_event_t;

typedef enum {
    KEY_STATE_IDLE = 0,                           // 空闲状态
    KEY_STATE_DEBOUNCE_PRESS,                     // 按键去抖状态   
    KEY_STATE_PRESSED,                            // 按键按下状态
    KEY_STATE_DEBOUNCE_RELEASE,                   // 按键释放去抖状态
    KEY_STATE_WAIT_DOUBLE,                        // 等待双击状态    
} key_state_t;


drv_gpio_type_t key_sw = {
    .port = GPIOA,
    .pin = GPIO_PIN_0,
    .mode = GPIO_MODE_INPUT,
    .pull = GPIO_PULLDOWN,
    .speed = GPIO_SPEED_FREQ_VERY_HIGH,
    .init_state = 0
};
/**
 * key_Task
 * @param argument
 * @return None
 * @note None
 */
void key_Task(void *argument)
{
    drv_key_init(&key_sw);
    key_event_t event = KEY_EVENT_NONE;
    while (1) {
        
        // event = keyScan(&key_sw);

        osDelay(pdMS_TO_TICKS(pdMS_TO_TICKS(1)));
    }
}

// key_event_t  keyScan(drv_gpio_type_t *key)
// {
//     static uint8_t last_state = 0;  // 上一次的按键状态
//     static key_state_t state = KEY_STATE_IDLE;
//     static uint32_t last_press_time = 0;

//     key_event_t event = KEY_EVENT_NONE;
//     uint8_t current_state = drv_key_get_level(key);
//     uint32_t now = HAL_GetTick();
//     uint32_t press_time = now - last_press_time;

//     swtich (state) {
//         case KEY_STATE_IDLE:
//             if (current_state != last_state) {
//                 event |= KEY_EVENT_PRESSED;
//             }
//             break;
//         case KEY_STATE_PRESSED:
//             if (current_state != last_state) {
//                 event |= KEY_EVENT_RELEASED;
//             }
//             break;
//         default:
//             break;
//     }
//     if (current_state != last_state) {
//         last_state = current_state;
//         if (current_state == GPIO_PIN_SET) {
//             // 按键按下事件
//             SEGGER_RTT_printf(0, "Key Pressed\r\n");
//         } else {
//             // 按键释放事件
//             SEGGER_RTT_printf(0, "Key Released\r\n");
//         }
//     }
//     return KEY_EVENT_NONE;
// }