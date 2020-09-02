/* Exported constants */
#define LCD_TASK_EXECUTE_PERIOD															1

/* LCD task state */
#define LCD_TASK_SUCCESS																		0x00
#define LCD_TASK_PENDING																		0x01

/* LCD commands */
#define LCD_CMD_CLEAR_DISPLAY																0x01
#define LCD_CMD_RETURN_HOME																	0x02
#define LCD_CMD_ENTRY_MODE_DECREMENT_NO_SHIFT								0x04
#define LCD_CDM_ENTRY_MODE_DECREMENT_SHIFT									0x05
#define LCD_CMD_ENTRY_MODE_INCREMENT_NO_SHIFT								0x06
#define LCD_CMD_ENTRY_MODE_INCREMENT_SHIFT									0x07
#define LCD_CMD_DISPLAY_ON_OFF_DISP_OFF											0x08
#define LCD_CMD_DISPLAY_ON_OFF_DISP_ON_CURSOR_OFF_BLINK_OFF	0x0C
#define LCD_CMD_DISPLAY_ON_OFF_DISP_ON_CURSOR_OFF_BLINK_ON	0x0D
#define LCD_CMD_DISPLAY_ON_OFF_DISP_ON_CURSOR_ON_BLINK_OFF	0x0E
#define LCD_CMD_DISPLAY_ON_OFF_DISP_ON_CURSOR_ON_BLINK_ON   0x0F
#define LCD_CMD_CURSOR_OR_DISPLAY_SHIFT_CURSOR_MOVE_LEFT		0x10
#define LCD_CMD_CURSOR_OR_DISPLAY_SHIFT_CURSOR_MOVE_RIGHT		0x14
#define LCD_CMD_CURSOR_OR_DISPLAY_SHIFT_DISPLAY_SHIFT_LEFT	0x18
#define LCD_CMD_CURSOR_OR_DISPLAY_SHIFT_DISPLAY_SHIFT_RIGHT	0x1C
#define LCD_CMD_FUNCTION_SET_DATA_LEN_4BIT									0x20
#define LCD_CMD_FUNCTION_SET_DATA_LEN_4BIT_1_LINE_FONT_5X8	0x20
#define LCD_CMD_FUNCTION_SET_DATA_LEN_4BIT_1_LINE_FONT_5X10	0x24
#define LCD_CMD_FUNCTION_SET_DATA_LEN_4BIT_2_LINE_FONT_5X8	0x28
#define LCD_CMD_FUNCTION_SET_DATA_LEN_4BIT_2_LINE_FONT_5X10	0x2C
#define LCD_CMD_FUNCTION_SET_DATA_LEN_8BIT_1_LINE_FONT_5X8	0x30
#define LCD_CMD_FUNCTION_SET_DATA_LEN_8BIT_1_LINE_FONT_5X10	0x34
#define LCD_CMD_FUNCTION_SET_DATA_LEN_8BIT_2_LINE_FONT_5X8	0x38
#define LCD_CMD_FUNCTION_SET_DATA_LEN_8BIT_2_LINE_FONT_5X10	0x3C

/* Exported functions */
void LCD_Init(void);
void LCD_SendString(unsigned char array[]);
void LCD_SendNumber(unsigned int number, unsigned char digits);
void LCD_SetAddress(unsigned char address);
void LCD_SendCommand(unsigned char command);
void LCD_SendCharacter(unsigned char character);

/* Periodic task */
unsigned char LCD_PeriodicTaskExecute(void);