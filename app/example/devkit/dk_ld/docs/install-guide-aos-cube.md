## 背景：
  aos-cube是AliOS Things基于Python开发的项目管理工具包，依赖 Python 2.7 版本（64bits，2.7.14已验证）。所以，本小节主要简单介绍python/pip安装、基于pip安装aos-cube及相关的依赖包。因涉及多种开发环境和具体版本的依赖，针对开发者的实际情况，还给出一种简单方便且不影响当前系统环境的方法----基于虚拟环境virtualenv的方法__<span data-type="background" style="background-color:#FADB14">[1]</span>__，以便备用。

## 步骤：
#### __Python和pip安装__
python下载地址：[https://www.python.org/downloads/release/python-2714/](https://www.python.org/downloads/release/python-2714/)，在 Python 官网下载对应的 2.7.x(64bits) 版本。
    1) Windowns安装如下图1，选择Python MSI 安装文件，选择 pip 和 add python.exe to Path 两个选项。
    2) Ubuntu可以使用如下命令安装。详细步骤也可以参考[这里](http://aliosthings.io/#/quickstart?id=install-packages)。
      可以使用： 
```plain
        $sudo apt-get install -y python
        $sudo apt-get install -y python-pip
```
    3) Mac OS可以使用 brew install python
__注1__：如遇到网络问题导致pip安装失败，可以使用阿里云镜像[https://mirrors.aliyun.com/pypi/simple/pip/](https://mirrors.aliyun.com/pypi/simple/pip/)，手动下载pip安装(`python setup.py install`)，或者使用 `python -m pip install --trusted-host=mirrors.aliyun.com -i https://mirrors.aliyun.com/pypi/simple/ --upgrade pip` 指定安装源进行升级，最新pip版本为18.0。
__注2__：如何判断pip和python是否安装成功，且可以被系统环境识别。1) windowns的cmd中输入where python和where pip, ubuntu中输出which python 和 which pip可以找到有效路径；2) 输入python --version和pip --version查看版本号、可验证是否已被添加到系统环境中。

![image.png | left | 400x332.6145552560647](https://img.alicdn.com/tfs/TB1bgoEXXzqK1RjSZFCXXbbxVXa-798-688.png)  
图1 windowns环境下安装python


#### __aos-cube及相关依赖安装__
         请确保上述python和pip已经被安装成功，在命令行中依次输入如下命令安装aos-cube及相关依赖库：
```plain
$ python -m pip install setuptools
$ python -m pip install wheel
$ python -m pip install aos-cube
```
**如遇到网络问题，可指定阿里云镜像或手动下载对应的安装包进行安装：**
```plain

# 安装/升级 pip
$python -m pip install --trusted-host=mirrors.aliyun.com -i https://mirrors.aliyun.com/pypi/simple/ --upgrade pip

# 基于pip依次安装第三方包和aos-cube
$ pip install --trusted-host=mirrors.aliyun.com -i https://mirrors.aliyun.com/pypi/simple/   setuptools
$ pip install --trusted-host=mirrors.aliyun.com -i https://mirrors.aliyun.com/pypi/simple/   wheel
$ pip install --trusted-host=mirrors.aliyun.com -i https://mirrors.aliyun.com/pypi/simple/   aos-cube

# 如需要使用doubanio作备用源，如需指定版本，可改成如aos-cube==0.2.50
$ pip install  --trusted-host pypi.doubanio.com -i  http://pypi.doubanio.com/simple/  aos-cube
```
     
 __补充介绍__：<strong><span data-type="background" style="background-color:#FADB14">[1]</span></strong><strong> </strong>基于虚拟环境virtualenv开发
      开发过程中，常常会遇到这类无法规避的问题：多个项目均需要依赖类似的系统环境，这时候如果强制配置特定的环境会产生各种冲突，这个时候虚拟环境便可以绕开该问题。这边以virtualenv为例，详细可参考：[https://virtualenv.pypa.io/en/stable/](https://virtualenv.pypa.io/en/stable/)
     __ __前提：上述python和pip安装完成；步骤如下：
```bash
# pip 安装 virtualenv, 如安装成功，可以在pip list查看版本
$ python -m pip install virtualenv

# 如遇网络问题，可使用阿里云镜像
$ pip install --trusted-host=mirrors.aliyun.com -i https://mirrors.aliyun.com/pypi/simple/ virtualenv

# 建立虚拟环境，如安装到~/venv(可更改)
$ virtualenv  ~/venv --no-download

# 启动虚拟环境
$. ~/venv/bin/activate

# 安装 aos-cube和必要的依赖包
$ `请参考上述b)aos-cube及相关依赖安装`

# 退出虚拟环境
$ deactivate
```
             

__注：__如何判断aos-cube及相关依赖是否成功，可在cmd(windows)中或terminal(ubuntu)中使用`aos --version`，或 `pip list`中可查看到如下版本列表，例如下面图2(基于windowns)
![image.png | left | 512x504](https://img.alicdn.com/tfs/TB142oGXcfpK1RjSZFOXXa6nFXa-512-504.png)  
图2 python,pip,aos版本信息


