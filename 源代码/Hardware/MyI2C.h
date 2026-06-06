#ifndef __MYI2C_H
#define __MYI2C_H

#include "stm32f10x.h"

/* I2C ACK 结果 */
#define I2C_ACK_OK      0
#define I2C_ACK_NACK    1
#define I2C_ACK_TIMEOUT 2

/* 全局错误计数器 — 供看门狗/诊断用 */
extern volatile uint32_t g_I2C_ErrorCount;

void MyI2C_Init(void);
void MyI2C_Start(void);
void MyI2C_Stop(void);
uint8_t MyI2C_SendByte(uint8_t Byte);       /* 返回值: I2C_ACK_OK/NACK/TIMEOUT */
uint8_t MyI2C_ReceiveByte(void);
void MyI2C_SendAck(uint8_t AckBit);
uint8_t MyI2C_ReceiveAck(void);
void MyI2C_BusRecovery(void);               /* 总线恢复: 9 SCL脉冲 + STOP + 重新初始化 */

#endif
