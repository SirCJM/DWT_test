#ifndef __DRV_IIC_H
#define __DRV_IIC_H 


#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif  

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include <stdbool.h>
#include "drv_dwt.h"
/*模拟IIC*/
/// @note 该驱动使用GPIO模拟IIC通信，支持可选的中断功能。用户需要根据实际连接的SDA、SCL和INT引脚配置IIC_GPIOTYPE_T结构体，并调用drv_iic_init进行初始化。中断配置通过drv_iic_enable_int函数完成，用户可以指定触发方式和优先级。驱动提供了基本的IIC操作函数，如start、stop、ack、nack、发送和接收字节等，方便用户进行IIC通信。
typedef struct
{
    uint32_t trigger;           // 中断触发方式 （如GPIO_MODE_IT_RISING, GPIO_MODE_IT_FALLING, GPIO_MODE_IT_RISING_FALLING）
    uint32_t preempt_priority;  // 抢占优先级
    uint32_t sub_priority;      // 优先级
}IIC_EXITTypeDef;
/// @brief IIC GPIO配置结构体
typedef void (*ExtiCallback_t)(uint16_t GPIO_PIN); // 中断回调函数类型定义
typedef struct
{
    GPIO_TypeDef  *SDA_PORT; // SDA GPIO端口
    GPIO_TypeDef  *SCL_PORT; // SCL GPIO端口
    GPIO_TypeDef  *INT_PORT; // 中断GPIO端口
    uint16_t SDA_PIN;   // SDA引脚
    uint16_t SCL_PIN;   // SCL引脚
    uint16_t INT_PIN;   // 中断引脚
    bool     IS_INT;    // 是否使用中断方式 
    IIC_EXITTypeDef EXITConfig; // 中断配置结构体 
    ExtiCallback_t ExtiCallback; // 中断回调函数指针
}IIC_GPIOTYPE_T;


void drv_iic_init(IIC_GPIOTYPE_T *iic);
void drv_iic_deinit(IIC_GPIOTYPE_T *iic);
bool drv_iic_enable_int(IIC_GPIOTYPE_T *iic);

void drv_iic_start(IIC_GPIOTYPE_T *iic);
void drv_iic_stop(IIC_GPIOTYPE_T *iic);
uint8_t drv_iic_wait_ack(IIC_GPIOTYPE_T *iic);
void drv_iic_ack(IIC_GPIOTYPE_T *iic);


void drv_iic_nack(IIC_GPIOTYPE_T *iic);
uint8_t drv_iic_wait_ack(IIC_GPIOTYPE_T *iic);
void drv_iic_send_byte(IIC_GPIOTYPE_T *iic, uint8_t byte);
int8_t drv_iic_read_byte(IIC_GPIOTYPE_T *iic, uint8_t ack);



#ifdef __cplusplus
}
#endif
#endif /* __DRV_IIC_H */
