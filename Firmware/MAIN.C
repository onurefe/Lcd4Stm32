#include "REG52.h"
#include "TIM2_TICK_GENERATOR.h"
#include "LCD.H"
#include "JOYSTICK.H"
#include "COUNTDOWN_TIMER.h"

#define STATE_STANDBY 			0
#define STATE_RUNNING				1
#define STATE_EXPIRED				2

#define TIMER_LCD_ADDRESS		5
#define CONTROL_LCD_ADDRESS	0x4A

#define CURSOR_RIGHT				0x00
#define CURSOR_LEFT					0x01

sbit LED_PIN_K = P3^3;

/* Private functions */
void cursorMove(unsigned char *pCursorAddress, unsigned char direction);
	
/* Menu */
unsigned char state = STATE_STANDBY;

/* Counters to run tasks at different periods */
unsigned char lcd_task_period_count = 0;
unsigned char joystick_task_period_count = 0;
unsigned char countdown_timer_task_period_count = 0;

unsigned char countdown_timer_events = 0;
unsigned char joystick_events = 0;

unsigned char cursor_address = 0;
unsigned char hours, minutes, seconds, split_seconds = 0;

void main(void)
{
	/* Turn off the led(led is connected to cathode on the supply side) */
	LED_PIN_K = 1;
	
	/* Init LCD module */
	LCD_Init();
		
	/* Init joystick module */
	Joystick_Init();
	
	/* 4bit mode 2 line font 5x8 */
	LCD_SendCommand(LCD_CMD_FUNCTION_SET_DATA_LEN_4BIT_2_LINE_FONT_5X8);
	
	/* Display cursor and blink on */
	LCD_SendCommand(LCD_CMD_DISPLAY_ON_OFF_DISP_ON_CURSOR_ON_BLINK_OFF);
	
	/* Set entry mode to increment shift of the cursor on every write */
	LCD_SendCommand(LCD_CMD_ENTRY_MODE_INCREMENT_NO_SHIFT);

	/* Turn off the cursor */
	LCD_SendCommand(LCD_CMD_DISPLAY_ON_OFF_DISP_ON_CURSOR_OFF_BLINK_OFF);

	/* Create the menu */
	LCD_SetAddress(0x05);
	LCD_SendNumber(hours, 2);
	LCD_SendCharacter(':');
	LCD_SetAddress(0x08);
	LCD_SendNumber(minutes, 2);
	LCD_SendCharacter(':');
	LCD_SetAddress(0x0B);
	LCD_SendNumber(seconds, 2);
	LCD_SendCharacter(':');
	LCD_SetAddress(0x0E);
	LCD_SendNumber(split_seconds, 2);
	LCD_SendCharacter(':');
	LCD_SetAddress(0x4A);
	LCD_SendString("BASLAT");
	
	/* Set cursor to home */
	LCD_SetAddress(0x00);
	LCD_SendCommand(LCD_CMD_DISPLAY_ON_OFF_DISP_ON_CURSOR_ON_BLINK_OFF);
									
	/* Configure TIMER2 */
	TIM2TickGenerator_Setup();

	/* Start tick generation */
	TIM2TickGenerator_Start();
			
	/* Infinite loop */
	while (1)
	{
		/* Get events */
		Joystick_GetEvents(&joystick_events);
		CountdownTimer_GetEvents(&countdown_timer_events);
		
		/* Take action according to events */
		/* Cursor move events are not state dependent, so process them anyway */
		/* Left button pressed event */
		if (joystick_events & JOYSTICK_LEFT_BUTTON_MASK)
		{
			cursorMove(&cursor_address, CURSOR_LEFT);
			LCD_SetAddress(cursor_address);
		}
		
		/* Right button pressed event */
		if (joystick_events & JOYSTICK_RIGHT_BUTTON_MASK)
		{
			cursorMove(&cursor_address, CURSOR_RIGHT);
			LCD_SetAddress(cursor_address);
		}
		
		/* OK Button pressed event */
		if (joystick_events & JOYSTICK_OK_BUTTON_MASK)
		{
			/* Turn off the cursor */
			LCD_SendCommand(LCD_CMD_DISPLAY_ON_OFF_DISP_ON_CURSOR_OFF_BLINK_OFF);
			
			switch (state)
			{
				case STATE_STANDBY:
				{
					/* Baslat/Durdur text selected and pressed */
					if ((0x4A <= cursor_address) && (cursor_address <= 0x4F))
					{
						/* Set countdown timer */
						CountdownTimer_SetValues(hours, minutes, seconds, split_seconds);
						
						/* If success to switch running state, do these */
						if (COUNTDOWN_TIMER_STATE_RUNNING == CountdownTimer_Start())
						{
							state = STATE_RUNNING;
							
							LCD_SetAddress(0x4A);
							LCD_SendString("DURDUR");
						}
					}
					
					/* Hour digit high selected */
					if (0x05 == cursor_address)
					{
						hours += 10;
						hours %= 60;
						
						LCD_SetAddress(0x05);
						LCD_SendNumber(hours, 2);
					}
					
					/* Hour digit low selected */
					if (0x06 == cursor_address)
					{
						hours += 1;
						hours %= 60;
						
						LCD_SetAddress(0x05);
						LCD_SendNumber(hours, 2);
					}
					
					/* Minute digit high selected */
					if (0x08 == cursor_address)
					{
						minutes += 10;
						minutes %= 60;
						
						LCD_SetAddress(0x08);
						LCD_SendNumber(minutes, 2);
					}
					
					/* Minute digit low selected */
					if (0x09 == cursor_address)
					{
						minutes += 1;
						minutes %= 60;
						
						LCD_SetAddress(0x08);
						LCD_SendNumber(minutes, 2);
					}
					
					/* Second digit high selected */
					if (0x0B == cursor_address)
					{
						seconds += 10;
						seconds %= 60;
						
						LCD_SetAddress(0x0B);
						LCD_SendNumber(seconds, 2);
					}
					
					/* Second digit low selected */
					if (0x0C == cursor_address)
					{
						seconds += 1;
						seconds %= 60;
						
						LCD_SetAddress(0x0B);
						LCD_SendNumber(seconds, 2);
					}
					
					/* Split second digit high selected */
					if (0x0E == cursor_address)
					{
						split_seconds += 10;
						split_seconds %= 60;
						
						LCD_SetAddress(0x0E);
						LCD_SendNumber(split_seconds, 2);
					}
					
					/* Split second digit low selected */
					if (0x0F == cursor_address)
					{
						split_seconds += 1;
						split_seconds %= 60;
						
						LCD_SetAddress(0x0E);
						LCD_SendNumber(split_seconds, 2);
					}
				}
				break;
				
				case STATE_RUNNING:
				case STATE_EXPIRED:
				{
					/* Baslat/Durdur text selected and pressed */
					if ((0x4A <= cursor_address) && (cursor_address <= 0x4F))
					{
						CountdownTimer_Stop();
						
						LCD_SetAddress(0x4A);
						LCD_SendString("BASLAT");
						
						state = STATE_STANDBY;
						
						/* Ensure that the led is turned off */
						LED_PIN_K = 1;
					}
				}
				break;
			}
			
			/* Retrieve the cursor */
			LCD_SetAddress(cursor_address);
			LCD_SendCommand(LCD_CMD_DISPLAY_ON_OFF_DISP_ON_CURSOR_ON_BLINK_OFF);
		}
		
		/* Countdown timer expired event */
		if (countdown_timer_events & COUNTDOWN_TIMER_EXPIRED)
		{
			/* Switch to the timer expired state */
			state = STATE_EXPIRED;
			
			/* Turn on the led*/
			LED_PIN_K = 0;
		}
		
		/* Timer update event */
		if (countdown_timer_events & COUNTDOWN_TIMER_UPDATE)
		{
			/* Turn off the cursor */
			LCD_SendCommand(LCD_CMD_DISPLAY_ON_OFF_DISP_ON_CURSOR_OFF_BLINK_OFF);
			
			CountdownTimer_GetValues(&hours, &minutes, &seconds, &split_seconds);
			
			/* Update LCD */
			LCD_SetAddress(0x05);
			LCD_SendNumber(hours, 2);
			LCD_SendCharacter(':');
			LCD_SetAddress(0x08);
			LCD_SendNumber(minutes, 2);
			LCD_SendCharacter(':');
			LCD_SetAddress(0x0B);
			LCD_SendNumber(seconds, 2);
			LCD_SendCharacter(':');
			LCD_SetAddress(0x0E);
			LCD_SendNumber(split_seconds, 2);
			LCD_SendCharacter(':');
			
			/* Retrieve the cursor */
			LCD_SetAddress(cursor_address);
			LCD_SendCommand(LCD_CMD_DISPLAY_ON_OFF_DISP_ON_CURSOR_ON_BLINK_OFF);
		}
	}
}

/* Timer 2 interrupt */
void timer0 (void) interrupt 5
{
	/* LCD module */
	if (++lcd_task_period_count >= LCD_TASK_EXECUTE_PERIOD)
	{
		if (LCD_PeriodicTaskExecute() == LCD_TASK_SUCCESS)
		{
			lcd_task_period_count = 0;
		}
	}
	
	/* Joystick module */
	if (++joystick_task_period_count >= JOYSTICK_TASK_EXECUTE_PERIOD)
	{
		if (Joystick_PeriodicTaskExecute() == JOYSTICK_TASK_SUCCESS)
		{
			joystick_task_period_count = 0;
		}
	}
	
	/* Countdown timer module */
	if (++countdown_timer_task_period_count >= COUNTDOWN_TIMER_TASK_EXECUTE_PERIOD)
	{
		if (CountdownTimer_PeriodicTaskExecute() == COUNTDOWN_TIMER_TASK_SUCCESS)
		{
			countdown_timer_task_period_count = 0;
		}
	}
	
	/* Clear interrupt flag */
	TF2 = 0;
}

void cursorMove(unsigned char *pCursorAddress, unsigned char direction)
{
	if (direction == CURSOR_LEFT)
	{
		if ((*pCursorAddress) == 0x0F)
		{
			(*pCursorAddress) = 0x40;
		}
		else if ((*pCursorAddress) == 0x4F)
		{
			(*pCursorAddress) = 0x00;
		}
		else
		{
			(*pCursorAddress)++;
		}
	}
	else if (direction == CURSOR_RIGHT)
	{
		if ((*pCursorAddress) == 0x00)
		{
			(*pCursorAddress) = 0x4F;
		}
		else if ((*pCursorAddress) == 0x40)
		{
			(*pCursorAddress) = 0x0F;
		}
		else
		{
			(*pCursorAddress)--;
		}
	}
}