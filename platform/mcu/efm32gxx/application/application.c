#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <k_api.h>
#include <aos/log.h>
#include <hal/soc/soc.h>
#include <hal/soc/timer.h>
#include <hal/base.h>
#include <aos/aos.h>
#include <hal/hal.h>
#include "board.h"

#include "port_mcu.h"

#define us2tick(us) \
((us * RHINO_CONFIG_TICKS_PER_SECOND + 999999) / 1000000)



#define app_task_test_certificate_STACKSIZE    400
#define app_task_test_certificate_PRIORITY     1

static ktask_t app_task_test_certificate_obj;

cpu_stack_t app_task_test_certificate_buf[app_task_test_certificate_STACKSIZE];

void app_task_test_certificate(void *arg)
{
  test_certificate();
  
  while(1);
}

void app_task_test_certificate_init(void)
{
  krhino_task_create(&app_task_test_certificate_obj,
                     "app_task_test_certificate", 
                     0,
                     app_task_test_certificate_PRIORITY,                        
                     50, 
                     app_task_test_certificate_buf, 
                     app_task_test_certificate_STACKSIZE, 
                     app_task_test_certificate, 
                     1);
  
  
  
}
int application_start(int argc, char **argv)
{
  
  app_task_test_certificate_init();
  aos_start();
  
  while(1);
}