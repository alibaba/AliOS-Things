# usocket - 套接字

* 模块功能:  
usocket 模块提供对BSD套接字接口的访问。

## usocket - 创建套接字

* 函数原型：
> usocket.socket(af = AF_INET, type = SOCK_STREAM, proto = IPPROTO_TCP)

* 函数功能：
根据给定的地址族、套接字类型以及协议类型参数，创建一个新的套接字。
注意，在大多数情况下不需要指定*proto*，也不建议这样做，因为某些MicroPython端口可能会省略 `IPPROTO_*`参数。

* 常量说明

|常量定义|常量说明|
|-----|----|
|usocket.AF_INET|IPV4 |
|usocket.AF_INET6|IPV6 |
|usocket.SOCK_STREAM|TCP的流式套接字 |
|usocket.SOCK_DGRAM|对应UDP的数据包套接字 |
|usocket.SOCK_RAW|原始套接字 |
|usocket.IPPROTO_TCP|TCP |
|usocket.IPPROTO_UDP|UDP |
|usocket.SOL_SOCKET|套接字选项级别|
|usocket.SO_REUSEADDR|允许绑定地址快速重用|

* 示例：

```python
# 基于TCP的流式套接字
socket = usocket.socket(usocket.AF_INET, usocket.SOCK_STREAM)
# 基于UDP的数据报套接字
socket = usocket.socket(usocket.AF_INET, usocket.SOCK_DGRAM)
```

## getaddrinfo - 用主机域名（host）和端口（port）生成用于创建套接字的5元组序列

* 函数原型：
> usocket.getaddrinfo(host, port)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|host|stirng|是| 主机域名 |
|port|int|是| 主机端口号 |

* 返回值  

用于创建套接字的5元组，元组结构：
`(family, type, proto, canonname, sockaddr)`

## listen - 监听

* 函数原型：
> usocket.listen(backlog)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|backlog|int|是| 接受套接字的最大个数，取值范围：>= 0 |

* 返回值  
允许服务端新连接接入。

## accept - 接受连接请求

* 函数原型：
> usocket.accept()

* 返回值  
返包含新的套接字和客户端地址的元祖，元祖形式为：`(conn, address)`

* `conn` ：新的套接字对象，可以用来发送和接收数据

* `address` ：连接到服务器的客户端地址

## connect - 连接到指定地址的服务器

* 函数原型：
> usocket.connect(address)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|address|元组/列表|是| 包含地址和端口号 |

* 返回值  
连接到指定地址address的服务器。

## read - 从套接字中读取size字节数据

* 函数原型：
> usocket.read([size])

* 函数功能：  
从套接字中读取size字节数据，返回一个字节对象。如果没有指定size，则会从套接字读取所有可读数据，直到读取到数据结束，这种情况下和 `usocket.readall()` 功能相同。

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|size|int|否| 目标接收字符数量 |

* 返回值  
返回读取到的内容，字节类型。

## readinto - 将接收到的数据读取到缓冲区中

* 函数原型：
> usocket.readinto(buf [, nbytes])

* 函数功能：  
将字节读取到缓冲区buf中。如果指定了nbytes，则最多读取nbytes数量的字节；如果没有指定nbytes，则最多读取len(buf)字节。

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|buf|bytearray|是| 接收数据缓冲区 |
|nbytes|int|否| 最长要收取数据的字节数 |

* 返回值  
实际读取到的字节数

## readline - 从接收到的数据中读取一行数据

* 函数原型：
> usocket.readline()

* 返回值  
返回接收到的数据，遇到换行符结束。

## write - 将数据写入发送缓冲区中

* 函数原型：
> usocket.write(buf)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|buf|bytearray|是| 要放到发送缓冲区的数据 |

* 返回值  
实际写入缓冲区的字节数。

## send - 发送数据

* 函数原型：
> usocket.send(bytes)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|bytes|bytes|是| 要发送的数据内容 |

* 返回值  
实际发送的字节数。

## sendall - 一次性发送所有数据

* 函数原型：
> usocket.sendall(bytes)

* 函数功能：  
将所有数据都发送到套接字。与`send()`方法不同的是，此方法将尝试通过依次逐块发送数据来发送所有数据。

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|bytes|bytes|是| 要发送的数据内容 |

* 返回值  
实际发送的字节数。

注意：该方法再非阻塞套接字上的行为是不确定的，MicroPython建议使用 `write()` 方法。`write` 方法具有相同的“禁止短写”策略来阻塞套接字，并且将返回通过非阻塞套接字发送的数据大小。

## sendto - 将数据发送到套接字

* 函数原型：
> usocket.sendto(bytes, address)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|bytes|bytes|是| 要发送的数据 |
|address|元组或列表|是| 发送的目标地址，包含地址和端口号|

* 返回值  
实际发送的字节数。

## recv - 从套接字接收数据

* 函数原型：
> usocket.recv(bufsize)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|bufsize|int|是| 要接收的数据的最大长度 |

* 返回值  
接收到的数据，字节类型的对象，

## close - 关闭套接字并释放所有资源

* 函数原型：
> usocket.close()

* 函数功能：  
将套接字标记为关闭并释放所有资源。

## recvfrom - 从套接字接收数据

* 函数原型：
> usocket.recvfrom(bufsize)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|bufsize|int|是| 要接收的数据的最大长度 |

* 返回值  
返回一个元组，包含字节对象和地址。返回值形式为：`(bytes, address)`
  * bytes ：接收数据的字节对象

  * address ：发送数据的套接字的地址

## setsockopt - 设置套接字选项

* 函数原型：
> usocket.setsockopt(level, optname, value)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|level|int|是| 套接字选项级别 |
|optname|string|是| socket选项名称 |
|value|int/bytes型对象|是| 要接收的数据的最大长度 |

* 返回值  
设置套接字选项的值。

* 示例：

```python
socket.setsockopt(usocket.SOL_SOCKET, usocket.SO_REUSEADDR, 1)
```

## setblocking - 设置套接字的阻塞模式

* 函数原型：
> usocket.setblocking(flag)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|flag|Bool|是| True代表设置为阻塞模式；False代表设置为非阻塞模式 |

该方法和调用 `settimeout()` 的部分用法灯效：

* `socket.setblocking(True)` 相当于 `socket.settimeout(None)`
* `socket.setblocking(False)` 相当于 `socket.settimeout(0)`

## settimeout - 设置套接字的发送和接收超时时间

* 函数原型：
> usocket.settimeout(value)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|value|float|是| 单位：秒。非负值；0代表非阻塞模式 |


## makefile - 返回与套接字关联的文件对象

* 函数原型：
> usocket.makefile(mode='rb')

* 返回值：  
与套接字关联的文件对象，返回值类型与指定的参数有关。仅支持二进制模式 (‘rb’, ‘wb’, and ‘rwb’)。

注意：关闭makefile返回的文件对象等同于调用了usocket的close()方法。

**socket通信示例**：

```python
import utime
import network  # 导入network库
import usocket  # 导入usocket库

wifiSsid = "haas-open"
wifiPassword = "12345678"


if __name__ == '__main__':

    wlan = network.WLAN(network.STA_IF)    #创建WLAN对象
    wlan.active(True)                      #激活界面
    wlan.scan()                            #扫描接入点
    wifi_connected = wlan.isconnected()
    if not wifi_connected:
        wlan.connect(wifiSsid, wifiPassword)

    while True:
        wifi_connected = wlan.isconnected()
        if wifi_connected:
            break
        else:
            utime.sleep(0.5)
            print("wifi_connected:", wifi_connected)

    ifconfig = wlan.ifconfig()
    print(ifconfig)

    # 创建一个套接字
    sock = usocket.socket(usocket.AF_INET, usocket.SOCK_STREAM)

    # 解析域名
    sockaddr = usocket.getaddrinfo('www.getman.com', 80)[0][-1]

    # 建立连接
    sock.connect(sockaddr)

    # 向服务端发送消息
    ret=sock.send('GET /News HTTP/1.1\r\nHost: www.getman.com/echo\r\nAccept-Encoding: deflate\r\nConnection: keep-alive\r\n\r\n')

    #接收服务端消息
    data=sock.recv(256)

    print('recv %s bytes:' % len(data))
    print(data.decode())

    # 关闭连接
    sock.close()

```

* 输出：
```python
HTTP/1.1 400 Bad Request
Date: Mon, 22 Nov 2021 13:52:59 GMT
Server: Apache/2.4.51
Content-Length: 297
Connection: close
Content-Type: text/html; charset=iso-8859-1

<!DOCTYPE HTML PUBLIC "-//IETF//DTD HTML 2.0//EN">
<html><head>
<title>400 Bad Requ
```
