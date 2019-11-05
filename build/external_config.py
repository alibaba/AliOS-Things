# Download external sources automatically
auto_download = "yes"

# Download external sources
externals = {
    'cmsis_nn':{
        'name': 'cmsis',
        'giturl': {
            'github': 'https://github.com/ARM-software/CMSIS_5.git',
            'gitee': 'https://gitee.com/mirrors/CMSIS_5',
        },
        'gittag': '5.6.0',
        'destdir': 'middleware/uai/3rdparty/cmsis_nn/CMSIS_5',
        'postscript': 'middleware/uai/3rdparty/cmsis_nn/get_cmsis_nn.py',
        'require': 'AOS_COMP_UAI=y',
        },
}
