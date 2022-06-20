# -*- coding: utf-8 -*-
import binascii
import time

OSS_SECURITY_TOKEN = "x-oss-security-token"
OSS_DEFAULT_CONTENT_TYPE  = "application/x-www-form-urlencoded"

_EXTRA_TYPES_MAP = {
    ".js": "application/javascript",
    ".xlsx": "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet",
    ".xltx": "application/vnd.openxmlformats-officedocument.spreadsheetml.template",
    ".potx": "application/vnd.openxmlformats-officedocument.presentationml.template",
    ".ppsx": "application/vnd.openxmlformats-officedocument.presentationml.slideshow",
    ".pptx": "application/vnd.openxmlformats-officedocument.presentationml.presentation",
    ".sldx": "application/vnd.openxmlformats-officedocument.presentationml.slide",
    ".docx": "application/vnd.openxmlformats-officedocument.wordprocessingml.document",
    ".dotx": "application/vnd.openxmlformats-officedocument.wordprocessingml.template",
    ".xlam": "application/vnd.ms-excel.addin.macroEnabled.12",
    ".xlsb": "application/vnd.ms-excel.sheet.binary.macroEnabled.12",
    ".apk": "application/vnd.android.package-archive",
    ".txt": "text/plain",
    ".py": "text/x-python"
}

OSS_CLIENT_ERROR_STATUS = -1
OSS_REQUEST_ERROR_STATUS = -2

class OssError(Exception):
    def __init__(self, status, message):
        self.status = status
        self.message = message

    def __str__(self):
        error = {'status': self.status,
                 'message': self.message}
        return str(error)

class ClientError(OssError):
    def __init__(self, message):
        OssError.__init__(self, OSS_CLIENT_ERROR_STATUS, 'ClientError: ' + message)


class RequestError(OssError):
    def __init__(self, message):
        OssError.__init__(self, OSS_REQUEST_ERROR_STATUS, 'RequestError: ' + message)

_ALPHA_NUM = 'abcdefghijklmnopqrstuvwxyz0123456789'
_HYPHEN = '-'
_BUCKET_NAME_CHARS = set(_ALPHA_NUM + _HYPHEN)

def is_valid_bucket_name(name):
    """判断是否为合法的Bucket名"""
    if len(name) < 3 or len(name) > 63:
        return False

    if name[-1] == _HYPHEN:
        return False

    if name[0] not in _ALPHA_NUM:
        return False

    return set(name) <= _BUCKET_NAME_CHARS

def content_type_by_name(name):
    """根据文件名，返回Content-Type。"""
    ext = name[name.rfind("."):].lower()
    if ext in _EXTRA_TYPES_MAP:
        return _EXTRA_TYPES_MAP[ext]

    return OSS_DEFAULT_CONTENT_TYPE

def set_content_type(headers, name):
    """根据文件名在headers里设置Content-Type。如果headers中已经存在Content-Type，则直接返回。"""
    headers = headers or {}

    if 'Content-Type' in headers:
        return headers

    content_type = content_type_by_name(name)
    if content_type:
        headers['Content-Type'] = content_type

    return headers

def to_bytes(data):
    """若输入为str（即unicode），则转为utf-8编码的bytes；其他则原样返回"""
    if isinstance(data, str):
        return data.encode('utf-8')
    else:
        return data

def to_string(data):
    """若输入为bytes，则认为是utf-8编码，并返回str"""
    if isinstance(data, bytes):
        return data.decode('utf-8')
    else:
        return data

def to_unicode(data):
    """把输入转换为unicode，要求输入是unicode或者utf-8编码的bytes。"""
    return to_string(data)

def stringify(input):
    return input

def b64encode_as_string(data):
    return to_string(binascii.b2a_base64(data)).strip("\n")

def http_date(timeval=None):
    """返回符合HTTP标准的GMT时间字符串，用strftime的格式表示就是"%a, %d %b %Y %H:%M:%S GMT"。
    但不能使用strftime，因为strftime的结果是和locale相关的。
    """
    weekstr = ("Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun")
    monthstr = ("Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec")
    datetuple = time.gmtime(time.time())
    datestr = "%s, %02d %s %04d %02d:%02d:%02d GMT" % (weekstr[datetuple[6]], datetuple[2],
        monthstr[datetuple[1] - 1], datetuple[0], datetuple[3], datetuple[4], datetuple[5])
    # print("WAHAHA:", datestr)
    return datestr

