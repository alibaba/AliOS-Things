/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */


#ifndef AOS_IRQDESC_H
#define AOS_IRQDESC_H

#include <stdbool.h>
#include <aos/kernel.h>
#include <aos/list.h>
#include <k_api.h>
#include <irq/irq_debug.h>

/**
aos_request_threaded_irq: irqflags
*/
#define     IRQ_TYPE_EDGE_RISING    0x00000001
#define     IRQ_TYPE_EDGE_FALLING   0x00000002
#define     IRQ_TYPE_EDGE_BOTH      (IRQ_TYPE_EDGE_FALLING | IRQ_TYPE_EDGE_RISING)
#define     IRQ_TYPE_LEVEL_HIGH     0x00000004
#define     IRQ_TYPE_LEVEL_LOW      0x00000008
#define     IRQ_TYPE_LEVEL_BOTH     (IRQ_TYPE_LEVEL_HIGH | IRQ_TYPE_LEVEL_LOW)
#define     IRQF_SHARED             0x00000080

/**
 * enum irqreturn
 * @IRQ_NONE            interrupt was not from this device or was not handled
 * @IRQ_HANDLED         interrupt was handled by this device
 * @IRQ_WAKE_USERMODE   interrupt was trigged to usermode
 */
typedef enum irqreturn {
    IRQ_NONE,
    IRQ_HANDLED,
    IRQ_WAKE_USERMODE,
}irqreturn_t;

#ifdef AOS_COMPILE_APP
#define IRQ_CRITICAL_ENTER(lockaddr,flags_cpsr)                     \
    do {                                                            \
        aos_mutex_lock(lockaddr, AOS_WAIT_FOREVER);            \
    } while (0);
#define IRQ_CRITICAL_EXIT(lockaddr,flags_cpsr)                      \
    do {                                                            \
        aos_mutex_unlock(lockaddr);                              \
    } while (0);
#else /* (RHINO_CONFIG_MM_REGION_MUTEX != 0) */
#define IRQ_CRITICAL_ENTER(lockaddr,flags_cpsr) krhino_spin_lock_irq_save(lockaddr,flags_cpsr);
#define IRQ_CRITICAL_EXIT(lockaddr,flags_cpsr)  krhino_spin_unlock_irq_restore(lockaddr,flags_cpsr);
#endif

typedef irqreturn_t (*irq_handler_t)(int, void *);
typedef void (*irq_flow_handler_t)(void *desc);

typedef struct irq_data {
    struct irq_chip     *chip;      /* irq cpu handler */
    int                 irq;        /* hw irq no */
    unsigned int        flags;      /* group0 or group1 for example */
}irq_data_t;

typedef struct irq_desc {
    const char          *name;          /* For example uart0,move to irqaction */
    unsigned int        depth;          /* irq lock/unlock depth */
    irq_flow_handler_t  handle_irq;     /* First level handler, dispatched to handle_edge_irq and handle_level_irq according to irqflags*/
    dlist_t             action_head;    /* irq handle list */
    irq_data_t     irq_data;       /* irq no; irq_chip handler */
    unsigned count;
}irq_desc_t;

typedef struct irq_chip {
    const char   *name;    // ARM-GICv3 for example
    void        (*irq_enable)(irq_data_t *data);
    void        (*irq_disable)(irq_data_t *data);
    void        (*irq_eoi)(irq_data_t *data);
    //void      (*ipi_send_single)(irq_data_t *data, unsigned int cpu);
    void        (*ipi_send_mask)(irq_data_t *data, unsigned int cpumask);
    int         (*irq_set_type)(irq_data_t *data, unsigned int flow_type);
    void        (*irq_set_nmi)(irq_data_t *data);
    int         (*irq_set_affinity)(irq_data_t *data, unsigned int cpumask, bool force);
    int         (*irq_is_active)(irq_data_t *data);
    int         (*irq_set_priority)(irq_data_t *data, unsigned int priority);
    int         (*irq_set_pending)(irq_data_t *data);
    int         (*irq_clear_pending)(irq_data_t *data);
}irq_chip_t;

typedef struct irq_action {
    dlist_t             action_list;
    irq_handler_t       handler;    /* kernel default handler */
    irq_handler_t       thread_fn;  /* user default thread handler */
    unsigned int        flags;      /* SHARED: IRQF_SHARED; IRQ_TYPE_EDGE */
    void                *devname;   /* device name */
    void                *dev_id;    /* dev id: for shared */
    unsigned int        pid;        /* user pid; not used for kernel */
    int                 usermode;   /* true: user mode */
    //void*             irq_q;
    //irq_action_t    *next;
}irq_action_t;

int aos_irq_system_init(void);
int aos_irq_system_deinit(void);
int aos_request_threaded_irq (int irq, irq_handler_t handler,
    irq_handler_t thread_fn, unsigned long irqflags,
        const char *devname, void *dev_id);
void *aos_free_irq(int irq, void *dev_id);
void aos_enable_irq(int irq);
void aos_disable_irq(int irq);
void aos_set_nmi_irq(int irq);
void aos_send_mask_ipi(int irq, unsigned int cpumask);
int aos_irq_is_active(int irq);
int aos_set_priority_irq(int irq, unsigned int priority);
int aos_irq_set_pending(int irq);
int aos_irq_clear_pending(int irq);
int aos_get_active_irq(void);

#endif
