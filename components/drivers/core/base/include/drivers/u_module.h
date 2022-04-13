/**
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef _U_MODULE_H_
#define _U_MODULE_H_

#define u_module_init(x) \
int u_aos_module_init(void) \
{\
	return x(); \
}
#define u_module_get_bus(x) \
u_bus_t *u_aos_module_get_bus(void) { \
	return x();\
}

typedef int (*u_aos_driver_init)(void);
typedef u_bus_t *(*u_aos_get_current_bus)(void);
#endif //_U_MODULE_H_
