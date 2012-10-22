
#ifndef RTC_H
#define RTC_H

/*** register summary ***/
#define RTC_SECONDS             0
#define RTC_SECONDS_ALARM       1
#define RTC_MINUTES             2
#define RTC_MINUTES_ALARM       3
#define RTC_HOURS               4
#define RTC_HOURS_ALARM         5
 
#define RTC_DAY_OF_WEEK         6
#define RTC_DAY_OF_MONTH        7
#define RTC_MONTH               8
#define RTC_YEAR                9
 
#define RTC_REG_A               10
#define RTC_REG_B               11
#define RTC_REG_C               12
#define RTC_REG_D               13

#define RTC_PORT		0x70
#define CMOS_PORT		0x71
#define INDEX_REGISTER_A	0x0A
#define	INDEX_REGISTER_B	0x0B
#define	INDEX_REGISTER_C	0x0C
#define ENABLE_PIE		0x30
#define SET_FREQ_1028_HZ	0x26

#define RTC_IRQ			8

/* Called to initialize RTC before using it. */
void rtc_init(void);

#endif
