## #概述
板级描述文件用来描述板级硬件资源和能力，板级配置文件是用户根据板级描述文件对外设或端口参数进行定制而生成的。

## #板级硬件资源

### #ESP32KIT
##### 开发板


![1.jpeg | left | 684x377](https://cdn.yuque.com/lark/2018/jpeg/66207/1523277514490-fd34eae8-d34d-44ab-8148-68ae95e448de.jpeg "")

##### 原理图


![1.png | left | 684x403](https://cdn.yuque.com/lark/2018/png/66207/1523276594348-8af3a920-a48a-4690-868f-f253cfe9af92.png "")


##### AliosThings HAL层支持情况

<span data-type="color" style="color: rgb(245, 34, 45);">'Y'代表支持，'N'代表目前不支持，Y和N判断的依据是AlisOSThings是否真正实现了HAL层头文件定义的API。</span>

| IO/端口 | AliOSThings支持情况 | BoneLite支持情况 |
| --- | --- | --- |
| I2C | N | Y（支持模拟I2C） |
| ADC | N | Y |
| DAC | N | Y |
| GPIO | Y | Y |
| UART | Y | Y |
| PWM | Y （仅支持pwm\_led） | Y |
| WIFI | Y | Y |
| BT | Y | N |
| WDG | N | Y |
| RTC | N | Y |
| SPI | N | N |
| I2S | N | N |
| RF | N | N |
|  |  |  |

<span data-type="color" style="color: rgb(245, 34, 45);">第一期BoneLite支持计划：I2C、ADC、DAC、GPIO、UART、PWM、WDG、RTC（包括HAL层和addon层）</span>
##### 开发板IO/资源映射
<span data-type="color" style="color: rgb(245, 34, 45);">port值和配置文件中的port段要保持一致；</span>

| 功能/接口 | IO/资源映射 | port值 |
| --- | --- | --- |
| I2C0 | SDA(IO2)  SCL(IO4) | 0 |
| I2C1 | SDA(IO5)  SCL(IO12) | 1 |
| ADC0 | ADC2\_CH4(IO13) | 0 |
| ADC1 | ADC2\_CH7(IO27) | 1 |
| UART0 | 默认板级log打印 | 0 |
| UART1 | TXD(IO17) RXD(IO16) CTS(IO15) RTS(IO14) | 1 |
| UART2 | TXD(IO32)  RXD(IO33) | 2 |
| GPIO | GPIO19(IO19) GPIO21(IO21) GPIO22(IO22) GPIO23(IO23) | pin num |
| DAC0 | DAC(IO25) | 0 |
| DAC1 | DAC(IO26) | 1 |
| PWM0 | PWM(IO4) | 0 |
| PWM1 | PWM(IO35) | 1 |
| WIFI | 内置 |  |
| BT | 内置 |  |
| WDG | 内置 |  |
| SD | SD0(SD0) SD1(SD1) SD2(SD2) SD3(SD3) CMD(CMD) CLK(CLK) |  |
| SPI | 待定 |  |

备注：esp32Kit开发板把esp32的功能引脚全部引出了，用户可以自由定义各个管脚的功能，上面各个IO功能的分配参考了乐鑫 ESP-WROOM-32开发板的设计以及结合了Bone@Lite的具体应用场景，如果有需要，可以参考esp32的数据手册，对其进行调整。


## #addon模块集
[GPIO](https://lark.alipay.com/rw78zh/clk1pd/ovaqhd)
[PWM](https://lark.alipay.com/rw78zh/clk1pd/xyx543)
[ADC](https://lark.alipay.com/rw78zh/clk1pd/ov0928)
[DAC](https://lark.alipay.com/rw78zh/clk1pd/scrcq2)
[WDG](https://lark.alipay.com/rw78zh/clk1pd/du5y88)
[I2C\_GPIO](https://lark.alipay.com/rw78zh/clk1pd/ubnbav)
[UART](https://lark.alipay.com/rw78zh/clk1pd/xro2ag)
[RTC](https://lark.alipay.com/rw78zh/clk1pd/oqnkmc)

## #板级配置文件

### #组织形式
每个子模块都是一个json对象，模块中所有子模块都被组织到一个数组中。子模块的组织形式都是固定的，其基本模式如下：
```json
[
    {
    "id":子模块标志符0,
    "port":对应的端口0,
    "参数0":value0,
    "参数1":value1,
    "参数2":value2,
    "参数n":valuen,
    },
    {
    "id":子模块标志符1,
    "port":对应的端口1,
    "参数0":value0,
    "参数1":value1,
    "参数2":value2,
    "参数n":valuen,
    },
]

```

* id:每个模块都有一个id，类型是string，需要保证在该数组内保持唯一性，js层调用open函数时会使用该id；
* port:模块对应的端口，该值参考"开发板IO/资源映射"或者根据各个模块的使用说用确定，每个port都会和实际的资源相对应;
* 参数：参数名和参数值根据各个模块的使用说用确定;
示范如下：
```json

{
    /*串口配置*/
	"UART":[
		{
		"id":"uart1",
		"port":1,
		"data_width":3,
		"baud_rate":115200,
		"stop_bits":0,
		"flow_control":0,
		"parity_config":0
		},
		{
		"id":"uart2",
		"port":2,
		"data_width":3,
		"baud_rate":115200,
		"stop_bits":0,
		"flow_control":0,
		"parity_config":0
		}
	]
}
```


### #软件实现
<span data-type="color" style="color: rgb(245, 34, 45);">说明：'Y'代表执行成功或'YES'，'N'代表执行失败或'NO'.</span>
#### #初始化板级管理流程图
系统上电完毕，并挂载完毕文件系统，板级管理系统就会开始执行初始化，如果文件"/spiffs/board\_config.json"存在，就会读取并解析该文件，否者读取默认配置文件"default\_config.h",每个开发板都应该有自己的默认配置文件。


![板级管理初始化流程.png | left | 534x416](https://cdn.yuque.com/lark/0/2018/png/66207/1524463342260-2c83ec77-af93-4c0c-a934-196804ed2e04.png "")


代码示范：
```c
#define BOARG_CONFIG_FILE_PATH		"/spiffs/board_config.json"

int main(int argc,char ** argv){
    ......
    gravity_spiffs_init();/*挂载文件系统*/
    board_mgr_init(BOARG_CONFIG_FILE_PATH);/*初始化板级管理*/
    ......

    return 0;
}
```

#### #TODO


#### #绑定板级配置资源流程图
当调用js层open()函数(或register()函数等api)时，会涉及到对板级资源的绑定，某个板级配置只能被对应的端口/IO绑定一次，直到其主动释放绑定为止。



![绑定获取资源.png | left | 581.9574468085107x416](https://cdn.yuque.com/lark/0/2018/png/66207/1524463349606-d277f1d5-a40c-4c78-89bf-1bf085410929.png "")

代码示范：
```c
/*js层代码*/
var handle=UART.open('uart2');   /*uart2对应配置文件中的id字段*/

/*addon层代码*/
static be_jse_symbol_t * uart_open(void){
    ......
    item_handle_t uart_handle; /*资源句柄*/
	uart_handle.handle = 0xFFFFFFFF;
    ......
	ret = board_attach_item(MODULE_UART,data,&uart_handle);/*绑定资源*/
	if(0 != ret){ /*绑定失败*/
		LOGE("uart","board_attach_item fail!\n");
		goto out; /*失败返回*/
	}
	LOGI("uart","uart handle:%u\n",uart_handle.handle);
	/*绑定成功，获取资源*/
	uart_device = board_get_node_by_handle(MODULE_UART,&uart_handle);
	if(NULL == uart_device){
		LOGE("uart","board_get_node_by_handle fail!\n");
		goto out;
	}
    ......
	return new_int_symbol(uart_handle.handle); /*返回资源句柄*/
}
```

#### #TODO

#### #获取板级配置资源流程图
当绑定成功某资源后，js层的其它api调用一般都会涉及到对板级资源的获取。



![获取资源.png | left | 533x386.34615384615387](https://cdn.yuque.com/lark/0/2018/png/66207/1524463354172-0504f6dd-b6ef-4350-a45a-cfcea14197a8.png "")


代码示范：
```c
/*js层代码*/
var value = UART.write(handle,'hello123\r\n');  /*handle值是open返回的句柄*/

/*addon 层代码*/
static be_jse_symbol_t * uart_write(void){
    ......
	item_handle_t uart_handle;
    ......
	uart_device = board_get_node_by_handle(MODULE_UART,&uart_handle);/*获取资源*/
	if(NULL == uart_device){ /*失败返回*/
		LOGE("uart","board_get_node_by_handle fail!\n");
		goto out;
	}
    /*成功*/
    ......
}

```

#### #TODO

#### #释放板级资源
当需要关闭或释放某端口/IO时，会涉及到该流程。



![释放资源.png | left | 390x335](https://cdn.yuque.com/lark/0/2018/png/66207/1524463358759-0f7b1cfa-39c4-4d1e-9dee-7549ed33b4a5.png "")


代码示范：
```c
/*js层代码*/
UART.close(handle);

/*addon 层代码*/
static be_jse_symbol_t * uart_close(void){
    ......
	uart_device = board_get_node_by_handle(MODULE_UART,&uart_handle);/*获取板级资源*/
	if(NULL == uart_device){
		LOGE("uart","board_get_node_by_handle fail!\n");
		goto out;
	}
	ret = hal_uart_finalize(uart_device);/*HAL层释放资源*/
	if(0 != ret){
		LOGE("uart","hal_uart_init fail!\n");
		goto out;
	}
	board_disattach_item(MODULE_UART,&uart_handle);/*板级层释放资源*/
    ......
}
```
#### #TODO

#### #board-mgr API定义
```c
int8_t 	 board_mgr_init(const char * json_path);
int8_t 	 board_attach_item(addon_module_m  module,const char * name_id,item_handle_t * out);
int8_t 	 board_disattach_item(addon_module_m  module,item_handle_t* handle);
int8_t 	 board_check_attach_status(addon_module_m module,item_handle_t* handle);
void * 	 board_get_node_by_name(addon_module_m module,const char * name_id);
void * 	 board_get_node_by_handle(addon_module_m module,item_handle_t* handle);
```

<div class="bi-table">
  <table>
    <colgroup>
      <col width="90px">
      <col width="90px">
      <col width="90px">
    </colgroup>
    <tbody>
      <tr height="34px">
        <td rowspan="1" colspan="1">
          <div data-type="p">API</div>
        </td>
        <td rowspan="1" colspan="1">
          <div data-type="p">功能/出入参</div>
        </td>
        <td rowspan="1" colspan="1">
          <div data-type="p">说明</div>
        </td>
      </tr>
      <tr height="34px">
        <td rowspan="1" colspan="1">
          <div data-type="p">board_mgr_init</div>
        </td>
        <td rowspan="1" colspan="1">
          <div data-type="p"><span data-type="color" style="color: rgb(245, 34, 45);">功能：</span>板级管理初始化;</div>
          <div data-type="p"><span data-type="color" style="color: rgb(245, 34, 45);">入参：</span></div>
          <div data-type="p">json_path：json配置文件路径； </div>
          <div data-type="p"><span data-type="color" style="color: rgb(245, 34, 45);">返回值：</span></div>
          <div data-type="p">0=ok； -1=fail</div>
        </td>
        <td rowspan="1" colspan="1">
          <div data-type="p">在该path指定的文件不存在或读取数据失败的情况下，会从default_config.h获取默认的配置信息</div>
        </td>
      </tr>
      <tr height="34px">
        <td rowspan="1" colspan="1">
          <div data-type="p">board_attach_item</div>
        </td>
        <td rowspan="1" colspan="1">
          <div data-type="p"><span data-type="color" style="color: rgb(245, 34, 45);">功能：</span>绑定板级资源;</div>
          <div data-type="p"><span data-type="color" style="color: rgb(245, 34, 45);">入参：</span></div>
          <div data-type="p">module：模块名称，枚举值； </div>
          <div data-type="p">name_id：名字id，对应配置文件中的id项； </div>
          <div data-type="p">out：返回的资源句柄，如果失败，该值为NULL;</div>
          <div data-type="p"><span data-type="color" style="color: rgb(245, 34, 45);">返回值：</span></div>
          <div data-type="p">0=ok; -1=fail</div>
          <div data-type="p"></div>
        </td>
        <td rowspan="1" colspan="1">
          <div data-type="p">下面几种情况会导致绑定资源失败：</div>
          <div data-type="p">1)id对应的资源不存在，即没有在配置文件中进行配置;</div>
          <div data-type="p">2)该资源已经被被的模块绑定了(一个资源不能被重复的绑定)；</div>
        </td>
      </tr>
      <tr height="34px">
        <td rowspan="1" colspan="1">
          <div data-type="p">board_disattach_item</div>
        </td>
        <td rowspan="1" colspan="1">
          <div data-type="p"><span data-type="color" style="color: rgb(245, 34, 45);">功能：</span>释放板级资源; </div>
          <div data-type="p"><span data-type="color" style="color: rgb(245, 34, 45);">入参： </span></div>
          <div data-type="p">module：模块名称，枚举值； handle：资源句柄；
            <span data-type="color" style="color: rgb(245, 34, 45);">返回值： </span></div>
          <div data-type="p">0=ok; -1=fail
          </div>
        </td>
        <td rowspan="1" colspan="1">
          <div data-type="p"></div>
        </td>
      </tr>
      <tr height="34px">
        <td rowspan="1" colspan="1">
          <div data-type="p">board_check_attach_status</div>
        </td>
        <td rowspan="1" colspan="1">
          <div data-type="p"><span data-type="color" style="color: rgb(245, 34, 45);">功能：</span>检测某资源是否被绑定;
            <span data-type="color" style="color: rgb(245, 34, 45);">入参：</span> module：模块名称，枚举值； handle：资源句柄；
            <span data-type="color" style="color: rgb(245, 34, 45);">返回值：</span> 1=已经被绑定；0=没有被绑定
          </div>
        </td>
        <td rowspan="1" colspan="1">
          <div data-type="p"></div>
        </td>
      </tr>
      <tr height="34px">
        <td rowspan="1" colspan="1">
          <div data-type="p">board_get_node_by_name</div>
        </td>
        <td rowspan="1" colspan="1">
          <div data-type="p"><span data-type="color" style="color: rgb(245, 34, 45);">功能</span>：通过模块名字获取对应的配置信息;
            <span data-type="color" style="color: rgb(245, 34, 45);">入参</span>： module：模块名称，枚举值； &nbsp;</div>
          <div data-type="p">name_id：名字id，对应配置文件中的id项；
            <span data-type="color" style="color: rgb(245, 34, 45);">返回值</span>： NULL=获取失败；非NULL=成功;
          </div>
        </td>
        <td rowspan="1" colspan="1">
          <div data-type="p">下面几种情况或导致获取配置信息失败：</div>
          <div data-type="p">1.该name-id没有绑定该配置信息;</div>
          <div data-type="p">2.该name-id没有在板级配置文件中配置；</div>
        </td>
      </tr>
      <tr height="34px">
        <td rowspan="1" colspan="1">
          <div data-type="p">board_get_node_by_handle</div>
        </td>
        <td rowspan="1" colspan="1">
          <div data-type="p"><span data-type="color" style="color: rgb(245, 34, 45);">功能：</span>通过模块资源句柄获取对的配置信息; </div>
          <div data-type="p"><span data-type="color" style="color: rgb(245, 34, 45);">入参：</span> </div>
          <div data-type="p">module：模块名称，枚举值； </div>
          <div data-type="p"> handle：模块对应的资源句柄； </div>
          <div data-type="p"><span data-type="color" style="color: rgb(245, 34, 45);"> 返回值：</span> </div>
          <div data-type="p">NULL=获取失败；非NULL=成功;</div>
        </td>
        <td rowspan="1" colspan="1">
          <div data-type="p">下面几种情况或导致获取配置信息失败: </div>
          <div data-type="p">1.该handle没有绑定该配置信息; </div>
          <div data-type="p">2.该handle无效；
          </div>
        </td>
      </tr>
    </tbody>
  </table>
</div>


## #存储和升级
配置文件默认存放的位置是："/spiffs/board\_config.json",升级功能复用app-mgr的升级通道。


## #资源占用统计
<span data-type="color" style="color: rgb(245, 34, 45);">TODO</span>

#### #TODO
?
## #参考
* 《ESP32\_DEVKITC\_V4-SCH-20171206A.pdf》
* 《esp32\_hardware\_design\_guidelines\_cn.pdf》
* 《esp32\_technical\_reference\_manual\_cn.pdf》
* 《esp32\_datasheet\_cn.pdf》
* [https://github.com/alibaba/AliOS-Things](https://github.com/alibaba/AliOS-Things)








 