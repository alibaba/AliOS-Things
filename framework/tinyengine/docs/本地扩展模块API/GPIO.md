## #GPIO API


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
          <div data-type="p">GPIO.open(id)</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p"><span data-type="color" style="color:rgb(245, 34, 45)">功能：</span>打开gpio</div>
          <div data-type="p"><span data-type="color" style="color:rgb(245, 34, 45)">参数：</span> id:和板级配置文件中的id保持一致</div>
          <div data-type="p"><span data-type="color" style="color:rgb(245, 34, 45)">返回值：</span>返回资源handle</div>
        </td>
      </tr>
      <tr>
        <td rowspan="1" colSpan="1">
          <div data-type="p">GPIO.close(handle)</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p"><span data-type="color" style="color:rgb(245, 34, 45)">功能：</span>关闭gpio</div>
          <div data-type="p"><span data-type="color" style="color:rgb(245, 34, 45)">参数：</span> handle:资源handle，为GPIO.open的返回值;</div>
          <div data-type="p"><span data-type="color" style="color:rgb(245, 34, 45)">返回值：</span>0=ok other=fail</div>
          <div data-type="p"></div>
        </td>
      </tr>
      <tr>
        <td rowspan="1" colSpan="1">
          <div data-type="p">GPIO.write(handle,level)</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p"><span data-type="color" style="color:rgb(245, 34, 45)">功能：</span>设置gpio输出电压</div>
          <div data-type="p"><span data-type="color" style="color:rgb(245, 34, 45)">参数：</span> handle:资源handle，为GPIO.open的返回值;  </div>
          <div data-type="p">          level：电平值，0或1 </div>
          <div data-type="p"><span data-type="color" style="color:rgb(245, 34, 45)">返回值：</span>0=ok other=fail</div>
          <div data-type="p"></div>
        </td>
      </tr>
      <tr height="34px">
        <td rowspan="1" colSpan="1">
          <div data-type="p">GPIO.read(handle)</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p"><span data-type="color" style="color:#F5222D">功能：</span>获取gpio输入电压 </div>
          <div data-type="p"><span data-type="color" style="color:#F5222D">参数：</span> handle:资源handle，为GPIO.open的返回值; </div>
          <div data-type="p">  <span data-type="color" style="color:#F5222D">返回值：</span>引脚电平值
          </div>
        </td>
      </tr>
      <tr height="34px">
        <td rowspan="1" colSpan="1">
          <div data-type="p">GPIO.on(handle,edge,cb)</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p"><span data-type="color" style="color:#F5222D">功能：</span>gpio中断监听 </div>
          <div data-type="p"><span data-type="color" style="color:#F5222D">参数：</span> handle:资源handle，为GPIO.open的返回值; </div>
          <div data-type="p">edge:中断触发类型，&quot;rising&quot;上升沿触发,&quot;falling&quot;下降沿触发,&quot;both&quot;双边沿触发;  </div>
          <div data-type="p">cb：服务function; </div>
          <div data-type="p"><span data-type="color" style="color:#F5222D">返回值：</span>0=ok other=fail</div>
        </td>
      </tr>
      <tr height="34px">
        <td rowspan="1" colSpan="1">
          <div data-type="p"></div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p"></div>
        </td>
      </tr>
    </tbody>
  </table>
</div>


## #板级配置参数


<div class="bi-table">  <table>    <colgroup>      <col width="90px" />      <col width="90px" />      <col width="90px" />    </colgroup>    <tbody>      <tr height="34px">        <td rowspan="1" colSpan="1">          <div data-type="p">参数名</div>        </td>        <td rowspan="1" colSpan="1">          <div data-type="p">类型/功能/值</div>        </td>        <td rowspan="1" colSpan="1">          <div data-type="p">说明</div>        </td>      </tr>      <tr height="34px">        <td rowspan="1" colSpan="1">          <div data-type="p">&quot;id&quot;</div>        </td>        <td rowspan="1" colSpan="1">          <div data-type="p"><span data-type="color" style="color:#F5222D">功能：</span>资源唯一性标志 </div>          <div data-type="p">            <span data-type="color" style="color:#F5222D">类型：</span>string </div>          <div data-type="p">            <span data-type="color" style="color:#F5222D">值：</span>任意，保持数组内id值唯一;</div>        </td>        <td rowspan="1" colSpan="1">          <div data-type="p">该id值和js层GPIO.open时的id值保持一致;</div>        </td>      </tr>      <tr height="34px">        <td rowspan="1" colSpan="1">          <div data-type="p">&quot;port&quot;</div>        </td>        <td rowspan="1" colSpan="1">          <div data-type="p"><span data-type="color" style="color:#F5222D">功能：</span>端口值; </div>          <div data-type="p">            <span data-type="color" style="color:#F5222D">类型：</span>number </div>          <div data-type="p">            <span data-type="color" style="color:#F5222D">值：</span>和板级资源描述保持一致；</div>        </td>        <td rowspan="1" colSpan="1">          <div data-type="p">该port值和HAL层API对应的port保持一致；</div>        </td>      </tr>      <tr height="34px">        <td rowspan="1" colSpan="1">          <div data-type="p">&quot;dir&quot;</div>        </td>        <td rowspan="1" colSpan="1">          <div data-type="p"><span data-type="color" style="color:#F5222D">功能：</span>配置gpio的输入或输出; </div>          <div data-type="p">            <span data-type="color" style="color:#F5222D">类型：</span>number </div>          <div data-type="p">            <span data-type="color" style="color:#F5222D">值：</span>0=&quot;output&quot; 1=&quot;input&quot; 2=&quot;irq&quot;  3=&quot;analog&quot;;</div>        </td>        <td rowspan="1" colSpan="1">          <div data-type="p">配置gpio中断触发功能，有些场景使用&quot;imput&quot;，有些场景使用&quot;irq&quot;,需要根据实际的场景设定。</div>        </td>      </tr>      <tr height="34px">        <td rowspan="1" colSpan="1">          <div data-type="p">&quot;pull&quot;</div>        </td>        <td rowspan="1" colSpan="1">          <div data-type="p"><span data-type="color" style="color:#F5222D">功能：</span>配置gpio的上拉下拉开漏; </div>          <div data-type="p">            <span data-type="color" style="color:#F5222D">类型：</span>number </div>          <div data-type="p">            <span data-type="color" style="color:#F5222D">值：</span>0=&quot;pulldown&quot; 1=&quot;pullup&quot; 2=&quot;open&quot;;</div>        </td>        <td rowspan="1" colSpan="1">          <div data-type="p"></div>        </td>      </tr>    </tbody>  </table></div>



## #板级配置示范
```json
{
	"GPIO":[
		{
		"id":"beep",
		"port":22,
		"dir":0,
		"pull":1
		},
		{
		"id":"key",
		"port":23,
		"dir":2,
		"pull":1
		}
	]
}
```


## #DeveloperKit之LED
### #硬件




![1.jpg | left | 410x458.96613190730835](https://cdn.yuque.com/lark/0/2018/jpeg/66207/1524643768587-64f5b217-995d-4587-a452-227e741942db.jpeg "")


### #接线
无

### #配置
```json
/*apps/js/board_config.json*/
{
	"GPIO":[
		{
		"id":"gled",
		"port":60,
		"dir":0,
		"pull":1
		}
	]
}
```

### #代码
```javascript
print('start gled test.....................');
var gled_level = 0;
var gled_handle = GPIO.open('gled'); 
setInterval(function() {
    GPIO.write(gled_level,1-gled_level);
    gled_level = 1-gled_level; 
}, 5000);
print('end gled test........................');
```

### #现象
gled 间隔5s进行闪烁。


## #ESP32之按键

### #硬件
1)ESP32开发板；
2)按键模块;


![key.png | left | 458x332](https://cdn.yuque.com/lark/0/2018/png/66207/1525854037946-b5ae98ef-5661-434b-af3d-05045c1cacb2.png "")

### #接线
EPS32 IO23引脚 连接 按键模块的OUT引脚；
EPS32 GND引脚 连接 按键模块的GND引脚；
EPS32 3.3V引脚 连接 按键模块的VCC引脚；

### #配置
```json
/*apps/js/board_config.json*/
{
	"GPIO":[
		{
		"id":"key",
		"port":23,
		"dir":2,
		"pull":1
		}
	]
}
```

### #代码
```javascript
print('start key test.....................');

var key_handle = GPIO.open('key');
GPIO.on(key_handle,'both',function(value){
	print('key_value:'+value);
});	
print('end key test........................');
```


### #现象
按下按键，终端会打印对应的状态

#### #硬件
1)ESP32开发板；
2)DHT11模块;


![dht112.png | left | 363x353](https://cdn.yuque.com/lark/0/2018/png/66207/1525845763715-08d48b86-2864-48ec-81d1-92df70e9a1ef.png "")



#### #接线

EPS32 IO19引脚 连接 DHT11模块的OUT引脚；
EPS32 GND引脚 连接 DHT11模块的GND引脚；
EPS32 3.3V引脚 连接 按键模块的VCC引脚；


#### #配置
```json
/*apps/js/board_config.json*/
{
        "GPIO":[
            {
                "id":"dht11",
                "port":19,
                "dir":3,
                "pull":1
            }
        ]
}
```

#### #代码
```javascript
/*samples/js/dht11.js*/
var dht11_handle = GPIO.open('dht11');
setInterval(function() {
		var dht11_buff = HW.getDht11(dht11_handle);
		console.log('dht11->hum:'+dht11_buff[0]+ '  temp:'+dht11_buff[1]);
}, 5000);
```

#### #现象
系统每间隔5s打印温湿度值。



![test.jpg | left | 434x245](https://cdn.yuque.com/lark/0/2018/jpeg/66207/1525846031088-9f8e7155-1e7d-4561-91a2-16e660fb552f.jpeg "")


## #TODO


 