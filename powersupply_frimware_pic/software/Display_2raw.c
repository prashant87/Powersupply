#include "micro.h"
#include "Display_2raw.h"
#include "buton.h"

#define InitWait40ms 20
#define InitWait2ms 1
#define InitWaitRefreshms 100

#define LCD_CMD_FUNC 0x20
#define LCD_8BIT_INTERFACE 0x10
#define LCD_2LINES 0x08
#define LCD_1LINES 0x00
#define LCD_FONT_SMALL 0x00
#define LCD_FONT_LARGE 0x04
#define LCD_NOT_USED 0x03
#define LCD_CMD_DISP_ONOFF 0x08
#define LCD_CMD_CLEAR 0x01
#define LCD_CMD_ENTRY 0x04
#define LCD_ENTRY_I 0x02
#define LCD_ENTRY_S 0x01
#define LCD_CMD_DISP_ON 0x04
#define LCD_CMD_CURSOR_ON 0x02
#define LCD_CMD_POSITION_ON 0x01
#define LCD_CMD_RETURN_HOME 0x02


bank1 T_UBYTE ubFirstByte,ubSecondByte,ubThrdByte,ubFourthByte;
bank1 T_UWORD ubNumberINT,uwNumberFRC;
bank1 T_UBYTE ub_state_Display2r;
bank1 T_UBYTE ub_counter_1;
bank1 T_UBYTE ub_busy;
bank1 T_UBYTE ub_char;
bank1 T_UBYTE ub_Datawrite;
bank1 T_UBYTE ubDispIndex;
bank1 T_UBYTE ubDisplay_buf[MaxRawDisp];

void InitDisplay2raw()
{
    ubFirstByte = 0;
    ubSecondByte = 0;
    ubNumberINT = 0;
    ubThrdByte=0;
    ubFourthByte=0;
	ub_counter_1 = 0;
	ub_busy = 0;
	ub_char = 0;
	RC0 = 0;
	for (ubDispIndex = 0;ubDispIndex<MaxRawDisp;ubDispIndex++)
	{
		ubDisplay_buf[ubDispIndex]= 0x20;
	}
	ubDisplay_buf[0] = 0x11;
	ub_state_Display2r = Display_no_init;
}


void SetDisplay_Voltage(T_UWORD uwVoltage)
{
    T_UBYTE lubNumber=0;
    T_UBYTE lubSecondByte,lubFirstByte;
	T_UWORD luwNumberFrac;
    
    /*transform to 8 bit int part*/
    ubNumberINT = uwVoltage >> Voltage_Resol_Sh;
    lubNumber = ubNumberINT;
    lubFirstByte = lubNumber %10;
    lubNumber = lubNumber /10;
    lubSecondByte = lubNumber %10;
	
	luwNumberFrac = uwVoltage & 0x07ff;
	luwNumberFrac = luwNumberFrac * 10;
	luwNumberFrac = luwNumberFrac >> Voltage_Resol_Sh;
	
    ubSecondByte = lubSecondByte;
	ubDisplay_buf[0] = ubSecondByte + 0x30;
    ubFirstByte = lubFirstByte;
	ubDisplay_buf[1] = ubFirstByte  + 0x30;
	ubDisplay_buf[2] = 0x56;
	ubDisplay_buf[3] = ((T_UBYTE)luwNumberFrac) + 0x30;
	
}

void SetDisplay_Current(T_UWORD uwCurent)
{
    T_UBYTE lubNumber=0;
    T_UBYTE lubSecondByte,lubFirstByte;
	T_UWORD luwTemp;    
    /*transform to 8 bit int part*/
	luwTemp = (T_UWORD)uwCurent >> Curent_Resol_Sh;
    ubNumberINT = luwTemp;
    lubNumber = ubNumberINT;
    
	uwNumberFRC = uwCurent & (0x0FFF);
    uwNumberFRC = uwNumberFRC * 10;
    /*copy to buffer numbers */
    uwNumberFRC = uwNumberFRC >> Curent_Resol_Sh;
	
    ubFourthByte = uwNumberFRC;
    ubThrdByte = ubNumberINT;
	ubDisplay_buf[4] = 0x02F; 
	ubDisplay_buf[5] = ubThrdByte + 0x30;
	ubDisplay_buf[6] =0x2E;
	ubDisplay_buf[7] = ubFourthByte + 0x30;
	ubDisplay_buf[8] = 0x41;
	
}

void SetDisplay_Current_target(T_UWORD uwCurent)
{
    T_UBYTE lubNumber=0;
    T_UBYTE lubSecondByte,lubFirstByte;
	T_UWORD luwTemp;    
    /*transform to 8 bit int part*/
	luwTemp = (T_UWORD)uwCurent >> Curent_Resol_Sh;
    ubNumberINT = luwTemp;
    lubNumber = ubNumberINT;
    
	uwNumberFRC = uwCurent & (0x0FFF);
    uwNumberFRC = uwNumberFRC * 10;
    /*copy to buffer numbers */
    uwNumberFRC = uwNumberFRC >> Curent_Resol_Sh;
	
    ubFourthByte = uwNumberFRC;
    ubThrdByte = ubNumberINT;
	ubDisplay_buf[12] = ubThrdByte + 0x30;
	ubDisplay_buf[13] =0x2e;
	ubDisplay_buf[14] = ubFourthByte + 0x30;
	ubDisplay_buf[15] =0x41;
}
void Dispaly2raw_task(void)
{
	switch (ub_state_Display2r)
	{
		case Display_no_init:
			/*wait 40ms to init*/
			if (ub_counter_1 > InitWait40ms)
			{
				/*setup interface , nr of lines and font size first */
				SetCommand(LCD_CMD_FUNC | LCD_8BIT_INTERFACE | LCD_1LINES | LCD_FONT_SMALL |LCD_NOT_USED );
				ub_state_Display2r = Display_init_step;
				ub_counter_1 = 0;
				//RC0 = ~RC0;
			}
			else
			{
				ub_counter_1++;
			}
		break;
		case Display_init_step:
			/*wait >37us*/
			/*setup interface , nr of lines and font size*/
			SetCommand(LCD_CMD_FUNC | LCD_8BIT_INTERFACE | LCD_1LINES | LCD_FONT_SMALL |LCD_NOT_USED );
			ub_state_Display2r = Display_init_DisplayOff;
			//RC0 = ~RC0 ;
		case Display_init_DisplayOff:
			/*wait >37us*/
			/*display off*/
			ub_busy = IsBusy();
			if (ub_busy ==0)
			{
				SetCommand(LCD_CMD_DISP_ONOFF | LCD_CMD_DISP_ON | LCD_CMD_CURSOR_ON | LCD_CMD_POSITION_ON);
				ub_state_Display2r = Display_init_ClearDisplay;		
			}
			else
			{
			
			}	
			//RC0 = ~RC0 ;
		break;
		case Display_init_ClearDisplay:
			/*wait >37us*/
			/*clear display*/
			ub_busy = IsBusy();
			if (ub_busy ==0)
			{
			
				SetCommand(LCD_CMD_CLEAR);
				ub_state_Display2r = Display_init_EntryMode;

				ub_counter_1 = 0;
				}
			else
			{
			
			}
			//RC0 = ~RC0 ;
		break;
		case Display_init_EntryMode:
			/*Wait time >1.52mS to clear*/
			if (ub_counter_1 > InitWait2ms)
			{
				ub_busy = IsBusy();
				if (ub_busy ==0)
				{
					/*setup interface , nr of lines and font size*/
					SetCommand(LCD_CMD_ENTRY | LCD_ENTRY_I);
					ub_state_Display2r = Display_init_DisplayOn;
					ub_counter_1 = 0;
					
				}
			}
			else
			{
				ub_counter_1++;
			}
		break;
		case Display_init_DisplayOn:
			/*setup interface , nr of lines and font size*/
			//SetCommand(LCD_CMD_DISP_ONOFF | LCD_CMD_DISP_ON | LCD_CMD_CURSOR_ON | LCD_CMD_POSITION_ON);
			ub_busy = IsBusy();
			if (ub_busy ==0)
			{
				ub_state_Display2r = Display_idle;
			}
			
			//RC0 = ~RC0 ;
		break;
		case Display_idle:
			ub_busy = IsBusy();
			if (ub_busy ==0)
			{
				if (ub_counter_1 > InitWaitRefreshms)
				{
					ub_state_Display2r = Display_clear_screen;
					ubDispIndex = 0;
					ub_counter_1 = 0;
				}
				else
				{
					ub_counter_1++;
				}
			}
		break;
		case Display_clear_screen:
			ub_busy = IsBusy();
			if (ub_busy ==0)
			{
				SetCommand(LCD_CMD_RETURN_HOME);
				ub_state_Display2r = Display_write_string;
			}
		break;
		case Display_write_string:
			ub_busy = IsBusy();
			if (ub_busy ==0)
			{
				ub_Datawrite = ubDisplay_buf[ubDispIndex];
				WriteData(ub_Datawrite);
				ubDispIndex++;
				if(ubDispIndex > MaxRawDisp)
				{
					ub_state_Display2r = Display_idle;
					ubDispIndex = 0;
				}
			}
		break;
		default :
			ub_state_Display2r = Display_no_init;
		break;
	}
}

void SetCommand(T_UBYTE ub_Command)
{
	TRISB = Direction_OUPUT;
	LCD_delay_us(10);
	PIN_RS = Disable;
	PIN_R = Disable;
	PIN_E = Enable;
	PORTB = ub_Command;	
	LCD_delay_us(10);
	PIN_E = Disable;
}

void WriteData(T_UBYTE ub_Data)
{
	TRISB = Direction_OUPUT;
	LCD_delay_us(10);
	PIN_RS = Enable;
	PIN_R = Disable;
	PIN_E = Enable;
	PORTB = ub_Data;
	LCD_delay_us(10);
	PIN_E = Disable;
}

T_UBYTE ReadData(void)
{
	T_UBYTE ub_Data;
	TRISB = Direction_INPUT;
	LCD_delay_us(10);
	PIN_RS = Enable;
	PIN_R = Enable;
	PIN_E = Enable;
	ub_Data = PORTB;
	LCD_delay_us(10);
	PIN_E = Disable;
	return ub_Data;
}

T_UBYTE IsBusy(void)
{
	T_UBYTE ub_Bussy = 0;
	TRISB = Direction_INPUT;
	LCD_delay_us(10);
	PIN_RS = Disable;
	PIN_R = Enable;
	PIN_E = Enable;
	ub_Bussy = PORTB;
	LCD_delay_us(10);
	PIN_E = Disable;
	LCD_delay_us(10);
	ub_Bussy = ((ub_Bussy) & 0x80 )>> 0x7;
	return ub_Bussy;
}

void LCD_delay_us(T_UBYTE delay)
{
	T_UBYTE index = 0;
	for (index = 0; index<delay; index++);
}



