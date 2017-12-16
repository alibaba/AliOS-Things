/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <k_config.h>
#include "stdio.h"

#define APB_DEFAULT_FREQ    20000000

void show_times_hdr(void)
{
    printf("\r\nFreeRTOS Function RealTime Test\r\n");
    printf("\r\nTesting Parameters:\r\n");
    printf("   TICK_RATE_HZ :         %dHZ\r\n",
           (unsigned long) RHINO_CONFIG_TICKS_PER_SECOND);
    printf("   CLOCK_SAMPLE :         %dHZ\r\n", (unsigned long) APB_DEFAULT_FREQ);

    printf("                                                             Confidence\r\n");
    printf(" Ave              Min             Max             Var         Ave Min        Function      SampleNum \r\n");
    printf("=====            =====           =====           =====        =======        ========      ========= \r\n");
}


void show_time_in_us(unsigned long  result)                 //ns to us
{
    printf("%03d.%03dus\t", (int)(result / 1000), (int)(result % 1000));
}

void show_times_detail(volatile double   *ft,  int nsamples,  char *title,
                       uint32_t ignore_first)
{
    int i;
    double  delta = 0.0, min = 0.0, max = 0.0, ave_dev = 0.0 , ave = 0.0 ;
    unsigned int start_sample = 0, total_samples = 0, con_ave = 0, con_min = 0,
                 error = 0;
    double   total = 0.0;

    if (ignore_first) {
        start_sample = 1;
        total_samples = nsamples - 1;
    } else {
        start_sample = 0;
        total_samples = nsamples;
    }

    for (i = start_sample;  i < nsamples;  i++) {
        if (ft[i] <= 0) {
            ft[i] = 0;
            error ++;
        }

        total += ft[i];
    }

    ave = total / (float)(total_samples - error);
    total = 0.0;
    min = 0xffffffff;
    max = 0.0;

    for (i = start_sample;  i < nsamples;  i++) {
        if (ft[i] > (10 * ave)) {
            ft[i] = 0;
            error ++;
        }

        delta = ft[i];

        if (delta != 0) {
            if (delta < min) {
                min = delta;
            }

            if (delta > max) {
                max = delta;
            }
        }

        total += delta;
    }

    ave = total / (float)(total_samples - error);

    total = 0.0;
    ave_dev = 0.0;

    for (i = start_sample;  i < nsamples;  i++) {
        delta  = ft[i];

        if (delta != 0) {
            delta = delta - ave;

            if (delta < 0) {
                delta = -delta;
            }

            ave_dev += delta;
        }


    }

    ave_dev /= (float)(total_samples - error);

    con_ave = 0;
    con_min = 0;

    for (i = start_sample;  i < nsamples;  i++) {

        delta  = ft[i];

        if (delta != 0) {
            if ((delta <= (ave + ave_dev)) && (delta >= (ave - ave_dev))) {
                con_ave++;
            }

            if ((delta <= (min + ave_dev)) && (delta >= (min - ave_dev))) {
                con_min++;
            }
        }
    }

    con_ave = (con_ave * 100) / total_samples;
    con_min = (con_min * 100) / total_samples;
    show_time_in_us((unsigned long) ave);
    show_time_in_us((unsigned long) min);
    show_time_in_us((unsigned long) max);
    show_time_in_us((unsigned long) ave_dev);
    printf("  %d%%, %d%%\t", con_ave, con_min);
    printf(title);
    printf("  %d\r\n", total_samples);

}


void show_times_detail_2(volatile  unsigned long  ft[], int nsamples,
                         char *title, uint32_t  ignore_first)
{
    int i;

    long long  delta, min, max, con_ave, con_min, ave_dev;
    int start_sample, total_samples;
    long long  total, ave;

    if (ignore_first) {
        start_sample = 1;
        total_samples = nsamples - 1;
    } else {
        start_sample = 0;
        total_samples = nsamples;
    }

    total = 0;
    min = 0x7fffffff;
    max = 0;

    for (i = start_sample;  i < nsamples;  i++) {
        delta = ft[i];
        delta *= 1000;
        total += delta;

        if (delta < min) {
            min = delta;
        }

        if (delta > max) {
            max = delta;
        }
    }

    ave = total / total_samples;
    total = 0;
    ave_dev = 0;

    for (i = start_sample;  i < nsamples;  i++) {
        delta = ft[i];
        delta *= 1000;
        delta = delta - ave;

        if (delta < 0) {
            delta = -delta;
        }

        ave_dev += delta;
    }

    ave_dev /= total_samples;
    con_ave = 0;
    con_min = 0;

    for (i = start_sample;  i < nsamples;  i++) {
        delta = ft[i];
        delta *= 1000;

        if ((delta <= (ave + ave_dev)) && (delta >= (ave - ave_dev))) {
            con_ave++;
        }

        if ((delta <= (min + ave_dev)) && (delta >= (min - ave_dev))) {
            con_min++;
        }
    }

    con_ave = (con_ave * 100) / total_samples;
    con_min = (con_min * 100) / total_samples;
    show_time_in_us(ave);
    show_time_in_us(min);
    show_time_in_us(max);
    show_time_in_us(ave_dev);
    printf("  %lld%% %lld%%", con_ave, con_min);
    printf(" %s\n", title);
}


void
end_of_test_group(void)
{
    printf("\n");
}


