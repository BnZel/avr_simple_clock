#include <stdio.h>
#include <string.h>

#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include <libraries/uart.h>
#include <libraries/twi.h>
#include <libraries/rtc.h>
#include <libraries/MAX7219_Matrix.h>
#include <libraries/buffer.h>

#define MAX7219_IC_NUM 6
#define SIZE 9

//=====================START DS3231=====================
void read_rtc(void)
{
	if (rtc_is_ds3231())
	{
		uartSendString("DS3231\n");
		int8_t t;
		uint8_t f;
		ds3231_get_temp_int(&t, &f);
	}
	else
	{
		uartSendString("DS1307\n");
		uint8_t hour, min, sec;
		rtc_get_time_s(&hour, &min, &sec);
	}
}

void init_ds3231(uint8_t hour, uint8_t min, uint8_t sec)
{
	uartInit();

	// 38400 for hc-05 (default baud rate)
	// 9600 otherwise
	uartSetBaudRate(9600);

	twi_init_master();
	rtc_init();
	rtc_set_time_s(hour, min, sec);

	read_rtc();
}
//=====================END DS3231=====================

//=====================START MAX7219=====================
uint64_t pgm_read_64(void *ptr)
{
	uint64_t result;
	memcpy_P(&result, ptr, sizeof(uint64_t));
	return result;
}

const uint64_t symbol[] PROGMEM = {
	0x000406b2b21e0c00, /* 0: ? */ 
	0x001c3e7cf87c3e1c, /* 1: heart */
	0x00c0e07e02040000, /* 2: music note */
	0x00383838fe7c3810, /* 3: right arrow */
	0x0010387cfe383838, /* 4: left arrow */
	0x0000006e6e000000, /* 5: ! */
	0x0000006666000000, /* 6: : */
	0x0006090638444400, /* 7: degree celsius */
	0x105438ee38541000, /* 8: sun */
	0x003c4281bdc38100, /* 9: moon */
	0x006030180c060000, /* 10: / */
	0x0000000000000000 /*  11: (blank) */
};

const uint64_t numbers[] PROGMEM = {
	/* 8     =>      1   row */
	0x007cfe928afe7c00, /* 0 */
	0x008088fefe808000, /* 1 */
	0x00c4e6a2929e8c00, /* 2 */
	0x0044c69292fe6c00, /* 3 */
	0x00302824fefe2000, /* 4 */
	0x004ece8a8afa7200, /* 5 */
	0x007cfe9292f66400, /* 6 */
	0x000606e2fa1e0600, /* 7 */
	0x006cfe9292fe6c00, /* 8 */
	0x004cde9292fe7c00  /* 9 */
};
//=====================END MAX7219=====================

//=====================START MISC=====================
void check_day_night(struct tm *t, int tens_hour, int ones_hour, int am_pm, int day_or_night)
{
		// ensure clock is set to 12-hour format
		// once the hour hits 13 o'clock 
		// update the hour portion only
		// if daytime is set to false from the start
		// then set to true (viceversa) and display appropriate 
		// icons for day and night
		if(tens_hour == 1 && ones_hour == 2){

			if(t->am==true && am_pm==false){

				// night time
				t->am = false;
				day_or_night = 9;

				MAX7219_MatrixSetRow64(0, pgm_read_64((void *)&symbol[day_or_night]));
				MAX7219_MatrixUpdate();
			}
			else{
				// daytime
				t->am = true;
				am_pm = t->am;
				day_or_night = 8;

				MAX7219_MatrixSetRow64(0, pgm_read_64((void *)&symbol[day_or_night]));
				MAX7219_MatrixUpdate();
			}
		}
		if(tens_hour == 1 && ones_hour == 3){

			tens_hour = 0;
			ones_hour = 1;

			rtc_set_time_s(ones_hour, t->min, t->sec);
		}
}
//=====================END MISC=====================
int main(void)
{
	// initialize variables
	struct tm *t = NULL;

	// am and pm indicator
	// to switch icon display 
	// sun (8) and moon (9)
	int am_pm;
	t->am = false;
	am_pm = t->am;
	int day_or_night = 9;

	// start display
	// when setting time
	// make sure when you set it
	// hence night_time (or day_time)
	init_ds3231(4, 20, 34);
	MAX7219_MatrixInit();
	MAX7219_MatrixSetRow64(0, pgm_read_64((void *)&symbol[day_or_night]));
	MAX7219_MatrixUpdate();	
	
	// rx buffer
	char rx_buf[SIZE];
	char data;
	int i,j = 0;

	while (1)
	{
		t = rtc_get_time();

		// separated hour and min and secs
		// into individual digits
		// to display into the matrix
		int ones_hour = (t->hour % 10); 
		int tens_hour = ((t->hour / 10) % 10); 

		int ones_min = (t->min % 10);
		int tens_min = ((t->min / 10) % 10);

		int ones_sec = (t->sec % 10);
		int tens_sec = ((t->sec / 10) % 10);
		
		check_day_night(t,tens_hour,ones_hour,am_pm,day_or_night);

		// set matrix clock display
		MAX7219_MatrixSetRow64(1, pgm_read_64((void *)&numbers[ones_min]));
		MAX7219_MatrixSetRow64(2, pgm_read_64((void *)&numbers[tens_min]));
		MAX7219_MatrixSetRow64(3, pgm_read_64((void *)&symbol[6]));
		MAX7219_MatrixSetRow64(4, pgm_read_64((void *)&numbers[ones_hour]));
		MAX7219_MatrixSetRow64(5, pgm_read_64((void *)&numbers[tens_hour]));
		MAX7219_MatrixUpdate();

		_delay_ms(500);

		// unable to properly receive spliced settings from user
		// on HC-05 BLE module
		// suppose to receive data in format of HH:MM:SS:AMPM
		// to remotely change time instead of reprogramming manually
		while(uartReceiveByte(&data)){

			i = data;
			rx_buf[j] = i;

			i++,j++;

			if(uartReceiveBufferIsEmpty()) { 

				i,j = 0; 

				char s[2] = ":";
				char *token;
				token = strtok(rx_buf, s);
				
				while(*token)
				{					
					strcpy((char *)&rx_buf[i++], token);
					token = strtok(NULL, s);
				}

				uartFlushReceiveBuffer();
				uartSendString("\nfinished...");
				uartSendString("\n");
						
			}
		}

	}
}
