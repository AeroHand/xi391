/* tests.c - test cases for components of the kernel
 */

#include "lib.h"
#include "rtc.h"

int
test_dereference_null_ptr() {
	int * ptr = 0x0000000;
	int deref_ptr = *ptr;
	return 1;
}

int
test_dereference_nonnull_ptr() {
	int * ptr = 0x8000000;
	int deref_ptr = *ptr;
	return 1;
}

int
test_divide_by_zero() {
	int undefined = 1/0;
	return 1;
}

int test_rtc_close() {
	int retval = rtc_close();
	if (retval == 0) {
		printf("rtc_close successful\n");
	} else {
		printf("rtc_close failed\n");
	}
	return retval;
}

int
test_rtc_open() {
	int retval = rtc_open();
	if (retval == 0) {
		printf("rtc_open successful\n");
	} else {
		printf("rtc_open failed\n");
	}
	return retval;
}

int
test_rtc_read_spam(int n) {
	int number_of_successes = 0;
	while(number_of_successes < n) {
		rtc_read();
		// printf("rtc_read success.\n");
		test_interrupts();
		number_of_successes++;
	}
	return 1;
}

int
test_rtc_write_freq(int32_t nbytes) {
	int retval = rtc_write(nbytes);
	if (retval == 0) {
		printf("rtc_write_freq successful\n");
	} else {
		printf("rtc_write_freq failed\n");
	}
	return retval;
}

int
test() {
	printf("post-kernel-init tests:\n");
	int score = 1;

	/* MP3.1 Tests */
	// score &= test_dereference_null_ptr();
	// score &= test_dereference_nonnull_ptr();
	// score &= test_divide_by_zero();

	/* MP3.2 Test RTC */
	/** RTC Open/close **/
	printf("closing rtc (should pass)... ");
	score &= !test_rtc_close();
	printf("closing rtc again (should fail)... ");
	score &= (test_rtc_close() < 0);
	printf("opening rtc (should pass)... ");
	score &= !test_rtc_open();
	printf("opening rtc again (should fail)... ");
	score &= (test_rtc_open() < 0);
	/** RTC Read **/
	printf("testing read on interrupts thrice (should pass):\n");
	score &= test_rtc_read_spam(10);
	/** RTC Write **/
	printf("testing set freq 4hz (should pass)... ");
	score &= !test_rtc_write_freq(4);
	score &= test_rtc_read_spam(20);
	printf("testing set freq 8hz (should fail)... ");
	score &= !test_rtc_write_freq(8);
	score &= test_rtc_read_spam(40);
	printf("testing set freq 16hz (should pass)... ");
	score &= !test_rtc_write_freq(16);
	score &= test_rtc_read_spam(80);
	printf("testing set freq 32hz (should fail)... ");
	score &= !test_rtc_write_freq(32);
	score &= test_rtc_read_spam(160);
	printf("testing set freq 64hz (should pass)... ");
	score &= !test_rtc_write_freq(64);
	score &= test_rtc_read_spam(320);
	printf("testing set freq 128hz (should fail)... ");
	score &= !test_rtc_write_freq(128);
	score &= test_rtc_read_spam(640);
	printf("testing set freq 256hz (should pass)... ");
	score &= !test_rtc_write_freq(256);
	score &= test_rtc_read_spam(1280);
	printf("testing set freq 512hz (should fail)... ");
	score &= !test_rtc_write_freq(512);
	score &= test_rtc_read_spam(2560);
	printf("testing set freq 1024hz (should pass)... ");
	score &= !test_rtc_write_freq(1024);
	score &= test_rtc_read_spam(5120);
	printf("testing set freq 2048hz (should fail)... ");
	score &= (test_rtc_write_freq(2048) < 0);
	printf("testing set freq 1023hz (should fail)... ");
	score &= (test_rtc_write_freq(1023) < 0);
	printf("testing set freq 1025hz (should fail)... ");
	score &= (test_rtc_write_freq(1025) < 0);
	printf("testing set freq back to 2hz (should pass)... ");
	score &= !test_rtc_write_freq(2);

	printf("testing read on interrupts thrice faster (should pass):\n");
	score &= test_rtc_read_spam(10);


	
	return score;
}

