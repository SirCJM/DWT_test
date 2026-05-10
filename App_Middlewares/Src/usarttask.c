#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "usart.h"
#include "SEGGER_RTT.h"

extern osMessageQueueId_t uartQueue01Handle;

void uart_Rx_Task(void *argument){
    /* USER CODE BEGIN uart_Rx_Task */
    msg_type recv_msg;
    SEGGER_RTT_printf(0, "UART RX Task started\r\n");
    /* Infinite loop */
    while(1)
    {
       osStatus_t status = osMessageQueueGet(uartQueue01Handle, &recv_msg, NULL, pdMS_TO_TICKS(1000)); // 1秒超时
       if(status == osOK){
           if(recv_msg.length > 0){
           
            SEGGER_RTT_printf(0, "Received %d bytes: ", recv_msg.length);
            for(int i = 0; i < recv_msg.length; i++){
                SEGGER_RTT_printf(0, "%c", recv_msg.buffer[i]);
            }
            SEGGER_RTT_printf(0, "\r\n");

            HAL_UART_Transmit(&huart1, recv_msg.buffer, recv_msg.length, 100); // 回显接收到的数据 

            osDelay(pdMS_TO_TICKS(2));
           }
       } else {
           SEGGER_RTT_printf(0, "Queue get timeout, status: %d\r\n", status);
       }

             // 在任务里打印，安全又稳定

    }
    /* USER CODE END uart_Rx_Task */
}