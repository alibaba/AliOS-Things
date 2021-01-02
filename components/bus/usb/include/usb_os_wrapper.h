#ifndef _USB_OS_WRAPPER_H
#define _USB_OS_WRAPPER_H

#include "k_api.h"
#include "aos/kernel.h"

#define USB_INTRPT_ALLOC            CPSR_ALLOC()
#define USB_INTRPT_DISABLE          RHINO_CPU_INTRPT_DISABLE()
#define USB_INTRPT_ENABLE           RHINO_CPU_INTRPT_ENABLE()

#define USB_WAIT_FOREVER AOS_WAIT_FOREVER

void *usb_malloc(unsigned int size);
void usb_free(void * mem);

int usb_sem_new(void **sem, int count);
void usb_sem_free(void *sem);
void usb_sem_signal(void *sem);
int usb_sem_wait(void *sem, unsigned int timeout_ms);

void usb_msleep(const unsigned int ms);
int usb_task_new_ext(void *task, char *name, void (*fn)(void *),
                     void *arg, int stack_size, int prio);


#endif /* _USB_OS_WRAPPER_H */
