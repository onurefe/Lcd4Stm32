#include "REG52.h"

/* Execute every 25 msec */
#define JOYSTICK_TASK_EXECUTE_PERIOD	25

/* Joystick task state */
#define JOYSTICK_TASK_SUCCESS					0x00
#define JOYSTICK_TASK_PENDING					0x01

#define JOYSTICK_PORT									P3
#define JOYSTICK_LEFT_BUTTON_MASK			0x20
#define JOYSTICK_OK_BUTTON_MASK				0x40
#define JOYSTICK_RIGHT_BUTTON_MASK		0x80
#define JOYSTICK_MASK									(JOYSTICK_LEFT_BUTTON_MASK | JOYSTICK_OK_BUTTON_MASK | JOYSTICK_RIGHT_BUTTON_MASK)

void Joystick_Init(void);
unsigned char Joystick_PeriodicTaskExecute(void);
void Joystick_GetEvents(unsigned char *pEvents);