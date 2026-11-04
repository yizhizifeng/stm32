#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "Key.h"
#include "Motor.h"
#include "Timer.h"

uint8_t KeyNum;
int8_t Speed;

int main()
{
	OLED_Init();
	Motor_Init();
	Key_Init();
	Timer_Init();
	
	OLED_ShowString(1,1,"Speed:");
	
	while(1)
	{ 
		KeyNum=Key_GetNum();
		if(KeyNum==1)
		{
			Speed+=20;
			if(Speed>100)
			{
				Speed=100;
			}
		}
		if(KeyNum==2)
		{
			Speed-=20;
			if(Speed<-100)
			{
				Speed=-100;
			}
		}
		Motor_SetSpeed(Speed);
		OLED_ShowSignedNum(1,7,Speed,3);
	}
}

void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET)
	{
		Key_Tick();
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
	}
}
