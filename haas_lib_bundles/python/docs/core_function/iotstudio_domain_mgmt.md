# IoT Studio应用发布、域名绑定与备案

根据 “[HaaS创意案例](https://haas.iot.aliyun.com/solution)” 中开发流程, 在进行了案例的“IoT Studio应用”开发之后，可通过IoT Studio工具提供的“预览”按钮，将该应用发布预览后，就可以通过预览链接远程地对设备进行控制以及状态查看了。本文以 “RGB灯带” 创意案例为例。
<div align="center">
<img src=../images/IoTStudio域名管理/preview_publish.png
 width=100%/>
</div>
<br>

> 注意：应用“预览”时提供的链接将会持续一段时间（约24小时），以满足开发调试需求，超时之后，该链接将会失效。如需使该应用长期在线，可在开发调试好之后，将该应用发布上线，这样就可以通过发布链接持续对设备进行远程访问了。

<br>

应用发布上线时，需要绑定到一个域名，本文就是详细描述 IoT Studio应用发布过程中的遇到的一些列问题，包括域名申请、域名绑定以及域名备案等相关信息。

<br>

## 1、IoT Studio域名相关流程
在HaaS开发框架中，你打造的设备、IoT Studio应用等都是归属于你所有，IoT Studio应用发布也是发绑定到你的域名。同时，这也是你很好的一张个人名片。

IoT Studio应用发布时域名相关流程如下：
<div align="center">
<img src=../images/IoTStudio域名管理/domain_management.png
 width=100%/>
</div>
<br>

- 如果你已申请过域名，可以如上图将该域名与IoT Studio应用进行绑定（本文2.2小节）
- 如果你未申请过域名，可以继续往下进行域名申请

<br>

## 2、域名添加与绑定
阿里云提供了完整的域名注册、查询、交易、管理等服务，本文只简单介绍IoT Studio应用中所需要用到的域名注册与CNAME解析服务，更多域名服务可以详细参考阿里云[域名服务](https://help.aliyun.com/product/35473.html)。

### 2.1、域名申请
以想要注册域名haaspython.cn为例，可以通过[阿里云万网](https://wanwang.aliyun.com)直接查询看看该域名是否被占用
<div align="center">
<img src=../images/IoTStudio域名管理/domain_search.png
 width=100%/>
</div>

<br>

结果查询到该域名已被注册，但是另一个不错的域名 haaspython.com.cn 是可用的，可以注册这个域名。将其“加入清单”。
<div align="center">
<img src=../images/IoTStudio域名管理/domain_addtocart.png
 width=100%/>
</div>

<br>
然后按照流程购买（域名可能需要个人或企业实名认证，按提示提供相关信息即可）。

<div align="center">
<img src=../images/IoTStudio域名管理/domain_purchase_success.png
 width=100%/>
</div>

<br>

接下来，就可以在域名控制台中进行域名管理及后续操作了。

<br>

### 2.2、添加域名绑定
到这里，你已经有了一个域名了，可以将IoT Studio应用绑定到该域名。回到IoT Studio应用开发工具，配置后续访问本应用的域名。如下图所示：
<br>
<div align="center">
<img src=../images/IoTStudio域名管理/domain_bind.png
 width=100%/>
</div>

<br>

本IoT Studio应用示例是“RGB灯带”，将访问URL定义为 RGBLight.haaspython.com.cn，后续访问本应用都将通过此链接访问。

<br>
域名绑定好后，为了使该访问该域名能访问到本IoT Studio应用，还需要在域名控制台中对该域名进行配置。

<br>

### 2.3、添加CNAME解析
接下来我们在[域名控制台](https://dc.console.aliyun.com/next/index#/domain-list/all)中对该域名进行CNAME解析配置。

<div align="center">
<img src=../images/IoTStudio域名管理/domain_analyse_configure.png
 width=100%/>
</div>

<br>
为什么是CNAME解析呢？IoT Studio应用发布时，会将应用发布在一个IoT Studio的服务器上并进行应用服务管理，CNAME会将你的域名重定向到IoT Studio相关的域名。这样就可以通过你的域名访问你发布在IoT Studio上的应用了。

<div align="center">
<img src=../images/IoTStudio域名管理/domain_cname_configure.png
 width=100%/>
</div>

<br>

添加完成后，等待几分钟等域名解析生效。

### 2.4、IoT Studio应用发布
至此，域名添加、绑定已经完成了，接下来将IoT Studio应用发布上线

<div align="center">
<img src=../images/IoTStudio域名管理/domain_publish_success.png
 width=100%/>
</div>

<br>
然后，就可以通过上述绑定的域名访问本应用了。（移动端和WEB浏览器都能访问，本案例是为移动应用进行尺寸设计，移动端访问效果更佳）
<div align="center">
<img src=../images/IoTStudio域名管理/domain_mobile_access_success.png
 width=33%/>
</div>

<br>

## 3、域名备案
### 3.1、为什么需要备案
按照上述步骤，域名添加与绑定、IoT Studio应用发布已完成，通过该域名逻辑上已经能访问到你的IoT Studio应用。不过通过该域名访问IoT Studio应用时，你可能会遇到未备案无法访问的情况。
<div align="center">
<img src=../images/IoTStudio域名管理/domain_access_deny.png
 width=33%/>
</div>

<br>

按照在境内提供互联网信息服务规定，应当依法履行ICP备案和公安备案手续，域名服务也是需要备案的，阿里云提供了完整的[备案服务](https://beian.aliyun.com)。


### 3.2、备案过程
在阿里云[备案服务](https://beian.aliyun.com)中点击“开始备案”，按照流程提供相应信息即可。
<div align="center">
<img src=../images/IoTStudio域名管理/ICP_general_process.png
 width=100%/>
</div>

<br>

#### 3.2.1、基础信息校验
根据实际情况填写备案的基础信息。
<div align="center">
<img src=../images/IoTStudio域名管理/ICP_fillin_basic_info.png
 width=100%/>
</div>

<br>

#### 3.2.2、主办者信息填写
根据实际情况填写此备案的主办者信息
<div align="center">
<img src=../images/IoTStudio域名管理/ICP_fillin_host_info.png
 width=100%/>
</div>

<br>

#### 3.2.3、网站信息填写
根据实际情况填写此备案的域名/网站相关信息。
<div align="center">
<img src=../images/IoTStudio域名管理/ICP_domain_config.png
 width=100%/>
</div>

<br>

注意：域名备案时需要绑定到一个服务器，可以选用阿里云[ECS服务器](https://www.aliyun.com/product/ecs)作为备案服务器。

<br>

#### 3.2.4、备案资料上传
根据实际情况上传备案所需资料。
<div align="center">
<img src=../images/IoTStudio域名管理/ICP_upload_files.png
 width=100%/>
</div>

<br>

点击“上传资料”后，手机扫码进行相关资料上传操作。

<div align="center">
<img src=../images/IoTStudio域名管理/ICP_upload_files_mobile.png
 width=100%/>
</div>

<br>

完成后提交所有信息、按照网站提示流程进行人脸核验，并进行材料审核评估。如果材料初审通过，将会被提交到管局终审，请随时关注备案进展及相关反馈信息。

<br>

备案成功后，就可以通过该域名正常访问你的IoT Studio应用了。

<br>

### 3.3、更多备案信息

更多备案相关信息，可以参考阿里云[备案服务专题文档](https://help.aliyun.com/product/35468.html)，全方位带你玩转ICP备案。


## 4、应用鉴权（守护你的IoT Studio应用安全）
至此，IoT Studio应用发布、域名绑定与备案就完成了，你也可以非常方便地通过你的域名访问该IoT Studio应用了。

<br>

但是，该域名并没有保护，得到该域名的人都能访问这个设备，在开发调试、原型验证展示时还好，如果是长期使用的设备，建议加上一个应用鉴权。

为物联网应用开发（IoT Studio）项目开通账号功能后，可为该应用添加鉴权功能，保障该应用的安全。 
### 4.1、开通应用账号功能
登陆[IoTStudio物联网应用开发控制台](https://studio.iot.aliyun.com/projects)，打开项目管理。
<div align="center">
<img src=../images/IoTStudio域名管理/IoTStudio_project_mgmt.png
 width=100%/>
</div>

<br>

点击该应用所属的项目，开通账号功能
<div align="center">
<img src=../images/IoTStudio域名管理/Enable_account_mgmt.png
 width=100%/>
</div>

<br>

### 4.2、添加应用鉴权
在IoT Studio应用开发工具中，点击“应用鉴权”，选择“账号”方式即可开启应用鉴权。
<div align="center">
<img src=../images/IoTStudio域名管理/IoTStudio_login_page.png
 width=100%/>
</div>

<br>

你还可以对应用登陆界面进行一定的配置。

### 4.3、发布上线

配置完成后，将IoT Studio重新发布上线，再通过域名访问该IoT Studio应用时，需要通过前面步骤中开通应用账号功能的手机号，以及验证码登录。
<div align="center">
<img src=../images/IoTStudio域名管理/IoTStudio_login_mobile.png
 width=33%/>
</div>

<br>

这样，为IoT Studio应用添加鉴权就完成了，可以登陆后，安全地访问你的IoT Studio应用。

<div align="center">
<img src=../images/IoTStudio域名管理/domain_mobile_access_success.png
 width=33%/>
</div>

<br>

恭喜你，完成了整个IoT Studio应用发布、域名绑定与备案的流程。更多IoT Studio相关文档，可以参考[IoT Studio物联网应用开发](https://help.aliyun.com/product/123205.html)系列文档


