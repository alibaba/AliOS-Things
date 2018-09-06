## #UART API

<div class="bi-table">
  <table>
    <colgroup>
      <col width="90px" />
      <col width="90px" />
    </colgroup>
    <tbody>
      <tr>
        <td rowspan="1" colSpan="1">
          <div data-type="p">API</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">说明</div>
        </td>
      </tr>
      <tr>
        <td rowspan="1" colSpan="1">
          <div data-type="p">UART.open(id)</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p"><span data-type="color" style="color:rgb(245, 34, 45)">功能：</span>打开串口</div>
          <div data-type="p"><span data-type="color" style="color:rgb(245, 34, 45)">参数：</span> id:和板级配置文件中的id保持一致</div>
          <div data-type="p"><span data-type="color" style="color:rgb(245, 34, 45)">返回值：</span>返回资源handle</div>
        </td>
      </tr>
      <tr>
        <td rowspan="1" colSpan="1">
          <div data-type="p">UART.read(handle，cb)</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p"><span data-type="color" style="color:rgb(245, 34, 45)">功能：</span>从串口中读取数据</div>
          <div data-type="p"><span data-type="color" style="color:rgb(245, 34, 45)">参数：</span> handle:资源handle，为UART.open的返回值;</div>
          <div data-type="p">cb：读回调，返回读取状态值err和数据；</div>
          <div data-type="p"><span data-type="color" style="color:rgb(245, 34, 45)">返回值：</span>0=ok other=fail</div>
          <div data-type="p"></div>
        </td>
      </tr>
      <tr height="34px">
        <td rowspan="1" colSpan="1">
          <div data-type="p">UART.on(handle，cb)</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p"><span data-type="color" style="color:#F5222D">功能：</span>从串口中读取数据; </div>
          <div data-type="p"><span data-type="color" style="color:#F5222D">参数：</span> handle:资源handle，为UART.open的返回值; </div>
          <div data-type="p"><span data-type="color" style="color:#F5222D">cb：</span>监控回调，返回读取状态值err和数据； </div>
          <div data-type="p"><span data-type="color" style="color:#F5222D">返回值：</span>0=ok other=fail </div>
          <div data-type="p">
          </div>
        </td>
      </tr>
      <tr>
        <td rowspan="1" colSpan="1">
          <div data-type="p">UART.write(handle,str)</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p"><span data-type="color" style="color:rgb(245, 34, 45)">功能：</span>串口发送数据</div>
          <div data-type="p"><span data-type="color" style="color:rgb(245, 34, 45)">参数：</span> handle:资源handle，为UART.open的返回值;  </div>
          <div data-type="p">          str：待发送字符串</div>
          <div data-type="p"><span data-type="color" style="color:rgb(245, 34, 45)">返回值：</span>0=ok other=fail</div>
          <div data-type="p"></div>
        </td>
      </tr>
      <tr height="34px">
        <td rowspan="1" colSpan="1">
          <div data-type="p">UART.close(handle)</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p"><span data-type="color" style="color:#F5222D">功能：</span>关闭串口; </div>
          <div data-type="p"><span data-type="color" style="color:#F5222D">参数：</span> handle:资源handle，为UART.open的返回值; </div>
          <div data-type="p"><span data-type="color" style="color:#F5222D">返回值：</span>0=ok other=fail
          </div>
        </td>
      </tr>
    </tbody>
  </table>
</div>


## #板级配置参数


<div class="bi-table">
  <table>
    <colgroup>
      <col width="90px" />
      <col width="90px" />
      <col width="90px" />
    </colgroup>
    <tbody>
      <tr height="34px">
        <td rowspan="1" colSpan="1">
          <div data-type="p">参数名</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">类型/功能/值</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">说明</div>
        </td>
      </tr>
      <tr height="34px">
        <td rowspan="1" colSpan="1">
          <div data-type="p">&quot;id&quot;</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p"><span data-type="color" style="color:#F5222D">功能：</span>资源唯一性标志 </div>
          <div data-type="p"><span data-type="color" style="color:#F5222D">类型：</span>string </div>
          <div data-type="p"><span data-type="color" style="color:#F5222D">值：</span>任意，保持数组内id值唯一;</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">该id值和js层open时的id值保持一致;</div>
        </td>
      </tr>
      <tr height="34px">
        <td rowspan="1" colSpan="1">
          <div data-type="p">&quot;port&quot;</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p"><span data-type="color" style="color:#F5222D">功能：</span>端口值; </div>
          <div data-type="p"><span data-type="color" style="color:#F5222D">类型：</span>number </div>
          <div data-type="p"><span data-type="color" style="color:#F5222D">值：</span>和板级资源描述保持一致； </div>
          <div data-type="p"></div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">该port值和HAL层API对应的port保持一致；</div>
        </td>
      </tr>
      <tr height="34px">
        <td rowspan="1" colSpan="1">
          <div data-type="p">&quot;data_width&quot;</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p"><span data-type="color" style="color:#F5222D">功能：</span>串口数据宽度值; </div>
          <div data-type="p"><span data-type="color" style="color:#F5222D">类型：</span>number </div>
          <div data-type="p"><span data-type="color" style="color:#F5222D">值：</span>0=5bit;1=6bit;2=7bit;3=8bit; 4=9bit;</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p"></div>
        </td>
      </tr>
      <tr height="34px">
        <td rowspan="1" colSpan="1">
          <div data-type="p">&quot;baud_rate&quot;</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p"><span data-type="color" style="color:#F5222D">功能：</span>串口波特率; </div>
          <div data-type="p"><span data-type="color" style="color:#F5222D">类型：</span>number </div>
          <div data-type="p"><span data-type="color" style="color:#F5222D">值：</span>用户自定义;
          </div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">波特率值一般是常见的波特率值，对于特殊的波特率值，在设定前要确定芯片是否支持;</div>
        </td>
      </tr>
      <tr height="34px">
        <td rowspan="1" colSpan="1">
          <div data-type="p">&quot;stop_bits&quot;</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p"><span data-type="color" style="color:#F5222D">功能：</span>串口停止位; </div>
          <div data-type="p"><span data-type="color" style="color:#F5222D">类型：</span>number </div>
          <div data-type="p"><span data-type="color" style="color:#F5222D">值：</span>0=1bit停止位; 1=2bit停止位；</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">一般是1bit停止位</div>
        </td>
      </tr>
      <tr height="34px">
        <td rowspan="1" colSpan="1">
          <div data-type="p">&quot;flow_control&quot;</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p"><span data-type="color" style="color:#F5222D">功能：</span>串口流控制; </div>
          <div data-type="p"><span data-type="color" style="color:#F5222D">类型：</span>number </div>
          <div data-type="p"><span data-type="color" style="color:#F5222D">值：</span>0=禁止流控; 1=使能cts流控；2=使能rts流控;3=使能cts和rts流控;</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p"></div>
        </td>
      </tr>
      <tr height="34px">
        <td rowspan="1" colSpan="1">
          <div data-type="p">&quot;parity_config&quot;</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p"><span data-type="color" style="color:#F5222D">功能：</span>串口奇偶校验 </div>
          <div data-type="p"><span data-type="color" style="color:#F5222D">类型：</span>number </div>
          <div data-type="p"><span data-type="color" style="color:#F5222D"> 值：</span>0=无校验;1=奇校验;2=偶校验;</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p"></div>
        </td>
      </tr>
    </tbody>
  </table>
</div>


## #板级配置示范
```json
{
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


## #DeveloperKit之UART
### #硬件
1)developerkit开发板；
2)FT232串口模块;



![1.jpg | left | 505x383.8](https://cdn.yuque.com/lark/0/2018/jpeg/66207/1524747150116-d0b75f5e-e4ca-4538-be81-6acfdf6102b4.jpeg "")



![123.jpg | left | 505x375](https://cdn.yuque.com/lark/0/2018/jpeg/66207/1524486906526-f2dc0870-69f7-4853-a2d6-16c486e20961.jpeg "")



### #接线
TX引脚 连接 FT232 的RX引脚；
RX引脚 连接 FT232 的TX引脚；
VCC引脚  连接 FT232 的VCC引脚;
GND引脚  连接 FT232 的GND引脚;

### #配置
```json
/*apps/js/board_config.json*/
{
	"UART":[
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


### #代码
```javascript
var handle=UART.open('uart2');
if(handle>0){
var value = UART.write(handle,'hello\r\n');
UART.on(handle,function(data){ 
    console.log(data);    
});
} else {
	console.log('open uart2 fail!');
}
```


### #现象
FT232连接PC端的串口软件，可以进行数据的接收和发送。


## #ESP32之UART

### #硬件
1)ESP32开发板；
2)FT232串口模块;

### #接线
EPS32 IO32引脚 连接 FT232 的RX引脚；
EPS32 IO33引脚 连接 FT232 的TX引脚；
ESP32 3.3v引脚  连接 FT232 的VCC引脚;
ESP32 GND引脚  连接 FT232 的GND引脚;


### #配置
```json
/*apps/js/board_config.json*/
{
	"UART":[
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

### #代码
```javascript
/*js*/
print('start uart test.....................');
setInterval(function() {

    var handle=UART.open('uart2'); 
    if(handle>0){ 
        var value = UART.write(handle,'hello\r\n'); 
        UART.close(handle);
    } else {
        console.log('open uart2 fail!');
    }

}, 2000);
print('end uart test........................');
```


### #现象
FT232连接PC端的串口软件，可以进行数据的发送。
 