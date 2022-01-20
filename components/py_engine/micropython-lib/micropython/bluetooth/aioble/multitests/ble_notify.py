# Test notification-specific behavior.

import sys

sys.path.append("")

from micropython import const
import time, machine

import uasyncio as asyncio
import aioble
import bluetooth

TIMEOUT_MS = 5000

SERVICE_UUID = bluetooth.UUID("A5A5A5A5-FFFF-9999-1111-5A5A5A5A5A5A")
CHAR_UUID = bluetooth.UUID("00000000-1111-2222-3333-444444444444")


# Acting in peripheral role.
async def instance0_task():
    service = aioble.Service(SERVICE_UUID)
    characteristic = aioble.Characteristic(service, CHAR_UUID, read=True, notify=True)
    aioble.register_services(service)

    multitest.globals(BDADDR=aioble.config("mac"))
    multitest.next()

    # Wait for central to connect to us.
    print("advertise")
    connection = await aioble.advertise(
        20_000, adv_data=b"\x02\x01\x06\x04\xffMPY", timeout_ms=TIMEOUT_MS
    )
    print("connected")

    # Send a subscribed-write (but client isn't subscribed, won't send anything).
    multitest.wait("discovery")
    await asyncio.sleep_ms(100)
    characteristic.write("before-subscribe", send_update=True)

    # Send a subscribed-write (now client is subscribed, client should get notified).
    multitest.wait("subscribed")
    await asyncio.sleep_ms(100)
    characteristic.write("after-subscribe", send_update=True)

    # Send a subscribed-write (now client is unsubscribed, won't send anything).
    multitest.wait("unsubscribed")
    await asyncio.sleep_ms(100)
    characteristic.write("after-unsubscribe", send_update=True)

    # Send 5 direct notifications.
    multitest.wait("start-direct")
    for i in range(5):
        # Send 1 notification each time, except for 3 quick notifications the third time.
        # The client should only see the last one.
        for j in range(3 if i == 2 else 1):
            if j > 0:
                await asyncio.sleep_ms(100)
            msg = "direct-{}-{}".format(i, j)
            print("notify", msg)
            characteristic.notify(connection, msg)

        # Tell client to wait for notification.
        multitest.broadcast("notified")
        # Wait until client is ready for next notification.
        multitest.wait("next")

    # Wait for the central to disconnect.
    await connection.disconnected(timeout_ms=TIMEOUT_MS)
    print("disconnected")


def instance0():
    try:
        asyncio.run(instance0_task())
    finally:
        aioble.stop()


# Acting in central role.
async def instance1_task():
    multitest.next()

    # Connect to peripheral and then disconnect.
    print("connect")
    device = aioble.Device(*BDADDR)
    connection = await device.connect(timeout_ms=TIMEOUT_MS)

    # Discover characteristics.
    service = await connection.service(SERVICE_UUID)
    print("service", service.uuid)
    characteristic = await service.characteristic(CHAR_UUID)
    print("characteristic", characteristic.uuid)

    # Expect to not receive a notification (not subscribed).
    multitest.broadcast("discovery")
    try:
        await characteristic.notified(timeout_ms=500)
        print("fail")
        return
    except asyncio.TimeoutError:
        print("no notification")

    # Subscribe and expect a notification.
    await characteristic.subscribe(notify=True)
    multitest.broadcast("subscribed")
    value = await characteristic.notified()
    print("notified", value)

    # Unsubscribe, and expect not to receive a notification.
    await characteristic.subscribe(notify=False)
    multitest.broadcast("unsubscribed")
    try:
        await characteristic.notified(timeout_ms=500)
        print("fail")
        return
    except asyncio.TimeoutError:
        print("no notification")

    # Receive 5 notifications.
    multitest.broadcast("start-direct")
    for i in range(5):
        multitest.wait("notified")
        await asyncio.sleep_ms(200)
        value = await characteristic.notified()
        print("notified", value)

        # Expect that after receiving a notification we don't get another one
        # until we broadcast to the server.
        try:
            value = await characteristic.notified(timeout_ms=100)
            print("unexpected notify", value)
        except asyncio.TimeoutError:
            pass

        multitest.broadcast("next")

    # Disconnect from peripheral.
    print("disconnect")
    await connection.disconnect(timeout_ms=TIMEOUT_MS)
    print("disconnected")


def instance1():
    try:
        asyncio.run(instance1_task())
    finally:
        aioble.stop()
