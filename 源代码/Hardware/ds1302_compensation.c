#include "ds1302_compensation.h"
#include "ds1302.h"
#include "Delay.h"
#include <math.h>   /* fmodf */

/*══════════════════════════════════════════════════════════════
 *  BCD 转换工具
 *══════════════════════════════════════════════════════════════*/

uint8_t bcd_to_dec(uint8_t bcd)
{
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

uint8_t dec_to_bcd(uint8_t dec)
{
    return ((dec / 10) << 4) | (dec % 10);
}

/*══════════════════════════════════════════════════════════════
 *  读 DS1302 指定 RAM 地址（读指令地址 = 写地址 | 0x01）
 *══════════════════════════════════════════════════════════════*/

static uint8_t ds1302_ram_read(uint8_t addr_write)
{
    return ds1302_read_rig(addr_write | 0x01);
}

/*══════════════════════════════════════════════════════════════
 *  写 DS1302 指定 RAM 地址
 *══════════════════════════════════════════════════════════════*/

static void ds1302_ram_write(uint8_t addr_write, uint8_t data)
{
    ds1302_wirte_rig(0x8E, 0x00);   /* 关闭写保护 */
    ds1302_wirte_rig(addr_write, data);
    ds1302_wirte_rig(0x8E, 0x80);   /* 开启写保护 */
}

/*══════════════════════════════════════════════════════════════
 *  初始化：从 RAM 恢复上次修正状态（若 RAM 未初始化则设初值）
 *══════════════════════════════════════════════════════════════*/

void ds1302_compensation_init(void)
{
    if (CORRECTION_SECONDS_PER_DAY == 0.0f) {
        return;  /* 补偿关闭，不做任何事 */
    }

    /* 首次上电时 RAM 可能是随机值，写入初始标记 */
    /* 用 day=0x01, hour=0x00 确保首次循环就触发修正 */
    uint8_t last_day = ds1302_ram_read(DS1302_RAM_CORR_DAY);
    if (last_day == 0x00 || last_day == 0xFF || last_day > 0x31) {
        ds1302_ram_write(DS1302_RAM_CORR_DAY,      0x01);
        ds1302_ram_write(DS1302_RAM_CORR_HOUR,     0x00);
        ds1302_ram_write(DS1302_RAM_CORR_REMAINDER, 0);
    }
}

/*══════════════════════════════════════════════════════════════
 *  把 TimeData（十进制）写回 DS1302 寄存器（BCD）
 *  用于修正后回写
 *══════════════════════════════════════════════════════════════*/

static void ds1302_write_time_back(void)
{
    ds1302_wirte_rig(0x8E, 0x00);  /* 关写保护 */

    ds1302_wirte_rig(0x80, dec_to_bcd(TimeData.second));   /* 秒 */
    ds1302_wirte_rig(0x82, dec_to_bcd(TimeData.minute));   /* 分 */
    ds1302_wirte_rig(0x84, dec_to_bcd(TimeData.hour));     /* 时 */
    ds1302_wirte_rig(0x86, dec_to_bcd(TimeData.day));      /* 日 */
    ds1302_wirte_rig(0x88, dec_to_bcd(TimeData.month));    /* 月 */
    ds1302_wirte_rig(0x8A, dec_to_bcd(TimeData.week));     /* 星期 */
    ds1302_wirte_rig(0x8C, dec_to_bcd(TimeData.year - 2000)); /* 年 */

    ds1302_wirte_rig(0x8E, 0x80);  /* 开写保护 */
}

/*══════════════════════════════════════════════════════════════
 *  从 TimeData 中减去 N 秒（带回卷处理：秒→分→时→日→月→年）
 *══════════════════════════════════════════════════════════════*/

static void time_subtract_seconds(int16_t secs)
{
    if (secs <= 0) return;

    int32_t total = TimeData.second
                  + TimeData.minute  * 60L
                  + TimeData.hour    * 3600L;
    /* 只处理同日内的时分秒，跨日折算太复杂且实际偏差很小 */
    /* 日偏差通常<30秒/天，不会跨日 */

    total -= secs;
    if (total < 0) total += 86400L;  /* 回卷到前一天 */

    TimeData.second = total % 60;
    TimeData.minute = (total / 60) % 60;
    TimeData.hour   = (total / 3600) % 24;
}

/*══════════════════════════════════════════════════════════════
 *  核心补偿逻辑 — 每次 main 循环调用
 *
 *  流程:
 *    1. 读当前日/时（从 DS1302 直接读 BCD，避免依赖 TimeData）
 *    2. 读 RAM 中上次修正的日/时 + 亚秒余数
 *    3. 计算距离上次修正过了多少小时
 *    4. 如果 >= CORRECTION_INTERVAL_H：
 *       - 计算总偏差秒数（含亚秒余数进位）
 *       - 读 TimeData → 减去偏差 → 写回 DS1302
 *       - 更新 RAM 标记
 *══════════════════════════════════════════════════════════════*/

void ds1302_compensation_tick(void)
{
    if (CORRECTION_SECONDS_PER_DAY == 0.0f) {
        return;  /* 补偿关闭 */
    }

    /* ── 第1步：获取当前日/时（BCD 原始值） ── */
    uint8_t raw_day  = ds1302_read_rig(0x87);   /* 日 (BCD) */
    uint8_t raw_hour = ds1302_read_rig(0x85);   /* 时 (BCD) */
    uint8_t cur_day  = bcd_to_dec(raw_day);
    uint8_t cur_hour = bcd_to_dec(raw_hour);

    /* ── 第2步：读取上次修正记录 ── */
    uint8_t last_day     = bcd_to_dec(ds1302_ram_read(DS1302_RAM_CORR_DAY));
    uint8_t last_hour    = bcd_to_dec(ds1302_ram_read(DS1302_RAM_CORR_HOUR));
    uint8_t remainder    = ds1302_ram_read(DS1302_RAM_CORR_REMAINDER);
    /* remainder: 上次未凑整的百分秒 (0-99)，累积到 100 进位为 1 秒 */

    /* ── 第3步：计算经过的小时数 ── */
    if (cur_hour != CORRECTION_TRIGGER_HOUR) {
        return;  /* 还没到触发时刻，直接返回 */
    }

    int16_t elapsed_hours;
    if (cur_day == last_day) {
        /* 同一天但到了触发时刻 → 说明上次触发在上一周期的同一时刻
         * elapsed 正好等于间隔 */
        elapsed_hours = CORRECTION_INTERVAL_H;
    } else {
        /* 跨天了 */
        int16_t day_diff = (int16_t)cur_day - (int16_t)last_day;
        if (day_diff <= 0) {
            day_diff += 31;  /* 跨月近似，误差在后续周期自动修正 */
        }
        elapsed_hours = day_diff * 24;
    }

    if (elapsed_hours < CORRECTION_INTERVAL_H) {
        return;  /* 还没到间隔，跳过 */
    }

    /* ── 第4步：计算总偏差 ── */
    /* 每小时偏差 = 每天偏差 / 24 */
    float drift_per_hour = CORRECTION_SECONDS_PER_DAY / 24.0f;
    float total_drift    = drift_per_hour * (float)elapsed_hours;

    /* 加上上次遗留的亚秒余数 */
    total_drift += (float)remainder / 100.0f;

    int16_t correction_secs = (int16_t)total_drift;  /* 整数秒 */
    uint8_t new_remainder   = (uint8_t)((total_drift - (float)correction_secs) * 100.0f);

    /* ── 第5步：读当前完整时间 → 修正 → 写回 ── */
    ds1032_read_realTime();  /* 刷新 TimeData */

    if (correction_secs > 0) {
        /* DS1302 偏快 → 从 TimeData 中减去修正值 */
        time_subtract_seconds(correction_secs);

        /* 把修正后的时间写回 DS1302 */
        ds1302_write_time_back();
    }
    /* 如果 correction_secs == 0（不够1秒），只累积余数不写回 */

    /* ── 第6步：更新 RAM 标记 ── */
    ds1302_ram_write(DS1302_RAM_CORR_DAY,      dec_to_bcd(cur_day));
    ds1302_ram_write(DS1302_RAM_CORR_HOUR,     dec_to_bcd(cur_hour));
    ds1302_ram_write(DS1302_RAM_CORR_REMAINDER, new_remainder);
}
