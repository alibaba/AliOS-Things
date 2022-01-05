#!/usr/bin/env python
# ##########################################################################
# (c) 2017 Microchip Technology Inc. and its subsidiaries.  You may use this
# software and any derivatives exclusively with Microchip products.
#
# THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER
# EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
# WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR
# PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION WITH ANY
# OTHER PRODUCTS, OR USE IN ANY APPLICATION.
#
# IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
# INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
# WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN
# ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST
# EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
# RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, THAT YOU
# HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
#
# MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
# TERMS.
#############################################################################
# Copyright 2020 Espressif Systems (Shanghai) Co., Ltd.
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
##############################################################################
import unicodedata
import base64
from base64 import urlsafe_b64encode
import json
import re
from cryptography.hazmat.primitives.asymmetric import ec
from cryptography.hazmat.primitives.asymmetric import utils as crypto_utils
from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives.serialization import Encoding, PublicFormat
from cryptography.utils import int_to_bytes
from cryptography import x509
from jose import utils
from cryptography.hazmat.backends import default_backend
import sys
import os
from . import serial
from . import cert_sign

def create_signed_entry(entry, log_key, jws_header):
    """
    Converts the unsigned manifest entry into the signed manifest format (divided jws)
    """
    jws_data = {'header': {'uniqueId': entry['uniqueId']}, 'protected': jws_header,
                'payload': jws_b64encode(json.dumps(entry).encode('ascii'))}

    tbs = jws_data['protected'] + '.' + jws_data['payload']

    signature = log_key.sign(tbs.encode('ascii'), ec.ECDSA(hashes.SHA256()))

    r_int, s_int = crypto_utils.decode_dss_signature(signature)

    signature = int_to_bytes(r_int, 32) + int_to_bytes(s_int, 32)

    jws_data['signature'] = jws_b64encode(signature)

    return jws_data


def jws_b64encode(source):
    """Simple helper function to remove base64 padding"""
    return urlsafe_b64encode(source).decode('ascii').rstrip('=')


def get_common_name(name):
    """
    Get the common name string from a distinguished name (RDNSequence)
    """
    for attr in name:
        if attr.oid == x509.oid.NameOID.COMMON_NAME:
            return attr.value
    return None


def make_valid_filename(s):
    """
    Convert an arbitrary string into one that can be used in an ascii filename.
    """
    if sys.version_info[0] <= 2:
        s = str(s).decode('utf-8')
    else:
        s = str(s)
    # Normalize unicode characters
    s = unicodedata.normalize('NFKD', s).encode('ascii', 'ignore').decode('ascii')
    # Remove non-word and non-whitespace characters
    s = re.sub(r'[^\w\s-]', '', s).strip()
    # Replace repeated whitespace with an underscore
    s = re.sub(r'\s+', '_', s)
    # Replace repeated dashes with a single dash
    s = re.sub(r'-+', '-', s)
    return s


def generate_manifest_file(esp, args, init_mfg):

    retval = init_mfg.exec_cmd(esp._port, "print-chip-info")
    serial.esp_cmd_check_ok(retval, "print-chip-info")
    # find index fo SN in string
    index = retval[1]['Return'].find("Serial Number:")
    index += len("Serial Number:")
    serial_number_hex = retval[1]['Return'][index:]
    serial_number_hex = serial_number_hex.strip()
    serial_number_hex = serial_number_hex.replace(' ','')
    print('Serial Number:')
    print(serial_number_hex.upper())
    if args.print_atecc608a_type is True:
       # only print chip info and exit.
        exit(0)

    print("Generating Manifest")
    certs = []

    retval = init_mfg.exec_cmd(esp._port, "get-tngtls-root-cert")
    serial.esp_cmd_check_ok(retval, "get-tngtls-root-cert")

    index = retval[1]['Return'].find("Root Cert Len:")
    index += len("Root Cert Len:")
    root_cert_size = retval[1]['Return'][index:index + 3]
    root_cert_size = int(root_cert_size)
    root_cert_der = bytearray(root_cert_size)
    index = retval[1]['Return'].find("Certificate:")
    index += len("Certificate:")
    root_cert_hex = retval[1]['Return'][index + 1:]
    root_cert_hex = root_cert_hex.strip()
    root_cert_der = bytearray.fromhex(root_cert_hex)

    root_cert = x509.load_der_x509_certificate(root_cert_der, default_backend())
    certs.insert(0, root_cert)

    print(get_common_name(root_cert.subject))
    print(root_cert.public_bytes(encoding=Encoding.PEM).decode('utf-8'))

    print('TNG Root Public Key:')

    print(root_cert.public_key().public_bytes(
        format=PublicFormat.SubjectPublicKeyInfo,
        encoding=Encoding.PEM
    ).decode('utf-8'))

    retval = init_mfg.exec_cmd(esp._port, "get-tngtls-signer-cert")
    serial.esp_cmd_check_ok(retval, "get-tngtls-signer-cert")

    index = retval[1]['Return'].find("Signer Cert Len:")
    index += len("Signer Cert Len:")
    signer_cert_size = retval[1]['Return'][index:index + 3]
    signer_cert_size = int(signer_cert_size)
    signer_cert_der = bytearray(signer_cert_size)
    index = retval[1]['Return'].find("Certificate:")
    index += len("Certificate:")
    signer_cert_hex = retval[1]['Return'][index:]
    signer_cert_hex = signer_cert_hex.strip()
    signer_cert_der = bytearray.fromhex(signer_cert_hex)

    print('TNG Signer Certificate:')
    signer_cert = x509.load_der_x509_certificate(signer_cert_der, default_backend())
    certs.insert(0, signer_cert)

    print(get_common_name(signer_cert.subject))
    print(signer_cert.public_bytes(encoding=Encoding.PEM).decode('utf-8'))

    print('TNG Signer Public Key:')

    print(signer_cert.public_key().public_bytes(
        format=PublicFormat.SubjectPublicKeyInfo,
        encoding=Encoding.PEM
    ).decode('utf-8'))

    # Note that this is a simple cryptographic validation and does not check
    # any of the actual certificate data (validity dates, extensions, names,
    # etc...)
    print('Validate Signer Certificate:')
    root_cert.public_key().verify(
        signature=signer_cert.signature,
        data=signer_cert.tbs_certificate_bytes,
        signature_algorithm=ec.ECDSA(signer_cert.signature_hash_algorithm)
    )
    print('OK\n')

    print('TNG Device Certificate:')

    retval = init_mfg.exec_cmd(esp._port, "get-tngtls-device-cert")
    serial.esp_cmd_check_ok(retval, "get-tngtls-device-cert")

    index = retval[1]['Return'].find("Device Cert Len:")
    index += len("Device Cert Len:")
    device_cert_size = retval[1]['Return'][index:index + 3]
    device_cert_size = int(device_cert_size)
    device_cert_der = bytearray(device_cert_size)
    index = retval[1]['Return'].find("Certificate:")
    index += len("Certificate:")

    device_cert_hex = retval[1]['Return'][index:]
    device_cert_hex = device_cert_hex.strip()
    device_cert_der = bytearray.fromhex(device_cert_hex)
    device_cert = x509.load_der_x509_certificate(device_cert_der, default_backend())
    certs.insert(0, device_cert)

    print(get_common_name(device_cert.subject))
    device_cert_pem = device_cert.public_bytes(encoding=Encoding.PEM).decode('utf-8')
    print(device_cert_pem)
    print("Saving device cert to output_files")
    with open("./output_files/device_cert.pem", "w+") as dev_cert_file:
        dev_cert_file.write(device_cert_pem)
    print('TNG Device Public Key:')
    # Note that we could, of course, pull this from the device certificate above.
    # However, this demonstrates the tng_atcacert_device_public_key() function.

    print(device_cert.public_key().public_bytes(
        format=PublicFormat.SubjectPublicKeyInfo,
        encoding=Encoding.PEM
    ).decode('utf-8'))

    print('Validate Device Certificate:')
    signer_cert.public_key().verify(
        signature=device_cert.signature,
        data=device_cert.tbs_certificate_bytes,
        signature_algorithm=ec.ECDSA(device_cert.signature_hash_algorithm)
    )
    print('OK\n')

    device_entry = {
        'version': 1,
        'model': 'ATECC608A',
        'partNumber': 'ATECC608A-TNGTLS',
        'manufacturer': {
            'organizationName': 'Microchip Technology Inc',
            'organizationalUnitName': 'Secure Products Group'
        },
        'provisioner': {
            'organizationName': 'Microchip Technology Inc',
            'organizationalUnitName': 'Secure Products Group'
        },
        'distributor': {
            'organizationName': 'Microchip Technology Inc',
            'organizationalUnitName': 'Microchip Direct'
        }
    }

    device_entry['provisioningTimestamp'] = device_cert.not_valid_before.strftime('%Y-%m-%dT%H:%M:%S.%f')[:-3] + 'Z'

    print(serial_number_hex)
    device_entry['uniqueId'] = serial_number_hex

    device_entry['publicKeySet'] = {
        'keys': [
            {
                'kid': '0',
                'kty': 'EC',
                'crv': 'P-256',
                'x': None,
                'y': None,
                'x5c': [
                    base64.b64encode(device_cert_der).decode('ascii'),
                    base64.b64encode(signer_cert_der).decode('ascii')
                ]
            },
            {'kid': '1', 'kty': 'EC', 'crv': 'P-256', 'x': None, 'y': None},
            {'kid': '2', 'kty': 'EC', 'crv': 'P-256', 'x': None, 'y': None},
            {'kid': '3', 'kty': 'EC', 'crv': 'P-256', 'x': None, 'y': None},
            {'kid': '4', 'kty': 'EC', 'crv': 'P-256', 'x': None, 'y': None}
        ]
    }

    for key in device_entry['publicKeySet']['keys']:
        public_key = bytearray(64)
        print('reading slot {} public key'.format(key['kid']))
        retval = init_mfg.exec_cmd(esp._port, "generate-pubkey {}".format(key['kid']))
        serial.esp_cmd_check_ok(retval, "pub-key-gen")
        index = retval[1]['Return'].find("Public Key:")
        index += len("Public Key:")
        public_key_hex = retval[1]['Return'][index:]
        public_key_hex = public_key_hex.strip()
        public_key = bytearray.fromhex(public_key_hex)
        key['x'] = utils.base64url_encode(public_key[0:32]).decode('ascii')
        key['y'] = utils.base64url_encode(public_key[32:64]).decode('ascii')

    # If a logging key and certificate was provided create a manifest file
    log_key = cert_sign.load_privatekey(args.signer_privkey)
    log_cert = cert_sign.load_certificate(args.signer_cert)
    # Generate the key and certificate ids for JWS
    log_key_id = jws_b64encode(log_cert.extensions.get_extension_for_class(x509.SubjectKeyIdentifier).value.digest)
    log_cert_id = jws_b64encode(log_cert.fingerprint(hashes.SHA256()))

    # Precompute the JWT header
    jws_header = {'typ': 'JWT', 'alg': 'ES256', 'kid': log_key_id, 'x5t#S256': log_cert_id}
    jws_header = jws_b64encode(json.dumps(jws_header).encode('ascii'))

    manifest = json.dumps([create_signed_entry(device_entry, log_key, jws_header)], indent=2).encode('ascii')

    filename = make_valid_filename(device_entry['uniqueId']) + '_manifest' + '.json'
    os.getcwd()
    with open('./output_files/' + filename, 'wb') as f:
        f.write(manifest)
    print('\n\nGenerated the manifest file ' + filename + ' in output_files')
