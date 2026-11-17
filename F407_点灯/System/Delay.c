#include "Delay.h"

/* ============================================================
 * 毫秒/微秒延时（查询方式驱动 SysTick，不占用中断）
 *
 * 用法：
 *      Delay_Init();      // 先调一次
 *      Delay_ms(500);
 *
 * 说明：
 *      基于 SystemCoreClock 计算，主频变了也不用改这里。
 *      本工程用复位后的 HSI 16MHz，Delay_Init 读到的就是 16MHz。
 * ============================================================ */

static uint32_t fac_us = 0;     /* 每微秒需要多少个 SysTick 计数 */

/**
 * @brief  延时初始化，必须在第一次使用延时前调用
 */
void Delay_Init(void)
{
    SystemCoreClockUpdate();            /* 更新 SystemCoreClock 变量 */
    fac_us = SystemCoreClock / 1000000u;/* 16MHz -> 16 */
}

/**
 * @brief  微秒延时
 * @param  xus: 微秒数（1 ~ 233015，超过请用 Delay_ms）
 */
void Delay_us(uint32_t xus)
{
    uint32_t temp;

    if (fac_us == 0) {
        Delay_Init();
    }
    if (xus == 0) {
        return;
    }

    SysTick->LOAD = xus * fac_us - 1;   /* 计数初值 */
    SysTick->VAL  = 0x00;               /* 清空计数器 */
    /* CTRL: bit0=ENABLE, bit2=CLKSOURCE(=1 表示用 HCLK，必须置 1！)
     * 若 CLKSOURCE=0 则用 HCLK/8，延时会慢 8 倍 */
    SysTick->CTRL = 0x05;               /* 使能 + 时钟源 HCLK，不开中断 */

    do {
        temp = SysTick->CTRL;
    } while ((temp & 0x01) && !(temp & (1 << 16)));   /* 等 COUNTFLAG */

    SysTick->CTRL = 0x00;               /* 关闭 */
    SysTick->VAL  = 0x00;
}

/**
 * @brief  毫秒延时
 * @param  xms: 毫秒数
 */
void Delay_ms(uint32_t xms)
{
    while (xms--) {
        Delay_us(1000);
    }
}

/**
 * @brief  秒延时
 * @param  xs: 秒数
 */
void Delay_s(uint32_t xs)
{
    while (xs--) {
        Delay_ms(1000);
    }
}
