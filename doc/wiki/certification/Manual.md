EN | [中文](Manual.zh)

# AliOS Things self test guide

- [AliOS Things certification structure](https://github.com/alibaba/AliOS-Things/wiki/Manual#framework)
- AliOS Things certification chip 
  - [Wi-Fi SoC ](https://github.com/alibaba/AliOS-Things/wiki/Manual#chip-wifi-soc)
  - [MCU+WiFi](https://github.com/alibaba/AliOS-Things/wiki/Manual#chip-mcu-wifi)
  - [MCU](https://github.com/alibaba/AliOS-Things/wiki/Manual#chip-mcu)
  - [Bluetooth SoC](https://github.com/alibaba/AliOS-Things/wiki/Manual#chip-bluetooth-soc)
  - [MCU + LoRa](https://github.com/alibaba/AliOS-Things/wiki/Manual#chip-mcu-lora)
- AliOS Things self test guide
  - [Kernel self test guide](Manual-API)
  - [Channel self test guide](Manual-Channel)
  - [FOTA self test guide](Manual-FOTA)
  - [uMesh self test guide](Manual-uMesh)
  - [Network self test guide](Manual-Network)
  - [Security self test guide](Manual-Security)
  - [Local communication self test guide](Manual-Local)

## AliOS Things certification structure

![img](https://github.com/alibaba/AliOS-Things/wiki/assets/certificate_framework.png)

> **AliOS Things Kernel test**: target to the basic items that all chips need
> **Protocol related test**: target to WiFi SoC and Bluetooth SoC
> **Channel test**: target to Wi-Fi SoC and MCU+Wi-Fi
> **FOTA test**: target to chips with channel ability 
> **Security test**: target to chips with ID2 security ability 

<a id="level"></a>

## AliOS Things certification level

<a id="chip"></a>

## Different types of certification 

<a id="chip-wifi-soc"></a>

### Wi-Fi SoC 

<table>
  <tr>

```
<th>Level</th>
<th>Test item</th>
<th>Description</th>
<th>Test guide</th>
<th>Test standard</th>
```

  </tr>
  <tr>

```
<td rowspan="4">Basic version</td>
<td>AliOS Things Kernel test</td>
<td>must</td>
<td><a href="Manual-API">Kernel self test guide</a></td>
<td><a href="Docs-API">Kernel test standard</a></td>
```

  </tr>
  <tr>

```
<td>AliOS Things channel test</td>
<td>must</td>
<td><a href="Manual-Channel">Channel self test guide</a></td>
<td><a href="Docs-Channel">Channel test standard</a></td>
```

  </tr>
  <tr>

```
<td>AliOS Things network test</td>
<td>must</td>
<td><a href="Manual-Network">network self test guide</a></td>
<td><a href="Docs-Network">network test standard</a></td>
```

  </tr>
  <tr>

```
<td>AliOS Things FOTA test</td>
<td>must</td>
<td><a href="Manual-FOTA">FOTA self test guide</a></td>
<td><a href="Docs-FOTA">FOTA test standard</a></td>
```

  </tr>
  <tr>

```
<td rowspan="1">Advanced version</td>
<td>AliOS Things uMesh test</td>
<td>must</td>
<td><a href="Manual-uMesh">uMesh self test guide</a></td>
<td><a href="Docs-uMesh">uMesh test standard</a></td>
```

  </tr>
  <tr>

```
<td rowspan="1">Security version</td>
<td>AliOS Things security test</td>
<td>must</td>
<td><a href="Manual-Security">security self test guide</a></td>
<td><a href="Docs-Security">security test standard</a></td>
```

  </tr>
  <tr>
  <tr>
</table>

<a id="chip-mcu-wifi"></a>

### MCU+Wi-Fi 

<table>
  <tr>

```
<th>Level</th>
<th>Test item</th>
<th>Description</th>
<th>Test guide</th>
<th>Test standard</th>
```

  </tr>
  <tr>

```
<td rowspan="4">Basic version</td>
<td>AliOS Things Kernel test</td>
<td>must</td>
<td><a href="Manual-API">Kernel self test guide</a></td>
<td><a href="Docs-API">Kernel test standard</a></td>
```

  </tr>
  <tr>

```
<td>AliOS Things channel test</td>
<td>must</td>
<td><a href="Manual-Channel">Channel self test guide</a></td>
<td><a href="Docs-Channel">Channel test standard</a></td>
```

  </tr>
  <tr>

```
<td>AliOS Things network test</td>
<td>must</td>
<td><a href="Manual-Network">network self test guide</a></td>
<td><a href="Docs-Network">network test standard</a></td>
```

  </tr>
  <tr>

```
<td>AliOS Things FOTA test</td>
<td>must</td>
<td><a href="Manual-FOTA">FOTA self test guide</a></td>
<td><a href="Docs-FOTA">FOTA test standard</a></td>
```

  </tr>
  <tr>

```
<td rowspan="1">Advanced version</td>
<td>AliOS Things uMesh test</td>
<td>must</td>
<td><a href="Manual-uMesh">uMesh self test guide</a></td>
<td><a href="Docs-uMesh">uMesh test standard</a></td>
```

  </tr>
  <tr>

```
<td rowspan="1">Security version</td>
<td>AliOS Things security test</td>
<td>must</td>
<td><a href="Manual-Security">security self test guide</a></td>
<td><a href="Docs-Security">security test standard</a></td>
```

  </tr>
  <tr>
  <tr>
</table>

<a id="chip-mcu"></a>

### MCU 

<table>
  <tr>

```
<th>Level</th>
<th>Test item</th>
<th>Description</th>
<th>Test guide</th>
<th>Test standard</th>
```

  </tr>
  <tr>

```
<td rowspan="1">Basic version</td>
<td>AliOS Things Kernel test</td>
<td>must</td>
<td><a href="Manual-API">Kernel self test guide</a></td>
<td><a href="Docs-API">Kernel test standard</a></td>
```

  </tr>
</table>

<a id="chip-bluetooth-soc"></a>

### Bluetooth SoC 

<table>
  <tr>

```
<th>Level</th>
<th>Test item</th>
<th>Description</th>
<th>Test guide</th>
<th>Test standard</th>
```

  </tr>
  <tr>

```
<td rowspan="1">Basic version</td>
<td>AliOS Things Kernel test</td>
<td>must</td>
<td><a href="Manual-API">Kernel self test guide</a></td>
<td><a href="Docs-API">Kernel test standard</a></td>
```

  </tr>
  <tr>

```
<td rowspan="2">Advanced version</td>
<td>AliOS Things channel test</td>
<td>must</td>
<td><a href="Manual-Channel">Channel self test guide</a></td>
<td><a href="Docs-Channel">Channel test standard</a></td>
```

  </tr>
  <tr>

```
<td>AliOS Things uMesh test</td>
<td>must</td>
<td><a href="Manual-uMesh">uMesh self test guide</a></td>
<td><a href="Docs-uMesh">uMesh test standard</a></td>
```

  </tr>
  <tr>

```
<td rowspan="1">Security version</td>
<td>AliOS Things security test</td>
<td>must</td>
<td><a href="Manual-Security">security self test guide</a></td>
<td><a href="Docs-Security">security test standard</a></td>
```

  </tr>
</table>

<a id="chip-mcu-lora"></a>

### MCU+LoRa 

<table>
  <tr>

```
<th>Level</th>
<th>Test item</th>
<th>Description</th>
<th>Test guide</th>
<th>Test standard</th>
```

  </tr>
  <tr>

```
<td rowspan="1">Basic version</td>
<td>AliOS Things Kernel test</td>
<td>must</td>
<td><a href="Manual-API">Kernel self test guide</a></td>
<td><a href="Docs-API">Kernel test standard</a></td>
```

  </tr>
  <tr>

```
<td rowspan="1">Advanced version</td>
<td>AliOS Things channel test</td>
<td>must</td>
<td><a href="Manual-Channel">Channel self test guide</a></td>
<td><a href="Docs-Channel">Channel test standard</a></td>
```

  </tr>
  <tr>

```
<td rowspan="1">Security version</td>
<td>AliOS Things security test</td>
<td>must</td>
<td><a href="Manual-Security">security self test guide</a></td>
<td><a href="Docs-Security">security test standard</a></td>
```

  </tr>
</table>

<a id="manual"></a>

#### AliOS Things Kernel self test guide](Manual-API)

#### [AliOS Things channel self test guide](Manual-Channel)

#### [AliOS Things FOTA self test guide](Manual-FOTA)

#### [AliOS Things uMesh self test guide](Manual-uMesh)

#### [AliOS Things network self test guide](Manual-Network)

#### [AliOS Things sucurity self test guide](Manual-Security)

#### [AliOS Things local communication self test guide](Manual-Local)