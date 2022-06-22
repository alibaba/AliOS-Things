# -*- coding: UTF-8 -*-

"""
Wrapper for micropython's urequests module to provide HaaS Python http APIs.
For detailed API description, please refer to:
https://haas.iot.aliyun.com/haasapi/index.html?spm=a2cpu.b16145223.0.0.595660b1Vllyah#/Python/docs/zh-CN/haas_extended_api/http
"""

import urequests
from _thread import start_new_thread
import uio

def request(data, cb):
    """Make a http request (asynchronous)

    Args:
        data: A dictionary that contains the information of request.
        cb: A function(data) that will be executed after this request complete.

    Returns:
        0: success, <0: error

    Raises:
        No
    """
    def request_thread(func_cb, method, url, http_headers, params):
        resp = urequests.request(method, url, data=params, headers=http_headers)

        request_header = {"method": method, "url": url, "headers": http_headers}
        body = resp.text
        func_cb_arg = {"header": request_header, "body": body }
        func_cb(func_cb_arg)

    if type(data) is not dict:
        return -1

    if not callable(cb):
        return -2

    if "method" not in data:
        return -3

    if "url" not in data:
        return -4

    method = data["method"]
    url = data["url"]
    if "headers" in data:
        headers = data["headers"]
    else:
        headers = {"content-type": "application/json"}

    # timeout not working now.
    if "timeout" in data:
        timeout = data["timeout"]

    if "params" in data:
        params = data["params"]
    else:
        params = None

    start_new_thread(request_thread, (cb, method, url, headers, params))

    return 0


def download(data, cb):
    """Download file to local from a http server (asynchronous)

    Args:
        data: A dictionary that contains the information of download, e.g.
              data = {'url': 'http://httpbin.org/image/png', 'filepath': '/httpbin.dat'}
        cb: A function that will be called as cb("success") or cb("fail") after this download complete.

    Returns:
        0: success, <0: error

    Raises:
        No
    """

    def download_thread(func_cb, url, filepath):
        """A new thread entry to make a download http request"""

        fd = None

        try:
            resp = urequests.get(url)
            fd = uio.open(filepath, "w")
            fd.write(resp.content)
        except:
            if (fd):
                fd.close()
            func_cb("fail")

        if (fd):
            fd.close()
        func_cb("success")

    if type(data) is not dict:
        return -1

    if not callable(cb):
        return -2

    if "url" not in data:
        return -3

    if "filepath" not in data:
        return -4

    start_new_thread(download_thread, (cb, data["url"], data["filepath"]))
    return 0
