src = Split('''
    MQTTConnectClient.c
	MQTTDeserializePublish.c
	MQTTPacket.c
	MQTTSerializePublish.c
	MQTTSubscribeClient.c
	MQTTUnsubscribeClient.c
''')

component = aos_component('MQTTPacket', src)