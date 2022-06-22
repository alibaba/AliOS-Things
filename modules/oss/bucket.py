# -*- coding: utf-8 -*-

from .utils import *
import urequests


class Request(object):
    def __init__(self, method, url, data=None, params=None, headers= None):
        self.method = method
        self.url = url
        self.data= data
        self.params = params or {}
        self.headers = headers

class _Base(object):
    def __init__(self, auth, endpoint):
        self.auth = auth
        self.endpoint = endpoint

    def _do(self, method, bucket_name, key, **kwargs):
        headers=kwargs["headers"]
        data=kwargs["data"]
        url = "https://{1}.{0}/{2}".format(self.endpoint, bucket_name, key)
        req = Request(method, url, data, headers=headers)

        self.auth._sign_request(req, bucket_name, key)

        resp = urequests.request(method, url, data=data, headers=req.headers)
        if resp.status_code // 100 != 2:
            raise RequestError('Request to : {0} error, error code:{1}'.format(url, resp.status_code))

        return url


class Bucket(_Base):
    """用于Bucket和Object操作的类，诸如创建，上传Object等。

    :param auth: 包含了用户认证信息的Auth对象
    :type auth: oss2.Auth

    :param str endpoint: 访问域名或者CNAME
    :param str bucket_name: Bucket名

    """

    def __init__(self, auth, endpoint, bucket_name):
        super(Bucket, self).__init__(auth, endpoint)

        self.bucket_name = bucket_name.strip()
        if is_valid_bucket_name(self.bucket_name) is not True:
            raise ClientError("The bucket_name is invalid, please check it.")

    def put_object(self, key, data, headers=None):
        """上传一个普通文件。

        用法 ::
            >>> bucket.put_object('readme.txt', 'content of readme.txt')
            >>> with open(u'local_file.txt', 'rb') as f:
            >>>     bucket.put_object('remote_file.txt', f)

        :param key: 上传到OSS的文件名

        :param data: 待上传的内容。
        :type data: bytes，str或file-like object

        :return: :class:`PutObjectResult <oss2.models.PutObjectResult>`
        """
        if not headers:
            headers = set_content_type({}, key)

        resp = self.__do_object('PUT', key, data=data, headers=headers)

        return resp

    def put_object_from_file(self, key, filename):
        """上传一个本地文件到OSS的普通文件。

        :param str key: 上传到OSS的文件名
        :param str filename: 本地文件名，需要有可读权限

        :return: :class:`PutObjectResult <oss2.models.PutObjectResult>`
        """
        headers = set_content_type({}, filename)
        f = open(filename, 'rb')
        url = self.put_object(key, f.read(), headers=headers)
        f.close()
        return url

    def __do_object(self, method, key, **kwargs):
        return self._do(method, self.bucket_name, key, **kwargs)
