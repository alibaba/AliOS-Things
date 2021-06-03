#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>
#include "../qrencode.h"

static struct timeval tv;
static void timerStart(const char *str)
{
	printf("%s: START\n", str);
	gettimeofday(&tv, NULL);
}

static void timerStop(void)
{
	struct timeval tc;

	gettimeofday(&tc, NULL);
	printf("STOP: %ld msec\n", (tc.tv_sec - tv.tv_sec) * 1000
			+ (tc.tv_usec - tv.tv_usec) / 1000);
}

static void prof_ver1to10(void)
{
	QRcode *code;
	int i;
	int version;
	static const char *data = "This is test.";

	timerStart("Version 1 - 10 (500 symbols for each)");
	for(i=0; i<500; i++) {
		for(version = 0; version < 11; version++) {
			code = QRcode_encodeString(data, version, QR_ECLEVEL_L, QR_MODE_8, 0);
			if(code == NULL) {
				perror("Failed to encode:");
			} else {
				QRcode_free(code);
			}
		}
	}
	timerStop();
}

static void prof_ver31to40(void)
{
	QRcode *code;
	int i;
	int version;
	static const char *data = "This is test.";

	timerStart("Version 31 - 40 (50 symbols for each)");
	for(i=0; i<50; i++) {
		for(version = 31; version < 41; version++) {
			code = QRcode_encodeString(data, version, QR_ECLEVEL_L, QR_MODE_8, 0);
			if(code == NULL) {
				perror("Failed to encode:");
			} else {
				QRcode_free(code);
			}
		}
	}
	timerStop();
}

int main()
{
	prof_ver1to10();
	prof_ver31to40();

	return 0;
}
