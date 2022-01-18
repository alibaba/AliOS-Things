# Test for shutting down and restarting the BLE stack.

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

_L2CAP_PSN = const(22)
_L2CAP_MTU = const(128)


# Acting in peripheral role.
async def instance0_task():
    multitest.globals(BDADDR=aioble.config("mac"))
    multitest.next()

    for i in range(3):
        service = aioble.Service(SERVICE_UUID)
        characteristic = aioble.Characteristic(service, CHAR_UUID, read=True)
        aioble.register_services(service)

        # Write initial characteristic value.
        characteristic.write("periph{}".format(i))

        multitest.broadcast("connect-{}".format(i))

        # Wait for central to connect to us.
        print("advertise")
        connection = await aioble.advertise(
            20_000, adv_data=b"\x02\x01\x06\x04\xffMPY", timeout_ms=TIMEOUT_MS
        )
        print("connected")

        multitest.broadcast("connected-{}".format(i))

        for j in range(3):
            channel = await connection.l2cap_accept(_L2CAP_PSN, _L2CAP_MTU)
            print("channel accepted")

            buf = bytearray(10)
            n = await channel.recvinto(buf)
            print("recv", n, buf[:n])

            multitest.broadcast("recv-{}-{}".format(i, j))

            await channel.disconnected(5000)
            print("channel disconnected")

        # Wait for the central to disconnect.
        await connection.disconnected(timeout_ms=TIMEOUT_MS)
        print("disconnected")

        # Shutdown aioble + modbluetooth.
        print("shutdown")
        aioble.stop()


def instance0():
    try:
        asyncio.run(instance0_task())
    finally:
        aioble.stop()


# Acting in central role.
async def instance1_task():
    multitest.next()

    for i in range(3):
        multitest.wait("connect-{}".format(i))
        # Connect to peripheral.
        print("connect")
        device = aioble.Device(*BDADDR)
        connection = await device.connect(timeout_ms=TIMEOUT_MS)

        multitest.wait("connected-{}".format(i))

        # Discover characteristics.
        service = await connection.service(SERVICE_UUID)
        print("service", service.uuid)
        characteristic = await service.characteristic(CHAR_UUID)
        print("characteristic", characteristic.uuid)

        # Issue read of characteristic, should get initial value.
        print("read", await characteristic.read(timeout_ms=TIMEOUT_MS))

        for j in range(3):
            print("connecting channel")
            channel = await connection.l2cap_connect(_L2CAP_PSN, _L2CAP_MTU)
            print("channel connected")

            await channel.send("l2cap-{}-{}".format(i, j))
            await channel.flush()

            multitest.wait("recv-{}-{}".format(i, j))

            print("disconnecting channel")
            await channel.disconnect()
            print("channel disconnected")

            await asyncio.sleep_ms(100)

        # Disconnect from peripheral.
        print("disconnect")
        await connection.disconnect(timeout_ms=TIMEOUT_MS)
        print("disconnected")

        # Shutdown aioble.
        print("shutdown")
        aioble.stop()


def instance1():
    try:
        asyncio.run(instance1_task())
    finally:
        aioble.stop()
