
#include <platform_opts.h>

#if (CONFIG_EXAMPLE_AMAZON_AWS_IOT)

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include <amazon_awsiot/example_amazon_awsiot.h>
#include "mqtt/MQTTClient/MQTTClient.h"
#include "wifi_conf.h"

#ifdef MQTT_TASK
#error "You need to undef MQTT_TASK in MQTTClient.h to enable WAIT_FOR_ACK features"
#endif

/* this example requires MQTT over SSL session */
#if (MQTT_OVER_SSL)

static unsigned char mqtt_sendbuf[1024], mqtt_readbuf[1024];

#define MQTT_BROKER_SERVER   "a2zweh2b7yb784.iot.ap-southeast-1.amazonaws.com"
#define MQTT_CLIENT_ID       "amebaClient"
#define THING_NAME           "ameba"

char publishTopic[]   = "$aws/things/" THING_NAME "/shadow/update";

char *subscribeTopic[5] = {
  "$aws/things/" THING_NAME "/shadow/update/accepted",
  "$aws/things/" THING_NAME "/shadow/update/rejected",
  "$aws/things/" THING_NAME "/shadow/update/delta",
  "$aws/things/" THING_NAME "/shadow/get/accepted",
  "$aws/things/" THING_NAME "/shadow/get/rejected"
};

int led_state = 0;

/* root CA can be download here:
 *  https://www.symantec.com/content/en/us/enterprise/verisign/roots/VeriSign-Class%203-Public-Primary-Certification-Authority-G5.pem
 **/
char* rootCABuff = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIE0zCCA7ugAwIBAgIQGNrRniZ96LtKIVjNzGs7SjANBgkqhkiG9w0BAQUFADCB\n" \
"yjELMAkGA1UEBhMCVVMxFzAVBgNVBAoTDlZlcmlTaWduLCBJbmMuMR8wHQYDVQQL\n" \
"ExZWZXJpU2lnbiBUcnVzdCBOZXR3b3JrMTowOAYDVQQLEzEoYykgMjAwNiBWZXJp\n" \
"U2lnbiwgSW5jLiAtIEZvciBhdXRob3JpemVkIHVzZSBvbmx5MUUwQwYDVQQDEzxW\n" \
"ZXJpU2lnbiBDbGFzcyAzIFB1YmxpYyBQcmltYXJ5IENlcnRpZmljYXRpb24gQXV0\n" \
"aG9yaXR5IC0gRzUwHhcNMDYxMTA4MDAwMDAwWhcNMzYwNzE2MjM1OTU5WjCByjEL\n" \
"MAkGA1UEBhMCVVMxFzAVBgNVBAoTDlZlcmlTaWduLCBJbmMuMR8wHQYDVQQLExZW\n" \
"ZXJpU2lnbiBUcnVzdCBOZXR3b3JrMTowOAYDVQQLEzEoYykgMjAwNiBWZXJpU2ln\n" \
"biwgSW5jLiAtIEZvciBhdXRob3JpemVkIHVzZSBvbmx5MUUwQwYDVQQDEzxWZXJp\n" \
"U2lnbiBDbGFzcyAzIFB1YmxpYyBQcmltYXJ5IENlcnRpZmljYXRpb24gQXV0aG9y\n" \
"aXR5IC0gRzUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCvJAgIKXo1\n" \
"nmAMqudLO07cfLw8RRy7K+D+KQL5VwijZIUVJ/XxrcgxiV0i6CqqpkKzj/i5Vbex\n" \
"t0uz/o9+B1fs70PbZmIVYc9gDaTY3vjgw2IIPVQT60nKWVSFJuUrjxuf6/WhkcIz\n" \
"SdhDY2pSS9KP6HBRTdGJaXvHcPaz3BJ023tdS1bTlr8Vd6Gw9KIl8q8ckmcY5fQG\n" \
"BO+QueQA5N06tRn/Arr0PO7gi+s3i+z016zy9vA9r911kTMZHRxAy3QkGSGT2RT+\n" \
"rCpSx4/VBEnkjWNHiDxpg8v+R70rfk/Fla4OndTRQ8Bnc+MUCH7lP59zuDMKz10/\n" \
"NIeWiu5T6CUVAgMBAAGjgbIwga8wDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8E\n" \
"BAMCAQYwbQYIKwYBBQUHAQwEYTBfoV2gWzBZMFcwVRYJaW1hZ2UvZ2lmMCEwHzAH\n" \
"BgUrDgMCGgQUj+XTGoasjY5rw8+AatRIGCx7GS4wJRYjaHR0cDovL2xvZ28udmVy\n" \
"aXNpZ24uY29tL3ZzbG9nby5naWYwHQYDVR0OBBYEFH/TZafC3ey78DAJ80M5+gKv\n" \
"MzEzMA0GCSqGSIb3DQEBBQUAA4IBAQCTJEowX2LP2BqYLz3q3JktvXf2pXkiOOzE\n" \
"p6B4Eq1iDkVwZMXnl2YtmAl+X6/WzChl8gGqCBpH3vn5fJJaCGkgDdk+bW48DW7Y\n" \
"5gaRQBi5+MHt39tBquCWIMnNZBU4gcmU7qKEKQsTb47bDN0lAtukixlE0kF6BWlK\n" \
"WE9gyn6CagsCqiUXObXbf+eEZSqVir2G3l6BFoMtEMze/aiCKm0oHw0LxOXnGiYZ\n" \
"4fQRbxC1lfznQgUy286dUV4otp6F01vvpX1FQHKOtw5rDgb7MzVIcbidJ4vEZV8N\n" \
"hnacRHr2lVz2XTIIM6RUthg/aFzyQkqFOFSDX9HoLPKsEdao7WNq\n" \
"-----END CERTIFICATE-----\n";

/* Fill your certificate.pem.crt wiht LINE ENDING */
char* certificateBuff = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDWTCCAkGgAwIBAgIUE1UsPqN2mfvCGh2DLX2HWs3NOIYwDQYJKoZIhvcNAQEL\n" \
"BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g\n" \
"SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTE2MDYyNzA2MzQ0\n" \
"NVoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0\n" \
"ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBANM8/fzFJeZoS2yIf0Yy\n" \
"seFqUlnwNwNsg0G4U+3mUPj47ogs3jLDFA208P85S3qnfDaUY553wYY3BcIlBzpp\n" \
"y7vstrgapTOxH2c/Nrk/QfDqg/gUBSZs24c12WvyqlfDnKcIsRxALbfO0yMWKATB\n" \
"fFfnnZRCOVO6eKcW2O3ptfYDH8tIhrzCAEAAAF6CYmlSzyw9KrWh7ypYCp3AcJ/Y\n" \
"Fp4+SsF6xxLRuLOk37NJL8HfuIappkUIN7seGU9Y6Bo2YgOj9yBXyW/dmV3IxKVW\n" \
"SUSMSdIP+pc/b8lFfnE14yFtfK7jKhdy4XOh9LpOMJYs0i74UVQKJs7NYL8MXHIu\n" \
"FlUCAwEAAaNgMF4wHwYDVR0jBBgwFoAUoTuxNa4LQJd07hfP4se/TevgQ0AwHQYD\n" \
"VR0OBBYEFAQ9fAKopGZ52+f+w0dJMAwTi/hVMAwGA1UdEwEB/wQCMAAwDgYDVR0P\n" \
"AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQB6YQ+w6YTCZYWOlZfH09D8WhwV\n" \
"zSAzcEUdWH5T7bNK1N5r7/5zVIedseFxPUqtuC8j+C7CWDuLykpI8A2WAnayXOXn\n" \
"UPc/SNIet0nGts50cd4zzg7xMMqseGVdSMLAjFwqI8npJp9Ij8inrsf5f1hY0nhb\n" \
"Zb6FkVghlYhgmWv9p949kBQ6ODrtiyQqAYjzzZa957HwR7ajZfMIWm+HaV6f+NYq\n" \
"O43eQbt14xrRQNjAhSQaoVT64PH1TA6XiyoF25xlnrrxDXGAo5cxVDa1MmXu249X\n" \
"9z2+uzKgCYuugQk+w+8JmC6hR0EH4q4+ydsNkYTC0LK3MndHumA1Dj3OwUfA\n" \
"-----END CERTIFICATE-----\n";

/* Fill your private.pem.key wiht LINE ENDING */
char* privateKeyBuff = \
"-----BEGIN RSA PRIVATE KEY-----\n" \
"MIIEpAIBAAKCAQEA0zz9/MUl5mhLbIh/RjKx4WpSWfA3A2yDQbhT7eZQ+PjuiCze\n" \
"MsMUDbTw/zlLeqd8NpRjnnfBhjcFwiUHOmnLu+y2uBqlM7EfZz82uT9B8OqD+BQF\n" \
"JmzbhzXZa/KqV8OcpwixHEAtt87TIxYoBMF8V+edlEI5U7p4pxbY7em19gMfy0iG\n" \
"vMIAQAAAXoJiaVLPLD0qtaHvKlgKncBwn9gWnj5KwXrHEtG4s6Tfs0kvwd+4hqmm\n" \
"RQg3ux4ZT1joGjZiA6P3IFfJb92ZXcjEpVZJRIxJ0g/6lz9vyUV+cTXjIW18ruMq\n" \
"F3Lhc6H0uk4wlizSLvhRVAomzs1gvwxcci4WVQIDAQABAoIBAQCObSVjdRokzFVu\n" \
"jGokTrIZJrL36Ttul4+4lCwiz5PxCwbp0jbPSzEOPN3xeBQoUx0xP8QbaOuXLyo2\n" \
"yPiirgqsXuKkJ3MT820VFE41gS1Y3wa0EeuXCPbLp8c9PZUVL9ND3Fxui+dHc7Bw\n" \
"i9PXwQ2xx308JZq5lZUcNA93+oixohfoN4kgPfT3je7yPvn0j1rMHfjQPrSvpIvk\n" \
"P2XA66BZf8YS9k6lpJLzslntusHO4ZoqdQxaqa3a79qawUjkJ4G4qx3aDvzqdjXU\n" \
"DTFgTLI8sicWl4Icx3vqCgZfGWYuzRX5Ka5eUZYkwv4PgUOSynVvc6n3fjX/2F1X\n" \
"aPEqLuIBAoGBAO+Akr7F5LnRyLxLEeywjruGxvG5vHS41YvRcHSukrNLoK4mRMRA\n" \
"fTqJe8oZCRuOGsDBkRujgnSEUJhqt2+kGf9dvv5pCS35PFvTzawbpe8HX/JjitDb\n" \
"kKxAd3edsbOo3ZCOwYLFwMPMIMzemqFX67cXyQwOUhcGf6N7YBKhA6JhAoGBAOHK\n" \
"AarQJXo3C69SpWMvei3zmV8pUdVLPiiiNb1q/pustt+hykRhXNrngV3ncB7mJBET\n" \
"Q+36c5F04A6EBSp5Nr8EdiPnsFwk+ILKx7wQ6y0E29xIRLxryyu27rMAGpk8D1Oy\n" \
"8F2mX23qGWQAsMi2sxKk7o+2EGoUF1f/HKeDcOB1AoGBAM5johG8P2rSGYYJuxyY\n" \
"2adIcdCFGp4LWhrvFVW3yruvhHwOhlwIpuH28DIseOjCANPy+rUypoz6KOnvrLwM\n" \
"Ukr54kkjAsIXcahAUZDrEod1d31NwqZRT87gjxMJVcVY0/Zqzt9+wqr4EZv6iI5Z\n" \
"UcuqN5qoDJ3C/+NFwnjLQHKBAoGAQI5zX5VXwdPPQXeN1ggTFORba7vyq9txkEig\n" \
"uOHInlYJi3NE07xKwkQC1wh/JDaFBWTOvVIojOQv07ani3dQ0djCto1d/VqMu0ij\n" \
"RwBHXX3QJvF6xazEUGFjakaTVFC5ySKWWxBgpJqUW+VepmSmWqRRmUFi/BF2gzBr\n" \
"zvFj6qkCgYANcCc6wXqWNwmAnH/bfTRXunUgt3AAxmMXtkakhXAjElgFymgmXhTN\n" \
"Huu00WjTeZqOSl8bE0Ki/bcRojlx2QMMaEHGSVRL28uPJCJ4HeBAH3m22dtceFgF\n" \
"+ZHhF5b6NFe6EeLBcd/TGLtuKvc/jeru8tciNrPj9MTSJR9T0TYxhw==\n" \
"-----END RSA PRIVATE KEY-----\n";

#define MQTT_SELECT_TIMEOUT 1
void messageArrived(MessageData* data)
{
    mqtt_printf(MQTT_INFO, "Message arrived on topic %.*s: %.*s\n", data->topicName->lenstring.len, data->topicName->lenstring.data,
        data->message->payloadlen, (char*)data->message->payload);

    if ((strstr(data->topicName->lenstring.data, "/shadow/get/accepted") != NULL) || (strstr(data->topicName->lenstring.data, "/shadow/update/accepted") != NULL)) {
        char *pch;

        // payload format: {"state":{"reported":{"led":1},"desired":{"led":0}},"metadata":{"reported":{"led":{"timestamp":1466996558}},"desired":{"led":{"timestamp":1466996558}}},"version":7,"timestamp":1466996558}
        pch = strstr((char*)data->message->payload, "\"desired\":{\"led\":");
        if (pch != NULL) {
            pch += strlen("\"desired\":{\"led\":");
            led_state = *pch - '0';
            mqtt_printf(MQTT_INFO, "Update led state to %d", led_state);
        }
    }
}

int reconnect(MQTTClient* client, char *address, int port, MQTTPacket_connectData* connectData) {
    int rc = 0;

    while (1) {
        do {
            if (wifi_is_ready_to_transceive(RTW_STA_INTERFACE) != RTW_SUCCESS) {
                break;
            }
            mqtt_printf(MQTT_INFO, "Wi-Fi connected.");

            if (client->isconnected == 0) {
                
                if (NetworkConnect(client->ipstack, address, port) != 0) {
                    break;
                }
                mqtt_printf(MQTT_INFO, "\"%s\" Connected", address);

                if (MQTTConnect(client, connectData) != 0) {
                    break;
                }
                mqtt_printf(MQTT_INFO, "MQTT Connected");
            }
        } while (0);

        if (client->isconnected) {
            break;
        }

        vTaskDelay(1000);
    }

    return rc;
}

//This example is original and cannot restart if failed. To use this example, define WAIT_FOR_ACK and not define MQTT_TASK in MQTTClient.h
static void example_amazon_awsiot_thread(void *pvParameters)
{
    /* connect to MQTT_BROKER_SERVER, subscribe to a topic, send and receive messages regularly every 5 sec */
    MQTTClient client;
    Network network;
    int rc = 0, count = 0, i;
    MQTTPacket_connectData connectData = MQTTPacket_connectData_initializer;
    char* address = MQTT_BROKER_SERVER;
    char* pub_topic = publishTopic;
    
    NetworkInit(&network);
    network.rootCA = rootCABuff;
    network.clientCA = certificateBuff;
    network.private_key = privateKeyBuff;
    network.use_ssl = 1;

    MQTTClientInit(&client, &network, 30000, mqtt_sendbuf, sizeof(mqtt_sendbuf), mqtt_readbuf, sizeof(mqtt_readbuf));

    connectData.MQTTVersion = 3;
    connectData.clientID.cstring = MQTT_CLIENT_ID;
    connectData.keepAliveInterval = 15 * 60;

    reconnect(&client, address, 8883, &connectData);

    for (i=0; i<5; i++) {
        mqtt_printf(MQTT_INFO, "Subscribe to Topic: %s", subscribeTopic[i]);
        if ((rc = MQTTSubscribe(&client, subscribeTopic[i], QOS1, messageArrived)) != 0) {
            mqtt_printf(MQTT_INFO, "Return code from MQTT subscribe is %d\n", rc);
        }
    }

    while (++count)
    {
        MQTTMessage message;
        char payload[300];

        message.qos = QOS1;
        message.retained = 0;
        message.payload = payload;
        sprintf(payload,  "{\"state\":{\"reported\":{\"led\":%d}},\"clientToken\":\"ameba\"}", led_state);
        message.payloadlen = strlen(payload);

        if ((rc = MQTTPublish(&client, pub_topic, &message)) != 0)
            mqtt_printf(MQTT_INFO,"Return code from MQTT publish is %d\n", rc);
        if ((rc = MQTTYield(&client, 10000)) != 0) {
            mqtt_printf(MQTT_INFO,"Return code from yield is %d\n", rc);
        }

        if (client.isconnected == 0) {
            reconnect(&client, address, 8883, &connectData);

            for (i=0; i<5; i++) {
                mqtt_printf(MQTT_INFO, "Subscribe to Topic: %s", subscribeTopic[i]);
                if ((rc = MQTTSubscribe(&client, subscribeTopic[i], QOS1, messageArrived)) != 0) {
                    mqtt_printf(MQTT_INFO, "Return code from MQTT subscribe is %d\n", rc);
                }
            }
        }

        vTaskDelay(1000);
    }
    /* do not return */
}

void example_amazon_awsiot(void)
{    
    if(xTaskCreate(example_amazon_awsiot_thread, ((const char*)"example_amazon_awsiot_thread"), 4096, NULL, tskIDLE_PRIORITY + 4, NULL) != pdPASS)
        printf("\n\r%s xTaskCreate(init_thread) failed", __FUNCTION__);
}

#endif // #if (MQTT_OVER_SSL)
/*-----------------------------------------------------------*/

#endif // #if (CONFIG_EXAMPLE_AMAZON_AWS_IOT)
