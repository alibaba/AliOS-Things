import sys, hashlib

def MD5(bytes):
    md5 = hashlib.md5()
    md5.update(bytes)
    return md5.hexdigest().decode('hex')

ota_bin = sys.argv[1]

with open(ota_bin, 'rb') as file:
    ota_md5_bytes = MD5(file.read())
with open(ota_bin, 'ab') as file:
    file.write(ota_md5_bytes)