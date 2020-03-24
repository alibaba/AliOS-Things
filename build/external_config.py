# Download external sources automatically
auto_download = "yes"

# Download external sources
externals = {
    'cmsis_nn':{
        'name': 'cmsis',
        'giturl': {
            'github': 'https://github.com/ARM-software/CMSIS_5.git',
<<<<<<< HEAD   (1e4346 BugID:26031966: copy non-utf8 file to appdir directly)
            'gitee': 'https://gitee.com/mirrors/CMSIS_5',
=======
            'gitee': 'https://code.aliyun.com/alios-things/CMSIS_5.git',
>>>>>>> CHANGE (dd1071 BugID:24784463: change CMSIS_5 repository from gitee to code)
        },
        'gittag': '5.6.0',
        'destdir': 'components/service/uai/3rdparty/cmsis_nn/CMSIS_5',
        'postscript': 'middleware/uai/3rdparty/cmsis_nn/get_cmsis_nn.py',
        'postscript': 'components/service/uai/3rdparty/cmsis_nn/get_cmsis_nn.py',
        'require': 'AOS_COMP_UAI=y',
        },
}
