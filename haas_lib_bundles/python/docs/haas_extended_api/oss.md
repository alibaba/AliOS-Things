# OSS  - OSS对象存储


- 模块功能：  
阿里云对象存储OSS（Object Storage Service）是一款海量、安全、低成本、高可靠的云存储服务，提供99.9999999999%(12个9)的数据持久性，99.995%的数据可用性。通过该模块可以将本地的文件上传到云端。目前支持文件以及内容的上传。

-  注意事项：
  * 在使用前，请确保已经开通阿里云账号以及对象存储功能，并创建好Bucket。

## uploadFile - 上传文件

-  函数功能：  
将本地文件上传到创建好的Bucket中

-  函数原型：
> oss.uploadFile(accessKey, accessSecret, endpoint, bucket, filePath)

- 参数说明：

| 参数 | 类型 | 必选参数？ | 说明 |
| --- | --- | --- | --- |
| accessKey | string | 是 | 阿里云访问密钥 |
| accessSecret | string | 是 | 阿里云访问密钥 |
| endpoint | string | 是 | oss访问节点 |
| bucket | string | 是 | 存储Bucket |
| filePath | string | 是 | 本地文件路径 |

-  返回值：  
上传后生成的文件路径。

-  示例： 

```python
import oss

# 阿里云访问账号
ACCESS_KEY = 'Your-Access-Key'
ACCESS_SECRET = 'Your-Access-Secret'

# OSS对象存储配置
OSS_ENDPOINT = "oss-cn-shanghai.aliyuncs.com"
OSS_BUCKET = "Your-Bucket"

# test.jpg存放在“/”目录下

file = oss.uploadFile(ACCESS_KEY, ACCESS_SECRET, OSS_ENDPOINT, OSS_BUCKET, "test.jpg")

print(file)
```

- 输出：

```python
http://oss-ai-dev.oss-cn-shanghai.aliyuncs.com/test.jpg
```


## uploadContent - 上传内容

-  函数功能：  
将内存中或文件中的内容上传到Bucket中

-  注意事项：  
需确保要关闭的UART处于open状态

-  函数原型：
> oss.uploadContent(accessKey, accessSecret, endpoint, bucket, content, ossFilePath)

- 参数说明：

| 参数 | 类型 | 必选参数？ | 说明 |
| --- | --- | --- | --- |
| accessKey | string | 是 | 阿里云访问密钥 |
| accessSecret | string | 是 | 阿里云访问密匙 |
| endpoint | string | 是 | oss访问节点 |
| bucket | string | 是 | 存储Bucket |
| content | string | 是 | 本地文件内容 |
| ossFilePath | string | 是 | Bucket中的文件路径 |

-  返回值：  
上传后生成的文件路径

-  示例： 

```python
import oss

# 阿里云访问账号
ACCESS_KEY = 'Your-Access-Key'
ACCESS_SECRET = 'Your-Access-Secret'

# OSS对象存储配置
OSS_ENDPOINT = "oss-cn-shanghai.aliyuncs.com"
OSS_BUCKET = "Your-Bucket"

# test.jpg存放在“/”目录下

content = "test data"
file = oss.uploadContent(ACCESS_KEY, ACCESS_SECRET, OSS_ENDPOINT, OSS_BUCKET,content, "oss/test.txt")
print(file)
```

- 输出

```
http://oss-ai-dev.oss-cn-shanghai.aliyuncs.com/oss%2Ftest.txt
```


