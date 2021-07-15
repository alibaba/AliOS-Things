# coding=utf-8
import shutil
import os
import sys
import stat

print('cp .py file to file system')
print(os.getcwd())

#源文件目录
#current_dir = os.getcwd()

script_dir = os.path.dirname(sys.argv[0])
framework_dir = os.path.join(script_dir, "framework")

pyFile = open(sys.argv[1])
data = pyFile.read()
print(data)
data_prebuild=""
pythonApp_dir=""
python_root_dir=""
if "ALI_AOS_HAAS_EDU_K1" in data:  # haasedu
    print("HaaS EDU platform")
    data_prebuild = "hardware/chip/haas1000/prebuild/data/"
    pythonApp_dir = os.path.join(script_dir, "tests/haas", "HaaSEdu", "python-apps")
    python_root_dir = os.path.join(script_dir, "tests/haas", "HaaSEdu", "pyamp")

elif "ALI_AOS_HAAS200" in data:     # haas200
    print("HaaS 200 platform")
    data_prebuild = "hardware/chip/rtl872xd/prebuild/data/"
    pythonApp_dir = os.path.join(
        script_dir, "tests/haas", "HaaS200", "python-apps")
    python_root_dir = os.path.join(
        script_dir, "tests/haas", "HaaS200", "pyamp")    
else:                               # haas100
    print("HaaS100 chip")
    data_prebuild="hardware/chip/haas1000/prebuild/data/"
    pythonApp_dir = os.path.join(
        script_dir, "tests/haas", "HaaS100", "python-apps")
    python_root_dir = os.path.join(
        script_dir, "tests/haas", "HaaS100", "pyamp")
python_home="lib/micropython/"
print(data_prebuild)

#return ../../ path
prebuild_dest1 = os.path.dirname(framework_dir)
prebuild_dest2 = os.path.dirname(prebuild_dest1)
prebuild_dest = os.path.dirname(prebuild_dest2)
# make /hardware/chip/haas1000/prebuild/data/lib/micropython/ path
prebuildDest = os.path.join(prebuild_dest, data_prebuild, python_home)

components_dest = os.path.dirname(script_dir)
root_dest = os.path.dirname(components_dest)
# make /hardware/chip/haas1000/prebuild/data/ path
pythonhomeDest = os.path.join(root_dest, data_prebuild, 'python-apps')
pythonrootDest = os.path.join(root_dest, data_prebuild, 'pyamp')

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

#cp pythonme

print(python_root_dir)
print(pythonrootDest)
if os.path.exists(pythonrootDest):
    for fileList in os.walk(pythonrootDest):
        for name in fileList[2]:
            os.chmod(os.path.join(fileList[0], name), stat.S_IWRITE)
            os.remove(os.path.join(fileList[0], name))
    shutil.rmtree(pythonrootDest)
    print("delete python boot root OK")
else:
    print("no filepath")

shutil.copytree(python_root_dir, pythonrootDest)
print("run external script success")
exit(0)
