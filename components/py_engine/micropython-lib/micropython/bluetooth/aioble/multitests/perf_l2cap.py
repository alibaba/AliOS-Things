import sys

sys.path.append("")

from micropython import const
import time, machine

import uasyncio as asyncio
import aioble
import bluetooth
import random

TIMEOUT_MS = 5000

_L2CAP_PSM = const(22)
_L2CAP_MTU = const(512)

_PAYLOAD_LEN = const(_L2CAP_MTU)
_NUM_PAYLOADS = const(20)

_RANDOM_SEED = 22


# Acting in peripheral role.
async def instance0_task():
    multitest.globals(BDADDR=aioble.config("mac"))
    multitest.next()

    connection = await aioble.advertise(
        20_000, adv_data=b"\x02\x01\x06\x04\xffMPY", timeout_ms=TIMEOUT_MS
    )

    channel = await connection.l2cap_accept(_L2CAP_PSM, _L2CAP_MTU, timeout_ms=TIMEOUT_MS)

    random.seed(_RANDOM_SEED)

    buf = bytearray(_PAYLOAD_LEN)

    for i in range(_NUM_PAYLOADS):
        for j in range(_PAYLOAD_LEN):
            buf[j] = random.randint(0, 255)
        await channel.send(buf)
    await channel.flush()

    await asyncio.sleep_ms(500)

    await channel.disconnect()

    # Disconnect the central.
    await connection.disconnect()


def instance0():
    try:
        asyncio.run(instance0_task())
    finally:
        aioble.stop()


# Acting in central role.
async def instance1_task():
    multitest.next()

    device = aioble.Device(*BDADDR)
    connection = await device.connect(timeout_ms=TIMEOUT_MS)

    await asyncio.sleep_ms(500)

    channel = await connection.l2cap_connect(_L2CAP_PSM, _L2CAP_MTU, timeout_ms=TIMEOUT_MS)

    random.seed(_RANDOM_SEED)

    buf = bytearray(_PAYLOAD_LEN)

    recv_bytes, recv_correct = 0, 0
    expected_bytes = _PAYLOAD_LEN * _NUM_PAYLOADS

    ticks_first_byte = 0
    while recv_bytes < expected_bytes:
        n = await channel.recvinto(buf)
        if not ticks_first_byte:
            ticks_first_byte = time.ticks_ms()
        recv_bytes += n
        for i in range(n):
            if buf[i] == random.randint(0, 255):
                recv_correct += 1

    ticks_end = time.ticks_ms()
    total_ticks = time.ticks_diff(ticks_end, ticks_first_byte)

    print(
        "Received {}/{} bytes in {} ms. {} B/s".format(
            recv_bytes, recv_correct, total_ticks, recv_bytes * 1000 // total_ticks
        )
    )

    # Wait for the peripheral to disconnect us.
    await connection.disconnected(timeout_ms=20000)


def instance1():
    try:
        asyncio.run(instance1_task())
    finally:
        aioble.stop()
