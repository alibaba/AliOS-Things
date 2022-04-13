/**
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef _U_IO_H_
#define _U_IO_H_

/**
 * BUS types
 * */
typedef enum bus_type {
	BUS_TYPE_PLATFORM,
	BUS_TYPE_USB,
	BUS_TYPE_MMC,
	BUS_TYPE_SDIO,
	BUS_TYPE_I2C,
	BUS_TYPE_I2S,
	BUS_TYPE_SPI,
	BUS_TYPE_PCI,
	BUS_TYPE_MIPI,
	BUS_TYPE_PRE_MAX,
	BUS_TYPE_MAX = 0xff,
} bus_type_e;

/**
 * hardware resource type
 * */
typedef enum res_type {
    RES_TYPE_IO = 0x81, /* GPIO */
	RES_TYPE_MEM,       /* registers */
    RES_TYPE_IRQ,       /* interrupt */
    RES_TYPE_MAX,
} res_type_e;

/**
 * hardware resources, GPIO, IRQ and registers are all included
 * */
typedef struct hw_res {
    res_type_e res_type;    /* IO, register, IRQ, etc.  */
    void *reg_start;        /* register base address */
    void *reg_end;          /* register end address */
    char *name;             /* hardware resource name , optional */
    struct hw_res *next;    /* hw resource chain , optional */
} hw_res_t;

/**
 * device resources
 * */
typedef struct dev_res {
    char *name;           /* device name */
    int id;               /* device group id, usually combined with name, e.g. name = "uart", id = 0, device is "uart0" */
    bus_type_e bus_type;  /* device name */
    unsigned int num;     /* resource number */
    hw_res_t p_res[5];      /* pointer to device resource  */
} dev_res_t;

#endif //_U_IO_H_
