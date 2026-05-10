#ifndef __DRV_DWT_H
#define __DRV_DWT_H 


#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif  

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include <stdbool.h>
void DWT_Init(uint32_t cpu_freq_mhz);
uint8_t DWT_Check(void);
void DWT_DelayUs(uint32_t us);


#ifdef __cplusplus
}
#endif
#endif /* __DRV_DWT_H */
