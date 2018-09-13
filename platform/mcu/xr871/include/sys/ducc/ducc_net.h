/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _SYS_DUCC_DUCC_NET_H_
#define _SYS_DUCC_DUCC_NET_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum ducc_net_cmd {
	/* data command */
	DUCC_NET_CMD_WLAN_INPUT,
	DUCC_NET_CMD_WLAN_MONITOR_INPUT,	/* data input for monitor mode */

	/* normal command */
#if (__CONFIG_MBUF_IMPL_MODE == 1)
	DUCC_NET_CMD_MBUF_GET,
	DUCC_NET_CMD_MBUF_FREE,
#endif
	DUCC_NET_CMD_POWER_NOTIFY,
	DUCC_NET_CMD_BIN_READ,
	DUCC_NET_CMD_EFUSE_READ,

	DUCC_NET_CMD_SYS_EVENT,		/* refer to enum ducc_net_sys_event */
	DUCC_NET_CMD_WLAN_EVENT,	/* refer to enum wlan_event */
};

#define DUCC_NET_IS_DATA_CMD(c) \
	((c) <= DUCC_NET_CMD_WLAN_MONITOR_INPUT)

enum ducc_net_sys_event {
	DUCC_NET_SYS_READY,
};

struct ducc_param_wlan_input {
	void *nif;
#if (__CONFIG_MBUF_IMPL_MODE == 0)
	uint8_t *data;
	int len;
#elif (__CONFIG_MBUF_IMPL_MODE == 1)
	void *mbuf;
#endif
};

struct ducc_param_wlan_mon_input {
	void *nif;
	uint8_t *data;
	uint32_t len;
	void *info;
};

#if (__CONFIG_MBUF_IMPL_MODE == 1)
struct ducc_param_mbuf_get {
	int len;
	int tx;
	void *mbuf;
};
#endif

#define DUCC_WLAN_BIN_TYPE_BL	(0)
#define DUCC_WLAN_BIN_TYPE_FW	(1)
#define DUCC_WLAN_BIN_TYPE_SDD	(2)

struct ducc_param_wlan_bin {
	int		type;
	int		index;
	int		len;
	void   *buf;
};

struct ducc_param_efuse {
	uint8_t *data;
	uint32_t start_bit;
	uint32_t bit_num;
};

#if (defined(__CONFIG_ARCH_DUAL_CORE) && defined(__CONFIG_ARCH_NET_CORE))

typedef int (*ducc_cb_func)(uint32_t param0, uint32_t param1);

struct ducc_net_param {
	ducc_cb_func cb;
};

int ducc_net_start(struct ducc_net_param *param);
int ducc_net_ioctl(enum ducc_net_cmd cmd, void *param);

#endif /* (defined(__CONFIG_ARCH_DUAL_CORE) && defined(__CONFIG_ARCH_NET_CORE)) */

#ifdef __cplusplus
}
#endif

#endif	/* _SYS_DUCC_DUCC_NET_H_ */
