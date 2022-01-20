# MIT license; Copyright (c) 2021 Jim Mussared

# This is a BLE file server, based very loosely on the Object Transfer Service
# specification. It demonstrated transfering data over an L2CAP channel, as
# well as using notifications and GATT writes on a characteristic.

# The server supports downloading and uploading files, as well as querying
# directory listings and file sizes.

# In order to access the file server, a client must connect, then establish an
# L2CAP channel. To being an operation, a command is written to the control
# characteristic, including a command number, sequence number, and filesystem
# path. The response will be either via a notification on the control
# characteristic (e.g. file size), or via the L2CAP channel (file contents or
# directory listing).

import sys

sys.path.append("")

from micropython import const

import uasyncio as asyncio
import aioble
import bluetooth

import struct
import os

# Randomly generated UUIDs.
_FILE_SERVICE_UUID = bluetooth.UUID("0492fcec-7194-11eb-9439-0242ac130002")
_CONTROL_CHARACTERISTIC_UUID = bluetooth.UUID("0492fcec-7194-11eb-9439-0242ac130003")

# How frequently to send advertising beacons.
_ADV_INTERVAL_MS = 250_000


_COMMAND_SEND = const(0)
_COMMAND_RECV = const(1)  # Not yet implemented.
_COMMAND_LIST = const(2)
_COMMAND_SIZE = const(3)
_COMMAND_DONE = const(4)

_STATUS_OK = const(0)
_STATUS_NOT_IMPLEMENTED = const(1)
_STATUS_NOT_FOUND = const(2)

_L2CAP_PSN = const(22)
_L2CAP_MTU = const(128)


# Register GATT server.
file_service = aioble.Service(_FILE_SERVICE_UUID)
control_characteristic = aioble.Characteristic(
    file_service, _CONTROL_CHARACTERISTIC_UUID, write=True, notify=True
)
aioble.register_services(file_service)


send_file = None
recv_file = None
list_path = None
op_seq = None
l2cap_event = asyncio.Event()


def send_done_notification(connection, status=_STATUS_OK):
    global op_seq
    control_characteristic.notify(connection, struct.pack("<BBB", _COMMAND_DONE, op_seq, status))
    op_seq = None


async def l2cap_task(connection):
    global send_file, recv_file, list_path
    try:
        channel = await connection.l2cap_accept(_L2CAP_PSN, _L2CAP_MTU)
        print("channel accepted")

        while True:
            await l2cap_event.wait()
            l2cap_event.clear()

            if send_file:
                print("Sending:", send_file)
                with open(send_file, "rb") as f:
                    buf = bytearray(channel.peer_mtu)
                    mv = memoryview(buf)
                    while n := f.readinto(buf):
                        await channel.send(mv[:n])
                await channel.flush()
                send_done_notification(connection)
                send_file = None
            if recv_file:
                print("Receiving:", recv_file)
                send_done_notification(connection, _STATUS_NOT_IMPLEMENTED)
                recv_file = None
            if list_path:
                print("List:", list_path)
                try:
                    for name, _, _, size in os.ilistdir(list_path):
                        await channel.send("{}:{}\n".format(size, name))
                    await channel.send("\n")
                    await channel.flush()
                    send_done_notification(connection)
                except OSError:
                    send_done_notification(connection, _STATUS_NOT_FOUND)
                list_path = None

    except aioble.DeviceDisconnectedError:
        print("Stopping l2cap")
        return


async def control_task(connection):
    global send_file, recv_file, list_path

    try:
        with connection.timeout(None):
            while True:
                print("Waiting for write")
                await control_characteristic.written()
                msg = control_characteristic.read()
                control_characteristic.write(b"")

                if len(msg) < 3:
                    continue

                # Message is <command><seq><path...>.

                command = msg[0]
                seq = msg[1]
                file = msg[2:].decode()

                if command == _COMMAND_SEND:
                    op_seq = seq
                    send_file = file
                    l2cap_event.set()
                elif command == _COMMAND_RECV:
                    op_seq = seq
                    recv_file = file
                    l2cap_event.set()
                elif command == _COMMAND_LIST:
                    op_seq = seq
                    list_path = file
                    l2cap_event.set()
                elif command == _COMMAND_SIZE:
                    try:
                        stat = os.stat(file)
                        size = stat[6]
                        status = 0
                    except OSError as e:
                        size = 0
                        status = _STATUS_NOT_FOUND
                    control_characteristic.notify(
                        connection, struct.pack("<BBI", seq, status, size)
                    )
    except aioble.DeviceDisconnectedError:
        return


# Serially wait for connections. Don't advertise while a central is
# connected.
async def peripheral_task():
    while True:
        print("Waiting for connection")
        connection = await aioble.advertise(
            _ADV_INTERVAL_MS,
            name="mpy-file",
            services=[_FILE_SERVICE_UUID],
        )
        print("Connection from", connection.device)

        t = asyncio.create_task(l2cap_task(connection))
        await control_task(connection)
        t.cancel()

        await connection.disconnected()


# Run both tasks.
async def main():
    await peripheral_task()


asyncio.run(main())
