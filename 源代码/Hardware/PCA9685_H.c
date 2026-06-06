#include "stm32f10x.h"
#include "PCA9685_H.h"
#include "MyI2C.h"
#include "Delay.h"

/**************************************************

 * 项目: 基于STM32的智能机械时钟
 * 修改: 2026.06.06 — 移除重复 I2C 初始化 + I2C 错误检测与重试
 * 原始: 自由飞 (2025.10.12)

***************************************************/

/* 写寄存器内部实现 — 带错误检测和重试 */
static uint8_t _WriteReg_Retry(uint8_t reg, uint8_t data)
{
    uint8_t ret;
    
    for (int attempt = 0; attempt < 2; attempt++)
    {
        MyI2C_Start();
        ret = MyI2C_SendByte(PCA9685_ADDR_H << 1);
        if (ret != I2C_ACK_OK) goto recovery;
        
        ret = MyI2C_SendByte(reg);
        if (ret != I2C_ACK_OK) goto recovery;
        
        ret = MyI2C_SendByte(data);
        if (ret != I2C_ACK_OK) goto recovery;
        
        MyI2C_Stop();
        return 1;  /* 成功 */
        
    recovery:
        MyI2C_Stop();
        MyI2C_BusRecovery();
        Delay_ms(1);
    }
    
    return 0;  /* 两次都失败 */
}

void PCA9685_H_Init(void)
{
    /* 注意: MyI2C_Init() 已在 main() 中调过，这里不重复调 */

    _WriteReg_Retry(MODE1, 0x00);
    Delay_ms(10);
    _WriteReg_Retry(MODE1, 0x01);
    Delay_ms(10);
    _WriteReg_Retry(MODE2, 0x04);
    Delay_ms(10);
    
    PCA9685_H_SetPWMFreq(50);
}

void PCA9685_H_WriteReg(uint8_t reg, uint8_t data)
{
    _WriteReg_Retry(reg, data);
}

uint8_t PCA9685_H_ReadReg(uint8_t reg)
{
    uint8_t data = 0;
    
    MyI2C_Start();
    if (MyI2C_SendByte(PCA9685_ADDR_H << 1) != I2C_ACK_OK) goto exit;
    if (MyI2C_SendByte(reg) != I2C_ACK_OK) goto exit;
    
    MyI2C_Start();
    if (MyI2C_SendByte((PCA9685_ADDR_H << 1) | 1) != I2C_ACK_OK) goto exit;
    data = MyI2C_ReceiveByte();
    MyI2C_SendAck(1);
    
exit:
    MyI2C_Stop();
    return data;
}

void PCA9685_H_SetPWMFreq(float freq)
{
    float prescale_val;
    
    _WriteReg_Retry(MODE1, 0x10);
    Delay_ms(10);
    
    prescale_val = 25000000.0f / (4096.0f * freq) - 0.5f;
    _WriteReg_Retry(PRE_SCALE, (uint8_t)prescale_val);
    Delay_ms(10);
    
    _WriteReg_Retry(MODE1, 0x01);
    Delay_ms(10);
}

void PCA9685_H_SetPWM(uint8_t channel, uint16_t on, uint16_t off)
{
    if (channel >= 16) return;
    
    uint8_t reg = LED0_ON_L + 4 * channel;
    _WriteReg_Retry(reg, on & 0xFF);
    _WriteReg_Retry(reg + 1, (on >> 8) & 0x0F);
    _WriteReg_Retry(reg + 2, off & 0xFF);
    _WriteReg_Retry(reg + 3, (off >> 8) & 0x0F);
}

void PCA9685_H_SetAngle(uint8_t channel, uint8_t angle)
{
    if (angle > 180) angle = 180;
    
    uint16_t pulse = 102 + (uint16_t)((angle / 180.0f) * 410);
    PCA9685_H_SetPWM(channel, 0, pulse);
}
