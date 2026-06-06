#ifndef __PCA9685M_H
#define __PCA9685M_H

#include "stm32f10x.h"

//PCA9685设备地址
#define PCA9685_ADDR_M 0x41  // 7位地址，发送时左移1位+读写位

// PCA9685寄存器地址
#define MODE1        0x00
#define MODE2        0x01
#define LED0_ON_L    0x06
#define LED0_ON_H    0x07
#define LED0_OFF_L   0x08
#define LED0_OFF_H   0x09
#define ALL_LED_ON_L 0xFA
#define ALL_LED_ON_H 0xFB
#define ALL_LED_OFF_L 0xFC
#define ALL_LED_OFF_H 0xFD
#define PRE_SCALE    0xFE

// 函数声明
void PCA9685_M_Init(void);
void PCA9685_M_WriteReg(uint8_t reg, uint8_t data);
uint8_t PCA9685_M_ReadReg(uint8_t reg);
void PCA9685_M_SetPWMFreq(float freq);
void PCA9685_M_SetPWM(uint8_t channel, uint16_t on, uint16_t off);
void PCA9685_M_SetAngle(uint8_t channel, uint8_t angle);
void PCA9685_M_Set_Time_Num(uint8_t num);

#endif
    
