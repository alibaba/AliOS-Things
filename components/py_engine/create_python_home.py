# coding=utf-8
import shutil
import os
import sys
import stat

print('cp .py file to file system')
print(os.getcwd())

#源文件目录
current_dir = os.getcwd()

script_dir = os.path.dirname(sys.argv[0])
framework_dir = os.path.join(script_dir, "framework")

pyFile = open(sys.argv[1])
data = pyFile.read()
print(data)
data_prebuild=""
pythonApp_dir=""
if "hardware/chip/rtl872xd" in data:
    print("rtl872xd chip")
    data_prebuild="hardware/chip/rtl872xd/prebuild/data/"
    pythonApp_dir = os.path.join(script_dir, "example", "HaaS200", "python-apps")
else:
    print("haas1000 chip")
    data_prebuild="hardware/chip/haas1000/prebuild/data/"
    pythonApp_dir = os.path.join(script_dir, "example", "HaaS100", "python-apps")
python_home="lib/micropython/"
print(data_prebuild)

#return ../../ path
prebuild_dest1 = os.path.dirname(framework_dir)
prebuild_dest2 = os.path.dirname(prebuild_dest1)
prebuild_dest = os.path.dirname(prebuild_dest2)
# make /hardware/chip/haas1000/prebuild/data/lib/micropython/ path
prebuildDest = os.path.join(prebuild_dest, data_prebuild, python_home)


pythonapp_dest1 = os.path.dirname(pythonApp_dir)
pythonapp_dest2 = os.path.dirname(pythonapp_dest1)
pythonapp_dest3 = os.path.dirname(pythonapp_dest2)
pythonapp_dest4 = os.path.dirname(pythonapp_dest3)
pythonapp_dest = os.path.dirname(pythonapp_dest4)
# make /hardware/chip/haas1000/prebuild/data/ path
pythonhomeDest = os.path.join(pythonapp_dest, data_prebuild, 'python-apps')

if not os.path.exists(prebuildDest):
    os.makedirs(prebuildDest)

# if not os.path.exists(pythonhomeDest):
#     os.makedirs(pythonhomeDest)

print(framework_dir)
print(prebuildDest)

# print(os.listdir(framework_dir))
#cp data prebuild files
for root, dirs, files in os.walk(framework_dir):
    for file in files:
        src_file = os.path.join(root, file)
        shutil.copy(src_file, prebuildDest)
        # print(src_file)


print(pythonApp_dir)
print(pythonhomeDest)
#cp pythonme
if os.path.exists(pythonhomeDest):
    for fileList in os.walk(pythonhomeDest):
        for name in fileList[2]:
            os.chmod(os.path.join(fileList[0], name), stat.S_IWRITE)
            os.remove(os.path.join(fileList[0], name))
    shutil.rmtree(pythonhomeDest)
    print("delete python home OK")
else:
    print("no filepath")
shutil.copytree(pythonApp_dir, pythonhomeDest)
print("run external script success")
exit(0)
