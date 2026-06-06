#include "stm32f10x.h"
#include "MyI2C.h"
#include "PCA9685_H.h"
#include "PCA9685_M.h"
#include "Delay.h"
#include "Time_Num.h"
#include "USART.h"
#include "ds1302.h"
#include "ds1302_compensation.h"

/**************************************************

 * 项目: 基于STM32的智能机械时钟
 * 修改: 2026.06.06
 *        - 修复 I2C ACK 检测 + 总线恢复 (MyI2C.c)
 *        - 添加显示缓存（值不变不动舵机，减少 90%+ I2C 写入）
 *        - I2C 仅在 main() 中初始化一次
 *        - 启用独立看门狗 IWDG（5 秒超时）
 *        - DS1302 晶振补偿: 约 2 秒/天
 * 原始: 自由飞 (2025.10.12)
 * MCU: STM32F103C8T6

***************************************************/

/*───── 看门狗配置 ─────*/
#define IWDG_TIMEOUT_MS    5000    /* 5 秒不喂狗则复位 */
#define IWDG_PRESCALER     IWDG_Prescaler_64  /* 40kHz LSI / 64 = 625 Hz */
#define IWDG_RELOAD_VAL    ((IWDG_TIMEOUT_MS * 625UL) / 1000UL)  /* = 3125 */

/*───── 全局变量 ─────*/
uint8_t Rxdata    = 0;
uint8_t LA_Time;
uint8_t LastMode  = 255;

/*───── 显示缓存：值不变不写舵机 ─────*/
static uint8_t Cache_H1    = 255;
static uint8_t Cache_H2    = 255;
static uint8_t Cache_M1    = 255;
static uint8_t Cache_M2    = 255;
static uint8_t Cache_Emoji = 255;

/*───── 强制刷新缓存 ─────*/
static void Display_ForceRefresh(void)
{
    Cache_H1 = Cache_H2 = Cache_M1 = Cache_M2 = Cache_Emoji = 255;
}

/*───── 带缓存的显示函数 ─────*/
static void Show_H1(uint8_t num)
{
    if (Cache_H1 != num) { Cache_H1 = num; PCA9685_Set_Time_Num_H(1, num); }
}

static void Show_H2(uint8_t num)
{
    if (Cache_H2 != num) { Cache_H2 = num; PCA9685_Set_Time_Num_H(2, num); }
}

static void Show_M1(uint8_t num)
{
    if (Cache_M1 != num) { Cache_M1 = num; PCA9685_Set_Time_Num_M(1, num); }
}

static void Show_M2(uint8_t num)
{
    if (Cache_M2 != num) { Cache_M2 = num; PCA9685_Set_Time_Num_M(2, num); }
}

static void Show_Emoji(uint8_t num)
{
    if (Cache_Emoji != num) { Cache_Emoji = num; PCA9685_emoji(num); }
}

/*───── 四位数字一次显示（时分/月日等）─────*/
static void Display_FourDigits(uint8_t h1, uint8_t h2, uint8_t m1, uint8_t m2)
{
    Show_H1(h1);
    Show_H2(h2);
    Show_M1(m1);
    Show_M2(m2);
}

/*───── 独立看门狗初始化 ─────*/
static void IWDG_Init_5s(void)
{
    /* 使能写访问 */
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    /* 设置预分频器和重装载值 */
    IWDG_SetPrescaler(IWDG_PRESCALER);
    IWDG_SetReload(IWDG_RELOAD_VAL);
    /* 喂狗启动 */
    IWDG_ReloadCounter();
    IWDG_Enable();
}

/*───── 主函数 ─────*/
int main(void)
{
    /*───── 1. 硬件 I2C 初始化（只调一次！）─────*/
    MyI2C_Init();

    /*───── 2. 外设初始化 ─────*/
    My_USART_Init();
    PCA9685_H_Init();           /* 不再内部调 MyI2C_Init */
    PCA9685_M_Init();           /* 不再内部调 MyI2C_Init */
    ds1302_gpio_init();
    ds1032_init();
    ds1302_compensation_init();

    /*───── 3. 启用看门狗 ─────*/
    IWDG_Init_5s();

    /*───── 4. 初始状态 ─────*/
    Display_ForceRefresh();

    while (1)
    {
        /* 喂狗 */
        IWDG_ReloadCounter();

        /* 读取时间 */
        ds1032_read_realTime();
        ds1302_compensation_tick();

        /* 洛杉矶时差（国内 -15h）*/
        LA_Time = (TimeData.hour >= 15)
                  ? (TimeData.hour - 15)
                  : (24UL - (15UL - TimeData.hour));

        /* 串口模式切换（蓝牙/上位机）*/
        if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET)
        {
            Rxdata = USART_ReceiveData(USART1);
            if (Rxdata > 12) Rxdata = 0;  /* 非法值回退 */
        }

        /* 模式切换时强制刷新一次 */
        if (Rxdata != LastMode) { Display_ForceRefresh(); }

        switch (Rxdata)
        {
        case 0: /* 休息模式 */
            Show_Emoji(2);
            break;

        case 1: /* 表情 */
            Show_Emoji(3);
            break;

        case 2: /* 时间（时:分）*/
            Display_FourDigits(TimeData.hour / 10,
                               TimeData.hour % 10,
                               TimeData.minute / 10,
                               TimeData.minute % 10);
            Show_Emoji(0);
            break;

        case 3: /* 编号 */
            Display_FourDigits(2, 3, 0, 1);
            Show_Emoji(1);
            break;

        case 4: /* 年份 */
            Display_FourDigits(TimeData.year / 1000,
                               (TimeData.year % 1000) / 100,
                               (TimeData.year % 100) / 10,
                               TimeData.year % 10);
            Show_Emoji(1);
            break;

        case 5: /* 月日 */
            Display_FourDigits(TimeData.month / 10,
                               TimeData.month % 10,
                               TimeData.day / 10,
                               TimeData.day % 10);
            Show_Emoji(0);
            break;

        case 6: /* 洛杉矶时间 */
            Display_FourDigits(LA_Time / 10,
                               LA_Time % 10,
                               TimeData.minute / 10,
                               TimeData.minute % 10);
            Show_Emoji(0);
            break;

        case 7: /* 分秒 */
            Display_FourDigits(TimeData.minute / 10,
                               TimeData.minute % 10,
                               TimeData.second / 10,
                               TimeData.second % 10);
            Show_Emoji(0);
            break;

        default:
            Rxdata = 0;
            break;
        }

        LastMode = Rxdata;

        /* 轻度延时：给舵机反应时间 + 避免 CPU 空转 */
        Delay_ms(50);
    }
}
