/**
 ******************************************************************************
 * @file    flow_pe0_pe3.c
 * @brief   STM32F407VET6 最小系统板 —— PE0~PE3 流水灯
 *
 * 硬件连接（本板的实际电路）：
 *
 *   3.3V ──[1K]── LED阳极 ── LED阴极 ── PE0   (LED2)
 *   3.3V ──[1K]── LED阳极 ── LED阴极 ── PE1   (LED3)
 *   3.3V ──[1K]── LED阳极 ── LED阴极 ── PE2   (LED4)
 *   3.3V ──[1K]── LED阳极 ── LED阴极 ── PE3   (LED5)
 *
 *   ⚠️ 低电平点亮：PE 输出 0 → 亮；输出 1 → 灭
 *
 * 时钟：
 *   本程序不配置 PLL，直接使用复位后的内部 HSI 16MHz。
 *   流水灯不需要高主频，这样最省事、最不容易出问题。
 *   （所以：灯会亮 ≠ 晶振正常，晶振要用别的程序单独验证）
 *
 * 编译：
 *   需要以下文件一起参与编译（Keil / STM32CubeIDE 新建工程时自带的）：
 *     - startup_stm32f407xx.s
 *     - system_stm32f4xx.c
 *     - stm32f4xx.h（CMSIS 头）
 ******************************************************************************
 */

#include "stm32f4xx.h"

/* ---------------- 硬件定义 ---------------- */
#define LED_PORT        GPIOE
#define LED_PIN_MASK    0x000Fu          /* PE0 | PE1 | PE2 | PE3 */

/* 点亮第 n 颗灯（PE[n] 输出低电平）
 * BSRR 的高 16 位写 1 = 复位 ODR 位 = 输出低 */
#define LED_ON(n)       (LED_PORT->BSRR = (1u << ((n) + 16u)))

/* 熄灭第 n 颗灯（PE[n] 输出高电平）
 * BSRR 的低 16 位写 1 = 置位 ODR 位 = 输出高 */
#define LED_OFF(n)      (LED_PORT->BSRR = (1u << (n)))

/* 全部熄灭 / 全部点亮 */
#define LED_ALL_OFF()   (LED_PORT->BSRR = LED_PIN_MASK)
#define LED_ALL_ON()    (LED_PORT->BSRR = (LED_PIN_MASK << 16u))

/* ---------------- 毫秒延时（SysTick） ---------------- */
static volatile uint32_t g_tick_ms = 0;

void SysTick_Handler(void)
{
    if (g_tick_ms != 0u) {
        g_tick_ms--;
    }
}

/** 基于 SysTick 的毫秒延时，精度与主频无关 */
static void delay_ms(uint32_t ms)
{
    g_tick_ms = ms;
    while (g_tick_ms != 0u) {
        __WFI();                         /* 睡眠等待中断，省电 */
    }
}

/* ---------------- 初始化 ---------------- */
static void led_init(void)
{
    /* 1. 打开 GPIOE 的时钟（不打开的话写寄存器没有任何反应） */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
    (void)RCC->AHB1ENR;                  /* 读回一次，确保时钟稳定生效 */

    /* 2. PE0~PE3 配置为通用推挽输出
     *    MODER 每个引脚占 2 位：00=输入 01=输出 10=复用 11=模拟
     *    PE0~PE3 对应 MODER[7:0] */
    LED_PORT->MODER   &= ~(0xFFu << 0);  /* 先清零这 8 位 */
    LED_PORT->MODER   |=  (0x55u << 0);  /* 0b01_01_01_01 → 4 个都设成输出 */

    /* 3. 推挽输出（而不是开漏） */
    LED_PORT->OTYPER  &= ~LED_PIN_MASK;

    /* 4. 输出速度：中速（25MHz）足够驱动 LED */
    LED_PORT->OSPEEDR &= ~(0xFFu << 0);
    LED_PORT->OSPEEDR |=  (0x55u << 0);

    /* 5. 不使用内部上下拉 */
    LED_PORT->PUPDR   &= ~(0xFFu << 0);

    /* 6. 上电先全部熄灭 */
    LED_ALL_OFF();
}

/* ---------------- 主程序 ---------------- */
int main(void)
{
    int i, round;      /* 变量声明放在最前面，兼容 C89 编译器设置 */

    /* SysTick 配成 1ms 中断一次
     * SystemCoreClock 默认 16MHz（HSI），所以除以 1000 即 16000 个周期 */
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / 1000u);

    led_init();

    /* 上电闪三下，表示程序跑起来了 */
    for (i = 0; i < 3; i++) {
        LED_ALL_ON();
        delay_ms(120);
        LED_ALL_OFF();
        delay_ms(120);
    }
    delay_ms(300);

    /* ---------------- 主循环 ---------------- */
    while (1) {
        /* ===== 花样 1：单灯从左往右流水 ===== */
        for (round = 0; round < 3; round++) {
            for (i = 0; i < 4; i++) {
                LED_ALL_OFF();           /* 先全灭 */
                LED_ON(i);               /* 只亮当前这颗 */
                delay_ms(150);
            }
        }

        /* ===== 花样 2：来回扫（乒乓球） ===== */
        for (round = 0; round < 2; round++) {
            for (i = 0; i < 4; i++) {
                LED_ALL_OFF();
                LED_ON(i);
                delay_ms(90);
            }
            for (i = 2; i >= 1; i--) {
                LED_ALL_OFF();
                LED_ON(i);
                delay_ms(90);
            }
        }

        /* ===== 花样 3：四灯一起闪两下 ===== */
        for (round = 0; round < 2; round++) {
            LED_ALL_ON();
            delay_ms(200);
            LED_ALL_OFF();
            delay_ms(200);
        }

        /* ===== 花样 4：逐颗累加，全亮后逐颗熄灭 ===== */
        for (i = 0; i < 4; i++) {
            LED_ON(i);
            delay_ms(120);
        }
        for (i = 0; i < 4; i++) {
            LED_OFF(i);
            delay_ms(120);
        }

        delay_ms(400);                   /* 一轮结束，歇一下 */
    }
}

/* ---------------- 断言用的故障处理 ---------------- */
#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
    (void)file;
    (void)line;
    while (1) { }
}
#endif
