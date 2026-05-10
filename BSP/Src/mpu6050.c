#include "mpu6050.h"


IIC_GPIOTYPE_T mpu6050_iic = {
    
    .SDA_PORT = GPIOB,
    .SDA_PIN  = GPIO_PIN_7,

    .SCL_PORT = GPIOB,
    .SCL_PIN  = GPIO_PIN_6,

    .INT_PORT = GPIOB,
    .INT_PIN  = GPIO_PIN_5,
    .IS_INT   = true, 

    .EXITConfig = {
        .trigger =GPIO_MODE_IT_RISING,    // 中断触发方式 上升沿触发
        .preempt_priority = 0,            // 抢占优先级
        .sub_priority = 0,                // 子优先级               
    }, // 中断配置
    .ExtiCallback = NULL // 中断回调函数
};
static bool mpu6050_write_bytes(IIC_GPIOTYPE_T *iic, uint8_t reg, const uint8_t *data, uint16_t len)
{
    if(iic == NULL || data == NULL || len == 0) return false;

    drv_iic_start(iic);
    drv_iic_send_byte(iic, (MPU6050_I2C_ADDR << 1) | 0);
    if(drv_iic_wait_ack(iic) != 0){
        drv_iic_stop(iic);
        return false;
    }

    drv_iic_send_byte(iic, reg);
    if(drv_iic_wait_ack(iic) != 0){
        drv_iic_stop(iic);
        return false;
    }

    for(uint16_t i = 0; i < len; i++){
        drv_iic_send_byte(iic, data[i]);
        if(drv_iic_wait_ack(iic) != 0){
            drv_iic_stop(iic);
            return false;
        }
    }

    drv_iic_stop(iic);
    return true;
}

static bool mpu6050_read_bytes(IIC_GPIOTYPE_T *iic, uint8_t reg, uint8_t *buf, uint16_t len)
{
    if(iic == NULL || buf == NULL || len == 0) return false;

    drv_iic_start(iic);
    drv_iic_send_byte(iic, (MPU6050_I2C_ADDR << 1) | 0);
    if(drv_iic_wait_ack(iic) != 0){
        drv_iic_stop(iic);
        return false;
    }

    drv_iic_send_byte(iic, reg);
    if(drv_iic_wait_ack(iic) != 0){
        drv_iic_stop(iic);
        return false;
    }

    drv_iic_start(iic);
    drv_iic_send_byte(iic, (MPU6050_I2C_ADDR << 1) | 1);
    if(drv_iic_wait_ack(iic) != 0){
        drv_iic_stop(iic);
        return false;
    }

    for(uint16_t i = 0; i < len; i++){
        if(i == (len - 1)){
            buf[i] = drv_iic_read_byte(iic, 1); // 发送NACK
        } else {
            buf[i] = drv_iic_read_byte(iic, 0); // 发送ACK
        }
    }
    drv_iic_stop(iic);
    return true;
}

bool MPU6050_WriteReg(IIC_GPIOTYPE_T *iic, uint8_t reg, uint8_t data)
{
    return mpu6050_write_bytes(iic, reg, &data, 1);
}

bool MPU6050_ReadRegs(IIC_GPIOTYPE_T *iic, uint8_t reg, uint8_t *buf, uint16_t len)
{
    return mpu6050_read_bytes(iic, reg, buf, len);
}

bool MPU6050_WhoAmI(IIC_GPIOTYPE_T *iic, uint8_t *who)
{
    if(who == NULL) return false;
    return MPU6050_ReadRegs(iic, MPU6050_WHO_AM_I, who, 1);
}

bool MPU6050_Init(IIC_GPIOTYPE_T *iic)
{
    if(iic == NULL) return false;

    uint8_t id = 0;
    if(!MPU6050_WhoAmI(iic, &id)) return false;
    if(id != MPU6050_I2C_ADDR) return false;

    if(!MPU6050_WriteReg(iic, MPU6050_PWR_MGMT_1, 0x00)) return false;
    if(!MPU6050_WriteReg(iic, MPU6050_SMPLRT_DIV, 0x07)) return false;
    if(!MPU6050_WriteReg(iic, MPU6050_CONFIG, 0x06)) return false;
    if(!MPU6050_WriteReg(iic, MPU6050_GYRO_CONFIG, 0x18)) return false;
    if(!MPU6050_WriteReg(iic, MPU6050_ACCEL_CONFIG, 0x01)) return false;
    if(!MPU6050_WriteReg(iic, MPU6050_INT_ENABLE, 0x01)) return false;

    return true;
}

bool MPU6050_ReadMotion(IIC_GPIOTYPE_T *iic, int16_t accel[3], int16_t gyro[3], int16_t *temp)
{
    if(iic == NULL || accel == NULL || gyro == NULL || temp == NULL) return false;

    uint8_t buf[14];
    if(!MPU6050_ReadRegs(iic, MPU6050_ACCEL_XOUT_H, buf, sizeof(buf))) return false;

    accel[0] = (int16_t)((buf[0] << 8) | buf[1]);
    accel[1] = (int16_t)((buf[2] << 8) | buf[3]);
    accel[2] = (int16_t)((buf[4] << 8) | buf[5]);
    *temp    = (int16_t)((buf[6] << 8) | buf[7]);
    gyro[0]  = (int16_t)((buf[8] << 8) | buf[9]);
    gyro[1]  = (int16_t)((buf[10] << 8) | buf[11]);
    gyro[2]  = (int16_t)((buf[12] << 8) | buf[13]);
    return true;
}
