#include "stm32f10x.h"
#include "MyI2C.h"
#include "Delay.h"

/**************************************************

 * 项目: 基于STM32的智能机械时钟
 * 修改: 2026.06.06 — 修复 I2C ACK 检测 + 总线恢复 + 错误计数
 * 原始: 自由飞 (2025.10.12)
 * MCU: STM32F103C8T6

***************************************************/

/* I2C 超时（循环次数） */
#define I2C_TIMEOUT 1000

/* 全局 I2C 错误计数器 — 供看门狗/诊断使用 */
volatile uint32_t g_I2C_ErrorCount = 0;

/* 等待 I2C 事件，超时返回 0 */
static uint8_t I2C_WaitEvent(I2C_TypeDef* I2Cx, uint32_t Event)
{
    uint32_t timeout = I2C_TIMEOUT;
    while (I2C_CheckEvent(I2Cx, Event) != SUCCESS)
    {
        if (--timeout == 0) return 0;
    }
    return 1;
}

/*───────────── 硬件 I2C2 初始化（只调一次！）─────────────*/
void MyI2C_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* 内部上拉 */
    GPIO_SetBits(GPIOB, GPIO_Pin_10 | GPIO_Pin_11);

    I2C_InitTypeDef I2C_InitStructure;
    I2C_InitStructure.I2C_ClockSpeed          = 400000;
    I2C_InitStructure.I2C_Mode                = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle           = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1         = 0x00;
    I2C_InitStructure.I2C_Ack                 = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init(I2C2, &I2C_InitStructure);

    I2C_Cmd(I2C2, ENABLE);
}

/*───────────── 总线恢复：踢出从机死锁状态 ─────────────*/
void MyI2C_BusRecovery(void)
{
    g_I2C_ErrorCount++;

    /* 先复位 I2C 外设 */
    I2C_DeInit(I2C2);

    /* 切换到 GPIO 开漏输出，手动发 9 个 SCL 脉冲 */
    GPIO_InitTypeDef gpio;
    gpio.GPIO_Mode  = GPIO_Mode_Out_OD;
    gpio.GPIO_Pin   = GPIO_Pin_10 | GPIO_Pin_11;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &gpio);

    /* 释放 SDA 为高 */
    GPIO_SetBits(GPIOB, GPIO_Pin_11);
    Delay_us(5);

    /* 发 9 个 SCL 脉冲 */
    for (int i = 0; i < 9; i++)
    {
        GPIO_ResetBits(GPIOB, GPIO_Pin_10);
        Delay_us(5);
        GPIO_SetBits(GPIOB, GPIO_Pin_10);
        Delay_us(5);
    }

    /* 发 STOP 条件: SDA 从低变高时 SCL 为高 */
    GPIO_ResetBits(GPIOB, GPIO_Pin_11);
    Delay_us(5);
    GPIO_SetBits(GPIOB, GPIO_Pin_10);
    Delay_us(5);
    GPIO_SetBits(GPIOB, GPIO_Pin_11);
    Delay_us(5);

    /* 重新初始化硬件 I2C */
    MyI2C_Init();
}

/*───────────── START ─────────────*/
void MyI2C_Start(void)
{
    I2C_GenerateSTART(I2C2, ENABLE);
    I2C_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);
}

/*───────────── STOP ─────────────*/
void MyI2C_Stop(void)
{
    I2C_GenerateSTOP(I2C2, ENABLE);
}

/*───────────── 发送一个字节（带 ACK 检测）─────────────*/
uint8_t MyI2C_SendByte(uint8_t Byte)
{
    /* 发送 */
    I2C_SendData(I2C2, Byte);

    /* 等待发送完成 */
    if (!I2C_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        return I2C_ACK_TIMEOUT;
    }

    /* 检查从机是否应答 — 这才是真正的 NACK 检测！ */
    if (I2C_GetFlagStatus(I2C2, I2C_FLAG_AF))
    {
        I2C_ClearFlag(I2C2, I2C_FLAG_AF);
        return I2C_ACK_NACK;
    }

    return I2C_ACK_OK;
}

/*───────────── 接收一个字节 ─────────────*/
uint8_t MyI2C_ReceiveByte(void)
{
    I2C_AcknowledgeConfig(I2C2, DISABLE);
    I2C_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED);
    return I2C_ReceiveData(I2C2);
}

/*───────────── 发送 ACK/NACK ─────────────*/
void MyI2C_SendAck(uint8_t AckBit)
{
    I2C_AcknowledgeConfig(I2C2, AckBit ? DISABLE : ENABLE);
}

/*───────────── 保留兼容（不推荐使用，推荐用 SendByte 返回值）─────────────*/
uint8_t MyI2C_ReceiveAck(void)
{
    I2C_AcknowledgeConfig(I2C2, ENABLE);
    I2C_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED);
    return I2C_ReceiveData(I2C2) ? 1 : 0;
}
