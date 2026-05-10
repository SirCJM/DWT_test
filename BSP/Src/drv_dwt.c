#include "drv_dwt.h"


static uint32_t DWT_CPU_FREQ_MHZ = 0;  // CPU频率，单位MHz
void DWT_Init(uint32_t cpu_freq_mhz) {
    DWT_CPU_FREQ_MHZ = cpu_freq_mhz;
    
    // 1. 使能调试跟踪模块
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    
    // 2. 清零周期计数器
    DWT->CYCCNT = 0;
    
    // 3. 使能CYCCNT计数器
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}
uint8_t DWT_Check(void) {
    return (DWT->CTRL & DWT_CTRL_CYCCNTENA_Msk) ? 1 : 0;
}
void DWT_DelayUs(uint32_t us) {
    uint32_t start = DWT->CYCCNT;
    uint32_t target = start + (us * DWT_CPU_FREQ_MHZ);
    
    // 处理溢出情况
    if (target > 0xFFFFFFFF) {
        // 等待计数器溢出
        while (DWT->CYCCNT < start);
        // 等待剩余时间
        while (DWT->CYCCNT < (target - 0xFFFFFFFF));
    } else {
        while (DWT->CYCCNT < target);
    }
}
void DWT_DelayMs(uint32_t ms) {
  if(ms == 0) return;
  while (ms--) {
    DWT_DelayUs(1000);
  }
    
}