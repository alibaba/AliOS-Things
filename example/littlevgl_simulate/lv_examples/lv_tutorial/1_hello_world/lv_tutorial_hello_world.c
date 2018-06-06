/**
 * @file lv_tutorial_hello_world
 *
 */

/*
 *-------------------------------------------------------------------------------
 * Create your first object: a "Hello world" label
 * ------------------------------------------------------------------------------
 *
 * If you have ported the LittlevGL you are ready to create content on your display.
 *
 * Now you will create a "Hello world!" label and align it to the middle.
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_tutorial_hello_world.h"
#if USE_LV_TUTORIALS

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
 * Create a simple 'Hello world!' label
 */
void lv_tutorial_hello_world(void)
{
    /*Create a Label on the currently active screen*/
    lv_obj_t * label1 =  lv_label_create(lv_scr_act(), NULL);

    /*Modify the Label's text*/
    lv_label_set_text(label1, "Hello world!");

    /* Align the Label to the center
     * NULL means align on parent (which is the screen now)
     * 0, 0 at the end means an x, y offset after alignment*/
    lv_obj_align(label1, NULL, LV_ALIGN_CENTER, 0, 0);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*USE_LV_TUTORIALS*/
