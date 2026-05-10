#include "drv_gpio.h"
#include <stdint.h>

/**
 * @brief GPIO初始化
 * @param gpio GPIO结构体指针
 * @retval 无
*/
void drv_gpio_init(drv_gpio_type_t *gpio)
{
    
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    /* gpio 参数检查 */
    if(gpio == NULL) return;
    /* GPIO Ports Clock Enable */
    if(gpio->port == GPIOA) __HAL_RCC_GPIOA_CLK_ENABLE();
    if(gpio->port == GPIOB) __HAL_RCC_GPIOB_CLK_ENABLE();
    if(gpio->port == GPIOC) __HAL_RCC_GPIOC_CLK_ENABLE();
    if(gpio->port == GPIOD) __HAL_RCC_GPIOD_CLK_ENABLE();
    if(gpio->port == GPIOE) __HAL_RCC_GPIOE_CLK_ENABLE();
    if(gpio->port == GPIOF) __HAL_RCC_GPIOF_CLK_ENABLE();
    if(gpio->port == GPIOG) __HAL_RCC_GPIOG_CLK_ENABLE();
    if(gpio->port == GPIOH) __HAL_RCC_GPIOH_CLK_ENABLE();

    /* GPIO 模式初始化  */
    GPIO_InitStruct.Pin = gpio->pin;
    GPIO_InitStruct.Mode = gpio->mode;
    GPIO_InitStruct.Pull = gpio->pull;
    GPIO_InitStruct.Speed = gpio->speed;
    HAL_GPIO_Init(gpio->port, &GPIO_InitStruct);
    /* 设置初始状态 */
    if(gpio->mode == GPIO_MODE_OUTPUT_PP || gpio->mode == GPIO_MODE_OUTPUT_OD)
    {
        HAL_GPIO_WritePin(gpio->port, gpio->pin, gpio->init_state);
    }

}
/**
 * @brief GPIO去初始化
 * @param gpio GPIO结构体指针
 * @retval 无
 */
void drv_gpio_deinit(drv_gpio_type_t *gpio)
{
    if(gpio == NULL) return;
    HAL_GPIO_DeInit(gpio->port, gpio->pin);  
}
/**
 * @brief 设置GPIO电平
 * @param gpio GPIO结构体指针
 * @param level 电平状态
 * @retval 无
 */
void drv_gpio_set_level(drv_gpio_type_t *gpio, GPIO_PinState level)
{
    if(gpio == NULL) return;
    HAL_GPIO_WritePin(gpio->port, gpio->pin, level);
}
/**
 * @brief 获取GPIO电平
 * @param gpio GPIO结构体指针
 * @retval 如果gpio指针为NULL则返回-1，否则返回GPIO电平 
*/
int8_t drv_gpio_get_level(drv_gpio_type_t *gpio)
{
    if(gpio == NULL) return -1;
    return HAL_GPIO_ReadPin(gpio->port, gpio->pin);
}
/**
 * @brief GPIO翻转
 * @param gpio GPIO结构体指针
 * @retval 无
 */
void drv_gpio_toggle(drv_gpio_type_t *gpio)
{
    if(gpio == NULL) return;
    HAL_GPIO_TogglePin(gpio->port, gpio->pin);
}