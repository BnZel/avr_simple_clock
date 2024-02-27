#include <stdio.h>

#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#include <libraries/uart.h>
#include <libraries/twi.h>
#include <libraries/rtc.h>
#include <libraries/MAX7219_Matrix.h>
#include <libraries/buffer.h>

#define MAX7219_IC_NUM 6

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
	// original
	// 0x1800183860663c00, /* 0: ? */
	// 0x10387cfefeee4400, /* 1: heart */
	// 0x060e0c0808281800, /* 2: music note */
	// 0x10307efe7e301000, /* 3: right arrow */
	// 0x1018fcfefc181000, /* 4: left arrow */
	// 0x0018180018181800, /* 5: ! */
	// 0x0018180000181800, /* 6: : */
	// 0x00601010146a0a04, /* 7: degree celsius */
	// 0x105438ee38541000, /* 8: sun */
	// 0x7824121212122478, /* 9: moon */
	// 0x0002060c18302000	/* 10: / */

	// rotated
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
		// rotated
	0x007cfe928afe7c00, /* 0 */
	0x008088fefe808000, /* 1 */
	0x00c4e6a2929e8c00, /* 2 */
	0x0044c69292fe6c00, /* 3 */
	0x00302824fefe2000, /* 4 */
	0x004ece8a8afa7200, /* 5 */
	0x007cfe9292f66400, /* 6 */
	0x000606e2fa1e0600, /* 7 */
	0x006cfe9292fe6c00, /* 8 */
	0x004cde9292fe7c00,  /* 9 */
	// 0x007e003c524a3c00, /* 10 */
	// 0x047e7e00047e7e00, /* 11 */
	// 0x047e7e0066525e00  /* 12 */
	
	 
	// original
	// 0x3c66666e76663c00, /* 0 */ 
	// 0x7e1818181c181800, /* 1 */
	// 0x7e060c3060663c00, /* 2 */
	// 0x3c66603860663c00, /* 3 */
	// 0x30307e3234383000, /* 4 */
	// 0x3c6660603e067e00, /* 5 */
	// 0x3c66663e06663c00, /* 6 */
	// 0x1818183030667e00, /* 7 */
	// 0x3c66663c66663c00, /* 8 */
	// 0x3c66607c66663c00, /* 9 */
	// 0x00324a5a6a4a3200, /* 10 */
	// 0x0066666666776600, /* 11 */
	// 0x0076166646577600 /* 12 */
};

const uint64_t Atoz[] PROGMEM = {
	0x6666667e66663c00, /* 0: A */
	0x3e66663e66663e00, /* 1: B */
	0x3c66060606663c00, /* 2: C */
	0x3e66666666663e00, /* 3: D */
	0x7e06063e06067e00, /* 4: E */
	0x0606063e06067e00, /* 5: F */
	0x3c66760606663c00, /* 6: G */
	0x6666667e66666600, /* 7: H */
	0x3c18181818183c00, /* 8: I */
	0x1c36363030307800, /* 9: J */
	0x66361e0e1e366600, /* 10: K */
	0x7e06060606060600, /* 11: L */
	0xc6c6c6d6feeec600, /* 12: M */
	0xc6c6e6f6decec600, /* 13: N */
	0x3c66666666663c00, /* 14: O */
	0x06063e6666663e00, /* 15: P */
	0x603c766666663c00, /* 16: Q */
	0x66361e3e66663e00, /* 17: R */
	0x3c66603c06663c00, /* 18: S */
	0x18181818185a7e00, /* 19: T */
	0x7c66666666666600, /* 20: U */
	0x183c666666666600, /* 21: V */
	0xc6eefed6c6c6c600, /* 22: W */
	0xc6c66c386cc6c600, /* 23: X */
	0x1818183c66666600, /* 24: Y */
	0x7e060c1830607e00, /* 25: Z */
	0x0000000000000000, /* 26: (space) */

	// 0x7c667c603c000000, /* 27: a */
	// 0x3e66663e06060600, /* 28: b */
	// 0x3c6606663c000000, /* 29: c */
	// 0x7c66667c60606000, /* 30: d */
	// 0x3c067e663c000000, /* 31: e */
	// 0x0c0c3e0c0c6c3800, /* 32: f */
	// 0x3c607c66667c0000, /* 33: g */
	// 0x6666663e06060600, /* 34: h */
	// 0x3c18181800180000, /* 35: i */
	// 0x1c36363030003000, /* 36: j */
	// 0x66361e3666060600, /* 37: k */
	// 0x1818181818181800, /* 38: l */
	// 0xd6d6feeec6000000, /* 39: m */
	// 0x6666667e3e000000, /* 40: n */
	// 0x3c6666663c000000, /* 41: o */
	// 0x06063e66663e0000, /* 42: p */
	// 0xf0b03c36363c0000, /* 43: q */
	// 0x060666663e000000, /* 44: r */
	// 0x3e403c027c000000, /* 45: s */
	// 0x1818187e18180000, /* 46: t */
	// 0x7c66666666000000, /* 47: u */
	// 0x183c666600000000, /* 48: v */
	// 0x7cd6d6d6c6000000, /* 49: w */
	// 0x663c183c66000000, /* 50: x */
	// 0x3c607c6666000000, /* 51: y */
	// 0x3c0c18303c000000	/* 52: z */

	// rotated
	// 0x00fcfe1212fefc00, /* A */
	// 0x00fefe8282fe7c00, /* D */
	// 0x00fefe9292928200, /* E */
	// 0x00fefe1212120200, /* F */
	// 0x00fefe0c180cfefe, /* M */
	// 0x00fefe0c1830fefe, /* N */
	// 0x007cfe8282fe7c00, /* O */
	// 0x004cde9292f66400, /* S */
	// 0x000602fefe020600, /* T */
	// 0x007efe8080fefe00, /* U */
	// 0x00fefe603060fefe, /* W */
	// 0x000e1ef0f01e0e00 /* Y */
};

// custom
const uint64_t IMAGES[] PROGMEM = {
	// replace data here
	// 0xff000000000000ff,
	// 0xff000001010000ff,
	// 0xff000003030000ff,
	// 0xff000006060000ff,
	// 0xff00000c0c0000ff,
	// 0xff000018180000ff,
	// 0xff000030300000ff,
	// 0xff000060600000ff,
	// 0xff0000c0c00000ff,
	// 0xff000080800000ff,
	// 0xff000000000000ff,

	0x8181818181818181,
	0x8181818181818199,
	0x8181818181819999,
	0x8181818181999981,
	0x8181818199998181,
	0x8181819999818181,
	0x8181999981818181,
	0x8199998181818181,
	0x9999818181818181,
	0x9981818181818181,
	0x8181818181818181};

void sample_test(void)
{
	// question marks
	MAX7219_MatrixSetRow64(0, pgm_read_64((void *)&symbol[0]));
	MAX7219_MatrixSetRow64(1, pgm_read_64((void *)&symbol[0]));
	MAX7219_MatrixSetRow64(2, pgm_read_64((void *)&symbol[0]));
	MAX7219_MatrixSetRow64(3, pgm_read_64((void *)&symbol[0]));
	MAX7219_MatrixSetRow64(4, pgm_read_64((void *)&symbol[0]));
	MAX7219_MatrixSetRow64(5, pgm_read_64((void *)&symbol[0]));
	MAX7219_MatrixUpdate();
	_delay_ms(3000);

	// example time
	MAX7219_MatrixSetRow64(0, pgm_read_64((void *)&numbers[4]));
	MAX7219_MatrixSetRow64(1, pgm_read_64((void *)&symbol[6]));
	MAX7219_MatrixSetRow64(2, pgm_read_64((void *)&numbers[2]));
	MAX7219_MatrixSetRow64(3, pgm_read_64((void *)&numbers[0]));
	MAX7219_MatrixSetRow64(4, pgm_read_64((void *)&Atoz[15]));
	MAX7219_MatrixSetRow64(5, pgm_read_64((void *)&Atoz[12]));
	MAX7219_MatrixUpdate();

	_delay_ms(2000);
	MAX7219_MatrixSetRow64(4, pgm_read_64((void *)&Atoz[0]));
	MAX7219_MatrixUpdate();
	_delay_ms(3000);

	// example temperature
	MAX7219_MatrixSetRow64(0, pgm_read_64((void *)&numbers[2]));
	MAX7219_MatrixSetRow64(1, pgm_read_64((void *)&numbers[5]));
	MAX7219_MatrixSetRow64(2, pgm_read_64((void *)&Atoz[26]));
	MAX7219_MatrixSetRow64(3, pgm_read_64((void *)&symbol[7]));
	MAX7219_MatrixSetRow64(4, pgm_read_64((void *)&Atoz[26]));
	MAX7219_MatrixSetRow64(5, pgm_read_64((void *)&symbol[8]));
	MAX7219_MatrixUpdate();

	_delay_ms(2000);
	MAX7219_MatrixSetRow64(5, pgm_read_64((void *)&symbol[9]));
	MAX7219_MatrixUpdate();
	_delay_ms(3000);

	// example date
	// MAX7219_MatrixSetRow64(0, pgm_read_64((void *)&numbers[1]));
	// MAX7219_MatrixSetRow64(1, pgm_read_64((void *)&symbol[10]));
	// MAX7219_MatrixSetRow64(2, pgm_read_64((void *)&numbers[1]));
	// MAX7219_MatrixSetRow64(3, pgm_read_64((void *)&numbers[8]));
	// MAX7219_MatrixSetRow64(4, pgm_read_64((void *)&symbol[10]));
	// MAX7219_MatrixSetRow64(5, pgm_read_64((void *)&numbers[24]));
	// MAX7219_MatrixUpdate();
	// _delay_ms(3000);

	// example text
	MAX7219_MatrixSetRow64(0, pgm_read_64((void *)&Atoz[8]));
	MAX7219_MatrixUpdate();
	_delay_ms(1000);

	MAX7219_MatrixSetRow64(1, pgm_read_64((void *)&symbol[1]));
	MAX7219_MatrixUpdate();
	_delay_ms(1000);

	MAX7219_MatrixSetRow64(2, pgm_read_64((void *)&Atoz[24]));
	MAX7219_MatrixUpdate();
	_delay_ms(1000);

	MAX7219_MatrixSetRow64(3, pgm_read_64((void *)&Atoz[14]));
	MAX7219_MatrixUpdate();
	_delay_ms(1000);

	MAX7219_MatrixSetRow64(4, pgm_read_64((void *)&Atoz[20]));
	MAX7219_MatrixUpdate();
	_delay_ms(1000);

	MAX7219_MatrixSetRow64(5, pgm_read_64((void *)&symbol[5]));
	MAX7219_MatrixUpdate();
	_delay_ms(1000);

	// shift left of previous text
	//			   duration
	for (int i = 0; i < 24; i++)
	{
		MAX7219_MatrixLShift(1);
		MAX7219_MatrixUpdate();
		_delay_ms(100);
	}

	// right shift previous text
	for (int i = 0; i < 24; i++)
	{
		MAX7219_MatrixRShift(2);
		MAX7219_MatrixUpdate();
		_delay_ms(100);
	}

	// loop through symbols
	for (int i = 0; i < MAX7219_IC_NUM; i++)
	{
		MAX7219_MatrixSetRow64(i, pgm_read_64((void *)&symbol[i]));
		MAX7219_MatrixUpdate();
		_delay_ms(1000);
	}

	// rotated symbols
	int j = 6;
	for (int i = 0; i < MAX7219_IC_NUM; i++)
	{
		MAX7219_MatrixSetRow64(i, pgm_read_64((void *)&symbol[j]));
		MAX7219_MatrixUpdate();
		_delay_ms(1000);
		j += 1;
	}

	// loop through numbers
	for (int i = 0; i < MAX7219_IC_NUM; i++)
	{
		MAX7219_MatrixSetRow64(i, pgm_read_64((void *)&numbers[i]));
		MAX7219_MatrixUpdate();
		_delay_ms(1000);
	}

	// loop through letters
	for (int i = 0; i < MAX7219_IC_NUM; i++)
	{
		MAX7219_MatrixSetRow64(i, pgm_read_64((void *)&Atoz[i]));
		MAX7219_MatrixUpdate();
		_delay_ms(1000);
	}

	// scrolling vertical pixel
	// draw borders first
	for (int i = 0; i < MAX7219_IC_NUM; i++)
	{
		MAX7219_MatrixSetRow64(i, pgm_read_64((void *)&IMAGES[0]));
	}

	// example animation
	// downward
	for (int i = 0; i < (sizeof(IMAGES) / sizeof(uint64_t)); i++)
	{
		for (int j = 0; j < (sizeof(IMAGES) / sizeof(uint64_t)); j++)
		{
			MAX7219_MatrixSetRow64(i, pgm_read_64((void *)&IMAGES[j]));
			MAX7219_MatrixUpdate();
			_delay_ms(20);
		}
	}

	// upward
	for (int i = 6; i < (sizeof(IMAGES) / sizeof(uint64_t)); i--)
	{
		for (int j = 6; j < (sizeof(IMAGES) / sizeof(uint64_t)); j--)
		{
			MAX7219_MatrixSetRow64(i, pgm_read_64((void *)&IMAGES[j]));
			MAX7219_MatrixUpdate();
			_delay_ms(20);
		}
	}

	_delay_ms(2000);
}
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
		if(tens_hour == 1 && ones_hour == 3){

			tens_hour = 0;
			ones_hour = 1;

			rtc_set_time_s(ones_hour, t->min, t->sec);

			// daytime
			t->am = true;
			am_pm = t->am;
			day_or_night = 8;

			MAX7219_MatrixSetRow64(0, pgm_read_64((void *)&symbol[day_or_night]));
			MAX7219_MatrixUpdate();
		}
		else if(t->am==true && am_pm==false){

			// night time
			t->am = false;
			day_or_night = 9;

			MAX7219_MatrixSetRow64(0, pgm_read_64((void *)&symbol[day_or_night]));
			MAX7219_MatrixUpdate();
		}
}

//=====================END MISC=====================


// TODO:
// 		- properly setup hc-05 ble (send commands over to mcu to display to matrix board)
// 			- set datetime to rtc 
// 		- research for other ds3231 rtc avr c libraries (for temp reading and proper datetime settings)
//			- or don't, can use this for now and solder an isp header whenever you want to reprogram it
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

	// for uart debugging
	// output to hc-05 ble
	// int o_hour, o_min, t_hour, t_min, secs;
	// char clock[6] = {o_hour, t_hour, o_min, t_min, secs, t->am};

	// start display
	// when setting time
	// make sure when you set it
	// hence night_time (or day_time)
	init_ds3231(12, 59, 57);
	MAX7219_MatrixInit();
	MAX7219_MatrixSetRow64(0, pgm_read_64((void *)&symbol[day_or_night]));
	MAX7219_MatrixUpdate();
	
	// rx buffer
	char data;
	int i = 0;

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

		// uart output
		// to debug if needed
		// clock[0] = ones_hour;
		// clock[1] = tens_hour;
		// clock[2] = ones_min;
		// clock[3] = tens_min;
		// clock[4] = t->sec;
		// clock[5] = t->am;

		//		hour    min   sec
		// sprintf(clock, "%d %d : %d %d : %d AM=%d\n", clock[1], clock[0], clock[3], clock[2], clock[4], clock[5]);
		// uartSendString(clock);
		// uartSendString("\n---\n");
		
		check_day_night(t,tens_hour,ones_hour,am_pm,day_or_night);

		// set matrix clock display
		MAX7219_MatrixSetRow64(1, pgm_read_64((void *)&numbers[ones_min]));
		MAX7219_MatrixSetRow64(2, pgm_read_64((void *)&numbers[tens_min]));
		MAX7219_MatrixSetRow64(3, pgm_read_64((void *)&symbol[6]));
		MAX7219_MatrixSetRow64(4, pgm_read_64((void *)&numbers[ones_hour]));
		MAX7219_MatrixSetRow64(5, pgm_read_64((void *)&numbers[tens_hour]));
		MAX7219_MatrixUpdate();

		_delay_ms(500);

		if(uartReceiveByte(&data) == true){
			uartSendString(&data);
			uartSendString("\n");

			i++;

			if(uartReceiveBufferIsEmpty() == true) { 
				uartFlushReceiveBuffer();
				uartSendString("finished...");
				uartSendString("\n");
				
				i = 0; 
			}
		}

	}
}