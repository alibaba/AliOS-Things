/**
 * @copyright Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <memory.h>
#include "MQTTClient.h"

#include <stdio.h>
#include <signal.h>
#include <sys/time.h>

#if AOS_COMP_CLI
#include "aos/cli.h"
#endif

/* 三元组 */
#define EXAMPLE_PRODUCT_KEY			"a1nEew2TMm8"
#define EXAMPLE_DEVICE_NAME			"abcd"
#define EXAMPLE_DEVICE_SECRET       "5719c4da9103cea09daaa9d70a513c65"

/* declare the external function aiotMqttSign() */
extern int aiotMqttSign(const char *productKey, const char *deviceName, const char *deviceSecret, char clientId[150], char username[65], char password[65]);

void messageArrived(MessageData* md)
{
	MQTTMessage* message = md->message;

	printf("%.*s\t", md->topicName->lenstring.len, md->topicName->lenstring.data);
	printf("%.*s\n", (int)message->payloadlen, (char*)message->payload);
}

// void aliyunIotPlatform(char clientId[150], char username[64], char password[65])
// {
// 	if ((rc = aiotMqttSign(EXAMPLE_PRODUCT_KEY, EXAMPLE_DEVICE_NAME, EXAMPLE_DEVICE_SECRET, clientId, username, password) < 0)) {
// 		printf("aiotMqttSign -%0x4x\n", -rc);
// 		return -1;
// 	}
// }

int mqtt_comp_example(int argc, char** argv)
{
	int rc = 0;

	/* setup the buffer, it must big enough */
	unsigned char buf[2000];
	unsigned char readbuf[2000];

	Network n;
	MQTTClient c;

	/* connect aliyun iot platform should deal with connect parameters */
#if CONFIG_AIOT_SIGN
	char *host = EXAMPLE_PRODUCT_KEY".iot-as-mqtt.cn-shanghai.aliyuncs.com";
	short port = 443;
	const char *subTopic = "/"EXAMPLE_PRODUCT_KEY"/"EXAMPLE_DEVICE_NAME"/user/get";
	const char *pubTopic = "/"EXAMPLE_PRODUCT_KEY"/"EXAMPLE_DEVICE_NAME"/user/update";
	char clientId[150] = {0};
	char username[65] = {0};
	char password[65] = {0};

	if ((rc = aiotMqttSign(EXAMPLE_PRODUCT_KEY, EXAMPLE_DEVICE_NAME, EXAMPLE_DEVICE_SECRET, clientId, username, password) < 0)) {
		printf("aiotMqttSign -%0x4x\n", -rc);
		return -1;
	}
	printf("clientid: %s\n", clientId);
	printf("username: %s\n", username);
	printf("password: %s\n", password);

#else
	char *host = "10.101.170.55";  //使用mosquitto搭建的linux服务器
	short port = 1883;
	const char *subTopic = "test/aiot/sub_topic";
	const char *pubTopic = "test/aiot/pub_topic";
	char clientId[150] = "haas";
	char username[65] = {0};
	char password[65] = {0};
#endif
	/* network init and establish network to aliyun IoT platform */
	NetworkInit(&n);

	rc = NetworkConnect(&n, host, port);
	if(rc != 0){
		printf("Network Connect failed:%d\n", rc);
		return;
	}else{
		printf("Network Connect Success!\n");
	}

	/* init mqtt client */
	MQTTClientInit(&c, &n, 1000, buf, sizeof(buf), readbuf, sizeof(readbuf));

	/* set the default message handler */
	c.defaultMessageHandler = messageArrived;

	/* set mqtt connect parameter */
	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
	data.willFlag = 0;
	data.MQTTVersion = 3;
	data.clientID.cstring = clientId;
	data.username.cstring = username;
	data.password.cstring = password;
	data.keepAliveInterval = 60;
	data.cleansession = 1;
	printf("Connecting to %s:%d\n", host, port);

	rc = MQTTConnect(&c, &data);
	if(rc != 0){
		printf("MQTT Connect server failed:%d\n", rc);
		return;
	}else{
		printf("MQTT Connect Success!\n");
	}

	rc = MQTTSubscribe(&c, subTopic, 1, messageArrived);
	if(rc != 0){
		printf("MQTT Subscribe failed:%d\n", rc);
		return;
	}else{
		printf("MQTT Subscribe Success! Topic:%s\n",subTopic);
	}

	int cnt = 0;
    unsigned int msgid = 0;
	while (1)
	{
		MQTTYield(&c, 5000);

		if (++cnt % 5 == 0) {
			MQTTMessage msg = {
				QOS1,
				0,
				0,
				0,
				"Hello world",
				strlen("Hello world"),
			};
            msg.id = ++msgid;
			rc = MQTTPublish(&c, pubTopic, &msg);
			printf("MQTTPublish %d, msgid %d\n", rc, msgid);

			if(msgid==10){
				break;
			}
		}
	}

	printf("MQTT example exit!\n");

	MQTTDisconnect(&c);
	NetworkDisconnect(&n);

	return 0;
}


#if AOS_COMP_CLI
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(mqtt_comp_example, mqtt_example, mqtt component base example)
#endif