#include <section_config.h>
#include <osdep_service.h>
#include <skbuff.h>

#define MAX_SKB_BUF_SIZE     1650	// should >= the size in wlan driver
#define MAX_SKB_BUF_NUM      8
#define MAX_LOCAL_SKB_NUM    (MAX_SKB_BUF_NUM + 2)

/* DO NOT modify skb_buf and skb_data structure */
struct skb_buf {
	struct list_head list;
	struct sk_buff skb;
};

struct skb_data {
	struct list_head list;
	unsigned char buf[MAX_SKB_BUF_SIZE];
	atomic_t ref;
};

unsigned int nr_xmitframe = MAX_SKB_BUF_NUM;
unsigned int nr_xmitbuff = MAX_SKB_BUF_NUM;
int max_local_skb_num = MAX_LOCAL_SKB_NUM;
int max_skb_buf_num = MAX_SKB_BUF_NUM;

/* DO NOT access skb_pool and skb_data_pool out of wlan driver */
struct skb_buf skb_pool[MAX_LOCAL_SKB_NUM];

// SRAM_BD_DATA_SECTION default in SRAM. Can modify image2.icf to link to the end of SDRAM
SRAM_BD_DATA_SECTION
struct skb_data skb_data_pool[MAX_SKB_BUF_NUM];

