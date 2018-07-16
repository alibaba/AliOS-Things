[EN](Certification-Process) | 中文

# AliOS Things 认证流程

* [Step1. 移植适配](#step1)
* [Step2. 认证申请](#step2)
* [Step3. 资质审核](#step3)
* [Step4. 商家自测](#step4)
* [Step5. 完善信息](#step5)
* [Step6. 设备寄样](#step6)
* [Step7. 认证测试](#step7)
* [Step8. 颁发证书](#step8)
  ![](assets/process.png)

<a id="step1"></a>
### Step1. 移植适配
AliOS Things已经在GitHub开源并且提供完善的移植文档。详见 [https://github.com/alibaba/AliOS-Things.git](https://github.com/alibaba/AliOS-Things.git)

<a id="step2"></a>
### Step2. 认证申请
认证申请步骤如下：
1. 访问 [IoT认证支撑系统](https://certification.aliyun.com)，并服务申请
2. 产品类型选择 **芯片**
3. 根据实际通信类型选择 **Wi-Fi/BLE** 等
4. 根据芯片能力选择套餐类型 **AliOS Things认证-基础版/高级版/专业版**，套餐包含的测试项见套餐页面

<a id="step3"></a>
### Step3. 资质审核
提交认证后可以在 **首页** 查看申请进度，建议线下跟任务处理人沟通，加快审核进度。资质审核主要核对商家信息和产品基本信息，请在认证申请阶段务必详细准确填写
> 原则上资质审核不超过两个工作日

<a id="step4"></a>
### Step4. 商家自测
资质审核通过后，商家根据自测用例列表进行自测，自测方法参见 [AliOS Things 自测指南](Manual)，自测过程中有任何问题可以通过钉钉沟通
连接配网自测需要使用手机APP和特定的设备激活码，为了提高测试效率，自测前邮件申请测试激活码和测试使用的APP

> 邮件地址：shaofa.lsf@alibaba-inc.com
> 邮件主题：配网测试设备申请-xxx公司

<a id="step5"></a>
### Step5. 完善信息
资质审核通过且商家自测通过后，请查询[IoT认证支撑系统](https://certification.aliyun.com)首页，完善认证申请的详细信息，包括基本信息、软件信息、硬件信息和电子资料
> 为了避免认证申请被驳回，请确保所填信息及上传的资料的**完整性**和**准确性**

<a id="step6"></a>
### Step6. 设备寄样
商家自测期间可以提前邮寄硬件，邮寄内容包括但不限于：
1. 开发板 5 套（含电源、连接线各5套）
2. 调试下载器 2 套（若需要）
3. 天线和通信卡 5套（若需要）
4. 通信模块 5套（若需要）
5. 串口线 5 套（Micro USB/Mini USB/DB9 RS232）
6. 杜邦线若干（若需要）
7. 等

```
邮寄地址：杭州市余杭区文一西路969号8号楼小邮局
联系电话：18557515801
收件人：少舒
```

> **注意：**
> 1. <a style="color:#ff0000">务必确保邮寄的硬件与自测的硬件相同</a>
> 2. <a style="color:#ff0000">为了避免二次邮寄，请务必一次性邮寄所有必需的硬件，邮寄前请通过钉钉或者电话确认</a>
> 3. <a style="color:#ff0000">若邮寄物中包含电池等易燃易爆物品，请务必提供相关合格证书或检验报告</a>

<a id="step7"></a>
### Step7. 认证测试
在商家自测全部通过并且收到设备后，阿里小二开始进行认证测试。认证期间会使用[阿里云云效系统](https://rdc.aliyun.com)作为缺陷管理系统，请提前注册阿里云账号。
> 为了提高认证效率，原则上认证周期不超过两个月，超期则判定为认证失败

<a id="step8"></a>
### Step8. 颁发证书
待所有认证测试项全部通过后，阿里云IoT会在[IoT认证支撑平台](https://certification.aliyun.com)颁发线上的AliOS Things认证证书。
