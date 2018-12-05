# coding=UTF-8
import sys

compiler = sys.argv[1]
file_in = sys.argv[2]
file_out = sys.argv[3]

fd_in = open(file_in)
fd_out = open(file_out,'w+')
line = fd_in.readline()

str_start = "#include \"sensor_hal.h\"" + "\n\n"
fd_out.writelines(str_start)
dataout = ''
sum = 0

if compiler == 'tool_armcc':
	while line:
		index = 0
		for word in line.split():
			index = index+1
			if str(word).startswith('__sensor_') and str(word).endswith('_func__'):
				dataout = dataout + word + "," + "\n"
				func = "extern int " + word + "(void);" + "\n"
				fd_out.writelines(func)
				sum = sum+1
			break
		line = fd_in.readline()
elif compiler == 'tool_rvct':
	while line:
		index = 0
		for word in line.split():
			index = index+1
			if str(word).startswith('__sensor_') and str(word).endswith('_func__'):
				dataout = dataout + word + "," + "\n"
				func = "extern int " + word + "(void);" + "\n"
				fd_out.writelines(func)
				sum = sum+1
			break
		line = fd_in.readline()	
elif compiler == 'tool_iar':
	print (compiler)
	while line:
		index = 0
		for word in line.split():
			index = index+1
			if (index == 1):
				continue
			if str(word).startswith('__sensor_') and str(word).endswith('_func__'):
				dataout = dataout + word + "," + "\n"
				func = "extern int " + word + "(void);" + "\n"
				fd_out.writelines(func)
				sum = sum+1
			break
		line = fd_in.readline()
else:
	while line:
		index = 0
		for word in line.split():
			index = index+1

			if (index == 2 and word != 'g'):
				break
			if (index == 3 and word != 'F'):
				break
			if index != 6:
				continue

			if str(word).startswith('__sensor_') and str(word).endswith('_func__'):
				dataout = dataout + word + "," + "\n"
				func = "extern int " + word + "(void);" + "\n"
				fd_out.writelines(func)
				sum = sum+1
				break

		line = fd_in.readline()

sum = sum+1
str = "\nconst int g_sensor_drv_num = " + str(sum) + ";\n"
fd_out.writelines(str)

str_start = "\nconst SENSOR_INIT_FUN g_sensor_func[] = {" + "\n"
fd_out.writelines(str_start)
fd_out.writelines("NULL,\n")
fd_out.writelines(dataout)

str_end = "};" + "\n"
fd_out.writelines(str_end)

fd_in.close()
fd_out.close()