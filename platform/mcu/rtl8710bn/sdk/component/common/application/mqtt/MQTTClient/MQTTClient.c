/*******************************************************************************
 * Copyright (c) 2014, 2015 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Allan Stockdill-Mander/Ian Craggs - initial API and implementation and/or initial documentation
 *******************************************************************************/
#include "MQTTClient.h"
const char * const msg_types_str[]=
{
	"Reserved",
	"CONNECT",
	"CONNACK",
	"PUBLISH",
	"PUBACK",
	"PUBREC",
	"PUBREL",
	"PUBCOMP",
	"SUBSCRIBE",
	"SUBACK",
	"UNSUBSCRIBE",
	"UNSUBACK",
	"PINGREQ",
	"PINGRESP",
	"DISCONNECT",
	"Reserved"
};
const char * const mqtt_status_str[]=
{
	"MQTT_START",
	"MQTT_CONNECT",
	"MQTT_SUBTOPIC",
	"MQTT_RUNNING"
};

static void NewMessageData(MessageData* md, MQTTString* aTopicName, MQTTMessage* aMessage) {
    md->topicName = aTopicName;
    md->message = aMessage;
}


static int getNextPacketId(MQTTClient *c) {
    return c->next_packetid = (c->next_packetid == MAX_PACKET_ID) ? 1 : c->next_packetid + 1;
}


static int sendPacket(MQTTClient* c, int length, Timer* timer)
{
    int rc = FAILURE, 
        sent = 0;
    
    while (sent < length && !TimerIsExpired(timer))
    {
        rc = c->ipstack->mqttwrite(c->ipstack, &c->buf[sent], length, TimerLeftMS(timer));
        if (rc < 0)  // there was an error writing the data
            break;
        sent += rc;
    }
    if (sent == length)
    {
        TimerCountdown(&c->ping_timer, c->keepAliveInterval); // record the fact that we have successfully sent the packet
        rc = SUCCESS;
    }
    else{
        rc = FAILURE;
        mqtt_printf(MQTT_DEBUG, "Send packet failed");
    }

    if (c->ipstack->my_socket < 0) {
        c->isconnected = 0;
    }

    return rc;
}


void MQTTClientInit(MQTTClient* c, Network* network, unsigned int command_timeout_ms,
		unsigned char* sendbuf, size_t sendbuf_size, unsigned char* readbuf, size_t readbuf_size)
{
    int i;
    c->ipstack = network;
    
    for (i = 0; i < MAX_MESSAGE_HANDLERS; ++i)
        c->messageHandlers[i].topicFilter = 0;
    c->command_timeout_ms = command_timeout_ms;
    c->buf = sendbuf;
    c->buf_size = sendbuf_size;
    c->readbuf = readbuf;
    c->readbuf_size = readbuf_size;
    c->isconnected = 0;
    c->ping_outstanding = 0;
    c->defaultMessageHandler = NULL;
	c->next_packetid = 1;
    c->ipstack->m2m_rxevent = 0;
    c->mqttstatus = MQTT_START;
    TimerInit(&c->cmd_timer);
    TimerInit(&c->ping_timer);
}


static int decodePacket(MQTTClient* c, int* value, int timeout)
{
    unsigned char i;
    int multiplier = 1;
    int len = 0;
    const int MAX_NO_OF_REMAINING_LENGTH_BYTES = 4;

    *value = 0;
    do
    {
        int rc = MQTTPACKET_READ_ERROR;

        if (++len > MAX_NO_OF_REMAINING_LENGTH_BYTES)
        {
            rc = MQTTPACKET_READ_ERROR; /* bad data */
            goto exit;
        }
        rc = c->ipstack->mqttread(c->ipstack, &i, 1, timeout);
        if (rc != 1)
            goto exit;
        *value += (i & 127) * multiplier;
        multiplier *= 128;
    } while ((i & 128) != 0);
exit:
    return len;
}


static int readPacket(MQTTClient* c, Timer* timer)
{
    int rc = FAILURE;
    MQTTHeader header = {0};
    int len = 0;
    int rem_len = 0;

    /* 1. read the header byte.  This has the packet type in it */
    if (c->ipstack->mqttread(c->ipstack, c->readbuf, 1, TimerLeftMS(timer)) != 1){
        mqtt_printf(MQTT_MSGDUMP, "read packet header failed");
        goto exit;
    }
    len = 1;
    /* 2. read the remaining length.  This is variable in itself */
    decodePacket(c, &rem_len, TimerLeftMS(timer));
    len += MQTTPacket_encode(c->readbuf + 1, rem_len); /* put the original remaining length back into the buffer */

    if(len + rem_len > c->readbuf_size){
        mqtt_printf(MQTT_WARNING, "rem_len = %d, read buffer will overflow", rem_len);
        rc = BUFFER_OVERFLOW;
        goto exit;
    }
    /* 3. read the rest of the buffer using a callback to supply the rest of the data */
    if (rem_len > 0 && (c->ipstack->mqttread(c->ipstack, c->readbuf + len, rem_len, TimerLeftMS(timer)) != rem_len)){
        mqtt_printf(MQTT_MSGDUMP, "read the rest of the data failed");
        goto exit;
    }
    header.byte = c->readbuf[0];
    rc = header.bits.type;
exit:
    if (c->ipstack->my_socket < 0) {
        c->isconnected = 0;
    }
    return rc;
}


// assume topic filter and name is in correct format
// # can only be at end
// + and # can only be next to separator
static char isTopicMatched(char* topicFilter, MQTTString* topicName)
{
    char* curf = topicFilter;
    char* curn = topicName->lenstring.data;
    char* curn_end = curn + topicName->lenstring.len;
    
    while (*curf && curn < curn_end)
    {
        if (*curn == '/' && *curf != '/')
            break;
        if (*curf != '+' && *curf != '#' && *curf != *curn)
            break;
        if (*curf == '+')
        {   // skip until we meet the next separator, or end of string
            char* nextpos = curn + 1;
            while (nextpos < curn_end && *nextpos != '/')
                nextpos = ++curn + 1;
        }
        else if (*curf == '#')
            curn = curn_end - 1;    // skip until end of string
        curf++;
        curn++;
    };
    
    return (curn == curn_end) && (*curf == '\0');
}


int deliverMessage(MQTTClient* c, MQTTString* topicName, MQTTMessage* message)
{
    int i;
    int rc = FAILURE;

    // we have to find the right message handler - indexed by topic
    for (i = 0; i < MAX_MESSAGE_HANDLERS; ++i)
    {
        if (c->messageHandlers[i].topicFilter != 0 && (MQTTPacket_equals(topicName, (char*)c->messageHandlers[i].topicFilter) ||
                isTopicMatched((char*)c->messageHandlers[i].topicFilter, topicName)))
        {
            if (c->messageHandlers[i].fp != NULL)
            {
                MessageData md;
                NewMessageData(&md, topicName, message);
                c->messageHandlers[i].fp(&md);
                rc = SUCCESS;
            }
        }
    }
    
    if (rc == FAILURE && c->defaultMessageHandler != NULL) 
    {
        MessageData md;
        NewMessageData(&md, topicName, message);
        c->defaultMessageHandler(&md);
        rc = SUCCESS;
    }   
    
    return rc;
}


int keepalive(MQTTClient* c)
{
    int rc = FAILURE;

    if (c->keepAliveInterval == 0)
    {
        rc = SUCCESS;
        goto exit;
    }

    if (TimerIsExpired(&c->ping_timer))
    {
        if (!c->ping_outstanding)
        {
            Timer timer;
            TimerInit(&timer);
            TimerCountdownMS(&timer, 1000);
            int len = MQTTSerialize_pingreq(c->buf, c->buf_size);
            if (len > 0 && (rc = sendPacket(c, len, &timer)) == SUCCESS) // send the ping packet
                c->ping_outstanding = 1;
        }
    }

exit:
    return rc;
}


int cycle(MQTTClient* c, Timer* timer)
{
    // read the socket, see what work is due
    unsigned short packet_type = readPacket(c, timer);
    int len = 0, rc = SUCCESS;
	
    if (packet_type == (unsigned short)BUFFER_OVERFLOW || packet_type == (unsigned short)FAILURE) {
        rc = FAILURE;
        goto exit;
    }

    mqtt_printf(MQTT_DEBUG, "Read packet type: %d", packet_type);
    switch (packet_type)
    {
        case CONNACK:
        case PUBACK:
        case SUBACK:
            break;
        case PUBLISH:
        {
            MQTTString topicName;
            MQTTMessage msg;
            int intQoS;
            if (MQTTDeserialize_publish(&msg.dup, &intQoS, &msg.retained, &msg.id, &topicName,
               (unsigned char**)&msg.payload, (int*)&msg.payloadlen, c->readbuf, c->readbuf_size) != 1)
                goto exit;
            msg.qos = (enum QoS)intQoS;
            deliverMessage(c, &topicName, &msg);
            if (msg.qos != QOS0)
            {
                if (msg.qos == QOS1)
                    len = MQTTSerialize_ack(c->buf, c->buf_size, PUBACK, 0, msg.id);
                else if (msg.qos == QOS2)
                    len = MQTTSerialize_ack(c->buf, c->buf_size, PUBREC, 0, msg.id);
                if (len <= 0)
                    rc = FAILURE;
                else {
#if 1
                    sendPacket(c, len, timer);
#else
                    // it's odd that ACK PUB also need success
                    rc = sendPacket(c, len, timer);
#endif
                }
                if (rc == FAILURE)
                    goto exit; // there was a problem
            }
            break;
        }
        case PUBREC:
        {
            unsigned short mypacketid;
            unsigned char dup, type;
            if (MQTTDeserialize_ack(&type, &dup, &mypacketid, c->readbuf, c->readbuf_size) != 1)
                rc = FAILURE;
            else if ((len = MQTTSerialize_ack(c->buf, c->buf_size, PUBREL, 0, mypacketid)) <= 0)
                rc = FAILURE;
            else if ((rc = sendPacket(c, len, timer)) != SUCCESS) // send the PUBREL packet
                rc = FAILURE; // there was a problem
            if (rc == FAILURE)
                goto exit; // there was a problem
            break;
        }
        case PUBREL:
        {
            unsigned short mypacketid;
            unsigned char dup, type;
            if (MQTTDeserialize_ack(&type, &dup, &mypacketid, c->readbuf, c->readbuf_size) != 1)
                rc = FAILURE;
            else if ((len = MQTTSerialize_ack(c->buf, c->buf_size, PUBCOMP, 0, mypacketid)) <= 0)
                rc = FAILURE;
            else if ((rc = sendPacket(c, len, timer)) != SUCCESS) // send the PUBREL packet
                rc = FAILURE; // there was a problem
            if (rc == FAILURE)
                goto exit; // there was a problem
            break;
        }
        case PUBCOMP:
            break;
        case PINGRESP:
            c->ping_outstanding = 0;
            break;
    }
exit:
    keepalive(c);
    if (rc == SUCCESS)
        rc = packet_type;
    return rc;
}


int MQTTYield(MQTTClient* c, int timeout_ms)
{
    int rc = SUCCESS;
    Timer timer;

    TimerInit(&timer);
    TimerCountdownMS(&timer, timeout_ms);

	do
    {
        if (cycle(c, &timer) == FAILURE)
        {
            rc = FAILURE;
            break;
        }
	} while (!TimerIsExpired(&timer));
        
    return rc;
}

int waitfor(MQTTClient* c, int packet_type, Timer* timer)
{
    int rc = FAILURE;
    
    do
    {
        if (TimerIsExpired(timer))
            break; // we timed out
    }
    while ((rc = cycle(c, timer)) != packet_type);  
    
    return rc;
}


int MQTTConnect(MQTTClient* c, MQTTPacket_connectData* options)
{
    Timer connect_timer;
    int rc = FAILURE;
    MQTTPacket_connectData default_options = MQTTPacket_connectData_initializer;
    int len = 0;

    if (c->isconnected) /* don't send connect packet again if we are already connected */
		goto exit;
    
    TimerInit(&connect_timer);
    TimerCountdownMS(&connect_timer, c->command_timeout_ms);

    if (options == 0)
        options = &default_options; /* set default options if none were supplied */
    
    c->keepAliveInterval = options->keepAliveInterval;
    TimerCountdown(&c->ping_timer, c->keepAliveInterval);
    if ((len = MQTTSerialize_connect(c->buf, c->buf_size, options)) <= 0)
        goto exit;
    if ((rc = sendPacket(c, len, &connect_timer)) != SUCCESS)  // send the connect packet
        goto exit; // there was a problem
    
#if defined(WAIT_FOR_ACK)	
    // this will be a blocking call, wait for the connack
    if (waitfor(c, CONNACK, &connect_timer) == CONNACK)
    {
        unsigned char connack_rc = 255;
        unsigned char sessionPresent = 0;
        if (MQTTDeserialize_connack(&sessionPresent, &connack_rc, c->readbuf, c->readbuf_size) == 1)
            rc = connack_rc;
        else
            rc = FAILURE;
    }
    else{
        mqtt_printf(MQTT_DEBUG, "Not received CONNACK");
        rc = FAILURE;
    }
#endif 
exit:
    if (rc == SUCCESS)
        c->isconnected = 1;

    return rc;
}


int MQTTSubscribe(MQTTClient* c, const char* topicFilter, enum QoS qos, messageHandler messageHandler)
{ 
    int rc = FAILURE;  
    Timer timer;
    int len = 0;
    MQTTString topic = MQTTString_initializer;
    topic.cstring = (char *)topicFilter;
    
    if (!c->isconnected)
		goto exit;

    TimerInit(&timer);
    TimerCountdownMS(&timer, c->command_timeout_ms);
    
    len = MQTTSerialize_subscribe(c->buf, c->buf_size, 0, getNextPacketId(c), 1, &topic, (int*)&qos);
    if (len <= 0)
        goto exit;
    if ((rc = sendPacket(c, len, &timer)) != SUCCESS) // send the subscribe packet
        goto exit;             // there was a problem
    
#if defined(WAIT_FOR_ACK)
    if (waitfor(c, SUBACK, &timer) == SUBACK)      // wait for suback 
    {
        int count = 0, grantedQoS = -1;
        unsigned short mypacketid;
        if (MQTTDeserialize_suback(&mypacketid, 1, &count, &grantedQoS, c->readbuf, c->readbuf_size) == 1)
            rc = grantedQoS; // 0, 1, 2 or 0x80 
        if (rc != 0x80)
        {
            int i;
            for (i = 0; i < MAX_MESSAGE_HANDLERS; ++i)
            {
                if (c->messageHandlers[i].topicFilter == topicFilter)
                {
                    rc = 0;
                    goto exit;  //already subscribed
                }
            }
            for (i = 0; i < MAX_MESSAGE_HANDLERS; ++i)
            {
                if (c->messageHandlers[i].topicFilter == 0)
                {
                    c->messageHandlers[i].topicFilter = topicFilter;
                    c->messageHandlers[i].fp = messageHandler;
                    rc = 0;
                    break;
                }
            }
        }
    }
    else 
        rc = FAILURE;
#endif
exit:
    return rc;
}


int MQTTUnsubscribe(MQTTClient* c, const char* topicFilter)
{   
    int rc = FAILURE;
    Timer timer;    
    MQTTString topic = MQTTString_initializer;
    topic.cstring = (char *)topicFilter;
    int len = 0;

    if (!c->isconnected)
		goto exit;

    TimerInit(&timer);
    TimerCountdownMS(&timer, c->command_timeout_ms);
    
    if ((len = MQTTSerialize_unsubscribe(c->buf, c->buf_size, 0, getNextPacketId(c), 1, &topic)) <= 0)
        goto exit;
    if ((rc = sendPacket(c, len, &timer)) != SUCCESS) // send the subscribe packet
        goto exit; // there was a problem
    
#if defined(WAIT_FOR_ACK)
    if (waitfor(c, UNSUBACK, &timer) == UNSUBACK)
    {
        unsigned short mypacketid;  // should be the same as the packetid above
        if (MQTTDeserialize_unsuback(&mypacketid, c->readbuf, c->readbuf_size) == 1)
            rc = 0;
            int i;
            for (i = 0; i < MAX_MESSAGE_HANDLERS; ++i)
            {
                if (c->messageHandlers[i].topicFilter == topicFilter)
                {
                    c->messageHandlers[i].topicFilter = 0;
                    c->messageHandlers[i].fp = NULL;
                }
            }
    }
    else
        rc = FAILURE;
#endif
exit:
    return rc;
}


int MQTTPublish(MQTTClient* c, const char* topicName, MQTTMessage* message)
{
    int rc = FAILURE;
    Timer timer;   
    MQTTString topic = MQTTString_initializer;
    topic.cstring = (char *)topicName;
    int len = 0;

    if (!c->isconnected)
		goto exit;

    TimerInit(&timer);
    TimerCountdownMS(&timer, c->command_timeout_ms);

    if (message->qos == QOS1 || message->qos == QOS2)
        message->id = getNextPacketId(c);
    
    len = MQTTSerialize_publish(c->buf, c->buf_size, 0, message->qos, message->retained, message->id, 
              topic, (unsigned char*)message->payload, message->payloadlen);
    if (len <= 0)
        goto exit;
    if ((rc = sendPacket(c, len, &timer)) != SUCCESS) // send the subscribe packet
        goto exit; // there was a problem
    
#if defined(WAIT_FOR_ACK)
    if (message->qos == QOS1)
    {
        if (waitfor(c, PUBACK, &timer) == PUBACK)
        {
            unsigned short mypacketid;
            unsigned char dup, type;
            if (MQTTDeserialize_ack(&type, &dup, &mypacketid, c->readbuf, c->readbuf_size) != 1)
                rc = FAILURE;
        }
        else{
            rc = FAILURE;
            mqtt_printf(MQTT_DEBUG, "Not received PUBACK");
        }
    }
    else if (message->qos == QOS2)
    {
        if (waitfor(c, PUBCOMP, &timer) == PUBCOMP)
        {
            unsigned short mypacketid;
            unsigned char dup, type;
            if (MQTTDeserialize_ack(&type, &dup, &mypacketid, c->readbuf, c->readbuf_size) != 1)
                rc = FAILURE;
        }
        else{
            rc = FAILURE;
            mqtt_printf(MQTT_DEBUG, "Not received PUBCOMP");
        }
    }
#endif 
exit:
    return rc;
}


int MQTTDisconnect(MQTTClient* c)
{  
    int rc = FAILURE;
    Timer timer;     // we might wait for incomplete incoming publishes to complete
    int len = 0;

    TimerInit(&timer);
    TimerCountdownMS(&timer, c->command_timeout_ms);

	len = MQTTSerialize_disconnect(c->buf, c->buf_size);
    if (len > 0)
        rc = sendPacket(c, len, &timer);            // send the disconnect packet
        
    c->isconnected = 0;

    return rc;
}

#if defined(MQTT_TASK)
void MQTTSetStatus(MQTTClient* c, int mqttstatus)
{	
	c->mqttstatus = mqttstatus;
	mqtt_printf(MQTT_INFO, "Set mqtt status to %s", mqtt_status_str[mqttstatus]);
}

int MQTTDataHandle(MQTTClient* c, fd_set *readfd, MQTTPacket_connectData *connectData, messageHandler messageHandler, char* address, char* topic)
{	
	short packet_type = 0;
	int rc = 0;
	int mqttstatus = c->mqttstatus;
	int mqtt_rxevent = 0;
	int mqtt_fd = c->ipstack->my_socket;

	mqtt_rxevent = (mqtt_fd >= 0) ? FD_ISSET( mqtt_fd, readfd) : 0;

	if(mqttstatus == MQTT_START) {
		mqtt_printf(MQTT_INFO, "MQTT start");
		if(c->isconnected){
			c->isconnected = 0;
		}
		mqtt_printf(MQTT_INFO, "Connect Network \"%s\"", address);
		if((rc = NetworkConnect(c->ipstack, address, 1883)) != 0){
			mqtt_printf(MQTT_INFO, "Return code from network connect is %d\n", rc);
			goto exit;
		}
		mqtt_printf(MQTT_INFO, "\"%s\" Connected", address);
		mqtt_printf(MQTT_INFO, "Start MQTT connection");
		TimerInit(&c->cmd_timer);
		TimerCountdownMS(&c->cmd_timer, c->command_timeout_ms);
		if ((rc = MQTTConnect(c, connectData)) != 0){
			mqtt_printf(MQTT_INFO, "Return code from MQTT connect is %d\n", rc);
			goto exit;
		}
		MQTTSetStatus(c, MQTT_CONNECT);
		goto exit;
	}
	
	if(mqtt_rxevent){
		c->ipstack->m2m_rxevent = 0;
		Timer timer;
		TimerInit(&timer);
		TimerCountdownMS(&timer, 1000);
		packet_type = readPacket(c, &timer);
		if(packet_type > 0 && packet_type < 15)
			mqtt_printf(MQTT_DEBUG, "Read packet type is %s", msg_types_str[packet_type]);
		else{
			mqtt_printf(MQTT_DEBUG, "Read packet type is %d", packet_type);
			MQTTSetStatus(c, MQTT_START);
			c->ipstack->disconnect(c->ipstack);
			rc = FAILURE;
			goto exit;
		}
	}
	switch(mqttstatus){
		case MQTT_CONNECT:
			if (packet_type == CONNACK)
			{
				unsigned char connack_rc = 255;
				unsigned char sessionPresent = 0;
				if (MQTTDeserialize_connack(&sessionPresent, &connack_rc, c->readbuf, c->readbuf_size) == 1){
					rc = connack_rc;
					mqtt_printf(MQTT_INFO, "MQTT Connected");
					TimerInit(&c->cmd_timer);
					TimerCountdownMS(&c->cmd_timer, c->command_timeout_ms);
					if ((rc = MQTTSubscribe(c, topic, QOS2, messageHandler)) != 0){
						mqtt_printf(MQTT_INFO, "Return code from MQTT subscribe is %d\n", rc);
					}else{
						mqtt_printf(MQTT_INFO, "Subscribe to Topic: %s", topic);
						MQTTSetStatus(c, MQTT_SUBTOPIC);
					}
				}else{
					mqtt_printf(MQTT_DEBUG, "Deserialize CONNACK failed");
					rc = FAILURE;
				}
			}else if(TimerIsExpired(&c->cmd_timer)){
				mqtt_printf(MQTT_DEBUG, "Not received CONNACK");
				rc = FAILURE;
			}
			if(rc == FAILURE){
				MQTTSetStatus(c, MQTT_START);
			}
			break;
		case MQTT_SUBTOPIC:
			if(packet_type == SUBACK){
				int count = 0, grantedQoS = -1;
				unsigned short mypacketid;
				int isSubscribed = 0;
				if (MQTTDeserialize_suback(&mypacketid, 1, &count, &grantedQoS, c->readbuf, c->readbuf_size) == 1){
					rc = grantedQoS; // 0, 1, 2 or 0x80 
					mqtt_printf(MQTT_DEBUG, "grantedQoS: %d", grantedQoS);
				}
				if (rc != 0x80)
				{
					int i;
					for (i = 0; i < MAX_MESSAGE_HANDLERS; ++i)
					{
						if (c->messageHandlers[i].topicFilter == topic)
						{   
							isSubscribed = 1;
							break;
						}
					}
					if(!isSubscribed)
						for (i = 0; i < MAX_MESSAGE_HANDLERS; ++i)
						{
							if (c->messageHandlers[i].topicFilter == 0)
							{
								c->messageHandlers[i].topicFilter = topic;
								c->messageHandlers[i].fp = messageHandler;
								break;
							}
						}
					rc = 0;
					MQTTSetStatus(c, MQTT_RUNNING);
				}
			}else if(TimerIsExpired(&c->cmd_timer)){
				mqtt_printf(MQTT_DEBUG, "Not received SUBACK");
				rc = FAILURE;
			}
			if(rc == FAILURE){
				MQTTSetStatus(c, MQTT_START);
			}
			break;
		case MQTT_RUNNING:
			if(packet_type>0){
				int len = 0;
				Timer timer;
				TimerInit(&timer);
				TimerCountdownMS(&timer, 10000);	
				switch(packet_type){
					case CONNACK:
						break;
					case PUBACK:
					{
						unsigned short mypacketid;
						unsigned char dup, type;
						if (MQTTDeserialize_ack(&type, &dup, &mypacketid, c->readbuf, c->readbuf_size) != 1)
							rc = FAILURE;
						break;
					}
					case SUBACK:
						break;
					case UNSUBACK:
						break;
					case PUBLISH:
					{
						MQTTString topicName;
						MQTTMessage msg;
						int intQoS;
						if (MQTTDeserialize_publish(&msg.dup, &intQoS, &msg.retained, &msg.id, &topicName,
						   (unsigned char**)&msg.payload, (int*)&msg.payloadlen, c->readbuf, c->readbuf_size) != 1)
						{
							rc = FAILURE;
							mqtt_printf(MQTT_DEBUG, "Deserialize PUBLISH failed");
							goto exit;
						}
							
						msg.qos = (enum QoS)intQoS;
						deliverMessage(c, &topicName, &msg);
						if (msg.qos != QOS0)
						{
							if (msg.qos == QOS1){
								len = MQTTSerialize_ack(c->buf, c->buf_size, PUBACK, 0, msg.id);
								mqtt_printf(MQTT_DEBUG, "send PUBACK");
							}else if (msg.qos == QOS2){
								len = MQTTSerialize_ack(c->buf, c->buf_size, PUBREC, 0, msg.id);
								mqtt_printf(MQTT_DEBUG, "send PUBREC");
							}else{
								mqtt_printf(MQTT_DEBUG, "invalid QoS: %d", msg.qos);
							}
							if (len <= 0){
								rc = FAILURE;
								mqtt_printf(MQTT_DEBUG, "Serialize_ack failed");
								goto exit;
							}else{
								if((rc = sendPacket(c, len, &timer)) == FAILURE){
									MQTTSetStatus(c, MQTT_START);
									goto exit; // there was a problem
								}
							}
						}
						break;
					}
					case PUBREC:
					{
						unsigned short mypacketid;
						unsigned char dup, type;
						if (MQTTDeserialize_ack(&type, &dup, &mypacketid, c->readbuf, c->readbuf_size) != 1){
							mqtt_printf(MQTT_DEBUG, "Deserialize PUBREC failed");
							rc = FAILURE;
						}else if ((len = MQTTSerialize_ack(c->buf, c->buf_size, PUBREL, 0, mypacketid)) <= 0){
							mqtt_printf(MQTT_DEBUG, "Serialize PUBREL failed");
							rc = FAILURE;
						}else if ((rc = sendPacket(c, len, &timer)) != SUCCESS){ // send the PUBREL packet
							rc = FAILURE; // there was a problem
							MQTTSetStatus(c, MQTT_START);
						}
						break;
					}
					case PUBREL:
					{
						unsigned short mypacketid;
						unsigned char dup, type;
						if (MQTTDeserialize_ack(&type, &dup, &mypacketid, c->readbuf, c->readbuf_size) != 1){
							mqtt_printf(MQTT_DEBUG, "Deserialize PUBREL failed");
							rc = FAILURE;
						}else if ((len = MQTTSerialize_ack(c->buf, c->buf_size, PUBCOMP, 0, mypacketid)) <= 0){
							mqtt_printf(MQTT_DEBUG, "Serialize PUBCOMP failed");
							rc = FAILURE;
						}else if ((rc = sendPacket(c, len, &timer)) != SUCCESS){ // send the PUBCOMP packet
							rc = FAILURE; // there was a problem
							MQTTSetStatus(c, MQTT_START);
						}
						break;
					}
					case PUBCOMP:
						break;
					case PINGRESP:
						c->ping_outstanding = 0;
						break;
				}
			}
			keepalive(c);
			break;			
		default:
			break;
	}
exit:
	return rc;
}

#endif


