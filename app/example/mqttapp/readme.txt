
1.modify device&product info at the header of mqtt_example*.c

2.make:
    1).mqtt_example.c:
        aos make mqttapp@xxxx   

    2).mqtt_example_rrpc.c:
        aos make mqttapp@xxxx case=rrpc

    3).mqtt_example_multithread.c:
        aos make mqttapp@xxxx case=multithread
