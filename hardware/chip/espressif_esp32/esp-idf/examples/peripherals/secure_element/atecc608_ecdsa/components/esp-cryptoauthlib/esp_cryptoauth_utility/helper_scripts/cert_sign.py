#!/usr/bin/env python
# Copyright 2020 Espressif Systems (Shanghai) Co., Ltd.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
from cryptography import x509
from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import serialization
from cryptography.x509.oid import NameOID
from cryptography.hazmat.primitives import hashes
from builtins import str
from datetime import datetime
import binascii

# Loads private key
def load_privatekey(key_file_path, password=None):
    key_file = open(key_file_path, "rb")
    key = key_file.read()
    key_file.close()
    return serialization.load_pem_private_key(key, password=password, backend=default_backend())


# Loads certificate
def load_certificate(cert_file_path):
    cert_file = open(cert_file_path, "rb")
    ca_cert = cert_file.read()
    cert_file.close()
    return x509.load_pem_x509_certificate(ca_cert, default_backend())


def encode_dates(nvb_time, expire_years):
    enc_dates = bytearray(b'\x00' * 3)
    enc_dates[0] = (enc_dates[0] & 0x07) | ((((nvb_time.year - 2000) & 0x1F) << 3) & 0xFF)
    enc_dates[0] = (enc_dates[0] & 0xF8) | ((((nvb_time.month) & 0x0F) >> 1) & 0xFF)
    enc_dates[1] = (enc_dates[1] & 0x7F) | ((((nvb_time.month) & 0x0F) << 7) & 0xFF)
    enc_dates[1] = (enc_dates[1] & 0x83) | (((nvb_time.day & 0x1F) << 2) & 0xFF)
    enc_dates[1] = (enc_dates[1] & 0xFC) | (((nvb_time.hour & 0x1F) >> 3) & 0xFF)
    enc_dates[2] = (enc_dates[2] & 0x1F) | (((nvb_time.hour & 0x1F) << 5) & 0xFF)
    enc_dates[2] = (enc_dates[2] & 0xE0) | ((expire_years & 0x1F) & 0xFF)
    enc_dates = bytes(enc_dates)
    return enc_dates

def sign_csr(cert_sign_req, ca_cert, ca_privkey, device_sn, nva_years):

    csr = x509.load_pem_x509_csr(cert_sign_req, default_backend())

    nvb_time = datetime.utcnow()
    nvb_time = nvb_time.replace(minute=0,second=0)
    nva_time = nvb_time
    nva_time = nva_time.replace(year=nvb_time.year + nva_years)
    expire_years = 0
    enc_dates = encode_dates(nvb_time, expire_years)
    # SAH256 hash of the public key and encoded dates
    digest = hashes.Hash(hashes.SHA256(), backend=default_backend())
    pub_nums = csr.public_key().public_numbers()


    try:
        pubkey = pub_nums.x.to_bytes(32, byteorder='big', signed=False)
        pubkey += pub_nums.y.to_bytes(32, byteorder='big', signed=False)
    except AttributeError: # In case of python2 above code block will raise AttributeError
        pubkey = bytes(bytearray.fromhex(hex(pub_nums.x)[2:-1] + hex(pub_nums.y)[2:-1]))

    digest.update(pubkey)
    digest.update(enc_dates)
    raw_sn = bytearray(digest.finalize()[:16])
    raw_sn[0] = raw_sn[0] & 0x7F  # Force MSB bit to 0 to ensure positive integer
    raw_sn[0] = raw_sn[0] | 0x40  # Force next bit to 1 to ensure the integer won't be trimmed in ASN.1 DER encoding
    try:
        cert_sn = int.from_bytes(raw_sn, byteorder='big', signed=False)
    except AttributeError: # In case of python2 above code block will raise AttributeError
        cert_sn = int(binascii.hexlify(raw_sn), 16)

    dev_sn = str(device_sn.upper())

    device_subject = x509.Name([
        x509.NameAttribute(NameOID.ORGANIZATION_NAME, ca_cert.subject.get_attributes_for_oid(NameOID.ORGANIZATION_NAME)[0].value),
        x509.NameAttribute(NameOID.COMMON_NAME,dev_sn),
    ])

    device_cert = x509.CertificateBuilder().subject_name(
        device_subject
    ).issuer_name(
        ca_cert.subject
    ).public_key(
        csr.public_key()
    ).serial_number(
        cert_sn
    ).not_valid_before(
        nvb_time
    ).not_valid_after(
        nva_time
    ).add_extension(
        extension=x509.KeyUsage(
            digital_signature=True, key_encipherment=True, content_commitment=True,
            data_encipherment=False, key_agreement=False, encipher_only=False, decipher_only=False, key_cert_sign=False, crl_sign=False
        ),
        critical=True
    ).add_extension(
        extension=x509.BasicConstraints(ca=False, path_length=None),
        critical=True
    ).add_extension(
        extension=x509.AuthorityKeyIdentifier.from_issuer_public_key(ca_privkey.public_key()),
        critical=False
    ).sign(
        private_key=ca_privkey,
        algorithm=hashes.SHA256(),
        backend=default_backend()
    )
    return device_cert.public_bytes(serialization.Encoding.PEM)
