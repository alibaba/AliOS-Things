# coding=UTF-8
import sys
import re


def process(para1, para2):
	sum = 0
	dataout = ''
	array = ''

	for file_in in para1:
		fd_in = open(file_in)
		line = fd_in.readline()
		while line:
			result = re.findall("SENSOR_DRV_ADD\((.*)\);",line)

			for func in result:
				func_name = "__sensor_" + func + "_func__"
				array = array + func_name + "," + "\n"
				dataout += "extern int " + func_name + "(void);" + "\n"
				sum += 1	

			line = fd_in.readline()	


		fd_in.close()

	
	sum+=1
	fd_out = open(para2,'w+')
	str_start = "#include \"sensor_hal.h\"" + "\n\n"
	fd_out.writelines(str_start)

	fd_out.writelines(dataout)

	str_num = "\nWEAK int g_sensor_drv_num = " + str(sum) + ";\n"
	fd_out.writelines(str_num)

	str_start = "\nWEAK SENSOR_INIT_FUN g_sensor_func[] = {" + "\n" + "NULL,\n"
	fd_out.writelines(str_start)
	fd_out.writelines(array)
	str_end = "};" + "\n"

	fd_out.writelines(str_end)

	fd_out.close()
	
