#include "COUNTDOWN_TIMER.h"

#define UNLOCKED			0
#define LOCKED				1

static unsigned char state = COUNTDOWN_TIMER_STATE_STANDBY;
static unsigned char lock_state = UNLOCKED;
static unsigned char l_hours, l_minutes, l_seconds, l_split_seconds = 0;
static unsigned char events = 0x00;

unsigned char CountdownTimer_PeriodicTaskExecute(void)
{
	if (lock_state == LOCKED)
	{
		return COUNTDOWN_TIMER_TASK_PENDING;
	}
	
	if (state != COUNTDOWN_TIMER_STATE_RUNNING)
	{
		return COUNTDOWN_TIMER_TASK_SUCCESS;
	}
	
	events |= COUNTDOWN_TIMER_UPDATE;
	
	/* Decrease counter objects */
	if (l_split_seconds >= 3)
	{
		l_split_seconds -= 3;
	}
	else if(l_seconds > 0)
	{
		l_split_seconds += (60 - 3);
		l_seconds--;
	}
	else if (l_minutes > 0)
	{
		l_split_seconds += (60 - 3);
		l_seconds = (60 - 1);
		l_minutes--;
	}
	else if (l_hours > 0)
	{
		l_split_seconds += (60 - 3);
		l_seconds = (60 - 1);
		l_minutes = (60 - 1);
		l_hours--;
	}
	else
	{
		l_split_seconds = 0;
		
		/* Set timer expired event */
		events |= COUNTDOWN_TIMER_EXPIRED;
		
		/* Set the module to standby */
		state = COUNTDOWN_TIMER_STATE_STANDBY;
	}
	
	return COUNTDOWN_TIMER_TASK_SUCCESS;
}

unsigned char CountdownTimer_Start(void)
{
	/* If time elements are zero, can't start the coundown timer */
	if ((l_hours == 0) && (l_minutes == 0) && (l_seconds == 0) && (l_split_seconds == 0))
	{
		return COUNTDOWN_TIMER_STATE_STANDBY;
	}
	
	lock_state = LOCKED;
	
	/* Set module's state */
	state = COUNTDOWN_TIMER_STATE_RUNNING;
	
	lock_state = UNLOCKED;
	
	return COUNTDOWN_TIMER_STATE_RUNNING;
}

void CountdownTimer_GetValues(unsigned char *pHours, unsigned char *pMinutes, 
															unsigned char *pSeconds, unsigned char *pSplitSeconds)
{
	lock_state = LOCKED;
	
	*pHours = l_hours;
	*pMinutes = l_minutes;
	*pSeconds = l_seconds;
	*pSplitSeconds = l_split_seconds;
	
	lock_state = UNLOCKED;
}

void CountdownTimer_SetValues(unsigned char hours, unsigned char minutes, 
															unsigned char seconds, unsigned char splitSeconds)
{
	if (state != COUNTDOWN_TIMER_STATE_STANDBY)
	{
		return;
	}
	
	lock_state = LOCKED;
	
	l_hours = hours;
	l_minutes = minutes;
	l_seconds = seconds;
	l_split_seconds = splitSeconds;
	
	lock_state = UNLOCKED;
}

void CountdownTimer_Stop(void)
{
	lock_state = LOCKED;
	
	state = COUNTDOWN_TIMER_STATE_STANDBY;
	
	lock_state = UNLOCKED;
	
	return;
}

void CountdownTimer_GetEvents(unsigned char *pEvents)
{
	/* Lock module */
	lock_state = LOCKED;
	
	*pEvents = events;
	
	/* Clear events, because they are read */
	events = 0;
	
	/* Unlock the module */
	lock_state = UNLOCKED;
}