#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include "../qrencode.h"

#define THREADS (10)

static pthread_t threads[THREADS];

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

static void *encode_ver1to10(void *arg)
{
	QRcode *code;
	int i;
	int version;
	static const char *data = "This is test.";

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

	return NULL;
}

static void prof_ver1to10(void)
{
	int i;

	timerStart("Version 1 - 10 (500 symbols for each)");
	for(i=0; i<THREADS; i++) {
		pthread_create(&threads[i], NULL, encode_ver1to10, NULL);
	}
	for(i=0; i<THREADS; i++) {
		pthread_join(threads[i], NULL);
	}
	timerStop();
}

static void *encode_ver31to40(void *arg)
{
	QRcode *code;
	int i;
	int version;
	static const char *data = "This is test.";

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

	return NULL;
}

static void prof_ver31to40(void)
{
	int i;

	timerStart("Version 31 - 40 (50 symbols for each)");
	for(i=0; i<THREADS; i++) {
		pthread_create(&threads[i], NULL, encode_ver31to40, NULL);
	}
	for(i=0; i<THREADS; i++) {
		pthread_join(threads[i], NULL);
	}
	timerStop();
}

int main()
{
	prof_ver1to10();
	prof_ver31to40();

	return 0;
}
