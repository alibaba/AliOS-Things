import sys, hashlib, os
import struct
  

# changed by yanxiaoyong.yxy
# https://blog.csdn.net/qq_43192819/article/details/108981008
# https://blog.csdn.net/whatday/article/details/107769032
def MD5(input):
    md5 = hashlib.md5()
    md5.update(input)
    return bytes.fromhex(md5.hexdigest())
    #return md5.hexdigest().decode('hex')
    
input_bin = sys.argv[1]
ota_bin = sys.argv[3]
version = int(sys.argv[2], 16)
b = struct.pack('<L', version)
filelen = os.path.getsize(input_bin)
sum = 0
i=0
with open(input_bin, 'rb') as file:
    ota_md5_bytes = MD5(file.read())
output = open(ota_bin, 'wb') 
output.write(b)
b = struct.pack('<L', 0x01)
output.write(b)
#output.write("OTA1")
output.write("OTA1".encode())
b = struct.pack('<L', 0x18)
output.write(b)
input = open(input_bin, 'rb') 
for i in range(filelen):
	data = input.read(1)
	sum += struct.unpack("B", data)[0]
input.close
b = struct.pack('<L', sum) #CRC
output.write(b)
b = struct.pack('<L', filelen)
output.write(b)
b = struct.pack('<L', 0x20)
output.write(b)
b = struct.pack('<L', 0)
output.write(b)
input = open(input_bin, 'rb') 
output.write(input.read())
input.close()
output.close()