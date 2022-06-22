# -*- coding: utf-8 -*-

class Credentials(object):
    def __init__(self, access_key_id="", access_key_secret="", security_token=""):
        self.access_key_id = access_key_id
        self.access_key_secret = access_key_secret
        self.security_token = security_token

    def get_access_key_id(self):
        return self.access_key_id

    def get_access_key_secret(self):
        return self.access_key_secret

    def get_security_token(self):
        return self.security_token

class CredentialsProvider(object):
    def get_credentials(self):
        return

class StaticCredentialsProvider(CredentialsProvider):
    def __init__(self, access_key_id="", access_key_secret="", security_token=""):
        self.credentials = Credentials(access_key_id, access_key_secret, security_token)

    def get_credentials(self):
        return self.credentials
