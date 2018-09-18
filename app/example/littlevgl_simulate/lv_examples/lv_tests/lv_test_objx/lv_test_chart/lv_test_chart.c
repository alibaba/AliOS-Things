/**
 * @file lv_test_chart.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_test_chart.h"

#if USE_LV_BTN && USE_LV_TESTS

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create charts to test their functionalities
 */
void lv_test_chart_1(void)
{
    /* Create a default object*/
    lv_obj_t * chart1 = lv_chart_create(lv_scr_act(), NULL);

    lv_chart_series_t * dl1_1 = lv_chart_add_series(chart1, LV_COLOR_RED);
    dl1_1->points[0] = 0;
    dl1_1->points[1] = 25;
    dl1_1->points[2] = 0;
    dl1_1->points[3] = 50;
    dl1_1->points[4] = 0;
    dl1_1->points[5] = 75;
    dl1_1->points[6] = 0;
    dl1_1->points[7] = 100;
    dl1_1->points[8] = 0;

    lv_chart_series_t * dl1_2 = lv_chart_add_series(chart1, LV_COLOR_BLUE);
    dl1_2->points[0] = 100;

    lv_chart_refresh(chart1);


    /* Create a chart with the same data and modify all appearance-like attributes
     * also modify the number of points, range, and type*/
    lv_obj_t * chart2 = lv_chart_create(lv_scr_act(), NULL);
    lv_obj_set_size(chart2, 140, 100);
    lv_obj_align(chart2, chart1, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
    lv_chart_set_series_darking(chart2, LV_OPA_90);
    lv_chart_set_series_opa(chart2, LV_OPA_40);
    lv_chart_set_series_width(chart2, 4);
    lv_chart_set_type(chart2, LV_CHART_TYPE_POINT | LV_CHART_TYPE_LINE);
    lv_chart_set_range(chart2, -20, 120);
    lv_chart_set_div_line_count(chart2, 4, 0);

    lv_chart_series_t * dl2_1 = lv_chart_add_series(chart2, LV_COLOR_RED);
    dl2_1->points[0] = 0;
    dl2_1->points[1] = 25;
    dl2_1->points[2] = 0;
    dl2_1->points[3] = 50;
    dl2_1->points[4] = 0;
    dl2_1->points[5] = 75;
    dl2_1->points[6] = 0;
    dl2_1->points[7] = 100;
    dl2_1->points[8] = 0;

    lv_chart_series_t * dl2_2 = lv_chart_add_series(chart2, LV_COLOR_BLUE);
    dl2_2->points[0] = 100;

    lv_chart_refresh(chart2);

    lv_chart_set_point_count(chart2, 15);


    /*Copy the previous chart, set COLUMN type and test lv_chart_set_next()*/
    lv_obj_t * chart3 = lv_chart_create(lv_scr_act(), chart2);
    lv_obj_align(chart3, chart2, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
    lv_chart_set_type(chart3, LV_CHART_TYPE_COLUMN);
    lv_chart_series_t * dl3_1 = lv_chart_add_series(chart3, LV_COLOR_RED);
    dl3_1->points[0] = 0;
    dl3_1->points[1] = 25;
    dl3_1->points[2] = 0;
    dl3_1->points[3] = 50;
    dl3_1->points[4] = 0;
    dl3_1->points[5] = 75;
    dl3_1->points[6] = 0;
    dl3_1->points[7] = 100;
    dl3_1->points[8] = 0;

    lv_chart_series_t * dl3_2 = lv_chart_add_series(chart3, LV_COLOR_BLUE);
    dl3_2->points[0] = 100;

    lv_chart_refresh(chart2);

    lv_chart_set_next(chart3, dl3_2, 110);
    lv_chart_set_next(chart3, dl3_2, 110);
    lv_chart_set_next(chart3, dl3_2, 110);
    lv_chart_set_next(chart3, dl3_2, 110);

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*USE_LV_BTN && USE_LV_TESTS*/
