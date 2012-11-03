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
test_rtc_read_spam() {
	int number_of_successes = 0;
	while(number_of_successes < 3) {
		rtc_read();
		printf("rtc_read success.\n");
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

	/* MP3.2 Test rtc */
	printf("closing rtc (should pass)... ");
	score &= !test_rtc_close();
	printf("closing rtc again (should fail)... ");
	score &= (test_rtc_close() < 0);
	printf("opening rtc (should pass)... ");
	score &= !test_rtc_open();
	printf("opening rtc again (should fail)... ");
	score &= (test_rtc_open() < 0);
	printf("testing read on interrupts thrice (should pass):\n");
	score &= test_rtc_read_spam();
	printf("testing set freq 8hz (should pass)... ");
	score &= !test_rtc_write_freq(8);
	printf("testing set freq 2048hz (should fail)... ");
	score &= (test_rtc_write_freq(2048) < 0);
	printf("testing set freq 1023hz (should fail)... ");
	score &= (test_rtc_write_freq(1023) < 0);
	printf("testing read on interrupts thrice faster (should pass):\n");
	score &= test_rtc_read_spam();
	printf("testing set freq 2hz (should pass)... ");
	score &= !test_rtc_write_freq(2);

	return score;
}

