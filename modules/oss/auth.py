# -*- coding: utf-8 -*-

import hmac
from .utils import *
from .credentials import StaticCredentialsProvider

class AuthBase(object):
    """用于保存用户AccessKeyId、AccessKeySecret，以及计算签名的对象。"""
    def __init__(self, credentials_provider):
        self.credentials_provider = credentials_provider

_DEFAULT_ADDITIONAL_HEADERS = set(['range',
                                   'if-modified-since'])

def v2_uri_encode(raw_text):
    raw_text = to_bytes(raw_text)

    res = ''
    for b in raw_text:
        if isinstance(b, int):
            c = chr(b)
        else:
            c = b

        if (c >= 'A' and c <= 'Z') or (c >= 'a' and c <= 'z')\
            or (c >= '0' and c <= '9') or c in ['_', '-', '~', '.']:
            res += c
        else:
            res += "%{0:02X}".format(ord(c))

    return res

class ProviderAuth(AuthBase):
    """签名版本2
    hmac.sha256认证算法
    默认构造函数同父类AuthBase，需要传递credentials_provider
    """
    _subresource_key_set = frozenset(
        ['response-content-type', 'response-content-language',
         'response-cache-control', 'logging', 'response-content-encoding',
         'acl', 'uploadId', 'uploads', 'partNumber', 'group', 'link',
         'delete', 'website', 'location', 'objectInfo', 'objectMeta',
         'response-expires', 'response-content-disposition', 'cors', 'lifecycle',
         'restore', 'qos', 'referer', 'stat', 'bucketInfo', 'append', 'position', 'security-token',
         'live', 'comp', 'status', 'vod', 'startTime', 'endTime', 'x-oss-process',
         'symlink', 'callback', 'callback-var', 'tagging', 'encryption', 'versions',
         'versioning', 'versionId', 'policy', 'requestPayment', 'x-oss-traffic-limit', 'qosInfo', 'asyncFetch',
         'x-oss-request-payer', 'sequential', 'inventory', 'inventoryId', 'continuation-token', 'callback',
         'callback-var', 'worm', 'wormId', 'wormExtend', 'replication', 'replicationLocation',
         'replicationProgress', 'transferAcceleration']
    )

    def _sign_request(self, req, bucket_name, key):
        credentials = self.credentials_provider.get_credentials()
        if credentials.get_security_token():
            req.headers[OSS_SECURITY_TOKEN] = credentials.get_security_token()

        additional_headers = self.__get_additional_headers(req, _DEFAULT_ADDITIONAL_HEADERS)
        req.headers["Date"] = http_date()
        signature = self.__make_signature(req, bucket_name, key, additional_headers, credentials)
        if additional_headers:
            req.headers['authorization'] = "OSS2 AccessKeyId:{0},AdditionalHeaders:{1},Signature:{2}"\
                .format(credentials.get_access_key_id(), ';'.join(additional_headers), signature)
        else:
            req.headers['authorization'] = "OSS2 AccessKeyId:{0},Signature:{1}".format(
                credentials.get_access_key_id(), signature)

    def __make_signature(self, req, bucket_name, key, additional_headers, credentials):
        string_to_sign = self.__get_string_to_sign(req, bucket_name, key, additional_headers)

        # print('WAHAHA:Make signature: string to be signed = {0}'.format(string_to_sign))

        # h = hmac.new(to_bytes(credentials.get_access_key_secret()), to_bytes(string_to_sign), hashlib.sha256)
        hmac_data = hmac.sha256(credentials.get_access_key_secret(), string_to_sign)
        return b64encode_as_string(binascii.unhexlify(hmac_data.replace(' ', '')))

    def __get_additional_headers(self, req, in_additional_headers):
        # we add a header into additional_headers only if it is already in req's headers.

        additional_headers = set(h.lower() for h in in_additional_headers)
        keys_in_header = set(k.lower() for k in req.headers.keys())

        return additional_headers & keys_in_header

    def __get_resource_string(self, req, bucket_name, key):
        if bucket_name:
            encoded_uri = v2_uri_encode('/' + bucket_name + '/' + key)
        else:
            encoded_uri = v2_uri_encode('/')

        return encoded_uri + self.__get_canonalized_query_string(req)

    def __get_canonalized_query_string(self, req):
        encoded_params = {}
        for param, value in req.params.items():
            encoded_params[v2_uri_encode(param)] = v2_uri_encode(value)

        if not encoded_params:
            return ''

        sorted_params = sorted(encoded_params.items(), key=lambda e: e[0])
        return '?' + '&'.join(self.__param_to_query(k, v) for k, v in sorted_params)

    def __param_to_query(self, k, v):
        if v:
            return k + '=' + v
        else:
            return k

    def __get_string_to_sign(self, req, bucket_name, key, additional_header_list):
        verb = req.method
        content_md5 = req.headers.get('Content-MD5', '')
        content_type = req.headers.get('Content-Type', '')
        date = req.headers.get('Date', '')

        canonicalized_oss_headers = self.__get_canonicalized_oss_headers(req, additional_header_list)
        additional_headers = ';'.join(sorted(additional_header_list))
        canonicalized_resource = self.__get_resource_string(req, bucket_name, key)

        return verb + '\n' +\
            content_md5 + '\n' +\
            content_type + '\n' +\
            date + '\n' +\
            canonicalized_oss_headers +\
            additional_headers + '\n' +\
            canonicalized_resource

    def __get_canonicalized_oss_headers(self, req, additional_headers):
        """
        :param additional_headers: 小写的headers列表, 并且这些headers都不以'x-oss-'为前缀.
        """
        canon_headers = []

        for k, v in req.headers.items():
            lower_key = k.lower()
            if lower_key.startswith('x-oss-') or lower_key in additional_headers:
                canon_headers.append((lower_key, v))

        canon_headers.sort(key=lambda x: x[0])

        return ''.join(v[0] + ':' + v[1] + '\n' for v in canon_headers)

class Auth(ProviderAuth):
    """签名版本2
    """
    def __init__(self, access_key_id, access_key_secret):
        credentials_provider = StaticCredentialsProvider(access_key_id.strip(), access_key_secret.strip())
        super(Auth, self).__init__(credentials_provider)
