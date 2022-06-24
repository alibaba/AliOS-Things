#!/usr/bin/env python3
# -*- encoding: utf-8 -*-
'''
@File       :    main.py
@Author     :    杭漂
@version    :    1.0
@Description:    通过跟HaaS Cloud控制台中“升级服务-应用升级”配合，实现设备端python应用的升级，该文件为针对本案例测试应用升级使用。
'''

import utime
import ujson as json

# manifest.json文件注意事项：
# 1、该文件内不要添加任何注释；
# 2、关键字appName对应的值，只能使用英文字母、数字，长度不能超过16个字符，在HaaS Cloud平台全局范围内唯一；
# 3、关键字appid对应的值，必须为16位的数字，且开头为88001000xxxxpppp，该appid在HaaS Cloud平台全局范围内唯一；
# 4、关键字verison对应的值，只能使用数字和'.'，必须是3段式，例如0.0.1，在相同appName下唯一；
# 下面提到的$appName、$appid、$verison值必须跟升级包中manifest.json中一致。
# cd /${YourProjectPath}/src/code/testUpgrade/
# tar cvzf $appName-$appid-$verison.tar.gz *
# Python升级包标准名字举例：python001-8800100099991000-0.0.9.tar.gz

app_info = {
    'appId':'',
    'localAppVersion':'',
    'appNewVersion':'',
    'mac':'10:10:10:10:10:10',
    'ip':'10.10.10.10'
}

def get_app_info():
    try:
        file_name = r'/data/pyamp/manifest.json'
        # 以只读方式打开文件
        f = open(file_name)
        # 一次读取整个文件
        content = f.read()
        print(content)
        app_dict = json.loads(content)
        app_info['localAppVersion'] = app_dict['version']
        app_info['appId'] = app_dict['appid']
    finally:
        f.close()

if __name__ == '__main__':
    get_app_info()
    while True:
        print("new version upgrade success.")
        utime.sleep(3)
