#
# uaiohttpclient - fetch URL passed as command line argument.
#
import uasyncio as asyncio
import uaiohttpclient as aiohttp


def print_stream(resp):
    print((yield from resp.read()))
    return
    while True:
        line = yield from reader.readline()
        if not line:
            break
        print(line.rstrip())


def run(url):
    resp = yield from aiohttp.request("GET", url)
    print(resp)
    yield from print_stream(resp)


import sys
import logging

logging.basicConfig(level=logging.INFO)
url = sys.argv[1]
loop = asyncio.get_event_loop()
loop.run_until_complete(run(url))
loop.close()
