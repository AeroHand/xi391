
#ifndef RTC_H
#define RTC_H

/*** register summary ***/
#define RTC_SECONDS             0
#define RTC_SECONDS_ALARM       1
#define RTC_MINUTES             2
#define RTC_MINUTES_ALARM       3
#define RTC_HOURS               4
#define RTC_HOURS_ALARM         5

#define RTC_ALARM_DONT_CARE    0xC0
 
#define RTC_DAY_OF_WEEK         6
#define RTC_DAY_OF_MONTH        7
#define RTC_MONTH               8
#define RTC_YEAR                9
 
#define RTC_REG_A               10
#define RTC_REG_B               11
#define RTC_REG_C               12
#define RTC_REG_D               13


/* Called to initialize RTC before using it. */
void rtc_init(void);

#endif
