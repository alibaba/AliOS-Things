@page packageyaml_user_manual 构建配置 package.yaml

**[更正文档](https://gitee.com/alios-things/documentation/edit/rel_3.3.0/yaml/package.yaml_user_manual.md)** &emsp;&emsp;&emsp;&emsp; **[贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)**

package.yaml是组件的配置文件，采用yaml文本的方式，配置组件包含哪些源文件，头文件，宏，以及组件依赖关系等等，替代了传统的makefile文件。
# 构建规则说明
yaml语法说明：[https://www.ruanyifeng.com/blog/2016/07/yaml.html](https://www.ruanyifeng.com/blog/2016/07/yaml.html)<br />[https://yaml.org/spec/1.2/spec.html](https://yaml.org/spec/1.2/spec.html)<br />
<br />yaml格式校验工具：[https://www.bejson.com/validators/yaml_editor/](https://www.bejson.com/validators/yaml_editor/)<br />
<br />package.yaml模板生成：<br />在aos**工作目录**下使用。 当用户想把一个源码目录快速变为一个aos组件时，将组件代码复制到aos内相应的组件目录下，然后使用这个命令，会在`<dir>`目录下生成`package.yaml`文件模板，用户可根据需求再适当的调整修改`package.yaml`文件。
```makefile
aos convert <dir>
```


# 构建文件字段说明
| package.yaml | 必选 | 说明 |
| --- | --- | --- |
|  |  |  |
| **基本信息** |  |  |
| name | Y | 包名称 （符合C语言变量命名规则），长度少于等于64字节 |
| description | Y | 简洁直白用一句完成组件介绍 |
| version | Y | 组件版本号，**版本号必须与组件代码仓库的branch或者tag保持一致**。 |
| type | Y | 组件类型，为：<br />solution，board，chip，arch ，drv_core，drv_peripheral， drv_external_device， kernel， common<br />solution组件放在solutions目录下；<br />board，chip，arch组件放在hardware目录下；<br />drv_core，drv_peripheral， drv_external_device组件放在components/drivers目录下；<br />kernel组件放在kernel目录下；<br />common组件放在components目录下。 |
| tag | N | 组件分类，缺省值： "''。可以写中文，与代码目录结构和编译无关。 |
| keywords | N | 标签，会影响到组件被搜索的效果，合理的标签很重要 |
| author | N | 原作者信息和维护人员信息 |
| license | N | 源代码的许可证，要确保所有代码、文件的许可证不冲突。 |
| homepage | N | 组件的主页 |
|  |  |  |
| **依赖关系** |  |  |
| depends<br />  - comp1: "v1.0" | N | 指定该组件依赖的组件及版本，目前仅支持指定版本：如v1.0。后续将支持>=v1.0, >v1.0, ==v1.0, <=v1.0, <v1.0, !=v1.0等，并且需要用“”号括起来。<br />组件的版本请查询组件的package.yaml里面的version字段。<br />**注：对于solution的组件，可以依赖多个board组件。由board_name指定本次构建所采用的组件。** |
| depends<br />  - comp1: "v1.0 ? <COND>" | N | 条件依赖，当条件COND=1时，才引入该组件。条件由下文的def_config中宏指定。<br />如果该条件未定义，不会引入该组件。<br />注意“”号的写法，包含了版本号和条件。 |
| aos_version | N | 组件支持的 OS 的基线版本，缺省值：支持所有版本的OS |
|  |  |  |
| **板级信息** |  | **注：AliOS Things暂时不考虑异构多CPU的情况** |
| solution板级配置 |  |  |
| board_name | Y | 指定开发板组件名，未设置时，使用depends中 board 第一个组件 |
| cpu_id | Y | cpu编号，固定为cpu0。 |
| ld_script | N | 链接时使用的 LD 脚本，未设置时，使用对应的 board 的 LD 脚本 |
| board板级配置 |  |  |
| chip_name | Y | 指定芯片组件名 |
| ld_script | N | 链接时使用 的LD 脚本，未设置时，使用对应的 chip 的 LD 脚本 |
| chip板级配置 |  |  |
| cpu_name | Y | 该芯片使用的 CPU 型号 |
| ld_script | Y | 链接时使用 的LD 脚本 |
| arch_name | N | 芯片架构名称 |
| flash_program | N | 芯片 FLASH 烧写程序 |
| program_data_files<br />  - filename: 11.bin    <br />  address: 0xB32000<br />  - filename: 22.bin    <br />  address: 0xB52000 | N | aos burn默认会烧录编译出来的bin文件，如果需要额外烧录其它文件，可以使用program_data_files配置1个或多个文件。<br />filename：文件名<br />address: 烧录起始地址 |
| toolchain_prefix | N | 编译器前缀，例如：csky-abiv2-elf。未指定时，根据 cpu_name 来推断编译器前缀 |
|  |  |  |
| **编译链接信息** |  |  |
| include | N | 组件对外的公共头文件所在的目录，只能是该软件包下的目录，**不能使用外部目录，不能使用外部目录，不能使用外部目录**<br />**对外的头文件目录尽量少于3个** |
| internal_include | N | 组件内部使用的头文件所在的目录 |
| cflag | N | 该组件 C 编译器所需要要的编译参数 |
| cxxflag | N | 该组件 C++ 编译器所需要要的编译参数 |
| asmflag | N | 该组件 汇编 编译器所需要要的编译参数 |
| define<br />  AAA: 1<br />  STR: "abc" | N | 该组件内的宏定义<br />以**key: value**方式罗列<br />**不支持**后续加条件，如~~AAA: 1 ? <COND>~~ |
| libs<br />  - lib1.a | N | 该组件中支持的二进制静态库<br />以**数组**方式罗列 |
| libs<br />  - lib1.a ? <COND><br />  - lib2.a ? <!COND> | N | 根据条件，引入该组件中支持的二进制静态库<br />**仅包含一个条件**，但是可使用**not**逻辑，即!COND，当COND为0时，引入lib1.a<br />**不需要加引号“”；或者整个加引号，如“**lib1.a ? <COND>**”** |
| libpath | N | 指定静态库所在的路径（相对于该组件路径） |
| source_file<br />  - src/*.c | N | 指定参与编译的源代码文件，支持通配符，采用相对路径 |
| source_file<br />  - src/xx.c ? <COND> | N | 根据条件，引入源代码文件<br />仅包含一个条件，但是可使用not逻辑，即!COND，当COND为0时，引入src/xx.c<br />**不需要加引号“”；或者整个加引号，如“**lib1.a ? <COND>**”** |
| cflag | N | chip、board或者solution组件里的C编译器选项 |
| cxxflag | N | chip、board或者solution组件里的C++编译器选项 |
| asmflag | N | chip、board或者solution组件里的汇编编译器选项 |
| ldflag | N | chip、board或者solution组件里的链接选项 |
| prebuild_script | N | 组件编译前执行的脚本文件（带参数运行，参数包含toolchain名称，lib库名称，组件defconfig等）<br />脚本文件可**直接运行**，包含x属性，脚本第一行用#!指明运行脚本的程序，如#!/usr/bin/evn python<br /> |
| postbuild_script | N | 组件编译后执行的脚本文件（带参数运行，参数包含toolchain名称，lib库名称，和组件defconfig等）<br />脚本文件可**直接运行**，包含x属性，脚本第一行用#!指明运行脚本的程序，如#!/usr/bin/evn python |
| postimage_script | N | 构建image后执行的脚本文件（带参数运行，参数包含toolchain名称，elf名称）<br />脚本文件可**直接运行**，包含x属性，脚本第一行用#!指明运行脚本的程序，如#!/usr/bin/evn python |
|  |  |  |
| **配置信息** |  |  |
| def_config | N | 组件的可配置项。<br />当 common 组件、chip 组件、board 组件、solution 组件都存在相同配置时，优先顺序为 :solution > board > chip > arch > (drv_core, drv_peripheral, drv_external_device) > kernel > common<br />**上文中所提到的COND，在组件的def_config中定义，取值为0或1** |
|  |  |  |
| **安装信息** |  | 导出sdk时使用，将头文件和库文件安装到指定目录，提供给二次开发者。 |
| dest |  | 安装的目的路径 dest是相对路径，通常是相对于AOS 安装目录 |
| source |  | 安装源列表，支持通配符，相对路径<br />详细说明请参考《aos-tools用户手册》。 |



# 附录
## prebuild、postbuild、postimage脚本示例
aos-tools可运行外部脚本，并以"--longopt=value"的方式传入参数：

- prebuild_script: 在组件编译之前运行
- postbuild_script: 在组件编译完成之后运行
- postimage_script: 在工程构建完成之后运行
### python版本
```python
#!/usr/bin/env python
import os, sys, re, codecs

params = sys.argv[1:]
if os.path.isfile(sys.argv[1]):
    with codecs.open(sys.argv[1], 'r', 'UTF-8') as fh:
        params = fh.read().split()

# choose those arguments we wanted, and discard others
key_value = {}
pattern = re.compile(r'--(.+?)=(.*)')
for arg in params:
    if arg.startswith("--"):
        match = pattern.match(arg)
        if match:
            key = match.group(1)
            value = match.group(2)
            if key in ["toolchain", "lib", "target"]:
                key_value[key] = value

print("the script is %s" % sys.argv[0])
print("current dir is %s" % os.getcwd())
for key in key_value.keys():
    print("%s is %s" %(key, key_value[key]))

# =======================================================
# do our work

# =======================================================    

# result
print("run external script success")
exit(0)

```
### shell版本
```shell
#!/usr/bin/env bash

if [ "$(uname)" == "Darwin" ]; then
    os="Darwin"
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then   
    os="Linux"
elif ["$(expr substr $(uname -s) 1 10)"=="MINGW32_NT"];then    
    os="Win32"
fi
echo "os is $os"

if [ -f $1 ]; then
# read arguments from file, and choose those arguments we wanted
for word in $(<$1)
do
    key=$(echo $word | cut -d '=' -f1)
    if [ "$os" == "Darwin" ]; then
        value=$(echo $word | cut -d '=' -f2-)
    elif [ "$os" == "Linux" ]; then
        value=$(echo $word | cut -d '=' --complement -f1)
    else
        value=""
    fi
    case "$key" in
        --toolchain) toolchain=$value;;
        --lib) lib=$value;;
        --target) target=$value;;
        *) echo "discard option $word";;
    esac;
done
else
# choose those arguments we wanted, and discard others
until [ $# -eq 0 ]
do
    key=$(echo $1 | cut -d '=' -f1)
    value=$(echo $1 | cut -d '=' --complement -f1)
    case "$key" in
        --toolchain) toolchain=$value;shift;;
        --lib) lib=$value;shift;;
        --target) target=$value;shift;;
        *) echo "discard option $1";shift;;
    esac;
done
fi

echo "the script is $0"
echo "current dir is $(pwd)"
echo "toolchain is $toolchain"
echo "lib is $lib"
echo "target is $target"

# =======================================================
# do our work

# =======================================================

# result
echo "run external script success"
exit 0

```
## 制作二次开发SDK

- 编写package.yaml

  在各个组件内部，将需要导出的h文件和预编译库添加到package.yaml的install部分。添加规则如下：<br />例如某个组件A的实际的文件列表如下：
```tree
compa
├── package.yaml
├── README.md
├── src
│   ├── aaa.c
│   ├── bbb.c
│   ├── ccc.c
│   └── ddd.c
├── include
│   ├── aaa.h
│   ├── bbb.h
│   └── fff
│       └── ccc.h
└── interal_include
│    └── ddd.h
└── prebuilt
     └── libeeeee.a
```
的 `build_confg`部分指定了对外头文件和预编译库：
```yaml
build_config:
  include:
    - include/
  libs:
    - eee
  libpath:
    - prebuilt
```
那么install部分如下，其中头文件都放在Include目录下，预编译库都放在lib目录下。
```yaml
install:
  # copy aaa.h bbb.h to aos_sdk/include
  - dest: "include"
    source:
      - *.h
  # copy fff/ccc.h to aos_sdk/include/fff
  - dest: "include/fff"
    source:
      - fff/*.h
  # copy drivers/libmcuhaas1000.a to aos_sdk/lib/libmcuhaas1000.a
  - dest: "lib"
    source:
      - prebuilt/libeee.a
```

- 在solution目录下执行编译，aos make，编译成功后会将组件编译生成的静态库自动放到aos_sdk/lib目录下。
- 在solution目录下执行导出，aos sdk，将组件对外头文件和预编译库复制到aos_sdk里的include和lib目录下。
- 删除out目录，将当前solution目录打包，提供给其他开发者做二次开发。
## <COND>条件
在`package.yaml`里面添加组件依赖，或者添加源文件，或者添加预编译库时，经常会使用条件。仅当条件满足时，才引入某个组件，或者添加某个文件，或者添加某个预编译库等等。
### 单条件
```
depends:
  - comp1: "v1.0 ? <COND1>"
source_file:
  - src/xx.c ? <COND2>
libs:
  - lib1.a ? <COND3>
```
此处<>中的内容即条件，是当前组件或其他组件里的`def_config`所定义的宏。当宏为1时，表示条件成立，执行相应的操作：引入某个组件等等。当宏为0，或者未定义时，则不执行相应的操作。
### 非条件
```
depends:
  - comp1: "v1.0 ? <!COND1>"
source_file:
  - src/xx.c ? <!COND2>
libs:
  - lib1.a ? <!COND3>
```
支持使用`!`做逻辑非操作。一个未定义的宏，如果前面加了`!`，则表示条件成立。
### 与条件
```
depends:
  - comp1: "v1.0 ? <COND1, COND4, !COND5>"
source_file:
  - src/xx.c ? <COND2, COND6>
libs:
  - lib1.a ? <COND3, !COND7>
```
支持使用`,`号分隔多个条件，做逻辑与操作，即所有条件都满足，才执行相应的操作。其中每个条件可以加`!`号，做逻辑非操作。
### 或条件
```
depends:
  - comp1: "v1.0 ? <COND1>"
  - comp1: "v1.0 ? <COND4>"
  - comp1: "v1.0 ? <!COND5>"
source_file:
  - src/xx.c ? <COND2>
  - src/xx.c ? <COND6>
libs:
  - lib1.a ? <COND3>
  - lib1.a ? <!COND7>
```
没有显式使用逻辑或操作的方式。可以参考上面的例子中，将需要或的条件拆成单独一行。
