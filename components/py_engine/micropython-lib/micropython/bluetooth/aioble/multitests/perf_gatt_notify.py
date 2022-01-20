# Ping-pong GATT notifications between two devices.

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

# How long to run the test for.
_NUM_NOTIFICATIONS = const(50)


def register_server():
    server_service = aioble.Service(SERVICE_UUID)
    server_characteristic = aioble.Characteristic(
        server_service, CHAR_UUID, read=True, write=True, notify=True, indicate=True
    )
    aioble.register_services(server_service)
    return server_characteristic


async def discover_server(connection):
    client_service = await connection.service(SERVICE_UUID)
    return await client_service.characteristic(CHAR_UUID)


# Acting in peripheral role.
async def instance0_task():
    server_characteristic = register_server()

    multitest.globals(BDADDR=aioble.config("mac"))
    multitest.next()

    connection = await aioble.advertise(
        20_000, adv_data=b"\x02\x01\x06\x04\xffMPY", timeout_ms=TIMEOUT_MS
    )

    client_characteristic = await discover_server(connection)

    # Give the central enough time to discover chars.
    await asyncio.sleep_ms(500)

    ticks_start = time.ticks_ms()

    for i in range(_NUM_NOTIFICATIONS):
        # Send a notification and wait for a response.
        server_characteristic.notify(connection, "peripheral" + str(i))
        await client_characteristic.notified()

    ticks_end = time.ticks_ms()
    ticks_total = time.ticks_diff(ticks_end, ticks_start)
    print(
        "Acknowledged {} notifications in {} ms. {} ms/notification.".format(
            _NUM_NOTIFICATIONS, ticks_total, ticks_total // _NUM_NOTIFICATIONS
        )
    )

    # Disconnect the central.
    await connection.disconnect()


def instance0():
    try:
        asyncio.run(instance0_task())
    finally:
        aioble.stop()


# Acting in central role.
async def instance1_task():
    server_characteristic = register_server()

    multitest.next()

    device = aioble.Device(*BDADDR)
    connection = await device.connect(timeout_ms=TIMEOUT_MS)

    client_characteristic = await discover_server(connection)

    for i in range(_NUM_NOTIFICATIONS):
        # Wait for notification and send response.
        data = await client_characteristic.notified()
        server_characteristic.notify(connection, b"central" + data)

    # Wait for the peripheral to disconnect us.
    await connection.disconnected(timeout_ms=20000)


def instance1():
    try:
        asyncio.run(instance1_task())
    finally:
        aioble.stop()
