[EN](Manual) | 中文

# AliOS Things 自测指南

* [AliOS Things 认证框架](#framework)
* [AliOS Things 认证芯片类型](#chip)
  * [Wi-Fi SoC 类](#chip-wifi-soc)
  * [MCU+WiFi 类](#chip-mcu-wifi)
  * [MCU 类](#chip-mcu)
  * [Bluetooth SoC 类](#chip-bluetooth-soc)
  * [MCU + LoRa 类](#chip-mcu-lora)
* [AliOS Things 自测指南](#manual)
  * [Kernel 自测指南](Manual-API)
  * [通道自测指南](Manual-Channel)
  * [FOTA 自测指南](Manual-FOTA)
  * [uMesh 自测指南](Manual-uMesh)
  * [连接自测指南](Manual-Network)
  * [安全自测指南](Manual-Security)
  * [本地通信自测指南](Manual-Local)

<a id="framework"></a>
## AliOS Things认证框架
![](assets/certificate_framework.png)

> **AliOS Things Kernel 测试**所有类型的芯片都需要测试的基础项  
> **协议相关测试**：只针对WiFi SoC和蓝牙SoC  
> **通道测试**：AliOS Things中集成连接阿里云的通道能力，只针对Wi-Fi SoC、MCU+Wi-Fi类型  
> **FOTA测试**：针对移植了通道能力的芯片  
> **安全测试**：针对移植ID2全套安全能力的芯片

<a id="level"></a>
## AliOS Things 认证等级


<a id="chip"></a>
## AliOS Things 认证芯片分类

<a id="chip-wifi-soc"></a>
### Wi-Fi SoC 类认证
<table>
  <tr>
    <th>等级</th>
    <th>测试项</th>
    <th>说明</th>
    <th>测试指南</th>
    <th>测试规范</th>
  </tr>
  <tr>
    <td rowspan="4">基础版</td>
    <td>AliOS Things Kernel 测试</td>
    <td>必测</td>
    <td><a href="Manual-API">Kernel 自测指南</a></td>
    <td><a href="Docs-API">Kernel 测试规范</a></td>
  </tr>
  <tr>
    <td>AliOS Things 通道测试</td>
    <td>必测</td>
    <td><a href="Manual-Channel">通道自测指南</a></td>
    <td><a href="Docs-Channel">通道测试规范</a></td>
  </tr>
  <tr>
    <td>AliOS Things 连接测试</td>
    <td>必测</td>
    <td><a href="Manual-Network">连接自测指南</a></td>
    <td><a href="Docs-Network">连接测试规范</a></td>
  </tr>
  <tr>
    <td>AliOS Things FOTA 测试</td>
    <td>必测</td>
    <td><a href="Manual-FOTA">FOTA 自测指南</a></td>
    <td><a href="Docs-FOTA">FOTA 测试规范</a></td>
  </tr>
  <tr>
    <td rowspan="1">高级版</td>
    <td>AliOS Things uMesh 测试</td>
    <td>必测</td>
    <td><a href="Manual-uMesh">uMesh 自测指南</a></td>
    <td><a href="Docs-uMesh">uMesh 测试规范</a></td>
  </tr>
  <tr>
    <td rowspan="1">安全版</td>
    <td>AliOS Things 安全测试</td>
    <td>必测</td>
    <td><a href="Manual-Security">安全自测指南</a></td>
    <td><a href="Docs-Security">安全测试规范</a></td>
  </tr>
  <tr>
  <tr>
</table>

<a id="chip-mcu-wifi"></a>
### MCU+Wi-Fi 类认证
<table>
  <tr>
    <th>等级</th>
    <th>测试项</th>
    <th>说明</th>
    <th>测试指南</th>
    <th>测试规范</th>
  </tr>
  <tr>
    <td rowspan="4">基础版</td>
    <td>AliOS Things Kernel 测试</td>
    <td>必测</td>
    <td><a href="Manual-API">Kernel 自测指南</a></td>
    <td><a href="Docs-API">Kernel 测试规范</a></td>
  </tr>
  <tr>
    <td>AliOS Things 通道测试</td>
    <td>必测</td>
    <td><a href="Manual-Channel">通道自测指南</a></td>
    <td><a href="Docs-Channel">通道测试规范</a></td>
  </tr>
  <tr>
    <td>AliOS Things 连接测试</td>
    <td>必测</td>
    <td><a href="Manual-Network">连接自测指南</a></td>
    <td><a href="Docs-Network">连接测试规范</a></td>
  </tr>
  <tr>
    <td>AliOS Things FOTA 测试</td>
    <td>必测</td>
    <td><a href="Manual-FOTA">FOTA 自测指南</a></td>
    <td><a href="Docs-FOTA">FOTA 测试规范</a></td>
  </tr>
  <tr>
    <td rowspan="1">高级版</td>
    <td>AliOS Things uMesh 测试</td>
    <td>必测</td>
    <td><a href="Manual-uMesh">uMesh 自测指南</a></td>
    <td><a href="Docs-uMesh">uMesh 测试规范</a></td>
  </tr>
  <tr>
    <td rowspan="1">安全版</td>
    <td>AliOS Things 安全测试</td>
    <td>必测</td>
    <td><a href="Manual-Security">安全自测指南</a></td>
    <td><a href="Docs-Security">安全测试规范</a></td>
  </tr>
  <tr>
  <tr>
</table>


<a id="chip-mcu"></a>
### MCU 类认证
<table>
  <tr>
    <th>等级</th>
    <th>测试项</th>
    <th>说明</th>
    <th>测试指南</th>
    <th>测试规范</th>
  </tr>
  <tr>
    <td rowspan="1">基础版</td>
    <td>AliOS Things Kernel 测试</td>
    <td>必测</td>
    <td><a href="Manual-API">Kernel 自测指南</a></td>
    <td><a href="Docs-API">Kernel 测试规范</a></td>
  </tr>
</table>

<a id="chip-bluetooth-soc"></a>
### Bluetooth SoC 类认证
<table>
  <tr>
    <th>等级</th>
    <th>测试项</th>
    <th>说明</th>
    <th>测试指南</th>
    <th>测试规范</th>
  </tr>
  <tr>
    <td rowspan="1">基础版</td>
    <td>AliOS Things Kernel 测试</td>
    <td>必测</td>
    <td><a href="Manual-API">Kernel 自测指南</a></td>
    <td><a href="Docs-API">Kernel 测试规范</a></td>
  </tr>
  <tr>
    <td rowspan="2">高级版</td>
    <td>AliOS Things 通道测试</td>
    <td>必测</td>
    <td><a href="Manual-Channel">通道自测指南</a></td>
    <td><a href="Docs-Channel">通道测试规范</a></td>
  </tr>
  <tr>
    <td>AliOS Things uMesh 测试</td>
    <td>选测</td>
    <td><a href="Manual-uMesh">uMesh 自测指南</a></td>
    <td><a href="Docs-uMesh">uMesh 测试规范</a></td>
  </tr>
  <tr>
    <td rowspan="1">安全版</td>
    <td>AliOS Things 安全测试</td>
    <td>必测</td>
    <td><a href="Manual-Security">安全自测指南</a></td>
    <td><a href="Docs-Security">安全测试规范</a></td>
  </tr>
</table>

<a id="chip-mcu-lora"></a>
### MCU+LoRa 类认证
<table>
  <tr>
    <th>等级</th>
    <th>测试项</th>
    <th>说明</th>
    <th>测试指南</th>
    <th>测试规范</th>
  </tr>
  <tr>
    <td rowspan="1">基础版</td>
    <td>AliOS Things Kernel 测试</td>
    <td>必测</td>
    <td><a href="Manual-API">Kernel 自测指南</a></td>
    <td><a href="Docs-API">Kernel 测试规范</a></td>
  </tr>
  <tr>
    <td rowspan="1">高级版</td>
    <td>AliOS Things 通道测试</td>
    <td>必测</td>
    <td><a href="Manual-Channel">通道自测指南</a></td>
    <td><a href="Docs-Channel">通道测试规范</a></td>
  </tr>
  <tr>
    <td rowspan="1">安全版</td>
    <td>AliOS Things 安全测试</td>
    <td>必测</td>
    <td><a href="Manual-Security">安全自测指南</a></td>
    <td><a href="Docs-Security">安全测试规范</a></td>
  </tr>
</table>

<a id="manual"></a>
## AliOS Things 自测指南
#### [AliOS Things Kernel 自测指南](Manual-API)
#### [AliOS Things 通道自测指南](Manual-Channel)
#### [AliOS Things FOTA 自测指南](Manual-FOTA)
#### [AliOS Things uMesh 自测指南](Manual-uMesh)
#### [AliOS Things 连接自测指南](Manual-Network)
#### [AliOS Things 安全自测指南](Manual-Security)
#### [AliOS Things 本地通信自测指南](Manual-Local)
