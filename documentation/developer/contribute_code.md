@page contribute_code 贡献代码说明

**[更正文档](https://gitee.com/alios-things/documentation/edit/rel_3.3.0/developer/contribute_code.md)** &emsp;&emsp;&emsp;&emsp; **[贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)**

项目地址：[https://gitee.com/alios-things/AliOS-Things](https://gitee.com/alios-things/AliOS-Things)


# 创建 Fork下载代码
1、Gitee 上的AliOS-Things仓库里单击 “fork” 按钮，将AliOS Things的仓库fork一份到开发者自己的名下

<div align=left display=flex>     <img src="https://intranetproxy.alipay.com/skylark/lark/0/2021/png/1556466/1614841243473-57572d9e-be00-463f-aec3-6c3e5962f8ad.png#align=left&display=inline&height=609&margin=%5Bobject%20Object%5D&name=image.png&originHeight=1218&originWidth=2590&size=1497232&status=done&style=none&width=1295" style="max-width:800px;" /> </div>

2、进入开发者fork出来的AliOS Things仓库，注意，该仓库地址已经从AliOS Things/AliOS-Things变成<YOUR_NAME>/AliOS-Things，如下图所示。然后根据图中提示的步骤复制仓库地址。

<div align=left display=flex>     <img src="https://intranetproxy.alipay.com/skylark/lark/0/2021/png/1556466/1614841664314-81b0e23d-b41e-42ee-adbb-ee8f0bb184b9.png#align=left&display=inline&height=1074&margin=%5Bobject%20Object%5D&name=%E6%88%AA%E5%B1%8F2021-03-04%20%E4%B8%8B%E5%8D%883.03.39.png&originHeight=1074&originWidth=2602&size=447285&status=done&style=none&width=2602" style="max-width:800px;" /> </div>




3、下载代码
注意：gitee提供多种下载方式，本文使用https，下载代码时会要求输入密码，密码是开发者的gitee登录密码。
```bash
# 注意YOUR_NAME替换成开发者Github账号的名称
$ git clone https://gitee.com/<YOUR_NAME>/AliOS-Things.git
```
# 本地开发并提交
本文以更新master分支的代码为例子，提供完整开发命令供参考
```bash
# 1、Checkout and switch to your new branch
$ cd AliOS-Things
$ git checkout master

# Commit and push your code
git add -A				#-A表示将本地所有修改文件提交，若只需要提交若干文件，请将-A替换成文件名
git commit -s			#填写log日志，说明变更的内容，影响功能点等
git push					#将本地代码push到github
```

在gitee网页按下图操作，如果出现你的提交记录，说明push成功

<div align=left display=flex>     <img src="https://intranetproxy.alipay.com/skylark/lark/0/2021/png/1556466/1614842472400-295066eb-fee4-4168-a685-a05b79060f8c.png#align=left&display=inline&height=482&margin=%5Bobject%20Object%5D&name=image.png&originHeight=964&originWidth=2718&size=788421&status=done&style=none&width=1359" style="max-width:800px;" /> </div>



# 创建pull request
当功能开发完成后，可以发起 Pull Request，申请合入代码到AliOS Things/AliOS-Things。

在开发者gitee的AliOS-Things仓库页面上，然后单击“ Pull requests ”按钮发起“+新建 pull request”. 如下图网页操作，创建一个pull request



<div align=left display=flex>     <img src="https://intranetproxy.alipay.com/skylark/lark/0/2021/png/1556466/1614842538613-e99d19d6-c512-47c2-81fc-00b0ed78d43e.png#align=left&display=inline&height=336&margin=%5Bobject%20Object%5D&name=image.png&originHeight=672&originWidth=2530&size=574736&status=done&style=none&width=1265" style="max-width:800px;" /> </div>

<div align=left display=flex>     <img src="https://intranetproxy.alipay.com/skylark/lark/0/2021/png/1556466/1614842828251-ff9faa1a-64ac-464f-86f3-b3348286d689.png#align=left&display=inline&height=448&margin=%5Bobject%20Object%5D&name=image.png&originHeight=896&originWidth=1260&size=282962&status=done&style=none&width=630" style="max-width:800px;" /> </div>



在AliOS Things/AliOS-Things 页面，看到自己的pull request请求，即说明创建成功，等待审核

<div align=left display=flex>     <img src="https://intranetproxy.alipay.com/skylark/lark/0/2021/png/1556466/1614842891037-aeafad5a-e058-4824-b755-ee84bac9e697.png#align=left&display=inline&height=160&margin=%5Bobject%20Object%5D&name=image.png&originHeight=320&originWidth=1238&size=166613&status=done&style=none&width=619" style="max-width:800px;" /> </div>




# 同步代码
在发起 Pull Request 之前，有可能原来的仓库AliOS Things/AliOS-Things有代码更新，那么请按照以下步骤，拉取AliOS Things/AliOS-Things最新的代码至本地。
```bash
$ git remote add upstream https://gitee.com/alios-things/AliOS-Things.git
$ git remote -v													#此时本地有两个远程地址，origin和upstream，origin对应fork仓库，upstream对应AliOS Things仓库
$ git fetch upstream										#将alibaba仓库代码更新到本地									
$ git checkout master				            #切换到master分支				
$ git merge upstream/master		          #合并AliOS Things/AliOS-Things的更新至本地		
```
如果merge过程中，代码有冲突，需要解决所有代码冲突以后，才能提交。




# 关注我们
如果你想实时了解Alios-Things的动态，请记得点击watch持续关注

<div align=left display=flex>     <img src="https://intranetproxy.alipay.com/skylark/lark/0/2021/png/1556466/1614840333650-bbf18cc9-f3e9-4ed5-876f-e00cc7d174f4.png#align=left&display=inline&height=912&margin=%5Bobject%20Object%5D&name=%E6%88%AA%E5%B1%8F2021-03-04%20%E4%B8%8B%E5%8D%882.42.52.png&originHeight=912&originWidth=2830&size=354484&status=done&style=none&width=2830" style="max-width:800px;" /> </div>



# 点赞我们
如果你喜欢这个项目，就请给我们一个小星星吧

<div align=left display=flex>     <img src="https://intranetproxy.alipay.com/skylark/lark/0/2021/png/1556466/1614840686285-3f46d5e6-4379-43d4-ad64-8bbd79c8b1e9.png#align=left&display=inline&height=850&margin=%5Bobject%20Object%5D&name=%E6%88%AA%E5%B1%8F2021-03-04%20%E4%B8%8B%E5%8D%882.48.37.png&originHeight=850&originWidth=2818&size=331043&status=done&style=none&width=2818" style="max-width:800px;" /> </div>



**完整创建视频请参考如下**
[CreatePullRequest.mp4](https://yuque.antfin.com/attachments/lark/0/2021/mp4/1556466/1615277106309-09891a36-3e08-44e0-9aaa-4183b0635ef7.mp4?_lake_card=%7B%22uid%22%3A%221615277100374-0%22%2C%22src%22%3A%22https%3A%2F%2Fyuque.antfin.com%2Fattachments%2Flark%2F0%2F2021%2Fmp4%2F1556466%2F1615277106309-09891a36-3e08-44e0-9aaa-4183b0635ef7.mp4%22%2C%22name%22%3A%22CreatePullRequest.mp4%22%2C%22size%22%3A48166341%2C%22type%22%3A%22video%2Fmp4%22%2C%22ext%22%3A%22mp4%22%2C%22progress%22%3A%7B%22percent%22%3A99%7D%2C%22status%22%3A%22done%22%2C%22percent%22%3A0%2C%22id%22%3A%22iRu44%22%2C%22card%22%3A%22file%22%7D)