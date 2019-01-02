/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <string.h>
#include <clock_gettime.h>
#include <k_api.h>

void cmd_date(char *pwbuf, int blen, int argc, char **argv) 
{
	struct tm *gmt;
	time_t t;
	
	t = time(NULL);
	gmt = gmtime(&t);

	printf("%d年%02d月%02d日",(1900+gmt->tm_year),(1+gmt->tm_mon),gmt->tm_mday);
	printf(" %02d:%02d:%02d\n",(gmt->tm_hour),gmt->tm_min,gmt->tm_sec);

}
