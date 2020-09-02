#define COUNTDOWN_TIMER_TASK_EXECUTE_PERIOD					50

/* Countdown timer task state */
#define COUNTDOWN_TIMER_TASK_SUCCESS								0x00
#define COUNTDOWN_TIMER_TASK_PENDING								0x01

/* Countdown timer state */
#define COUNTDOWN_TIMER_STATE_STANDBY								0
#define COUNTDOWN_TIMER_STATE_RUNNING								1

/* Countdown timer events */
#define COUNTDOWN_TIMER_EXPIRED											0x01
#define COUNTDOWN_TIMER_UPDATE											0x02

/* Exported functions */
unsigned char CountdownTimer_PeriodicTaskExecute(void);
unsigned char CountdownTimer_Start(void);
void CountdownTimer_GetValues(unsigned char *pHours, unsigned char *pMinutes, 
															unsigned char *pSeconds, unsigned char *pSplitSeconds);
void CountdownTimer_SetValues(unsigned char pHours, unsigned char minutes, 
														  unsigned char seconds, unsigned char splitSeconds);
void CountdownTimer_Stop(void);
void CountdownTimer_GetEvents(unsigned char *pEvents);