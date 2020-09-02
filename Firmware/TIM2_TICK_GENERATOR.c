#include "REG52.h"
#include "TIM2_TICK_GENERATOR.h"

/* Define T2MOD register(which is at 0xC9 address) */
sfr T2MOD  = 0xC9;

#define RELOAD_VALUE_H	0xFC
#define RELOAD_VALUE_L	0x18

void TIM2TickGenerator_Setup(void)
{
	/* Configure T2 mod register */
	T2MOD = 0x00;
	
	/* Configure T2 control register */
	T2CON = 0x00;
	
	/* Configure reload registers to split milisecond interval */
	RCAP2H = RELOAD_VALUE_H;
	RCAP2L = RELOAD_VALUE_L;
	
	/* Enable timer2 interrupt */
	ET2 = 1;
	EA = 1;
}

void TIM2TickGenerator_Start(void)
{
	/* Start Timer2 */
	TR2 = 1;
}

void TIM2TickGenerator_Stop(void)
{
	/* Stop Timer2 */
	TR2 = 0;
}