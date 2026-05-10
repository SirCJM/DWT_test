#include "drv_iic.h"
#include "stm32f429xx.h"
#include "stm32f4xx_hal_gpio.h"
#include <stdint.h>


/**
 * @brief DWT（Data Watchpoint and Trace）延时函数实现
 * @param cpu_freq_mhz CPU频率，单位MHz
 * @return 无
 * @note 该函数初始化DWT模块，使能周期计数器，并设置CPU频率以便正确计算延时。
 *       在调用DWT_DelayUs之前必须先调用DWT_Init进行初始化。
 *       DWT_DelayUs函数使用DWT的CYCCNT寄存器来实现微秒级的延时。
 *       DWT_Check函数用于检查DWT是否已正确初始化和使能。
*/
static void drc_iic_delay_us(void) {
    DWT_DelayUs(2);
}
/**
 * @brief 启动IIC时钟
 * @param port GPIO_TypeDef* 需要启动时钟的GPIO端口
 * @return 无
 * @note 该函数根据传入的GPIO端口指针，启用对应的GPIO时钟。支持GPIOA到GPIOH。
 * @attention 该函数必须在IIC_GPIOTYPE_T结构体初始化之后调用。
 *            如果SDA和SCL端口相同，确保时钟只启用一次。
 *            该函数不检查传入的端口指针是否合法，调用者需要确保传入的端口指针有效。
*/
static void drv_enable_clk(GPIO_TypeDef  *port)
{
    if(port == GPIOA) __HAL_RCC_GPIOA_CLK_ENABLE();
    if(port == GPIOB) __HAL_RCC_GPIOB_CLK_ENABLE();
    if(port == GPIOC) __HAL_RCC_GPIOC_CLK_ENABLE();
    if(port == GPIOD) __HAL_RCC_GPIOD_CLK_ENABLE();
    if(port == GPIOE) __HAL_RCC_GPIOE_CLK_ENABLE();
    if(port == GPIOF) __HAL_RCC_GPIOF_CLK_ENABLE();
    if(port == GPIOG) __HAL_RCC_GPIOG_CLK_ENABLE();
    if(port == GPIOH) __HAL_RCC_GPIOH_CLK_ENABLE();
    // if(port == GPIOI) __HAL_RCC_GPIOI_CLK_ENABLE();
    // if(port == GPIOJ) __HAL_RCC_GPIOJ_CLK_ENABLE();
    // if(port == GPIOK) __HAL_RCC_GPIOK_CLK_ENABLE();
}
/**
 * @brief 设置IIC SCL引脚电平
 * @param iic IIC_GPIOTYPE_T* IIC结构体指针
 * @param State GPIO_PinState SCL引脚电平状态
 * @return 无
 */
static void drv_iic_SCL_level(IIC_GPIOTYPE_T *iic, GPIO_PinState State)
{
    if(iic == NULL) return;
    HAL_GPIO_WritePin(iic->SCL_PORT, iic->SCL_PIN, State);
}

/**
 * @brief 设置IIC SDA引脚电平
 * @param iic IIC_GPIOTYPE_T* IIC结构体指针
 * @param State GPIO_PinState SDA引脚电平状态
 * @return 无
 */
static void drv_iic_SDA_level(IIC_GPIOTYPE_T *iic, GPIO_PinState State)
{
    if(iic == NULL) return;
    HAL_GPIO_WritePin(iic->SDA_PORT, iic->SDA_PIN, State);
}

/**
 * @brief 初始化IIC
 * @param iic IIC_GPIOTYPE_T* IIC结构体指针
 * @return 无
 */
void drv_iic_init(IIC_GPIOTYPE_T *iic)
{
    if(iic == NULL) return;
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    /* 1.时钟使能 SCL和 SDA端口 */
    drv_enable_clk(iic->SDA_PORT);
    drv_enable_clk(iic->SCL_PORT);
    /* 2.如果启用了中断，使能INT端口时钟 */
    if(iic->IS_INT && iic->INT_PORT != NULL)  drv_enable_clk(iic->INT_PORT);
    /* 3.配置SCL(推挽输出,上拉) */
    GPIO_InitStruct.Pin = iic->SCL_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(iic->SCL_PORT, &GPIO_InitStruct);
    /* 4.配置SDA(开漏输出,上拉) 这样就可以不用设置IO口方向了，开漏输出的时候(=1),也可以读取外部信号的高低电平*/
    GPIO_InitStruct.Pin = iic->SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;  //开漏输出 
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(iic->SDA_PORT, &GPIO_InitStruct);
    /* 5.如果启用了中断，配置INT引脚为输入模式 */
    if(iic->IS_INT && iic->INT_PORT != NULL){
        GPIO_InitStruct.Pin = iic->INT_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        HAL_GPIO_Init(iic->INT_PORT, &GPIO_InitStruct);
    }
    /* 5.初始化延时函数 */
    DWT_Init(180);  // STM32F429主频180MHz
    /* 6.默认拉高SCL和SDA */
    drv_iic_SCL_level(iic, GPIO_PIN_SET);   
    drv_iic_SDA_level(iic, GPIO_PIN_SET); 
}
/**
 * @brief 反初始化IIC
 * @param iic IIC_GPIOTYPE_T* IIC结构体指针
 * @return 无
 */
void drv_iic_deinit(IIC_GPIOTYPE_T *iic)
{
    if(iic == NULL) return;
    /* 1.复位SCL和SDA引脚 */
    HAL_GPIO_DeInit(iic->SCL_PORT, iic->SCL_PIN);
    HAL_GPIO_DeInit(iic->SDA_PORT, iic->SDA_PIN);
    /* 2.如果启用了中断，复位INT引脚 */
    if(iic->IS_INT && iic->INT_PORT != NULL){
        HAL_GPIO_DeInit(iic->INT_PORT, iic->INT_PIN);
    }
}


/**
 * @brief 发送IIC起始信号
 * @param iic IIC_GPIOTYPE_T* IIC结构体指针
 * @return 无
 */
void drv_iic_start(IIC_GPIOTYPE_T *iic){
    if(iic == NULL) return;
    drv_iic_SDA_level(iic, GPIO_PIN_SET);
    drv_iic_SCL_level(iic, GPIO_PIN_SET);
    drc_iic_delay_us();
    drv_iic_SDA_level(iic, GPIO_PIN_RESET);
    drc_iic_delay_us();
    drv_iic_SCL_level(iic, GPIO_PIN_RESET);
    drc_iic_delay_us();    
}

/**
 * @brief 发送IIC停止信号
 * @param iic IIC_GPIOTYPE_T* IIC结构体指针
 * @return 无
 * @note  stop信号的时序为：SCL保持高电平，SDA从低电平变为高电平。
 *        确保在发送停止信号之前，SCL已经拉高，并且SDA处于低电平状态。
 */
void drv_iic_stop(IIC_GPIOTYPE_T *iic){
    if(iic == NULL) return;
    drv_iic_SDA_level(iic, GPIO_PIN_RESET);   //
    drc_iic_delay_us();
    drv_iic_SCL_level(iic, GPIO_PIN_SET);
    drc_iic_delay_us();
    drv_iic_SDA_level(iic, GPIO_PIN_SET);     // 发送I2C停止信号
    drc_iic_delay_us();
}
/**
 * @brief 等待IIC应答信号
 * @param iic IIC_GPIOTYPE_T* IIC结构体指针
 * @return uint8_t 0-收到ACK，1-收到NACK，2-等待超时
 * @note 该函数在发送完一个字节后调用，用于等待从设备的ACK/NACK响应。
 *       函数会拉高SDA线并释放总线，然后等待SCL线被拉高。
 *       在SCL拉高期间，函数会读取SDA线的状态以判断是ACK还是NACK。
 *       如果在指定的超时时间内没有收到响应，函数将返回超时状态。
*/
uint8_t drv_iic_wait_ack(IIC_GPIOTYPE_T *iic){
    if(iic == NULL) return 1; //参数错误返回超时状态
    uint8_t waittime = 0;
    drv_iic_SDA_level(iic, GPIO_PIN_SET); //释放SDA线
    drc_iic_delay_us();

    drv_iic_SCL_level(iic, GPIO_PIN_SET); //拉高SCL线，准备读取ACK/NACK
    drc_iic_delay_us();

    while(HAL_GPIO_ReadPin(iic->SDA_PORT, iic->SDA_PIN)){
        waittime++;
        if(waittime > 100) {
            drv_iic_stop(iic);
            return 2; //等待超时，发送停止信号并退出循环
        }
        drc_iic_delay_us();
    }
    drv_iic_SCL_level(iic, GPIO_PIN_RESET); //拉低SCL线，结束ACK/NACK读取
    drc_iic_delay_us();
    return 0; // 收到ACK

}
/**
 * @brief 发送ACK信号
 * @param iic IIC_GPIOTYPE_T* IIC结构体指针
 * @return 无
 * @note 该函数在接收完一个字节后调用，用于向从设备发送ACK信号。
 *       函数会将SDA线拉低，并在SCL线拉高期间保持SDA线的状态，以表示ACK。
 *       确保在调用该函数之前，SCL线已经处于低电平状态。
*/
void drv_iic_ack(IIC_GPIOTYPE_T *iic){
    if(iic == NULL) return;

    drv_iic_SDA_level(iic, GPIO_PIN_RESET); //拉低SDA线，准备发送ACK信号
    drc_iic_delay_us();

    drv_iic_SCL_level(iic, GPIO_PIN_SET); //拉高SCL线，发送ACK信号
    drc_iic_delay_us();

    drv_iic_SCL_level(iic, GPIO_PIN_RESET); //拉低SCL线，结束ACK信号发送
    drc_iic_delay_us();

    drv_iic_SDA_level(iic, GPIO_PIN_SET); //释放SDA线，准备下一次通信
    drc_iic_delay_us();

}
/**
 * @brief 发送NACK信号
 * @param iic IIC_GPIOTYPE_T* IIC结构体指针
 * @return 无
 * @note 该函数在接收完一个字节后调用，用于向从设备发送NACK信号。
 *       函数会将SDA线拉高，并在SCL线拉高期间保持SDA线的状态，以表示NACK。
 *       确保在调用该函数之前，SCL线已经处于低电平状态。
*/
void drv_iic_nack(IIC_GPIOTYPE_T *iic){
    if(iic == NULL) return;

    drv_iic_SDA_level(iic, GPIO_PIN_SET); //拉高SDA线，准备发送NACK信号 SCL 0 -> 1 时SDA保持高电平表示NACK    
    drc_iic_delay_us();

    drv_iic_SCL_level(iic, GPIO_PIN_SET); //拉高SCL线，发送NACK信号
    drc_iic_delay_us();

    drv_iic_SCL_level(iic, GPIO_PIN_RESET);//拉低SCL线，结束NACK信号发送
    drc_iic_delay_us();
}
/**
 * @brief 发送一个字节数据
 * @param iic IIC_GPIOTYPE_T* IIC结构体指针
 * @param byte uint8_t 要发送的字节数据
 * @return 无
 * @note 该函数将输入的字节数据逐位发送到IIC总线上。
 *       函数会从最高位开始，依次将每一位的数据通过SDA线输出，
 *       并在每一位数据发送完成后拉高SCL线以通知从设备读取数据。
 *       确保在调用该函数之前，IIC总线已经处于起始状态，并且SCL线处于低电平状态。
*/
void drv_iic_send_byte(IIC_GPIOTYPE_T *iic, uint8_t byte){
    if(iic == NULL) return;
    for(int i = 0; i < 8; i++){
        drv_iic_SDA_level(iic, (byte & 0x80) ? GPIO_PIN_SET : GPIO_PIN_RESET); //发送当前位
        drc_iic_delay_us();

        drv_iic_SCL_level(iic, GPIO_PIN_SET); //拉高SCL线，通知从设备读取数据
        drc_iic_delay_us();
        drv_iic_SCL_level(iic, GPIO_PIN_RESET); //拉低SCL线，准备发送下一位数据

        byte <<= 1; //移位准备发送下一位
    }
    drv_iic_SDA_level(iic, GPIO_PIN_SET); //发送完一个字节后释放SDA线，准备等待ACK/NACK
}
/*
 * @brief 读取一个字节数据
 * @param iic IIC_GPIOTYPE_T* IIC结构体指针
 * @param ack uint8_t ACK/NACK信号 (0-ACK, 1-NACK)
 * @return int8_t 读取到的字节数据，若参数错误返回-1    
 * @note 该函数将从IIC总线上逐位读取数据，并在每一位数据读取完成后拉高SCL线以通知从设备发送数据。
 *       确保在调用该函数之前，IIC总线已经处于起始状态，并且SCL线处于低电平状态。
*/
int8_t drv_iic_read_byte(IIC_GPIOTYPE_T *iic, uint8_t ack){
    if(iic == NULL) return -1; //参数错误返回-1
    uint8_t recbyte = 0;
    for(uint8_t i = 0; i < 8; i++){ 
        recbyte <<= 1; //准备接收下一位数据
        drv_iic_SCL_level(iic, GPIO_PIN_SET); //拉高SCL线，通知从设备发送数据
        drc_iic_delay_us();
        if(HAL_GPIO_ReadPin(iic->SDA_PORT, iic->SDA_PIN))
        {
            recbyte++; //读取当前位数据
        }
        drv_iic_SCL_level(iic, GPIO_PIN_RESET); //拉低SCL线，准备接收下一位数据
        drc_iic_delay_us();
    }
    if(!ack){
        drv_iic_nack(iic); //发送ACK信号 告诉从设备nACK
    } else {
        drv_iic_ack(iic); //发送NACK信号 告诉从设备ACK
    }
    return recbyte;
}

/**
 * @brief 启用IIC中断
 * @param iic IIC_GPIOTYPE_T* IIC结构体指针 
 * @param trigger 中断触发方式 （如GPIO_MODE_IT_RISING, GPIO_MODE_IT_FALLING, GPIO_MODE_IT_RISING_FALLING）
 * @return 无
 */
bool drv_iic_enable_int(IIC_GPIOTYPE_T *iic){
    if( iic == NULL || !iic->IS_INT || iic->INT_PORT == NULL ) return false;
    /* 1.检查参数 触发方式非法*/
    if( iic->EXITConfig.trigger != GPIO_MODE_IT_RISING   && 
        iic->EXITConfig.trigger != GPIO_MODE_IT_FALLING  && 
        iic->EXITConfig.trigger != GPIO_MODE_IT_RISING_FALLING ) return false;
    /* 2.检查参数 优先级非法 */
    if(iic->EXITConfig.preempt_priority > 15 || iic->EXITConfig.sub_priority > 15) return false;
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    /* 1.重新配置INT引脚为中断模式  */
    GPIO_InitStruct.Pin = iic->INT_PIN;
    GPIO_InitStruct.Mode = iic->EXITConfig.trigger; // 触发方式由参数指定
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(iic->INT_PORT, &GPIO_InitStruct); 
    /* 2.使能中断 */
    uint32_t pin_pos = 0; //注意：这里需要根据引脚PIN号来确定具体的IRQn，以下是常见引脚对应的中断号，实际使用时请根据具体引脚调整：
    for(int i = 0; i < 16; i++){
        if((iic->INT_PIN & (1 << i))){
            pin_pos = i;
            break;
        }
    }
    __HAL_GPIO_EXTI_CLEAR_FLAG(iic->INT_PIN);
    IRQn_Type EXTI_IRQn;
    EXTI_IRQn = (pin_pos < 5) ? (EXTI0_IRQn + pin_pos) : ((pin_pos < 10) ? EXTI9_5_IRQn : EXTI15_10_IRQn);
    HAL_NVIC_SetPriority(EXTI_IRQn, iic->EXITConfig.preempt_priority, iic->EXITConfig.sub_priority);
    HAL_NVIC_EnableIRQ(EXTI_IRQn);  
    return true;
}