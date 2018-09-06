#I2C

<div class="bi-table">
  <table>
    <colgroup>
      <col width="90px" />
      <col width="90px" />
    </colgroup>
    <tbody>
      <tr>
        <td rowspan="1" colSpan="1">
          <div data-type="p"></div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p"><span data-type="color" style="color:rgb(245, 34, 45)">功能：</span>打开i2c端口</div>
          <div data-type="p"><span data-type="color" style="color:rgb(245, 34, 45)">参数：</span> id:和板级配置文件中的id保持一致</div>
          <div data-type="p"><span data-type="color" style="color:rgb(245, 34, 45)">返回值：</span>ok=资源handle，fail=-1;</div>
        </td>
      </tr>
      <tr>
        <td rowspan="1" colSpan="1">
          <div data-type="p">I2C.close(handle)</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p"><span data-type="color" style="color:rgb(245, 34, 45)">功能：</span>关闭i2c端口</div>
          <div data-type="p"><span data-type="color" style="color:rgb(245, 34, 45)">参数：</span> handle:资源handle，为I2C.open的返回值;</div>
          <div data-type="p"><span data-type="color" style="color:rgb(245, 34, 45)">返回值：</span>0=ok other=fail</div>
          <div data-type="p"></div>
        </td>
      </tr>
      <tr>
        <td rowspan="1" colSpan="1">
          <div data-type="p">I2C.write(handle,buff)</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p"><span data-type="color" style="color:rgb(245, 34, 45)">功能：</span>向某I2C设备写入数据</div>
          <div data-type="p"><span data-type="color" style="color:rgb(245, 34, 45)">参数：</span> handle:资源handle，为I2C.open的返回值;  </div>
          <div data-type="p">buff：需要写入数据，类型是array;</div>
          <div data-type="p"><span data-type="color" style="color:rgb(245, 34, 45)">返回值：</span>0=ok other=fail</div>
          <div data-type="p"></div>
        </td>
      </tr>
      <tr height="34px">
        <td rowspan="1" colSpan="1">
          <div data-type="p">I2C.read(handle,size)</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p"><span data-type="color" style="color:#F5222D">功能：</span>读取寄存器值； </div>
          <div data-type="p"><span data-type="color" style="color:#F5222D">参数：</span> handle:资源handle，为I2C.open的返回值; </div>
          <div data-type="p"> size：需要读取的数据长度; </div>
          <div data-type="p"><span data-type="color" style="color:#F5222D">返回值：</span><span data-type="color" style="color:#262626">read返回值，类型是array;</span></div>
          <div data-type="p">
          </div>
        </td>
      </tr>
    </tbody>
  </table>
</div>


## #板级配置参数


<div class="bi-table">  <table>    <colgroup>      <col width="90px" />      <col width="90px" />      <col width="90px" />    </colgroup>    <tbody>      <tr height="34px">        <td rowspan="1" colSpan="1">          <div data-type="p"><span data-type="color" style="color:rgb(38, 38, 38)"><span data-type="background" style="background-color:rgb(250, 250, 250)">参数名</span></span>          </div>        </td>        <td rowspan="1" colSpan="1">          <div data-type="p">类型/功能/值</div>        </td>        <td rowspan="1" colSpan="1">          <div data-type="p">说明</div>        </td>      </tr>      <tr height="34px">        <td rowspan="1" colSpan="1">          <div data-type="p">&quot;id&quot;</div>        </td>        <td rowspan="1" colSpan="1">          <div data-type="p"><span data-type="color" style="color:#F5222D">功能：</span>资源唯一性标志 </div>          <div data-type="p">            <span data-type="color" style="color:#F5222D">类型：</span>string </div>          <div data-type="p">            <span data-type="color" style="color:#F5222D">值：</span>任意，保持数组内id值唯一;</div>        </td>        <td rowspan="1" colSpan="1">          <div data-type="p">该id值和js层I2C.open时的id值保持一致;</div>        </td>      </tr>      <tr height="34px">        <td rowspan="1" colSpan="1">          <div data-type="p">&quot;port&quot;</div>        </td>        <td rowspan="1" colSpan="1">          <div data-type="p"><span data-type="color" style="color:rgb(245, 34, 45)">功能：</span>端口值; </div>          <div data-type="p"><span data-type="color" style="color:rgb(245, 34, 45)">类型：</span>number </div>          <div data-type="p"><span data-type="color" style="color:rgb(245, 34, 45)">值：</span>和板级资源描述保持一致；</div>        </td>        <td rowspan="1" colSpan="1">          <div data-type="p">该port值和HAL层API对应的port保持一致；</div>        </td>      </tr>      <tr height="34px">        <td rowspan="1" colSpan="1">          <div data-type="p">&quot;address_width&quot;</div>        </td>        <td rowspan="1" colSpan="1">          <div data-type="p"><span data-type="color" style="color:#F5222D">功能：</span>设备I2C地址宽度; </div>          <div data-type="p"><span data-type="color" style="color:#F5222D">类型：</span>number </div>          <div data-type="p"><span data-type="color" style="color:#F5222D">值：</span>用户自定义；          </div>        </td>        <td rowspan="1" colSpan="1">          <div data-type="p">该值一般是7;</div>        </td>      </tr>      <tr height="34px">        <td rowspan="1" colSpan="1">          <div data-type="p">&quot;freq&quot;</div>        </td>        <td rowspan="1" colSpan="1">          <div data-type="p"><span data-type="color" style="color:#F5222D">功能：</span>I2C总线速率; </div>          <div data-type="p"><span data-type="color" style="color:#F5222D">类型：</span>number </div>          <div data-type="p"><span data-type="color" style="color:#F5222D">值：</span>用户自定义；</div>        </td>        <td rowspan="1" colSpan="1">          <div data-type="p">该值一般在100K-400K范围；</div>        </td>      </tr>      <tr height="34px">        <td rowspan="1" colSpan="1">          <div data-type="p">&quot;mode&quot;</div>        </td>        <td rowspan="1" colSpan="1">          <div data-type="p"><span data-type="color" style="color:#F5222D">功能：</span>声明I2C的主从模式; </div>          <div data-type="p"><span data-type="color" style="color:#F5222D">类型：</span>number </div>          <div data-type="p"><span data-type="color" style="color:#F5222D">值：</span>1=master,2=slave；</div>        </td>        <td rowspan="1" colSpan="1">          <div data-type="p"></div>        </td>      </tr>      <tr height="34px">        <td rowspan="1" colSpan="1">          <div data-type="p">&quot;dev_addr&quot;</div>        </td>        <td rowspan="1" colSpan="1">          <div data-type="p"><span data-type="color" style="color:#F5222D">功能：</span>声明I2C的外设的I2C地址; </div>          <div data-type="p"><span data-type="color" style="color:#F5222D">类型：</span>number </div>          <div data-type="p"><span data-type="color" style="color:#F5222D">值：</span>根据芯片实际情况定义；</div>        </td>        <td rowspan="1" colSpan="1">          <div data-type="p"></div>        </td>      </tr>    </tbody>  </table></div>

## #板级配置示范
```json
{
"I2C":[{
        "id":"shtc1",
        "port":2,
        "address_width":7,
        "freq":100000,
        "mode":1,
        "dev_addr":224
}],

}

```


## #DeveloperKit之SHTC1

### #硬件


![1.jpg | left | 300x375](https://cdn.yuque.com/lark/0/2018/jpeg/66207/1524681161552-e6167e27-fdec-46b8-a849-cb5f78b9fbdd.jpeg "")


### #接线
无

### #配置
```javascript
/*apps/js/board_config.json*/
{
"I2C":[{
        "id":"shtc1",
        "port":2,
        "address_width":7,
        "freq":100000,
        "mode":1,
        "dev_addr":224
}],

}
```

### #代码
```javascript
print('start i2c test.....................');
setInterval(function() {

    var shtc1Handle = I2C.open('shtc1'); 
    var Reg=[0xEF,0xC8]; 
    I2C.write(shtc1Handle,Reg); 
    var retArray=I2C.read(shtc1Handle,2); 
    console.log('ID:'+retArray[1]); 
    Reg[0]=0x80; 
    Reg[1]=0x5D; 
    I2C.write(shtc1Handle,Reg); 
    Reg[0]=0x7C; 
    Reg[1]=0xA2; 
    I2C.write(shtc1Handle,Reg); 
    var TempHumm = I2C.read(shtc1Handle,6); 
    console.log('read len:'+TempHumm.length); 
    var i =0; 
    for(i=0;i<TempHumm.length;i++){ 
    	console.log('rawData['+i+']:'+TempHumm[i]); 
    }
    I2C.close(shtc1Handle); 
}, 2000);
print('end i2c test........................');
```


### #现象
每隔2s，程序会去读取SHTC1的温度和湿度的裸数据，用手触摸SHTC1传感器，其裸值将会改变。



![2.jpg | left | 404x159](https://cdn.yuque.com/lark/0/2018/jpeg/66207/1524681488580-67a22170-4079-46cf-88ea-92d0c79ff6b4.jpeg "")

 