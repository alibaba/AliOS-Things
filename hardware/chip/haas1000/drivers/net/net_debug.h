#ifndef __NET_DEBUG_H
#define __NET_DEBUG_H

#include "hal_trace.h"
#include "net_defs.h"

enum {
	DUMP_PREFIX_NONE,
	DUMP_PREFIX_ADDRESS,
	DUMP_PREFIX_OFFSET
};

#define wlan_debug(fmt, ...) TRACE(fmt, ##__VA_ARGS__)

//#define NET_ASSERT(cond, ...)       { if (!(cond)) {wlan_debug("wlan assert %s line %d \n",__func__, __LINE__); while(1); } }
#define NET_ASSERT(cond, ...)       {ASSERT(cond, "net assert")}

extern uint32_t kernel_debug_level;


#define KERN_ALERT		0
#define KERN_CRITICAL	1
#define KERN_ERR		2
#define KERN_WARN		3
#define KERN_INFO		4
#define KERN_DEBUG		5
#define KERN_LOUD		6

//#define KERN_DBG_LEVEL		KERN_INFO

//#define printk TRACE
#define printk(level, str, ...) \
({								\
    if ((level) <= kernel_debug_level) \
        TRACE(str, ##__VA_ARGS__); \
})

#define BUILD_BUG_ON(condition)

#define WARN_ON(condition)	\
({								\
	int __ret_warn_on = !!(condition);			\
	if (unlikely(__ret_warn_on)){				\
		wlan_debug("WARN_ON:%s %d \n", __func__, __LINE__);			\
    }\
	unlikely(__ret_warn_on);				\
})


#ifndef WARN
#define WARN(condition, format...) ({						\
	int __ret_warn_on = !!(condition);				\
	if (unlikely(__ret_warn_on))					\
		wlan_debug(format);					\
	unlikely(__ret_warn_on);					\
})
#endif
#ifndef MAC2STR
#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
#endif

#ifndef WARN_ON_ONCE
#define WARN_ON_ONCE(condition)	({				\
	static bool  __warned;		\
	int __ret_warn_once = !!(condition);			\
								\
	if (unlikely(__ret_warn_once))				\
		if (WARN_ON(!__warned)) 			\
			__warned = true;			\
	unlikely(__ret_warn_once);				\
})

#endif

#define BUG()

#define BUG_ON(cond)	ASSERT(!(cond), "ASSERT:%s line:%d", __func__, __LINE__)
extern uint32_t  rt_get_PSP (void);
//#define ENTER() printk(KERN_DEBUG, "Enter:%s %d\n", __func__, __LINE__);
//#define LEAVE() printk(KERN_DEBUG, "Leave:%s %d\n", __func__, __LINE__);
#define ENTER()
#define LEAVE() 

extern void print_hex_dump_bytes(const char *prefix_str, int prefix_type,
			  const void *buf, size_t len);

#endif /* __NET_DEBUG_H */
