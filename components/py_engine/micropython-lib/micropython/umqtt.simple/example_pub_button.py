import time
import ubinascii
import machine
from umqtt.simple import MQTTClient
from machine import Pin


# Many ESP8266 boards have active-low "flash" button on GPIO0.
button = Pin(0, Pin.IN)

# Default MQTT server to connect to
SERVER = "192.168.1.35"
CLIENT_ID = ubinascii.hexlify(machine.unique_id())
TOPIC = b"led"


def main(server=SERVER):
    c = MQTTClient(CLIENT_ID, server)
    c.connect()
    print("Connected to %s, waiting for button presses" % server)
    while True:
        while True:
            if button.value() == 0:
                break
            time.sleep_ms(20)
        print("Button pressed")
        c.publish(TOPIC, b"toggle")
        time.sleep_ms(200)

    c.disconnect()
