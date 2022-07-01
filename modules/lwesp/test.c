/*
 * =====================================================================================
 *
 *       Filename:  aa.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/10/2022 02:57:28 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include "lwesp/lwesp.h"
size_t i, apf;
lwesp_ap_t aps[100];

void test_lwesp(void)
{
    lwesp_set_wifi_mode(1, NULL, NULL, 1);
    if (lwesp_sta_list_ap(NULL, aps, LWESP_ARRAYSIZE(aps), &apf, NULL, NULL, 1) == lwespOK) {
        for (i = 0; i < apf; i++) {
            printf("AP found: %s\r\n", aps[i].ssid);
        }
    }
    lwesp_sta_join("KIDS", "12345678", NULL, NULL, NULL, 1);
    
    while (!lwesp_sta_is_joined()) {
        sleep(1);
    } 
    {
        printf("wifi connected\r\n");
    }
}
