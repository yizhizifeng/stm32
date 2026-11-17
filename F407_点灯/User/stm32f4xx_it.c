#include "stm32f4xx.h"

/* ============================================================
 * 中断服务函数
 *
 * 说明：
 *   启动文件 startup_stm32f407xx.s 里把每个中断都定义成了"弱符号"，
 *   这里写同名函数就会覆盖它。用不到的中断不用写。
 *
 *   本工程的 Delay 用查询方式驱动 SysTick，不产生中断，
 *   所以这里暂时没有 SysTick_Handler。
 * ============================================================ */

/* 不可屏蔽中断 */
void NMI_Handler(void)
{
}

/* 硬件错误：程序跑飞、访问非法地址、除零等都会进这里 */
void HardFault_Handler(void)
{
    while (1)
    {
    }
}

/* 存储器管理错误 */
void MemManage_Handler(void)
{
    while (1)
    {
    }
}

/* 总线错误 */
void BusFault_Handler(void)
{
    while (1)
    {
    }
}

/* 用法错误 */
void UsageFault_Handler(void)
{
    while (1)
    {
    }
}

/* 系统服务调用 */
void SVC_Handler(void)
{
}

/* 调试监控 */
void DebugMon_Handler(void)
{
}

/* PendSV（RTOS 用） */
void PendSV_Handler(void)
{
}

/* SysTick 中断（本工程不用，留着备用） */
void SysTick_Handler(void)
{
}
