本章节主要介绍windows下验证过的整体开发环境准备。
be-cli的安装需要node环境+python环境+visual studio环境

1，安装node/npm
去到下面，下载最新的LTS版本的node即可
[https://nodejs.org/zh-cn/](https://nodejs.org/zh-cn/)

在cmd里面执行命令，查看版本：
C:\Users\Administrator>node -v
v8.11.1
C:\Users\Administrator>npm -v
5.6.0

错误提示：
```
gyp ERR! stack     at failNoPython (D:\Program Files\nodejs\node_modules\npm\nod
e_modules\node-gyp\lib\configure.js:103:14)
gyp ERR! stack     at D:\Program Files\nodejs\node_modules\npm\node_modules\node
-gyp\lib\configure.js:64:11
gyp ERR! stack     at FSReqWrap.oncomplete (evalmachine.<anonymous>:95:15)
gyp ERR! System Windows_NT 6.3.9600
gyp ERR! command "node" "D:\\Program Files\\nodejs\\node_modules\\npm\\node_modu
les\\node-gyp\\bin\\node-gyp.js" "rebuild"
gyp ERR! cwd G:\nodejs\moviesite\node_modules\bcrypt
gyp ERR! node -v v0.12.2
gyp ERR! node-gyp -v v1.0.3
gyp ERR! not ok
npm ERR! Windows_NT 6.3.9600
```
2，安装python
下载安装2.7.x版本成都python： [http://www.python.org/ftp/python/2.7.3/python-2.7.3.msi](http://www.python.org/ftp/python/2.7.3/python-2.7.3.msi)
验证在3.x的python版本里面gyp支持不好，导致有依赖native编译的模块编译不成功，最终be-cli下载安装失败。




错误提示：
```
在此解决方案中一次生成一个项目。若要启用并行生成，请添加“/m”开关。 
MSBUILD : error MSB3428: 未能加载   组件“VCBuild.exe”。要解决此问题， 
1) 安装 .NET Fram 
ework 2.0 SDK；2) 安装 Microsoft  Studio 2005；或 3) 如果将该组件安装到了 
其他位置，请将其位置添加到系统 
路径中。 [G:\node_modules\bcrypt\build\binding.sln] 
gyp ERR! build error
```
3，安装visual studio工具环境

在cmd里面执行下面npm命令来安装：
npm install --global --production windows-build-tools

4，安装板子的USB转串口驱动
[https://www.silabs.com/documents/public/software/CP210x\_Windows\_Drivers.zip](https://www.silabs.com/documents/public/software/CP210x_Windows_Drivers.zip)

5，安装be-cli工具
npm install be-cli@0.1.32 -g -P

如果长时间安装不到be-cli，请使用下面的更换代理镜像命令：
npm install be-cli@0.1.32 -g -P —registry=https://registry.npm.taobao.org
该命令不会更改你本地原始的镜像代理，只是本次下载安装be-cli生效。

安装完毕，在cmd里面执行 be 可以看到使用说明
