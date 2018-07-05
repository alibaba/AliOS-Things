[EN](Add-a-new-board-example) | 中文

下面以stm32433为例，来说明如何根据现有的stm32433 helloworld keil工程来stm32433这一类型单板添加到现有make构建体系中：

## 1 添加board组件
如编译的目标是：nano@stm32l433-nucleo, 那么make体系在查找依赖组件时，会先从board目录下查找名为 stm32l433-nucleo的组件。所以需创建 board/stm32l433-nucleo目录，并在其中添加 stm32l433-nucleo.mk 文件，其内容可以参照已有的b_l475e.mk（删去其中一些不需要的内容 ）。
## 2 添加mcu组件
每个 board 组件都会依赖于一个 mcu组件，这个mcu组件名称定义在 board.mk 的 HOST_MCU_FAMILY 中。make体系会根据这个变量的值去platform/mcu下查找相应的组件。所以需在 platform/mcu/stm32l4xx 目录下创建 stm32l4xx.mk。mk可以参照原有的stm32l475.mk，但须注意修改其中的board名-D选项。注意组件的mk文件名必须和组件的目录名一样，否则会查找不到文件。另外，由于stm32l4xx下有多种board类型，所以在 stm32l433-nucleo.mk 又定义了 HOST_MCU_NAME，在 stm32l4xx.mk 中使用，用以区分。
## 3 增加源文件
一般原来一块board有多个keil工程，每个工程都有自己的包含main函数的源文件。比如helloworld工程，main是在rhinorun.c中。一般我们每种board的入口main函数是在 aos.c中，所以拷贝一份475的aos.c过来。但是不同的板子支持的kernel功能不同，433目前不支持动态分配内存，则需要将475 aoc.c中原有 krhino_task_dyn_create 改为 krhino_task_create。 另外将aos.c所依赖的hw.c也进行拷贝添加，并删除一些其中不需要的内容。
## 4 与原有工程隔离
在添加到make体系时，会有事先进行参照的keil工程，一般一个mcu/board的目录下会有 helloworld,cli,gui等工程。当加入make体系的文件和project共用同一个时，由于 make 体系是根据依赖查找组件，目前我们的依赖中也有一些冗余，会加入比原来keil工程中更多的源文件。比如vacall，vfs相关的源文件。要使得这些文件能够编过，需要修改 k_conig.h中的宏开关。但是一旦改了这些开关，原有keil工程就会由于缺少文件而编不过，为了不影响原有的keil工程。不能共用源文件，所以单独创建一个runapp目录（参照b_475e）与原有工程分开，将原有helloworld目录下文件拷贝过去，并且放置自己的k_config.h, aos.c在其中。
## 5 链接脚本分开
不同的board的链接脚本也要相互分开，mk中指定各自不同的。432可以拷贝475的链接脚本。
