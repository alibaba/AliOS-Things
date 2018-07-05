EN | [中文](AliOS-Things-build-system.zh)

## AliOS Things build system

## build components

Generally speaking, the construction of a project contains following elements:

## ![img](https://camo.githubusercontent.com/37981e18fc025abcb28438b5e0c74362d9ccbf6e/68747470733a2f2f696d672e616c6963646e2e636f6d2f7466732f544231636455416a783649384b4a6a79304667585858587a5658612d3337372d3235372e706e67)

### Component-based thought

The so-called component-based thought means that all functional modules can be tailored and spliced at will. The realization of this idea is mainly dependent on the construction of system, where each component has its corresponding .mk file.

Because of the idea of component-based management, in order to isolate each component and configure each of them independently, a .mk file is placed in each component's directory to store its specific operation configuration, and association among components is defined through dependency relationship. The unified operating mechanism shared among all components of the system is placed in the .mk file under the build directory. In this way, isolation of components and flexibility of modification can be achieved at the same time. 

Overall process

There are three steps in the overall construction process:

![img](https://camo.githubusercontent.com/d6f91a86c9399e467210b5111d7842e72bc974d3/68747470733a2f2f696d672e616c6963646e2e636f6d2f7466732f544231486536506a435f49384b4a6a7930466f585861466e5658612d313239352d3434352e706e67)

In short, all the information required in the elements mentioned above except tool chain is included in, for example  config.mk, xx.c_opts and link.opts in the second steps, and the core of the construction is the generation and usage of these files.

More details are shown as followed:

![img](https://camo.githubusercontent.com/417e772b09ab7bc8c99e3f801d5a5e49eaf9fb94/68747470733a2f2f696d672e616c6963646e2e636f6d2f7466732f544231573145656a763648384b4a6a7930466a58586158657058612d313831362d3832302e706e67)

## Content of mk file

A module's mk file basically describes how the component is build, so it's very important. In the following text, examples will be given to illustrate its main content:

![img](https://camo.githubusercontent.com/561344a67df9fb3e43c8c719530061599fe93f51/68747470733a2f2f696d672e616c6963646e2e636f6d2f7466732f544231453550786a777644384b4a6a5373706c58586149454658612d313037332d3535322e706e67)

In this list, the global is the settings used in compilation of all components, while the local is used only in compilation of a specific component. A mk file describes the configuration information of a component. All the configuration settings can be specified in _CFLAGS and _LDFLAGS, including link scripts used in links.

### Detailed implementation process

In the following part, detailed implementation process and corresponding key code will be illustrated according to the elements mentioned above:

### Choose tool chain

### ![img](https://camo.githubusercontent.com/33b272521f12d7f3e93cebf0f6913ab4ed398efd/68747470733a2f2f696d672e616c6963646e2e636f6d2f7466732f544231583867766a764448384b4a6a7931586358586370645858612d3933372d3334382e706e67)

The host platform is set in aos.py, while the auxiliary command tools are set in aos_host_cmd.mk, which currently mainly support two host platforms, windows and linux64.

![img](https://camo.githubusercontent.com/3a860293ecf1046030c20ae013ac5cbae2d7b6ac/68747470733a2f2f696d672e616c6963646e2e636f6d2f7466732f5442316d4767646a787249384b4a6a7930467058586235685658612d3338362d3133322e706e67)

The setting of compilation tool chain is set in aos_target_xx.mk and aos_library_xx.mk.

### Find source file

![img](https://camo.githubusercontent.com/0d286130ba8754144b43145c6eee51223422fff9/68747470733a2f2f696d672e616c6963646e2e636f6d2f7466732f544231396f31326869714158754e6a7931586458586159635658612d313034342d3138312e706e67)

The compilation order is always the name of app @ the name of board. App and board are two entry components that the search process is relied on. 

The dependency search is mainly implemented through recursion: 

![img](https://camo.githubusercontent.com/797cbd1a28687569fe34e4415f5d459fd9820fa6/68747470733a2f2f696d672e616c6963646e2e636f6d2f7466732f5442316c3242496a4c4448384b4a6a7931586358586370645858612d313039382d3234362e706e67)

In fact, in addition to the source file, the search process will also find out information about the defined compilation options in mk file. The process is actually parsing the mk file of the component through recursion. It makes preparation for the later compilation and link steps.

### Generate config.mk

Store the information getting from the above recursion parsing about mk file in config.mk: 

![img](https://camo.githubusercontent.com/6b50b2583d4b3180b56a2546c60d4ccefd13a84a/68747470733a2f2f696d672e616c6963646e2e636f6d2f7466732f54423174574f376869714158754e6a7931586458586159635658612d3930332d35392e706e67)

Config.mk is actually the collection of information in mk file of all the components, while opts file is an independent file that reorganizes information in mk file based on each component.

### Compile

compilation order

![img](https://camo.githubusercontent.com/1cdaaab019de3161d0364c6975321ca00e10d362/68747470733a2f2f696d672e616c6963646e2e636f6d2f7466732f54423135515a676c5a6a49384b4a6a5373707058585862795658612d313237392d39342e706e67)

![img](https://camo.githubusercontent.com/745df825f2b9ab186e399f0f96994f41c45a2c0d/68747470733a2f2f696d672e616c6963646e2e636f6d2f7466732f544231756362446c337644384b4a6a5373706c58586149454658612d313438312d37352e706e67)

![img](https://camo.githubusercontent.com/34619ca79153cb3862d6370c231098c660dbd7d8/68747470733a2f2f696d672e616c6963646e2e636f6d2f7466732f544231665341676c5a6a49384b4a6a5373707058585862795658612d313439342d3130302e706e67)

compilation option of each component is generated.

![img](https://camo.githubusercontent.com/d915ff584b2005c990b3ea2b5bf5b2521876593e/68747470733a2f2f696d672e616c6963646e2e636f6d2f7466732f544231796567556c323648384b4a6a5373706d58586232575858612d313532382d3235342e706e67)

### Link

Link order

![img](https://camo.githubusercontent.com/4b6b1139f7220ff6b651f7d1cad9ea1582abebaa/68747470733a2f2f696d672e616c6963646e2e636f6d2f7466732f5442314d7a514c6c324448384b4a6a7931586358586370645858612d313237342d38362e706e67)

link option is generated.

![img](https://camo.githubusercontent.com/2c4a755c298fab29c7cb490aae97e1699993ed51/68747470733a2f2f696d672e616c6963646e2e636f6d2f7466732f544231797a336f6c384448384b4a6a5373706e5858624e415658612d313532312d3134312e706e67)

![img](https://camo.githubusercontent.com/90302135a6779f0f6d44ff1bec7c7b1e0ba33806/68747470733a2f2f696d672e616c6963646e2e636f6d2f7466732f5442313454506d6c386648384b4a6a793158625858624c645858612d313237342d36392e706e67)

### Binary and other processing

Unified binary processing, such as strip.

![img](https://camo.githubusercontent.com/a082ac59c2ae3cc91b694359f0160865c3455fe8/68747470733a2f2f696d672e616c6963646e2e636f6d2f7466732f5442315358774f6c324448384b4a6a7931586358586370645858612d3938362d3332352e706e67)

Execute self-defined operations for each component.

![img](https://camo.githubusercontent.com/4d978bfba06d8224c8ed1d363c5e439d126257d1/68747470733a2f2f696d672e616c6963646e2e636f6d2f7466732f544231563133576c347249384b4a6a7930467058586235685658612d313039352d36352e706e67)

![img](https://camo.githubusercontent.com/6fe1348a424c5393544e670dd0a06cb2e4ded3c0/68747470733a2f2f696d672e616c6963646e2e636f6d2f7466732f5442314e4f2e436c395f49384b4a6a7930466f585861466e5658612d313034382d32332e706e67)

## summary of key macro call relationship

FIND_COMPONENT --Find all the required component parameters: all basic components, recursive calls.

```
PREPROCESS_TEST_COMPONENT  -- Add the components needed for tests with no parameter.

```

PROCESS_COMPONENT -- Parse the mk file of each component, parameters: all components.

```
PROCESS_ONE_COMPONENT  -- Parse a component, parameter: a specific component.

```

WRITE_FILE_CREATE -- Write all the relevant information in config.mk and write compilation and link options in opts file.
