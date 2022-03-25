/**
 * @file lv_ex_conf.h
 * Configuration file for v7.4.0
 *
 */
/*
 * COPY THIS FILE AS lv_ex_conf.h
 */

#if 1 /*Set it to "1" to enable the content*/

#ifndef LV_EX_CONF_H
#define LV_EX_CONF_H


/*******************
 * GENERAL SETTING
 *******************/

/* Enable printf-ing data in demoes and examples */
#ifdef CONFIG_LV_EX_PRINTF
#define LV_EX_PRINTF       1
#else
#define LV_EX_PRINTF       0
#endif

#define LV_EX_KEYBOARD     0       /*Add PC keyboard support to some examples (`lv_drivers` repository is required)*/
#define LV_EX_MOUSEWHEEL   0       /*Add 'encoder' (mouse wheel) support to some examples (`lv_drivers` repository is required)*/

/*********************
 * DEMO USAGE
 *********************/

/*Show some widget*/
#ifdef CONFIG_LV_USE_DEMO_WIDGETS
#define LV_USE_DEMO_WIDGETS        1
#else
#define LV_USE_DEMO_WIDGETS        0
#endif

#if LV_USE_DEMO_WIDGETS
#ifdef CONFIG_LV_DEMO_WIDGETS_SLIDESHOW
#define LV_DEMO_WIDGETS_SLIDESHOW   1
#else
#define LV_DEMO_WIDGETS_SLIDESHOW   0
#endif
#endif

/*Printer demo, optimized for 800x480*/
#define LV_USE_DEMO_PRINTER     0

/*Demonstrate the usage of encoder and keyboard*/
#ifdef CONFIG_LV_USE_DEMO_KEYPAD_AND_ENCODER
#define LV_USE_DEMO_KEYPAD_AND_ENCODER  1
#else
#define LV_USE_DEMO_KEYPAD_AND_ENCODER  0
#endif

/*Benchmark your system*/
#ifdef CONFIG_LV_USE_DEMO_BENCHMARK
#define LV_USE_DEMO_BENCHMARK   1
#else
#define LV_USE_DEMO_BENCHMARK   0
#endif

/*Stress test for LVGL*/
#ifdef CONFIG_LV_USE_DEMO_STRESS
#define LV_USE_DEMO_STRESS  1 
#else
#define LV_USE_DEMO_STRESS  0 

#endif

#endif /*LV_EX_CONF_H*/

#endif /*End of "Content enable"*/

