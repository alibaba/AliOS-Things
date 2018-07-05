EN | [中文](CI-System.zh)

### Background

[AliOS Things](https://github.com/alibaba/AliOS-Things) is a light-weight embedded operating system for Internet of Things (IoT). It contains plenty of components (real-time operation system kernel, connection protocols, file system, libc interface, FOTA, Mesh, speech recognition, etc.), and can support various kinds of chips (ARM series, Xtensa, C-Sky, etc.). It can be applied to scenarios such as smart homes and smart cities, making connecting of things easier.

AliOS Things is under rapid evolution. How to ensure the quality of submitted code and maintain its stability in different hardware platforms are our tough tasks. At the same time, since devices are fragmented in IoT, it brings about special requirements for CI system. In the following article, we will introduce the CI system in AliOS Things.

###  Introduction of AliOS Things CI system

CI system in AliOS Things is an automated code verification test system with close link to code base. The system will automatically run a full-platform compilation and full-function verification test for each code submitted by developers, and report them the PV test result. The operating process is as followed:

1. developers submit code (merge request) .
2. Precheck and Prebuild system check and compile the code automatically.
3. Prevalidate system and uDevice Center run the verification test of each function in that device.

Different from traditional CI system, the tests involved in AliOS Things are required to operate in varies kinds of embedded chips. In order to shield the diversity of different embedded systems, we have developed uDevice Ceter. uDevice Center is an online device (module/ board) debug platform, which standardize the differences in boards, enabling automatic test program to run by calling a unified interface. 



![CI系统](https://img.alicdn.com/tfs/TB1VrI6nhrI8KJjy0FpXXb5hVXa-8044-2986.png)



The architecture of AliOS Things CI system is shown as above. The system has currently supported  code.aliyun base, and support for github is being added. Besides code base, CI system mainly includes Precheck, Prebuild, Prevalidate system and uDevice Center. In the following text, we will give a detailed description of functions of each subsystem.




### Precheck system

Precheck is a system works for automated code scanning and quality monitoring. It can help developers to improve the quality of code and decrease the potential risks in code. The general operating process of it is:

1. Developers submit merge request to code base, which then sends request to Precheck system.
2. Precheck system receives the submitted code, pulls it to the local, and starts static code scanning (such as pmd, findbugs and androidlint).
3. Precheck system submits the result to code base ( and sends report to developers by e-mail automatically).

The architecture of Precheck system is shown as followed. It is flexible to increase and reduce service capacity according to different requests. 

![precheck](https://img.alicdn.com/tfs/TB15ZPlnf2H8KJjy0FcXXaDlFXa-5752-1619.png)



### Prebuild system

Prebulild is a distributed compilation system. It automatically compiles the code in different embedded systems with the help of Build Cluster, helping developers to verify the correctness of submitted code. The general operating process of it is:

1. Developers submit code to code base, and Prebuild system start compilation.
2. Prebuild system compiles the code, and reports build result to code base.
3. Prebuild system triggers PV test.

The architecture of Prebuild system is shown as followed. It's a fully Doctor system, which can increase and reduce service capacity according to different requests.

![Prebuild](https://img.alicdn.com/tfs/TB10ZIjnfDH8KJjy1XcXXcpdXXa-5752-1580.png)



### Prevalidate & uDevice Center system

Prevalidate is a automated test platform. It can manage the test code (java/python/shell ) written by users, combine test programs as test tasks, create/distribute/enforce/supervise/manage test tasks as required, get test results and notify users. However, it can't work in firmwares in embedded hardwares of AliOS Things. To solve it, we has developed uDevice Center, an online test platform for hardwares (modules/boards/devices). It is connected to these hardwares, and shield their differences in operation by a standardized interface, enabling test program to work in different boards. 

We have uploaded the test program for each function of AliOS Things to Prevalidate system, and combine them as different PV test tasks that can be triggered and implemented. When developers submit code and Prebuild system completes the compilation process：

1. Prebuild system will trigger Prevalidate system to create a PV test task.
2. Prevalidate system will choose an appropriate device in slave Cluster to carry out the test.
3. uDevice Center will allocate appropriate devices to test program, program the compilation to devices, shell interact with devices with standard interfaces and complete the test.
4. Prevalidate system will report the test result to code base, and e-mail it to developers

The architecture of Prevalidate system and uDevice Center is shown as followed. Similarly, the system has a good flexibility.

![Prevalidate](https://img.alicdn.com/tfs/TB1JlCbnDvI8KJjSspjXXcgjXXa-5755-1413.png)



### Summary

CI system can and will continue to support the rapid evolution of [AliOS Things](https://github.com/alibaba/AliOS-Things). In the near future, we plan to share it with our developers and partners, improving the development efficiency in IoT age.



### Acknowledge

Thanks to OS team and AliOS Things team for their massive help. 
