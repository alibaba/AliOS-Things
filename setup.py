import os, sys, platform, urllib, zipfile, shutil, glob

def unzip_file(zipfilename, unziptodir):
    zfobj = zipfile.ZipFile(zipfilename)
    zfobj.extractall(unziptodir)

#flush to output immediately
sys.stdout = os.fdopen(sys.stdout.fileno(), 'w', 0)
sys.stderr = os.fdopen(sys.stderr.fileno(), 'w', 0)

downloads = {}

if os.path.exists('build/OpenOCD') == False:
    downloads['openocd.zip'] = 'https://code.aliyun.com/vivid8710/aos-openocd/repository/archive.zip?ref=master'

if os.path.exists('platform/mcu/esp32/esptool_py') == False:
    downloads['esptool.zip'] = 'https://code.aliyun.com/vivid8710/aos-esptool/repository/archive.zip?ref=master'

if os.path.exists('build/compiler') == False:
    host_os = platform.system()
    if host_os == 'Windows':
        downloads['toolchain.zip'] = 'https://code.aliyun.com/vivid8710/aos-toolchain-windows/repository/archive.zip?ref=master'
    elif host_os == 'Linux':
        if platform.machine().endswith('64'):
            bit = '64'
        else:
            bit = '32'
        downloads['toolchain.zip'] = 'https://code.aliyun.com/vivid8710/aos-toolchain-linux{0}/repository/archive.zip?ref=master'.format(bit)
    elif host_os == 'Darwin':
        downloads['toolchain.zip'] = 'https://code.aliyun.com/vivid8710/aos-toolchain-osx/repository/archive.zip?ref=master'
    else:
        print "error: unknown OS {0}".format(repr(host_os))
        sys.exit(1)

if downloads == {}:
    print "You already have all everything installed!"
    sys.exit(0)

if host_os == 'Windows':
    downloads['unzip.zip'] = 'https://code.aliyun.com/vivid8710/windows-unzip/repository/archive.zip?ref=master'

for name in downloads:
    try:
        print "downloading {0} ...".format(name),
        urllib.urlretrieve(downloads[name], name)
        print "done"
    except:
        print "error: download {0} to {1} failed".format(downloads[name], name)
        sys.exit(1)

print 'unzip files ...',
if host_os == 'Windows':
    unzip_file('unzip.zip', './')
    unzip_dir = glob.glob('./windows-unzip-*')[0]
    shutil.move(unzip_dir, 'win-unzip')
    unzip_program = os.getcwd() + '\\win-unzip\\7za.exe'
    unzip_pre_opt = ' x '
    unzip_post_opt = ' -y'
else:
    unzip_program = 'unzip'
    unzip_pre_opt = ' -q '
    unzip_post_opt = ''

filename = 'toolchain.zip'
if filename in downloads:
    unzip_cmd = unzip_program + unzip_pre_opt + filename + unzip_post_opt
    ret = os.system(unzip_cmd)
    if ret != 0:
        print 'error: unzip {0} failed'.format(filename)
        sys.exit(1)
    name = glob.glob('aos-toolchain-*.git')[0]
    shutil.move(name + '/compiler', 'build/compiler')
    shutil.rmtree(name)

filename = 'openocd.zip'
if filename in downloads:
    unzip_cmd = unzip_program + unzip_pre_opt + filename + unzip_post_opt
    ret = os.system(unzip_cmd)
    if ret != 0:
        print 'error: unzip {0} failed'.format(filename)
        sys.exit(1)
    name = glob.glob('aos-openocd-*.git')[0]
    shutil.move(name + '/OpenOCD', './build/OpenOCD')
    shutil.rmtree(name)

filename = 'esptool.zip'
if filename in downloads:
    unzip_cmd = unzip_program + unzip_pre_opt + filename + unzip_post_opt
    ret = os.system(unzip_cmd)
    if ret != 0:
        print 'error: unzip {0} failed'.format(filename)
        sys.exit(1)
    name = glob.glob('aos-esptool-*.git')[0]
    shutil.copytree(name + '/esptool_py', './platform/mcu/esp32/esptool_py')
    shutil.move(name + '/esptool_py', './platform/mcu/esp8266/esptool_py')
    shutil.rmtree(name)
print 'done'

print 'cleanup...',
if host_os == 'Windows':
    shutil.rmtree('win-unzip')
for name in downloads:
    os.remove(name)
print 'done'

print 'setup finished'
