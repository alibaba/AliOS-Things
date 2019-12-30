import sys
import json

file_in = sys.argv[1]
file_out = sys.argv[2]

fd = open(file_in)
data = fd.read()
fd.close()

data_strip = data.replace(" ","").replace("\n","").replace("\t","").replace("\r","").strip()

data_encode = json.dumps(data_strip)

data_out = "const char* service_config_str = " + data_encode + ";"

fd = open(file_out,'w')
fd.write(data_out)
fd.close()