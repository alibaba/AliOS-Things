@page contribute_code 贡献代码说明

**[更正文档](https://gitee.com/alios-things/documentation/edit/rel_3.3.0/developer/contribute_code.md)** &emsp;&emsp;&emsp;&emsp; **[贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)**

项目地址：[https://gitee.com/alios-things/AliOS-Things](https://gitee.com/alios-things/AliOS-Things)

AliOS Things为开发者在Gitee平台上提供了贡献代码的入口，开发者可以在AliOS Things项目下找到想到修改的仓库进行代码提交，具体步骤如下。

# 查找目标仓库
AliOS Things 3.3版本在Gitee平台上，每个功能模块分配一个独立的仓库。
若开发者想对某个组件或案例仓库贡献代码，可以在AliOS Things概览页面点击选项栏中的“仓库”切换到“仓库”页，在搜索栏中输入目标组件或案例的名称，确认后即可显示搜索结果。以目标仓库为netmgr为例，搜索方式如下图：

<div align="left">
  <img src="https://img.alicdn.com/imgextra/i2/O1CN01FLjYJF1abQiC3Xk26_!!6000000003348-2-tps-789-326.png" style="max-width:800px;" >
</div>


# 切换分支
打开目标组件或案例的仓库，切换代码分支为rel_3.3.0，即可查看到该组件或案例3.3版本对应的源代码。以目标仓库为netmgr为例，切换分支方法如下图。

<div align="left">
  <img src="https://img.alicdn.com/imgextra/i1/O1CN01Ul5th61rda2VfNObR_!!6000000005654-2-tps-919-555.png" style="max-width:800px;" >
</div>


# 创建 Fork下载代码
1、在Gitee 上的目标仓库里单击 “fork” 按钮，将目标仓库fork一份到开发者自己的名下。以目标仓库为amp为例，Fork代码的方法如下图：

<div align=left display=flex>     <img src="https://img.alicdn.com/imgextra/i1/O1CN01Zg2mW91z0CvYKEaGT_!!6000000006651-2-tps-1236-791.png" style="max-width:800px;" /> </div>

2、进入开发者fork出来的目标仓库，注意，该仓库地址已经从AliOS Things/<TARGET> 变成<YOUR_NAME>/<TARGET>，如下图所示。然后根据图中提示的步骤复制仓库地址。

<div align=left display=flex>     <img src="https://img.alicdn.com/imgextra/i1/O1CN01njxqmz1MMP6YUS3Co_!!6000000001420-2-tps-1236-499.png" style="max-width:800px;" /> </div>



3、下载代码
注意：gitee提供多种下载方式，本文使用https，下载代码时会要求输入密码，密码是开发者的gitee登录密码。

```bash
# 注意YOUR_NAME替换成开发者Gitee账号的名称；TARGET替换为Gitee仓库名称
$ git clone https://gitee.com/<YOUR_NAME>/<TARGET>.git
```
# 本地开发并提交
本文以提交amp仓库下的代码为例子，提供完整开发命令供参考

```bash
# 1、Checkout and switch to your new branch
# 注意TARGET替换为Gitee仓库名称
$ cd <TARGET>
$ git checkout rel_3.3.0

# Commit and push your code
git add -A				#-A表示将本地所有修改文件提交，若只需要提交若干文件，请将-A替换成文件名
git commit -s			#填写log日志，说明变更的内容，影响功能点等
git push					#将本地代码push到github
```

在gitee网页按下图操作，如果出现你的提交记录，说明push成功

<div align=left display=flex>     <img src="https://img.alicdn.com/imgextra/i4/O1CN013qWfcb1hMV7Wl8uch_!!6000000004263-2-tps-1236-475.png" style="max-width:800px;" /> </div>


# 创建pull request
当功能开发完成后，可以发起 Pull Request，申请合入代码到AliOS Things/<TARGET>。

在开发者gitee的AliOS-Things仓库页面上，然后单击“ Pull requests ”按钮发起“+新建 pull request”. 如下图网页操作，创建一个pull request

<div align=left display=flex>     <img src="https://img.alicdn.com/imgextra/i4/O1CN01cH38pj1E96gMPplyV_!!6000000000308-2-tps-1240-1045.png" style="max-width:800px;" /> </div>

<div align=left display=flex>     <img src="https://img.alicdn.com/imgextra/i4/O1CN013qWfcb1hMV7Wl8uch_!!6000000004263-2-tps-1236-475.png" style="max-width:800px;" /> </div>


在AliOS Things/<TARGET> 页面，看到自己的pull request请求，即说明创建成功，等待审核

<div align=left display=flex>     <img src="https://img.alicdn.com/imgextra/i2/O1CN01CVuWdl1G5YqVci86P_!!6000000000571-2-tps-1240-487.png" style="max-width:800px;" /> </div>



# 同步代码
在发起 Pull Request 之前，有可能原来的仓库AliOS Things/<TARGET>有代码更新，那么请按照以下步骤，拉取AliOS Things/<TARGET>最新的代码至本地。
```bash
# 注意TARGET替换为Gitee仓库名称
$ git remote add upstream https://gitee.com/alios-things/<TARGET>.git
$ git remote -v								#此时本地有两个远程地址，origin和upstream，origin对应fork仓库，upstream对应<TARGET>仓库
$ git fetch upstream						#将alibaba仓库代码更新到本地
$ git checkout rel_3.3.0				    #切换到master分支
$ git merge upstream/rel_3.3.0		        #合并AliOS Things/<TARGET>的更新至本地
```
如果merge过程中，代码有冲突，需要解决所有代码冲突以后，才能提交。


# 关注我们
如果你想实时了解Alios-Things的动态，请记得点击watch持续关注

<div align=left display=flex>     <img src="https://img.alicdn.com/imgextra/i4/O1CN01sTnsXX1XMpFDKSs9b_!!6000000002910-2-tps-2830-912.png" style="max-width:800px;" /> </div>



# 点赞我们
如果你喜欢这个项目，就请给我们一个小星星吧

<div align=left display=flex>     <img src="https://img.alicdn.com/imgextra/i2/O1CN01J3HMIe1ecaBDvfWOl_!!6000000003892-2-tps-2818-850.png" style="max-width:800px;" /> </div>


**完整创建视频请参考如下**

[CreatePullRequest.mp4](https://yuque.antfin.com/attachments/lark/0/2021/mp4/1556466/1615277106309-09891a36-3e08-44e0-9aaa-4183b0635ef7.mp4?_lake_card=%7B%22uid%22%3A%221615277100374-0%22%2C%22src%22%3A%22https%3A%2F%2Fyuque.antfin.com%2Fattachments%2Flark%2F0%2F2021%2Fmp4%2F1556466%2F1615277106309-09891a36-3e08-44e0-9aaa-4183b0635ef7.mp4%22%2C%22name%22%3A%22CreatePullRequest.mp4%22%2C%22size%22%3A48166341%2C%22type%22%3A%22video%2Fmp4%22%2C%22ext%22%3A%22mp4%22%2C%22progress%22%3A%7B%22percent%22%3A99%7D%2C%22status%22%3A%22done%22%2C%22percent%22%3A0%2C%22id%22%3A%22iRu44%22%2C%22card%22%3A%22file%22%7D)