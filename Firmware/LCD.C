#include "REG52.h"
#include <intrins.h>
#include "LCD.H"

#define LCD_PORT						P2

#define RS_INST_REG					0
#define RS_DATA_REG					1
#define RW_WRITE						0
#define RW_READ							1

#define LCD_STATE_BUSY 			0
#define LCD_STATE_NOT_BUSY	1
#define QUEUE_SIZE					30

#define UNLOCKED						0
#define LOCKED							1

#define SET_DDRAM_ADDRESS_INST_MASK	0x80

typedef struct Process_t
{
	unsigned char register_select;
	unsigned char byte;
};

typedef struct Queue_t
{
	struct Process_t buffer[QUEUE_SIZE];
	unsigned char rear;
	unsigned char front;
	unsigned char size;
};

static struct Queue_t process_queue;
static unsigned char lock_state;
static const unsigned char ascii_table[] = {"0123456789"};

sbit LCD_RS_PIN = LCD_PORT^0;
sbit LCD_RW_PIN	=	LCD_PORT^1;
sbit LCD_EN_PIN	=	LCD_PORT^2;

/* Private function prototypes */
static void send(unsigned char byte);
static unsigned char readBusyFlag(void);
static void front(struct Process_t *pElement);
static void enqueue(struct Process_t *pElement);
static void dequeue(void);

void LCD_Init(void)
{
	unsigned int delay_counter;
	unsigned char temp;
	
	/* Lock the module */
	lock_state = LOCKED;
	
	/* Initialize queue variables */
	process_queue.rear = 0;
	process_queue.front = 0;
	process_queue.size = 0;
	
	/* Wait for some time that the HD44780 can finish it's initialization process */
	for (delay_counter = 0; delay_counter < 20000; delay_counter++);
	
	LCD_EN_PIN = 0;
	LCD_RW_PIN = RW_WRITE;
	LCD_RS_PIN = RS_INST_REG;
	
	/* Irregular write, because initially the HD44780 is at 8bit mode, so we do a 8bit write */
	temp = LCD_PORT & 0x0F;
	temp |= LCD_CMD_FUNCTION_SET_DATA_LEN_4BIT;
	LCD_PORT = temp;
	
	LCD_EN_PIN = 1;
	_nop_ ();
	_nop_ ();
	_nop_ ();
	_nop_ ();
  LCD_EN_PIN = 0;
	
	/* Wait for LCD to initialize */
	
	lock_state = UNLOCKED;
}

void LCD_SendNumber(unsigned int number, unsigned char digits)
{
	unsigned char i;
	unsigned char quotient, remainder;
	unsigned char number_string[5];
	struct Process_t element;
	
	quotient = number;
	
	/* Calculate the digits */
	for (i = 0; i < digits; i++)
	{
		remainder = quotient % 10;
		quotient /= 10;
		number_string[i] = ascii_table[remainder];
	}
	
	/* Set register select to data reg, because it's a data which is to be send */
	element.register_select = RS_DATA_REG;
	
	lock_state = LOCKED;
	
	/* Add all elements to queue */
	for (i = 0; i < digits; i++)
	{
		element.byte = number_string[digits - (i + 1)];
		enqueue(&element);
	}
	
	lock_state = UNLOCKED;
}

/* Exported functions */
void LCD_SendString(unsigned char string[])
{
	struct Process_t element;
	unsigned char i = 0;
	
	element.register_select = RS_DATA_REG;
	
	lock_state = LOCKED;
	
	/* Until the last element of the string, queue all elements */
	while (string[i] != 0)
	{
		element.byte = string[i];
		enqueue(&element);
		i++;
	}
	
	lock_state = UNLOCKED;
}

void LCD_SetAddress(unsigned char address)
{
	struct Process_t element;
	
	element.register_select = RS_INST_REG;
	element.byte = (address | SET_DDRAM_ADDRESS_INST_MASK);
	
	lock_state = LOCKED;
	
	enqueue(&element);
	
	lock_state = UNLOCKED;
}

void LCD_SendCommand(unsigned char command)
{
	struct Process_t element;
	
	element.register_select = RS_INST_REG;
	element.byte = command;
	
	lock_state = LOCKED;
	
	enqueue(&element);
	
	lock_state = UNLOCKED;
}

void LCD_SendCharacter(unsigned char character)
{
	struct Process_t element;
	
	element.register_select = RS_DATA_REG;
	element.byte = character;
	
	lock_state = LOCKED;
	
	enqueue(&element);
	
	lock_state = UNLOCKED;
}

/* Execute the task periodically */
unsigned char LCD_PeriodicTaskExecute(void)
{
	struct Process_t element;
	
	/* If the module is locked, return */
	if (lock_state == LOCKED)
	{
		return LCD_TASK_PENDING;
	}
	
	/* If there is a pending process execute it, if the LCD is available */
	if (process_queue.size > 0)
	{
		/* If internal process is going on at the HD44780, return */
		if (readBusyFlag() == LCD_STATE_BUSY)
		{
			return LCD_TASK_SUCCESS;
		}
		
		/* Parse front element and send it */
		front(&element);
		
		LCD_RW_PIN = RW_WRITE;
		LCD_RS_PIN = element.register_select;
		
		send(element.byte);
		
		/* Dequeue element */
		dequeue();
	}
	
	return LCD_TASK_SUCCESS;
}
	
/* Private functions */
/* Send byte */
void send(unsigned char byte)
{
	unsigned char temp = 0;

	/* Parse the first nibble, and move it to the LCD port */
	temp = LCD_PORT & 0x0F;
	temp |= (byte & 0xF0);
	LCD_PORT = temp; 
	
	/* Clock EN pin, to move first nibble to HD44780 instruction _register */
	LCD_EN_PIN = 1;
	_nop_ ();
	_nop_ ();
	_nop_ ();
	_nop_ ();
  LCD_EN_PIN = 0;
	
	/* Parse the second nibble, and move it to the LCD port */
	temp = LCD_PORT & 0x0F;
	temp |= ((byte & 0x0F) << 4);
	LCD_PORT = temp;
	
	/* Clock EN pin, to move second nibble to HD44780 instruction _register */
	LCD_EN_PIN = 1;
	_nop_ ();
	_nop_ ();
	_nop_ ();
	_nop_ ();
  LCD_EN_PIN = 0;
	return;
}

/* Read busy flag */
unsigned char readBusyFlag(void)
{
	unsigned char temp;
	
	LCD_RW_PIN = RW_READ;
	LCD_RS_PIN = RS_INST_REG;
	LCD_PORT |= 0xF0;
	
	/* Clock EN pin, and read besides it */
	LCD_EN_PIN = 1;
	_nop_ ();
	_nop_ ();
	_nop_ ();
	temp = LCD_PORT;
  LCD_EN_PIN = 0;
	
	LCD_EN_PIN = 1;
	_nop_ ();
	_nop_ ();
	_nop_ ();
	_nop_ ();
	LCD_EN_PIN = 0;
	
	/* Check busy flag */
	if (temp & 0x80)
	{
		return LCD_STATE_BUSY;
	}
	else
	{
		return LCD_STATE_NOT_BUSY;
	}
}

static void front(struct Process_t *pElement)
{
  /* Read element which is at the front */
  pElement->register_select = process_queue.buffer[process_queue.front].register_select;
	pElement->byte = process_queue.buffer[process_queue.front].byte;
}

static void enqueue(struct Process_t *pElement)
{
  /* Insert the element in its rear side */
  process_queue.buffer[process_queue.rear].register_select = pElement->register_select;
	process_queue.buffer[process_queue.rear].byte = pElement->byte;
  
  process_queue.size++;
  process_queue.rear++;
    
  /* As we fill the queue in circular fashion */
  if (process_queue.rear == QUEUE_SIZE)
  {
    process_queue.rear = 0;
  }
}

static void dequeue(void)
{
  process_queue.size--;
  process_queue.front++;
    
  /* As we fill elements in circular fashion */
  if(process_queue.front == QUEUE_SIZE)
  {
    process_queue.front = 0;
  }
}