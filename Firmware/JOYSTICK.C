#include "REG52.h"
#include "JOYSTICK.H"

#define UNLOCKED			0
#define LOCKED				1

static unsigned char events = 0x00;
static unsigned char lock_state = UNLOCKED;

void Joystick_Init(void)
{
	/* Set joystick pins as inputs */
	JOYSTICK_PORT |= JOYSTICK_MASK;
}

unsigned char Joystick_PeriodicTaskExecute(void)
{
	static unsigned char last_joystick_state = JOYSTICK_MASK;
	unsigned char joystick_state;
	
	if (lock_state == LOCKED)
	{
		return JOYSTICK_TASK_PENDING;
	}
	
	/* Parse joystick state */
	joystick_state = JOYSTICK_PORT & JOYSTICK_MASK;
	
	/* Calculate the latest press events(logic 1->0 transitions) and return them */
	events = (last_joystick_state & (~joystick_state));
	
	/* Update last joystick state */
	last_joystick_state = joystick_state;
	
	return JOYSTICK_TASK_SUCCESS;
}

void Joystick_GetEvents(unsigned char *pEvents)
{
	/* Lock module */
	lock_state = LOCKED;
	
	*pEvents = events;
	
	/* Clear events, because they are read */
	events = 0;
	
	/* Unlock the module */
	lock_state = UNLOCKED;
}