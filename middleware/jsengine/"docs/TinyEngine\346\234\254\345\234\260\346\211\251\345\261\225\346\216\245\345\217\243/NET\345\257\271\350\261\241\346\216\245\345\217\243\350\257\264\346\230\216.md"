# <a name="w9yskx"></a>NET 模块说明


<div class="bi-table">
  <table>
    <colgroup>
      <col width="90px" />
      <col width="380px" />
      <col width="380px" />
    </colgroup>
    <tbody>
      <tr>
        <td rowspan="1" colSpan="1">
          <div data-type="p">API</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">参数说明</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">说明</div>
        </td>
      </tr>
      <tr>
        <td rowspan="1" colSpan="1">
          <div data-type="p">NET.connect（options）</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">功能：创建socket connect连接</div>
          <div data-type="p"></div>
          <div data-type="p">参数：options 为连接的参数：如下：</div>
          <div data-type="p">{   addr: &#x27;tcp://127.0.0.1:1234&#x27;,   onconnect: function(conn) {},   ondata: function(conn, data){}, onclose: function(conn) {}, onerror: function(err) {}, }</div>
          <div data-type="p"></div>
          <div data-type="p">返回值： 0,成功，其他失败。</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">该接口接受options对象的参数，</div>
          <div data-type="p">addr：为protocol://ip:port,其中protocol可以为tcp/udp/raw。</div>
          <div data-type="p"></div>
          <div data-type="p">onconnect方法在connect成功后回调，</div>
          <div data-type="p">ondata方法在收到对端数据时回调，返回参数中有conn：socket的handle，和data数据。</div>
          <div data-type="p">onclose方法在对端关闭时回调。</div>
          <div data-type="p">onerror方法在连接出错时回调，其中参数err为出错原因，类型字符串可以为：</div>
          <div data-type="p">&quot;parameter error&quot;，&quot;connect error&quot;，&quot;recv error&quot;</div>
          <div data-type="p"></div>
        </td>
      </tr>
      <tr>
        <td rowspan="1" colSpan="1">
          <div data-type="p">NET.close(connection)</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">功能：关闭连接</div>
          <div data-type="p"></div>
          <div data-type="p">参数：连接</div>
          <div data-type="p"></div>
          <div data-type="p">返回值：0 成功 其他：失败</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p"></div>
        </td>
      </tr>
      <tr>
        <td rowspan="1" colSpan="1">
          <div data-type="p">NET.send(connection，data)</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">功能：向连接connection发送数据</div>
          <div data-type="p"></div>
          <div data-type="p">参数：data，待发送的数据</div>
          <div data-type="p"></div>
          <div data-type="p">返回值：0 成功 其他：失败</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p"></div>
        </td>
      </tr>
      <tr>
        <td rowspan="1" colSpan="1">
          <div data-type="p">NET.isip(string)</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">功能： 判断字符串string是否是一个合法的ip地址。</div>
          <div data-type="p"></div>
          <div data-type="p">参数：字符串string</div>
          <div data-type="p"></div>
          <div data-type="p">返回值 : 1 代表string是一个合法ip，0代表string不是合法ip。</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p"></div>
        </td>
      </tr>
      <tr>
        <td rowspan="1" colSpan="1">
          <div data-type="p"></div>
        </td>
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


__js示例程序：__

可参考gravity@lite sample/js/net.js

```
var ssid = "Xiaomi_296E_rock";
var passwd = "rockzhou";

WIFI.connect(ssid,passwd,function(state){console.log('wifi state:'+state);
var ip = WIFI.getip();
console.log('WIFI state getip ='+ip);

var mac = WIFI.getmac();
console.log('WIFI.getmac:'+mac);

var channel = WIFI.getchannel();
console.log('WIFI.getchannel:'+channel);

if (state == 'CONNECTED'){
	HTTP.request("http://www.baidu.com",function(result){
		console.log('http requst reulst=:'+result);
	});
	NET.connect({addr: 'tcp://192.168.8.144:80',  
    onconnect: function(conn) {console.log('onconnect :'+conn);
    	NET.send(conn,"i am wuzhun");
	},  
    ondata: function(conn, data){
    	console.log('ondata :'+conn +'data= '+data);
    }, 
    onclose: function(conn) {
    	console.log('onclose :'+conn);
    }, 
    onerror: function(conn) {
    	console.log('onerror :'+conn);
    }, 
	});
	}
});
```

__运行结果展示：__

```
BoneEngine > wifi state:CONNECTED 
BoneEngine > WIFI state getip =192.168.8.140 
BoneEngine > WIFI.getmac:0z\30:AE:A4:44:7A:5C 
BoneEngine > WIFI.getchannel:8 
BoneEngine > onconnect :1 

[003170]<I> module_handle_cb Enter: name=send
[003170]<I> NET.send(1,i am wuzhun)
task_net_send_fun sent: 12[003230]<W> Receiving data ...

BoneEngine > ondata :1data= tcp hello test  

[023090]<I> module_handle_cb Enter: name=close
[023090]<W> socket id[1] has been closed
BoneEngine > onclose :1 
```

__server端测试工具：__


![image | left](https://cdn.yuque.com/lark/0/2018/png/89215/1525440824861-eec804d9-718e-4e57-98f8-b78a5c177186.png "")


