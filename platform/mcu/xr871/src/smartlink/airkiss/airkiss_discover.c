#include "stdlib.h"
#include "string.h"

#include "sys/param.h"

#include "kernel/os/os.h"
#include "lwip/sockets.h"
#include "lwip/inet.h"
#include "lwip/ip.h"

#include "smartlink/airkiss/wlan_airkiss.h"
#include "airkiss.h"

#define g_debuglevel  ERROR

#define AIRKISS_LAN_PORT 12476
#define AIRKISS_BIND_PORT 12476
#define AIRKISS_TIMEOUT  100 /* mS */
#define AIRKISS_RECV_BUF_LEN  256

typedef struct airkiss_lan_discover_priv {
	airkiss_config_t func;
	Airkiss_Online_Ack_Info ack_info;
	uint8_t lan_thread_run;
	OS_Thread_t lan_thread;
	uint8_t lan_buf[AIRKISS_LAN_BUF_LEN];
} airkiss_lan_discover_priv_t;

static airkiss_lan_discover_priv_t *airkiss_lan_discover_priv;

static int
airkiss_send_active_lan_discovery_packets(airkiss_lan_discover_priv_t *priv, int sock_fd)
{
	int ret = -1;
	struct sockaddr_in to_addr;
	uint16_t lan_buf_len = AIRKISS_LAN_BUF_LEN;
	Airkiss_Online_Ack_Info *info = &priv->ack_info;

	ret = airkiss_lan_pack(AIRKISS_LAN_SSDP_NOTIFY_CMD,
	                       info->app_id, info->device_id,  0, 0,
	                       priv->lan_buf, &lan_buf_len, &priv->func);
	if (ret != AIRKISS_LAN_PAKE_READY) {
		AIRKISS_DBG(ERROR, "airkiss pack lan packet error!\n");
		return -1;
	}

	FD_ZERO(&to_addr);
	to_addr.sin_family = AF_INET;
	to_addr.sin_port = htons(AIRKISS_LAN_PORT);
	to_addr.sin_addr.s_addr = inet_addr("255.255.255.255");

	AIRKISS_DBG(INFO, "%s\n", __func__);
	ret = sendto(sock_fd, (unsigned char *)priv->lan_buf, lan_buf_len, 0,
	             (struct sockaddr *)&to_addr, sizeof(struct sockaddr));
	if (ret <= 0) {
		AIRKISS_DBG(ERROR, "%s error ret:%d!\n", __func__, ret);
		return -1;
	}

	return 0;
}

static void
airkiss_lan_server_reply(airkiss_lan_discover_priv_t *priv, int sock_fd,
                         struct sockaddr_in addr, char *pdata, unsigned short len)
{
	airkiss_lan_ret_t ret = -1;
	airkiss_lan_ret_t pack_ret;
	uint16_t lan_buf_len = AIRKISS_LAN_BUF_LEN;
	Airkiss_Online_Ack_Info *info = &priv->ack_info;

	ret = airkiss_lan_recv(pdata, len, &priv->func);

	switch (ret) {
	case AIRKISS_LAN_CONTINUE:
		break;
	case AIRKISS_LAN_SSDP_REQ:
		addr.sin_port = htons(AIRKISS_LAN_PORT);
		lan_buf_len = AIRKISS_LAN_BUF_LEN;
		pack_ret = airkiss_lan_pack(AIRKISS_LAN_SSDP_RESP_CMD,
		                            info->app_id, info->device_id, 0, 0,
		                            priv->lan_buf, &lan_buf_len, &priv->func);
		if (pack_ret != AIRKISS_LAN_PAKE_READY) {
			AIRKISS_DBG(ERROR, "%s,%d Pack lan packet error!\n",
			            __func__, __LINE__);
			return;
		}

		AIRKISS_DBG(INFO, "%s\n", __func__);
		ret = sendto(sock_fd, (unsigned char *)priv->lan_buf,
		             lan_buf_len, 0, (struct sockaddr *) &addr,
		             sizeof(struct sockaddr));
		if (ret <= 0) {
			AIRKISS_DBG(ERROR, "%s error ret=%d!\n", __func__, ret);
		}
		break;
	default:
		AIRKISS_DBG(ERROR, "Pack ssdq req not supported:%d!\n", ret);
		break;
	}
}

static void airkiss_lan_discover_task(void *arg)
{
	int ret, tmp = 1;
	airkiss_lan_discover_priv_t *priv = (airkiss_lan_discover_priv_t *)arg;
	int sock_fd, len;
	struct sockaddr_in addr;
	int sock_timeout_val = AIRKISS_TIMEOUT;
	int addr_len = sizeof(struct sockaddr_in);
	char *buffer;
	int times;
	int time_next = OS_JiffiesToMSecs(OS_GetJiffies()) +
		priv->ack_info.ack_period_ms;

	buffer = malloc(AIRKISS_RECV_BUF_LEN);
	if (!buffer) {
		AIRKISS_DBG(ERROR, "%s create sock error!\n", __func__);
		goto malloc_faild;
	}

	OS_ThreadSuspendScheduler();
	priv->lan_thread_run |= AK_TASK_RUN;
	OS_ThreadResumeScheduler();

	sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock_fd < 0){
		AIRKISS_DBG(ERROR, "%s create sock error!\n", __func__);
		goto err_out;
	}

	ret = setsockopt(sock_fd, SOL_SOCKET, SO_BROADCAST, &tmp, sizeof(int));
	if (ret != 0) {
		AIRKISS_DBG(ERROR, "%s,%d, setsockopt error!\n", __func__, __LINE__);
		goto out;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(AIRKISS_BIND_PORT);
	addr.sin_addr.s_addr = htonl(INADDR_ANY) ;

	ret = bind(sock_fd, (struct sockaddr *)&addr, sizeof(addr));
	if (ret < 0) {
		AIRKISS_DBG(ERROR, "%s bind sock error!\n", __func__);
		goto out;
	}

	ret = setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, &sock_timeout_val,
	                 sizeof(sock_timeout_val));
	if (ret != 0) {
		AIRKISS_DBG(ERROR, "%s,%d, setsockopt error!\n", __func__, __LINE__);
		goto out;
	}

	while (!(priv->lan_thread_run & AK_TASK_STOP)) {
		memset(buffer, 0, AIRKISS_RECV_BUF_LEN);
		len = recvfrom(sock_fd, buffer, AIRKISS_RECV_BUF_LEN, 0,
		               (struct sockaddr *)&addr , (socklen_t *)&addr_len);
		if (len != -1) {
			/* send ack when packet received */
			airkiss_lan_server_reply(priv, sock_fd, addr, buffer, len);
		}
		times = OS_JiffiesToMSecs(OS_GetJiffies());
		if (OS_TimeAfterEqual(times, time_next)) {
			time_next = OS_JiffiesToMSecs(OS_GetJiffies()) +
				priv->ack_info.ack_period_ms;
			airkiss_send_active_lan_discovery_packets(priv, sock_fd);
		}
	}

out:
	closesocket(sock_fd);
err_out:
	free(buffer);
malloc_faild:
	OS_ThreadSuspendScheduler();
	priv->lan_thread_run = 0;
	OS_ThreadResumeScheduler();
	OS_ThreadDelete(&priv->lan_thread);
}

int wlan_airkiss_lan_discover_start(char *app_id, char *dev_id, uint32_t period_ms)
{
	airkiss_lan_discover_priv_t *priv = airkiss_lan_discover_priv;

	if (priv) {
		AIRKISS_DBG(ERROR, "%s has already started!\n", __func__);
		return -1;
	}

	priv = malloc(sizeof(airkiss_lan_discover_priv_t));
	if (!priv) {
		AIRKISS_DBG(ERROR, "%s malloc failed!\n", __func__);
		return -1;
	}
	airkiss_lan_discover_priv = priv;
	memset(priv, 0, sizeof(airkiss_lan_discover_priv_t));

	priv->func.memset = (airkiss_memset_fn)&memset;
	priv->func.memcpy = (airkiss_memcpy_fn)&memcpy;
	priv->func.memcmp = (airkiss_memcmp_fn)&memcmp;
	priv->func.printf = (airkiss_printf_fn)&printf;

	priv->ack_info.app_id = app_id;
	priv->ack_info.device_id = dev_id;
	if (period_ms == 0)
		priv->ack_info.ack_period_ms = AIRKISS_TIMEOUT;
	else
		priv->ack_info.ack_period_ms =
			DIV_ROUND_UP(period_ms, AIRKISS_TIMEOUT) * AIRKISS_TIMEOUT;

	OS_ThreadSuspendScheduler();
	priv->lan_thread_run |= AK_TASK_RUN;
	OS_ThreadResumeScheduler();

	if (OS_ThreadCreate(&priv->lan_thread,
	                    "",
	                    airkiss_lan_discover_task,
	                    (void *)priv,
	                    OS_THREAD_PRIO_APP,
	                    AIRKISS_ONLINE_DIALOG_THREAD_STACK_SIZE) != OS_OK) {
		AIRKISS_DBG(ERROR, "%s,%d create airkiss ask thread failed!\n",
		            __func__, __LINE__);
		return -1;
	}

	return 0;
}

int wlan_airkiss_lan_discover_stop(void)
{
	airkiss_lan_discover_priv_t *priv = airkiss_lan_discover_priv;

	if (!priv) {
		AIRKISS_DBG(INFO, "%s has already stoped!\n", __func__);
		return -1;
	}

	airkiss_lan_discover_priv = NULL;

	OS_ThreadSuspendScheduler();
	priv->lan_thread_run |= AK_TASK_STOP;
	OS_ThreadResumeScheduler();
	while (priv->lan_thread_run & AK_TASK_RUN)
		OS_MSleep(10);

	free(priv);

	return 0;
}
