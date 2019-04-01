
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "osal.h"
#include "MQTTClient.h"

#define MQTT_SERVER		"172.104.178.117"
#define MQTT_PORT		"1883"
#define MQTT_TOPIC		"hello/world"
#define MQTT_CLIENT_ID	"cabrio test string"
#define MQTT_MSG_VER	"0.50"

static int arrivedcount = 0;

//log_create_module(mqtt, PRINT_LEVEL_INFO);


/**
* @brief          MQTT message RX handle
* @param[in]      MQTT received message data
* @return         None
*/
static void messageArrived(MessageData *md)
{
    MQTTMessage *message = md->message;

    //printf("Message arrived: qos %d, retained %d, dup %d, packetid %d\n", 
        //message->qos, message->retained, message->dup, message->id);
    *((char *)(message->payload +(size_t)message->payloadlen)) = '\0';
    printf( "\n--Payload:%s--\n", (char *)(message->payload));
    //printf("messageArrived\n");
    ++arrivedcount;
}

/**
* @brief          MQTT client example entry function
* @return         None
*/
void mqtt_client_test(void)
{
    int rc = 0;
    int cnt = 0;
    unsigned char msg_buf[100];     //Buffer for outgoing messages, such as unsubscribe.
    unsigned char msg_readbuf[100]; //Buffer for incoming messages, such as unsubscribe ACK.
    char buf[100];                  //Buffer for application payload.
    
    Network n;  //TCP network
    struct MQTTClient c;   //MQTT client
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    char *topic = MQTT_TOPIC;
    MQTTMessage message;
    printf("mqtt_client_test begin\n");
    //Initialize MQTT network structure
    NetworkInit(&n);
    
    //Connect to remote server
    //printf("Connect to %s:%s\n", MQTT_SERVER, MQTT_PORT);
    for(cnt = 0; cnt < 10; cnt++) {
        rc = NetworkConnect(&n, MQTT_SERVER, MQTT_PORT);
        if (rc != 0) {
            printf("TCP connect failed,status -%4X\n", -rc);
        } else {
            break;
        }
    }
    if(cnt == 10)
        goto exit;
    
    //Initialize MQTT client structure
    //printf("MQTTClientInit\n");
    MQTTClientInit(&c, &n, 12000, msg_buf, 100, msg_readbuf, 100);

    //The packet header of MQTT connection request
    data.willFlag = 0;
    data.MQTTVersion = 3;
    data.clientID.cstring = MQTT_CLIENT_ID;
    data.username.cstring = NULL;
    data.password.cstring = NULL;
    data.keepAliveInterval = 10;
    data.cleansession = 1;
    //printf("MQTTConnect\n");
    //Send MQTT connection request to the remote MQTT server
    rc = MQTTConnect(&c, &data);

    if (rc != 0) {
        printf("MQTT connect failed,status -%4d\n", -rc);
        goto exit;
    }
    //printf("MQTTSubscribe\n");
    rc = MQTTSubscribe(&c, topic, QOS1, messageArrived);
    
    // QoS 0
    sprintf(buf, "Hello World! QoS 0 message from app version %s\n", MQTT_MSG_VER);
    
    message.qos = QOS0;
    message.retained = false;
    message.dup = false;
    message.payload = (void *)buf;
    message.payloadlen = strlen(buf);
    //printf("MQTTPublish\n");
    rc = MQTTPublish(&c, topic, &message);
    
    while (arrivedcount < 1) {
        //printf("MQTTYield\n");
        MQTTYield(&c, 1000);
    }
    
    // QoS 1
    sprintf(buf, "Hello World! QoS 1 message from app version %s\n", MQTT_MSG_VER);
    message.qos = QOS1;
    message.payloadlen = strlen(buf);
    rc = MQTTPublish(&c, topic, &message);
    while (arrivedcount < 2) {
        MQTTYield(&c, 1000);
    }
    
    // QoS 2
    sprintf(buf, "Hello World! QoS 2 message from app version %s\n", MQTT_MSG_VER);
    message.qos = QOS2;
    message.payloadlen = strlen(buf);
    rc = MQTTPublish(&c, topic, &message);
    while (arrivedcount < 3) {
        MQTTYield(&c, 1000);
    }
    
    if ((rc = MQTTUnsubscribe(&c, topic)) != 0) {
        printf("The return from unsubscribe was %d\n", rc);
    }
    //printf("MQTT unsubscribe done\n");

    if ((rc = MQTTDisconnect(&c)) != 0) {
        printf("The return from disconnect was %d\n", rc);
    }
    //printf("MQTT disconnect done\n");

    n.disconnect(&n);
    //printf("Network disconnect done\n");


exit:
    printf("mqtt_client_test end\n");
    OS_TaskDelete(NULL);
}

