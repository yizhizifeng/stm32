#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "LED.h"
#include "Key.h"
#include "Timer.h"

uint16_t i;
uint8_t Keynum;
uint8_t LED1Mode;
uint8_t LED2Mode;

int main()
{
	OLED_Init();
	LED_Init();
	Key_Init();
	Timer_Init();
	
	OLED_ShowString(1,1,"i:");
	OLED_ShowString(2,1,"LED1Mode:");
	OLED_ShowString(3,1,"LED2Mode:");
	while(1)
	{ 
		Keynum=Key_GetNum();
		if(Keynum==1)
		{
			LED1Mode++;
			LED1Mode%=5;
			LED1_SetMode(LED1Mode);
		}
		if(Keynum==2)
		{
			LED2Mode++;
			LED2Mode%=5;
			LED2_SetMode(LED2Mode);
		}
		OLED_ShowNum(1,3,i++,5);
		OLED_ShowNum(2,10,LED1Mode,1);
		OLED_ShowNum(3,10,LED2Mode,1);
	} 
}

void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET)
	{
		Key_Tick();
		LED_Tick();
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
	}
}
