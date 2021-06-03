* 切换到当前路径(路径下必须包含app.zip,zip解压以后是一个app的目录)
* 执行
* python  haasdevice.py tftpcfg.json

# 执行环节要求

```Python
pip install py3tftp
pip install pyserial
```

# tftpcfg.json

* "file": "main.py"
表示需要推送的是文件 main.py, 去掉file 字段推送的文件就是app.zip
