EN | [中文](Starter-Kit-Tutorial.zh)

## Steps

### Download/install Visual Studio Code

<https://code.visualstudio.com/>

### Install AliOS Studio plug-in

1. Install C/C++ plug-in ![img](https://camo.githubusercontent.com/95b35a6d5c6fabdad7988dc46f1671301af0e1a7/68747470733a2f2f696d672e616c6963646e2e636f6d2f7466732f544231595653346768475942754e6a7930466e585858356c7058612d333237302d323138322e706e67)
2. install alios-studio plug-in ![img](https://camo.githubusercontent.com/f1283d5fce7661126bd4d8ce58e8157b438de9e4/68747470733a2f2f696d672e616c6963646e2e636f6d2f7466732f544231654653346768475942754e6a7930466e585858356c7058612d333237302d323138322e706e67)

> If you have already installed, please make sure that its version is higher than 0.7.1

### Development environment preparation

#### Linux/Mac

Refer to [AliOS-Things-Linux-Environment-Setup](https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-Linux-Environment-Setup)

Install :

- [GNU ARM Embedded Toolchain](https://launchpad.net/gcc-arm-embedded/+download)
- [Python 2](https://www.python.org/downloads/)
- [Git](https://git-scm.com/downloads)

Install python pip manager and install/upgrade  `aos-cube`  to global environment: `pip install -U aos-cube`

> aos-cube version >= 0.2.22

We recommend you to use serial tool minicom or [picocom](https://github.com/npat-efault/picocom).

#### Windows

Refer to [AliOS-Things-Windows-Environment-Setup](https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-Windows-Environment-Setup)
You can directly use the software package we have prepared : http://p28phe5s5.bkt.clouddn.com/setup_windows.zip>
You can also install the following software/tool : 

- [GNU ARM Embedded Toolchain](https://launchpad.net/gcc-arm-embedded/+download)
- [Python 2](https://www.python.org/downloads/)
- [Git](https://git-scm.com/downloads)
- [ST-LINK USB Driver](http://www.st.com/en/development-tools/stsw-link009.html)

After Python 2 and Git have been installed, install/upgrade `aos-cube`: `pip install -U aos-cube` in Git Bash 

> aos-cube version >= 0.2.22

### Download AliOS Things code

Copy from GitHub : `git clone https://github.com/alibaba/AliOS-Things.git`
Or from mirror site : `git clone https://gitee.com/alios-things/AliOS-Things.git`

#### Start compiling helloworld

1. Open AliOS Things code directory in Visual Studio Code ![img](https://camo.githubusercontent.com/29cb5eca1bcd2b7adc55d9808c764eedc56382b6/68747470733a2f2f696d672e616c6963646e2e636f6d2f7466732f544231524a794d6766315442754e6a7930466a5858616a795858612d333237302d323138322e706e67)![img](https://camo.githubusercontent.com/4dfa268081e5d1968607c25a02be794b5c0c7fa1/68747470733a2f2f696d672e616c6963646e2e636f6d2f7466732f544231763675506765755342754e6a79315863585863596a4658612d333237302d323138322e706e67)
2. Click  ![img](https://camo.githubusercontent.com/80128a0a0d83ad687a460df49f3cff30f4d4375f/68747470733a2f2f696d672e616c6963646e2e636f6d2f7466732f5442314843394b676b795742754e6a7930467058586173735858612d37322d32322e706e67) to start compiling. If toolchain is not found, it will be download to correct location.

> If toolchain is still not found after configuring, you can add `arm-none-eabi-` to global PATH.
> If not, you can copy the toolchain to the following path, or create symbolic link in the following link as long as toolchain exists in that path :
> AliOS-Things/build/compiler/arm-none-eabi-5_4-2016q3-20160926/OSX/
> AliOS-Things/build/compiler/arm-none-eabi-5_4-2016q3-20160926/Win32/
> AliOS-Things/build/compiler/arm-none-eabi-5_4-2016q3-20160926/Linux64/

```
../OSX
../Win32
../Linux64
├── arm-none-eabi/
├── bin/
├── lib/
└── share/

```

1. Occupation status of each component will be shown after compiling :![img](https://camo.githubusercontent.com/f387e327641f60edded345af88eda3596b232d60/68747470733a2f2f696d672e616c6963646e2e636f6d2f7466732f544231712e7755676e745942654e6a7931586458585858795658612d333237302d323138322e706e67)

#### Program to Starter Kit

1. Connect Starter Kit with your computer trough USB Micro line.
2. Click ![img](https://camo.githubusercontent.com/0035d5a101f84e8422c9b801891bf0ce01162efe/68747470733a2f2f696d672e616c6963646e2e636f6d2f7466732f544231547969706762475942754e6a7930466f58586369424658612d37352d32322e706e67) at the bottom to complete hardware programming. ![img](https://camo.githubusercontent.com/38e1cfa5270e84782d240fa8ce6a03c045df6caa/68747470733a2f2f696d672e616c6963646e2e636f6d2f7466732f5442316a57326e6765755342754e6a5373706c58586265387058612d333237302d323138322e706e67)

Open the serial port, and you can see `app_delayed_action` start in one second and it will be triggered every  seconds.

```
 [   1.000]<V> AOS [app_delayed_action#9] : app_delayed_action:9 app
 [   6.000]<V> AOS [app_delayed_action#9] : app_delayed_action:9 app
 [  11.000]<V> AOS [app_delayed_action#9] : app_delayed_action:9 app
 [  16.000]<V> AOS [app_delayed_action#9] : app_delayed_action:9 app

```

### GUI application case

Double click on  `starterkitgui@starterkit` in `BUILD TARGET`  on the left to start compiling GUI demo OF Starter Kit . When Starter Kit is connected to the computer through USB, click on `Upload`  to program the hardware.  ![img](https://camo.githubusercontent.com/57a833db3c217e0c679e803b64b7799a73978a87/68747470733a2f2f696d672e616c6963646e2e636f6d2f7466732f54423152576f396762535942754e6a537370695858584e7a7058612d333237302d323138322e706e67)

The demo will read real-time data of accelerometer and draw a picture displayed in LCD. ![img](https://camo.githubusercontent.com/c31117a6c143c46de428d0b5000350bf85eafebb/68747470733a2f2f696d672e616c6963646e2e636f6d2f7466732f5442316675694567776d5442754e6a793158625858614d725658612d313037322d313435382e706e67)

### Debug

#### Debug the configuration 

Click on the bottom on the left to switch to `Debug` page.

![img](https://camo.githubusercontent.com/2c578d033bb87064b16c7a8acf0dd88920e7f5eb/68747470733a2f2f696d672e616c6963646e2e636f6d2f7466732f54423170506c706775755342754e6a79315863585863596a4658612d333834302d323430302e706e67)

##### Mac/Linux

Select  `Debug @ Mac/Linux` .![img](https://camo.githubusercontent.com/f198da04cf30c2850820e80ff95f3ec85f3539f8/68747470733a2f2f696d672e616c6963646e2e636f6d2f7466732f5442315a7a68706775755342754e6a79315863585863596a4658612d333834302d323430302e706e67)

Update `AliOS-Things/.vscode/launch.json`  debug configuration file based on compiled and burned `app@board`. For example,  modify related configuration after compiling and programing   `starterkitgui@starterkit`.![img](https://camo.githubusercontent.com/8a645c81f8390605ae492cd80f195b9494b1c635/68747470733a2f2f696d672e616c6963646e2e636f6d2f7466732f544231745270746778535942754e6a5373706a58585837335658612d333834302d323430302e706e67)

##### Windows

Debugging in Windows is similar to configuration in Mac/Linux. The difference is that you need to select  `Debug @ Windows` and modify the path of `program` to a mirror file corresponding to the programed firmware, as shown in the following picture：

```
"program": "D:\\work\\AliOS-Things\\out\\starterkitgui@starterkit\\binary\\starterkitgui@starterkit.elf"

```

![img](https://camo.githubusercontent.com/7201925fbe5072d83657412e07d72a1be6baca80/68747470733a2f2f696d672e616c6963646e2e636f6d2f7466732f544231313479466743745942654e6a5373706b58586255385658612d333834302d323430302e706e67)

#### Start debugging 

Click the green ▶️ button in the top left corner (or F5) to start debugging.![img](https://camo.githubusercontent.com/ba60bcabcd14a90b7c4090b3f492627d98a05239/68747470733a2f2f696d672e616c6963646e2e636f6d2f7466732f54423137484e706741795742754e6a7930467058586173735858612d333834302d323430302e706e67)

According to the setting in  `launch.json` 

```
"text": "break application_start"

```

After debugging, it will automatically turn to the set breakpoint `application_start` , and debug toolbar will appear on the top to provide commonly-used single step debugging functions, as shown in the picture : ![img](https://camo.githubusercontent.com/b6a46ba20f4f7dc5c899a772ca2d69c05a5820a3/68747470733a2f2f696d672e616c6963646e2e636f6d2f7466732f5442317453386a6770755742754e6a5373706e585858314e5658612d333834302d323430302e706e67)

Similarly, you can right click to select`Run to Cursor` :![img](https://camo.githubusercontent.com/67e25eafbdc9667d1427c3764dc3a4cafee4a4b9/68747470733a2f2f696d672e616c6963646e2e636f6d2f7466732f54423157445a366762475942754e6a7930466f58586369424658612d333834302d323430302e706e67)

You can see variable values in the left view area : ![img](https://camo.githubusercontent.com/7b1016deec8561809ce5db23817537e8db748caa/68747470733a2f2f696d672e616c6963646e2e636f6d2f7466732f5442314a545a366762475942754e6a7930466f58586369424658612d333834302d323430302e706e67)

#### Complete debugging

So far, the basic debugging process has been completed. You can click on the red stop key on the upper toolbar to end debugging. 