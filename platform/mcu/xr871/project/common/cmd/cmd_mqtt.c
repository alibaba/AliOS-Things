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

#include "cmd_util.h"
#include "cmd_mqtt.h"
#include "net/mqtt/MQTTClient-C/MQTTClient.h"


static MQTTPacket_connectData connectData = MQTTPacket_connectData_initializer;
static char *client_name;
static uint8_t *send_buf;
static uint8_t *recv_buf;
static Client client;
static Network network;

#define MAX_SUB_TOPICS 5
static char *sub_topic[MAX_SUB_TOPICS];

#define CMD_MQTT_BG_THREAD_STACK_SIZE (1024)
static OS_Thread_t mqtt_bg_thread;

static OS_Mutex_t lock;


static enum cmd_status cmd_mqtt_init_exec(char *cmd)
{
	int32_t cnt;
	uint32_t buf_size;
	uint32_t alive_interval;
	uint32_t clean;
	char *tmp;

	/* get param */
	cnt = cmd_sscanf(cmd, "bufsize=%u alive=%u clean=%u",
			 &buf_size, &alive_interval, &clean);

	/* check param */
	if (cnt != 3) {
		CMD_ERR("invalid param number %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}

	if (clean > 1) {
		CMD_ERR("invalid clean %d\n", clean);
		return CMD_STATUS_INVALID_ARG;
	}

	if (OS_MutexCreate(&lock) != OS_OK)
		return CMD_STATUS_FAIL;;

	if ((tmp = cmd_strrchr(cmd, '\"')) == NULL)
		return CMD_STATUS_INVALID_ARG;
	*tmp = '\0';
	if ((tmp = cmd_strchr(cmd, '\"')) == NULL)
		return CMD_STATUS_INVALID_ARG;
	tmp++;

	client_name = cmd_malloc(cmd_strlen(tmp) + 1);

	cmd_memcpy(client_name, tmp, cmd_strlen(tmp) + 1);
	CMD_DBG("client name = %s\n", client_name);

	connectData.clientID.cstring = client_name;
	connectData.keepAliveInterval = alive_interval;
	connectData.cleansession = clean;

	send_buf = cmd_malloc(buf_size);
	if (send_buf == NULL) {
		CMD_ERR("no memory\n");
		OS_MutexDelete(&lock);
		return CMD_STATUS_FAIL;
	}
	recv_buf = cmd_malloc(buf_size);
	if (recv_buf == NULL) {
		cmd_free(send_buf);
		CMD_ERR("no memory\n");
		OS_MutexDelete(&lock);
		return CMD_STATUS_FAIL;
	}

	NewNetwork(&network);
	MQTTClient(&client, &network, 6000, send_buf, buf_size, recv_buf, buf_size);



	return CMD_STATUS_OK;
}

static enum cmd_status cmd_mqtt_will_exec(char *cmd)
{
	int32_t cnt;
	char *topic;
	uint8_t *buf;
	char *tmp;
	uint32_t qos;
	uint32_t retain;
	uint32_t size;

	/* get param */
	cnt = cmd_sscanf(cmd, "qos=%u retain=%u",
			 &qos, &retain);

	if ((tmp = cmd_strrchr(cmd, '\"')) == NULL)
		return CMD_STATUS_INVALID_ARG;
	*tmp = '\0';
	cnt += cmd_sscanf(tmp + 2, "size=%u", &size);
	if ((tmp = cmd_strchr(cmd, '\"')) == NULL)
		return CMD_STATUS_INVALID_ARG;
	tmp++;

	/* check param */
	if (cnt != 3) {
		CMD_ERR("invalid param number %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}

	if (qos > 2) {
		CMD_ERR("invalid qos %d\n", qos);
		return CMD_STATUS_INVALID_ARG;
	}

	if (retain > 1) {
		CMD_ERR("invalid retain %d\n", retain);
		return CMD_STATUS_INVALID_ARG;
	}

	topic = tmp;
	CMD_DBG("topic name = %s\n", topic);

	buf = cmd_malloc(size);
	if (buf == NULL) {
		CMD_ERR("no memory\n");
		return CMD_STATUS_FAIL;
	}


	cmd_write_respond(CMD_STATUS_OK, "OK");
	cmd_raw_mode_enable();
	cmd_raw_mode_read(buf, size, 30000);
	cmd_raw_mode_disable();

	//will function
	connectData.willFlag = 1;
	connectData.will.message.lenstring.data = (char *)buf;
	connectData.will.message.lenstring.len = size;
	connectData.will.qos = qos;
	connectData.will.retained = retain;
	connectData.will.topicName.cstring = topic;

	cmd_free(buf);

	return CMD_STATUS_ACKED;
}

static enum cmd_status cmd_mqtt_user_exec(char *cmd)
{
	char *tmp;
	uint32_t size;

	if ((tmp = cmd_strrchr(cmd, '\"')) == NULL)
		return CMD_STATUS_INVALID_ARG;
	*tmp = '\0';
	if ((tmp = cmd_strchr(cmd, '\"')) == NULL)
		return CMD_STATUS_INVALID_ARG;
	tmp++;

	if ((size = cmd_strlen(tmp)) == 0)
		return CMD_STATUS_INVALID_ARG;
	size++;

	connectData.username.lenstring.data = cmd_malloc(size);
	connectData.username.lenstring.len = size;
	memcpy(connectData.username.lenstring.data, tmp, size);

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_mqtt_password_exec(char *cmd)
{
	char *tmp;
	uint32_t size;

	if ((tmp = cmd_strrchr(cmd, '\"')) == NULL)
		return CMD_STATUS_INVALID_ARG;
	*tmp = '\0';
	if ((tmp = cmd_strchr(cmd, '\"')) == NULL)
		return CMD_STATUS_INVALID_ARG;
	tmp++;

	if ((size = cmd_strlen(tmp)) == 0)
		return CMD_STATUS_INVALID_ARG;
	size++;

	connectData.password.lenstring.data = cmd_malloc(size);
	connectData.password.lenstring.len = size;
	memcpy(connectData.password.lenstring.data, tmp, size);

	return CMD_STATUS_OK;
}


static void cmd_mqtt_bg(void *arg)
{
	int rc;

	while (1) {
		OS_MutexLock(&lock, 0xffffffffU);
		if ((rc = MQTTYield(&client, 3000)) != 0)
			CMD_WRN("Return code from yield is %d\n", rc);
		OS_MutexUnlock(&lock);
	}

}

static enum cmd_status cmd_mqtt_connect_exec(char *cmd)
{
	int32_t cnt;
	char addr_str[15];
	uint32_t port;
	int32_t rc;
	uint32_t i;

	/* get param */
	cnt = cmd_sscanf(cmd, "server=%15s port=%u",
			 addr_str, &port);

	/* check param */
	if (cnt != 2) {
		CMD_ERR("invalid param number %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}

	i = 10;
	char* address = addr_str;
	while ((rc = ConnectNetwork(&network, address, port)) != 0)
	{
		CMD_WRN("Return code from network connect is %d\n", rc);
		cmd_msleep(2000);
		if (!(i--))
			return CMD_STATUS_FAIL;
	}

	if ((rc = MQTTConnect(&client, &connectData)) != 0) {
		CMD_ERR("Return code from MQTT connect is %d\n", rc);
		return CMD_STATUS_FAIL;
	}
	else
		CMD_DBG("MQTT Connected\n");



	if (OS_ThreadCreate(&mqtt_bg_thread,
		                "",
		                cmd_mqtt_bg,
		                NULL,
		                OS_PRIORITY_NORMAL,
		                CMD_MQTT_BG_THREAD_STACK_SIZE) != OS_OK) {
		CMD_ERR("create mqtt background failed\n");
		return CMD_STATUS_FAIL;
	}


	return CMD_STATUS_OK;
}

void mqtt_msg_cb(MessageData* data)
{
	cmd_write_event(CMD_EVENT_MQTT_MSG_RECV, "[topic: %.*s] %.*s", data->topicName->lenstring.len,
					data->topicName->lenstring.data, data->message->payloadlen,
					(char *)data->message->payload);
}

static enum cmd_status cmd_mqtt_subscribe_exec(char *cmd)
{
	int32_t cnt;
	uint32_t i;
	char *tmp;
	uint32_t qos;
	int rc;

	/* get param */
	cnt = cmd_sscanf(cmd, "qos=%u",
			 &qos);

	if ((tmp = cmd_strrchr(cmd, '\"')) == NULL)
		return CMD_STATUS_INVALID_ARG;
	*tmp = '\0';
	if ((tmp = cmd_strchr(cmd, '\"')) == NULL)
		return CMD_STATUS_INVALID_ARG;
	tmp++;


	/* check param */
	if (cnt != 1) {
		CMD_ERR("invalid param number %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}

	if (qos > 2) {
		CMD_ERR("invalid qos %d\n", qos);
		return CMD_STATUS_INVALID_ARG;
	}

	i = 0;
	while (sub_topic[i] != NULL) {
		i++;
		if (i >= MAX_SUB_TOPICS) {
			CMD_WRN("Subscribe topics is max\n");
			return CMD_STATUS_FAIL;
		}
	}

	sub_topic[i] = cmd_malloc(cmd_strlen(tmp) + 1);
	cmd_memcpy(sub_topic[i], tmp, cmd_strlen(tmp) + 1);

	if (OS_MutexLock(&lock, 60000) == OS_E_TIMEOUT)
		return CMD_STATUS_FAIL;

	if ((rc = MQTTSubscribe(&client, sub_topic[i], qos, mqtt_msg_cb)) != 0) {
		CMD_ERR("Return code from MQTT subscribe is %d\n", rc);
		return CMD_STATUS_FAIL;
	}

	OS_MutexUnlock(&lock);

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_mqtt_unsubscribe_exec(char *cmd)
{
	uint32_t i;
	char *tmp;
	int rc;

	/* get param */
	if ((tmp = cmd_strrchr(cmd, '\"')) == NULL)
		return CMD_STATUS_INVALID_ARG;
	*tmp = '\0';
	if ((tmp = cmd_strchr(cmd, '\"')) == NULL)
		return CMD_STATUS_INVALID_ARG;
	tmp++;

	i = 0;
	while (i < MAX_SUB_TOPICS) {
		if ((sub_topic[i] != NULL) && (!cmd_memcmp(sub_topic[i], tmp, cmd_strlen(tmp) + 1)))
			break;
		i++;
	}

	if (OS_MutexLock(&lock, 60000) == OS_E_TIMEOUT)
		return CMD_STATUS_FAIL;

	if ((rc = MQTTUnsubscribe(&client, tmp/*sub_topic[i]*/)) != 0) {
		CMD_ERR("Return code from MQTT unsubscribe is %d\n", rc);
		return CMD_STATUS_FAIL;
	}

	if (i == MAX_SUB_TOPICS)
		CMD_WRN("Unsubscribe topics is inexist\n");
	else {
		cmd_free(sub_topic[i]);
		sub_topic[i] = NULL;
	}

	OS_MutexUnlock(&lock);

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_mqtt_publish_exec(char *cmd)
{
	int32_t cnt;
	MQTTMessage message;
	char *topic;
	uint8_t *buf;
	char *tmp;
	uint32_t qos;
	uint32_t retain;
	uint32_t size;
	int rc;

	/* get param */
	cnt = cmd_sscanf(cmd, "qos=%u retain=%u",
			 &qos, &retain);

	if ((tmp = cmd_strrchr(cmd, '\"')) == NULL)
		return CMD_STATUS_INVALID_ARG;
	*tmp = '\0';
  	cnt += cmd_sscanf(tmp + 2, "size=%u", &size);
	if ((tmp = cmd_strchr(cmd, '\"')) == NULL)
		return CMD_STATUS_INVALID_ARG;
	tmp++;


	/* check param */
	if (cnt != 3) {
		CMD_ERR("invalid param number %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}

	if (qos > 2) {
		CMD_ERR("invalid qos %d\n", qos);
		return CMD_STATUS_INVALID_ARG;
	}

	if (retain > 1) {
		CMD_ERR("invalid retain %d\n", retain);
		return CMD_STATUS_INVALID_ARG;
	}

	topic = tmp;
	CMD_DBG("topic name = %s\n", topic);

	buf = cmd_malloc(size);
	if (buf == NULL) {
		CMD_ERR("no memory\n");
		return CMD_STATUS_FAIL;
	}


	cmd_write_respond(CMD_STATUS_OK, "OK");
	cmd_raw_mode_enable();
	cmd_raw_mode_read(buf, size, 30000);

	message.qos = qos;
	message.retained = retain;
	message.payload = buf;
	message.payloadlen = size;

	if (OS_MutexLock(&lock, 60000) == OS_E_TIMEOUT)
		return CMD_STATUS_FAIL;

	if ((rc = MQTTPublish(&client, topic, &message)) != 0)
		CMD_ERR("Return code from MQTT publish is %d\n", rc);
	else
		CMD_DBG("MQTT publish is success\n");

	OS_MutexUnlock(&lock);

	cmd_raw_mode_write((uint8_t *)&rc, sizeof(rc));
	cmd_raw_mode_disable();

	cmd_free(buf);

	return CMD_STATUS_ACKED;

}

static enum cmd_status cmd_mqtt_disconnect_exec(char *cmd)
{
	int32_t cnt;
	uint32_t tcponly;
	int rc;
	uint32_t i;

	/* get param */
	cnt = cmd_sscanf(cmd, "tcponly=%u",
			 &tcponly);

	/* check param */
	if (cnt != 1) {
		CMD_ERR("invalid param number %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}

	if (tcponly > 1) {
		CMD_ERR("invalid tcponly %d\n", tcponly);
		return CMD_STATUS_INVALID_ARG;
	}


	OS_ThreadDelete(&mqtt_bg_thread);

	i = 0;
	while (i < MAX_SUB_TOPICS) {
		if (sub_topic[i] != NULL) {
			cmd_free(sub_topic[i]);
			sub_topic[i] = NULL;
		}
		i++;
	}

	if (tcponly == 0) {
		if ((rc = MQTTDisconnect(&client)) != 0){
			CMD_ERR("Return code from MQTT disconnect is %d\n", rc);
			network.disconnect(&network);
			return CMD_STATUS_FAIL;
		}
	} else {
		network.disconnect(&network);
	}

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_mqtt_deinit_exec(char *cmd)
{



	if (connectData.username.lenstring.len != 0)
		cmd_free(connectData.username.lenstring.data);
	if (connectData.password.lenstring.len != 0)
		cmd_free(connectData.password.lenstring.data);

	cmd_free(send_buf);
	cmd_free(recv_buf);
	cmd_free(client_name);

	OS_MutexDelete(&lock);

	connectData = (MQTTPacket_connectData)MQTTPacket_connectData_initializer;

	return CMD_STATUS_OK;
}

static struct cmd_data g_mqtt_cmds[] = {
	{ "init", 	cmd_mqtt_init_exec },
	{ "will", 	cmd_mqtt_will_exec },
	{ "user", 	cmd_mqtt_user_exec },
	{ "password", 	cmd_mqtt_password_exec },
	{ "connect", 	cmd_mqtt_connect_exec },
	{ "subscribe", 	cmd_mqtt_subscribe_exec },
	{ "unsubscribe", 	cmd_mqtt_unsubscribe_exec },
	{ "publish", 	cmd_mqtt_publish_exec },
	{ "disconnect",	cmd_mqtt_disconnect_exec },
	{ "deinit",	cmd_mqtt_deinit_exec },
};

enum cmd_status cmd_mqtt_exec(char *cmd)
{
	return cmd_exec(cmd, g_mqtt_cmds, cmd_nitems(g_mqtt_cmds));
}
