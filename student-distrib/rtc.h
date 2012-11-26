
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

/*** io constants ***/
#define RTC_PORT		0x70
#define CMOS_PORT		0x71
#define INDEX_REGISTER_A	0x8A
#define	INDEX_REGISTER_B	0x8B
#define	INDEX_REGISTER_C	0x8C
#define	INDEX_REGISTER_D	0x8D

/*** useful init masks ***/
#define KILL_DV_RS		0x80 
#define KILL_RS			0xF0 
#define DV_RS			0x2F
#define KILL_SET_PIE_AIE_UIE	0x0F 
#define SET_PIE_AIE_UIE		0x40 

/*** frequency constants ***/
#define HZ0			0x00
#define HZ2			0x0F
#define HZ4			0x0E
#define HZ8			0x0D
#define HZ16			0x0C
#define HZ32			0x0B
#define HZ64			0x0A
#define HZ128			0x09
#define HZ256			0x08
#define HZ512			0x07
#define HZ1024			0x06

/*** irq constant ***/
#define RTC_IRQ			8

/* Clock Interrupt */
void clock_interruption(void); 

/* System Call: read */
int32_t rtc_read (uint32_t a, int32_t b, int32_t c, int32_t d);
/* System Call: write */
int32_t rtc_write (int32_t * buf, int32_t nbytes);
/* System Call: open */
int32_t rtc_open (void);
/* System Call: close */
int32_t rtc_close (void);

#endif

