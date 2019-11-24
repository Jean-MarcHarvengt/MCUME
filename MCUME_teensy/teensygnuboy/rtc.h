

#ifndef __RTC_H__
#define __RTC_H__


struct rtc
{
	int batt;
	int sel;
	int latch;
	int d, h, m, s, t;
	int stop, carry;
	byte regs[8];
};

extern struct rtc rtc;

extern void rtc_tick();



#endif



