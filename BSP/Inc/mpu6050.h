#ifndef __MPU6050_H
#define __MPU6050_H

#include <stdint.h>
#include <stdbool.h>
#include "drv_iic.h"

#ifdef __cplusplus
extern "C" {
#endif
#define ADDR_0X68 // MPU6050 AD0引脚接地时地址为0x68，接VCC时地址为0x69

#if defined(ADDR_0X68)
#define MPU6050_I2C_ADDR             0x68u
#else
#define MPU6050_I2C_ADDR             0x69u
#endif


extern IIC_GPIOTYPE_T mpu6050_iic;


#define MPU6050_WHO_AM_I             0x75u
#define MPU6050_PWR_MGMT_1           0x6Bu
#define MPU6050_SMPLRT_DIV           0x19u
#define MPU6050_CONFIG               0x1Au
#define MPU6050_GYRO_CONFIG          0x1Bu
#define MPU6050_ACCEL_CONFIG         0x1Cu
#define MPU6050_INT_ENABLE           0x38u
#define MPU6050_ACCEL_XOUT_H         0x3Bu

bool MPU6050_Init(IIC_GPIOTYPE_T *iic);
bool MPU6050_WhoAmI(IIC_GPIOTYPE_T *iic, uint8_t *who);
bool MPU6050_WriteReg(IIC_GPIOTYPE_T *iic, uint8_t reg, uint8_t data);
bool MPU6050_ReadRegs(IIC_GPIOTYPE_T *iic, uint8_t reg, uint8_t *buf, uint16_t len);
bool MPU6050_ReadMotion(IIC_GPIOTYPE_T *iic, int16_t accel[3], int16_t gyro[3], int16_t *temp);

#ifdef __cplusplus
}
#endif

#endif /* __MPU6050_H */
