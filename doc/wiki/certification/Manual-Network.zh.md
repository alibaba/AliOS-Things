[EN](Manual-Network)|中文

# AliOS Things 连接自测指南

## 下载测试使用的APP

详见[自有品牌APP和公版APP的介绍](https://living.aliyun.com/doc#muti-app.html)

## 测试前设置
### 权限配置
<span data-type="color" style="color:#F5222D;">APP安装后最好允许所有的权限，防止因为权限的问题导致wifi设备配网失败</span>
手机设置->安全设置->权限管理->demaopp->允许所有权限(不同手机设置路径可能不太一样)  
如下图:![](assets/awss_1.png)
### 注册测试账户
1.安装demoapp后，请注册有效的测试账户（手机账号即可注册）
如下图：![](assets/awss_2.png)

### 用户登入成功界面
<span data-type="color" style="color:#F5222D;">一个账户只能同时在一台手机上登入</span>
登入成功界面如下图:![](assets/awss_3.png)

## 设备配网
通过以下配网方式将设备添加至个人账户并绑定使用
1. 一键配网
2. 热点配网
3. 零配
4. 路由器配网
5. 扫码添加

### 一键配网
**前置条件**：手机需要连接设备欲连接的路由器
##### IOS 一键配网
<span data-type="color" style="color:#F5222D;">IOS上默认先走一键配网方式，失败后可以继续一键配网或者选择热点配网</span>
1.点击添加设备按钮,如下图：![](assets/awss_4.png)

2.选着设备类型（<span data-type="color" style="color:#F5222D;">请选择正确的产品类型，不正确的类型会配网失败</span>），点击添加  
![](assets/awss_5.png)

3.输入wifi密码<span data-type="color" style="color:#F5222D;">（请确保输入wifi密码正确，密码也不正确会配网失败）</span>再进行下一步   
![](assets/awss_6.png)

4.操作设备进入配网状态，设备进入配网状态后在demoapp上确认  
![](assets/awss_7.png)


5.设备开始进入配网
![](assets/awss_8.png)

6.配网成功后进行绑定，即可使用![](assets/awss_9.png)

##### Android 一键配网
<span data-type="color" style="color:#F5222D;">Android的默认的是热点配网模式，需要等热点配网失败后才可以进行一键配网，所以在android上想进行一键配网，得先模拟热点失败的场景</span>
方法一.手机关闭数据流量使热点配网失败  
方法二.设备在进入配网状态，等意见配网时再让设备进入配网状态   
 1.热点配网失败后选择“试试另一种配网方式”![](assets/awss_10.png)

 2.再次操作设备进入配网状态，设备进入配网状态后手机上点击确认 
![](assets/awss_11.png)

 3.设备开始配网  
![](assets/awss_12.png)

 4.设备配网成功，开始绑定使用![](assets/awss_13.png)

#### 热点配网
**前置条件**:
 1.手机插入SIM卡并开启数据流量  
 2.手机连接需要配网的wifi网络 
##### IOS 热点配网:
<span data-type="color" style="color:#F5222D;">IOS手机默认的配网方式是一键配网，在ios上验证热点配网需要先模拟一键配网失败场景，然后在选择热点配网</span>
模拟一键配网失败的场景
方法一.设备不进入配网，等进行热点配网时在操作设备进入配网
方法二.配网时先将网线的wlan扣断开，等提示一键配网失败后在重新插入  
 1.一键配网失败的后，在界面上选择“试试另一种配网方式”
![](assets/awss_14.png)

 2.重新操作设备进入配网模式，然后在手机上确认
![](assets/awss_15.png)


 3.根据界面提示，手动开启手机aha热点（ios需要手动开启）
![](assets/awss_16.png)

 4.设备连接上aha热点后返回到demoapp上进行下一步操作
![](assets/awss_17.png)

 5.设备连接上热点，等待手机获取待配网的设备信息
![](assets/awss_18.png)

 6.手机获取待配网的设备信息后开始配网
![](assets/awss_19.png)


 7.设备配网成功后提示手机切换网络，手动将手机的网络切回配网的wifi
![](assets/awss_20.png)

 8.手机收到配网成功信息，提示配网成功![](assets/awss_21.png)

##### Android 热点配网:
<span data-type="color" style="color:#F5222D;">Android默认配网方式就是热点配网，可以自动实现aha热点的开启与wifi的切换（需要用户允许权限与开启数据流量）</span>
 1.与一键配网一样，需要用户选择正确设备类型与输入wifi\_password，然后操作设备进入配网模式，android手机会自动开启aha与切换wifi,如下图所示:![](assets/awss_22.png)

2.手机自动切换上网ap![](assets/awss_23.png)
![](assets/awss_24.png)

3.配网成功开始绑定![](assets/awss_23.png)

#### 零配
待配网设备通过已配网设备获取到路由器的ssid与password,从而接入到wifi网络中  
**前置条件**
 1.需要用户名下已经绑定了一台支持零配的设备

##### 操作步骤
<span data-type="color" style="color:#F5222D;">零配在android和ios上的操作一致</span>
 1.用户可以通过一键配网或者热点配网先添加一台支持零配的设备
 2.待配网设备上电
 3.添加添加，在本地发现设备列表中选着待配网设备，点击添加
![](assets/awss_16.png)

 4.操作设备进入配网，设备开始配网
![](assets/awss_27.png)

 5.配网成功进行绑定使用![](assets/awss_28.png)

#### 路由器配网
路由器配网方式是设备上电主动连接路由器的发现热点，在请求配网后由路由器自动完成配网的剩余流程
**前置条件**
 1.设备需要支持路由器配网（上电扫描adha的功能）
 2.需要有集成配网功能的路由器（H3C）
 3.账户已经绑定了H3C路由器，且手机连接在该路由器的wifi
##### 操作步骤
 1.手机连接上H3C路由器的网络，点击添加/绑定
![](assets/awss_29.png)

 2.设备上电，等待设备连接发现热点后，app在本地发现列表中显示待配网设备![](assets/awss_30.png)

 3.App上选择发现的设备，点击添加
 4.在设备上操作设备进入配网模式，设备开始配网
![](assets/awss_31.png)

 5.配网完成后，绑定开始使用![](assets/awss_32.png)

#### 局域网内绑定
同一局域网内可以发现已经配好网但是没有绑定的设备，可以直接绑定，不需要再重新配网
#### 常见配网失败的原因:
 1.设备类型选择错误，导致wifi密码解析错误
 2.Wifi密码输入错误，导致设备连接不上wifi
 3.配网时没有是设备在正确的配网状态，导致设备不接收手机发出广播
 4.手机权限没有允许，导致手机开启热点失败与切换wifi失败
 5.手机未开启数据流量，导致热点配网时无法拿到解析password的密钥
 6.配网时同时有其他同学在配网，存在互相的干扰
