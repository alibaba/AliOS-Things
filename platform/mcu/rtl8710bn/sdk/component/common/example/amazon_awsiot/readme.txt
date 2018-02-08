AMAZON AWS IOT EXAMPLE

Description:
Connect to Amazon AWS IoT. It requires MQTT protocol over SSL session. The original MQTT library client handler does
not support IP port for MQTT over SLL (8883). So we implement it without client handler. You need to undef MQTT_TASK
to enable related configuration.

Configuration:
Modify MQTT_BROKER_SERVER, THING_NAME, rootCABuff, certificateBuff, privateKeyBuff based on you account settings.

Execution:
You need to connect to WiFi manually by AT command. After connected the mqtt connection will preceed.

