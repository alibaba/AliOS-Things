# coding=UTF-8
import sys

file_name = sys.argv[1]
para = sys.argv[2]

#file_name = "config_mk.py"
#para = "'./drivers/sensor/drv/drv_temp_humi_sensirion_shtc1.cccccc',"

lines=[]
ret = -1;
num = 0;
index = -1;
search_flag = -1;
insert_flag = -1;

fd_out = open(file_name,'r')
line = fd_out.readline()

while line:
	num = num+1
	lines.append(line)
	for word in line.split():
	
		if (search_flag == -1):
			ret = str(word).find("'name':'sensor'")
			if ret != -1:
				search_flag = 1;
				continue;
		if (search_flag == 1 and  index == -1):
			ret = str(word).find("'src':[")
			if ret != -1:
				index = num;
		ret = str(word).find(para)
		if ret != -1:
			sys.exit(0)
	line = fd_out.readline()
	
fd_out.close()


lines.insert(index, "'" + para+"',"+"\n")
s=''.join(lines)

fd_out = open(file_name,'w+')
fd_out.write(s)
fd_out.close()
del lines[:]
