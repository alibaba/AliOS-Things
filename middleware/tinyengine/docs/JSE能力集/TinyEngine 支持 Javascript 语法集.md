# 语法列表

#### __目前Lite JSE 支持的JS语法列表，后续根据需要进行扩展：__

<div class="bi-table">
  <table>
    <colgroup>
      <col width="83px" />
      <col width="281px" />
      <col width="228px" />
      <col width="297px" />
    </colgroup>
    <tbody>
      <tr>
        <td rowspan="1" colSpan="1">
          <div data-type="p">　</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">支持</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">不支持</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">备注</div>
        </td>
      </tr>
      <tr>
        <td rowspan="1" colSpan="1">
          <div data-type="p">词法</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">空白字符</div>
          <div data-type="p">行终结符</div>
          <div data-type="p">注释</div>
          <div data-type="p">Tokens</div>
          <div data-type="p">标识符名和标识符</div>
          <div data-type="p">保留字</div>
          <div data-type="p">字面量</div>
          <div data-type="p">自动分号插入</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">Unicode 格式控制字符</div>
          <div data-type="p">正则表达式字面量</div>
          <div data-type="p">
          </div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">声明变量/常量时，变量名/值表达式命名最长为63个字符</div>
          <div data-type="p">定义函数原型时，参数名称不要超过8个字符</div>
        </td>
      </tr>
      <tr>
        <td rowspan="1" colSpan="1">
          <div data-type="p">类型</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">undefined</div>
          <div data-type="p">null</div>
          <div data-type="p">true、false</div>
          <div data-type="p">string</div>
          <div data-type="p">Number 类型(整数、浮点数、指数）</div>
          <div data-type="p">Object类型</div>
          <div data-type="p">列表规范类型</div>
          <div data-type="p">完结规范类型</div>
          <div data-type="p">属性描述符及属性标识符规范类型</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">Infinity</div>
          <div data-type="p">NaN</div>
          <div data-type="p">Object 内部属性</div>
          <div data-type="p">引用规范类型</div>
          <div data-type="p">词法环境和环境记录项规范类型</div>
          <div data-type="p">对象内部方法的算法</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">支持二进制/八进制/十六进制</div>
          <div data-type="p">
          </div>
        </td>
      </tr>
      <tr>
        <td rowspan="1" colSpan="1">
          <div data-type="p">类型转换</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">SameValue 算法</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">ToPrimitive</div>
          <div data-type="p">ToBoolean
          </div>
          <div data-type="p">ToNumber</div>
          <div data-type="p">ToInteger</div>
          <div data-type="p">ToInt32</div>
          <div data-type="p">ToUint32</div>
          <div data-type="p">ToUint16</div>
          <div data-type="p">ToString</div>
          <div data-type="p">ToObject</div>
          <div data-type="p">CheckObjectCoercible</div>
          <div data-type="p">IsCallable</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">可根据需要扩展支持</div>
        </td>
      </tr>
      <tr>
        <td rowspan="1" colSpan="1">
          <div data-type="p">表达式</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">主值表达式</div>
          <div data-type="p">左值表达式</div>
          <div data-type="p">后缀表达式（++，--）</div>
          <div data-type="p">一元运算符 (部份支持)</div>
          <div data-type="p">乘法运算符</div>
          <div data-type="p">加法运算符</div>
          <div data-type="p">位运算移位运算符</div>
          <div data-type="p">比较运算符</div>
          <div data-type="p"> in 运算符</div>
          <div data-type="p">等值运算符</div>
          <div data-type="p">严格等于运算符 ( === ) 严格不等于运算符 ( !== )</div>
          <div data-type="p">二元逻辑运算符</div>
          <div data-type="p"> 条件运算符</div>
          <div data-type="p">赋值运算符</div>
          <div data-type="p">逗号运算符</div>
          <div data-type="p"></div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">delete 运算符</div>
          <div data-type="p">void 运算符</div>
          <div data-type="p">typeof 运算符</div>
          <div data-type="p">instanceof 运算符</div>
          <div data-type="p">二进制位运算符</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p"></div>
        </td>
      </tr>
      <tr>
        <td rowspan="1" colSpan="1">
          <div data-type="p">语句</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">块</div>
          <div data-type="p">变量语句</div>
          <div data-type="p">空语句</div>
          <div data-type="p">表达式语句</div>
          <div data-type="p">if 语句</div>
          <div data-type="p">迭代语句</div>
          <div data-type="p">for-in 语句 continue 语句 break 语句 with 语句 switch 语句</div>
          <div data-type="p">return 语句</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">do-while 语句</div>
          <div data-type="p">标签语句</div>
          <div data-type="p">throw 语句</div>
          <div data-type="p">try 语句</div>
          <div data-type="p">debugger 语句</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">default必须在最后一个case之后且default不能使用break结束字</div>
        </td>
      </tr>
      <tr>
        <td rowspan="1" colSpan="1">
          <div data-type="p">全局对象</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">undefined</div>
          <div data-type="p">全局对象的函数属性（部份支持）</div>
          <div data-type="p">eval (x)</div>
          <div data-type="p">全局对象的构造器属性 （部份支持)</div>
          <div data-type="p"></div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">NaN</div>
          <div data-type="p">Infinity</div>
          <div data-type="p"> parseInt (string , radix)</div>
          <div data-type="p">parseFloat (string)</div>
          <div data-type="p"> isNaN (number)</div>
          <div data-type="p"> isFinite (number)</div>
          <div data-type="p">处理 URI 的函数属性</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">　</div>
        </td>
      </tr>
      <tr>
        <td rowspan="1" colSpan="1">
          <div data-type="p">Object 对象</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">作为函数调用 Object 构造器</div>
          <div data-type="p">Object 构造器</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">Object 构造器的属性</div>
          <div data-type="p">Object 的 prototype 对象的属性</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">　</div>
        </td>
      </tr>
      <tr>
        <td rowspan="1" colSpan="1">
          <div data-type="p">Function 对象</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">Function 对象</div>
          <div data-type="p">Function 构造器</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">Function 构造器</div>
          <div data-type="p">Function 的 prototype 对象的属性</div>
          <div data-type="p">Function 的实例的属性</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">　</div>
        </td>
      </tr>
      <tr>
        <td rowspan="1" colSpan="1">
          <div data-type="p">Array 对象</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">作为函数调用 Array 构造器</div>
          <div data-type="p">Array 构造器</div>
          <div data-type="p">数组原型对象的属性 (部份支持）</div>
          <div data-type="p">Array.prototype.contains(element)</div>
          <div data-type="p">Array.prototype.indexOf(searchElement)</div>
          <div data-type="p">Array.prototype.push(element)</div>
          <div data-type="p">Array.prototype.pop()</div>
          <div data-type="p">Array.length</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">Array 构造器的属性</div>
          <div data-type="p"> [[GetOwnProperty]] ( P )</div>
          <div data-type="p">
          </div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">可根据需求扩充</div>
        </td>
      </tr>
      <tr>
        <td rowspan="1" colSpan="1">
          <div data-type="p">String 对象</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">作为函数调用 String 构造器</div>
          <div data-type="p">String 构造器</div>
          <div data-type="p">字符串原型对象的属性(部份支持)</div>
          <div data-type="p">String.prototype.charAt (pos)</div>
          <div data-type="p">String.prototype.substring (start, end)</div>
          <div data-type="p">String.prototype.substr(start[, length])</div>
          <div data-type="p">String.prototype.str.indexOf(searchValue)</div>
          <div data-type="p">String.length</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">String 构造器的属性</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">可根据需求扩充</div>
        </td>
      </tr>
      <tr>
        <td rowspan="1" colSpan="1">
          <div data-type="p">Math 对象</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">random ( )</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">　</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">可根据需求扩充</div>
        </td>
      </tr>
      <tr>
        <td rowspan="1" colSpan="1">
          <div data-type="p">Date 对象</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">　</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">Date 对象</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">　</div>
        </td>
      </tr>
      <tr>
        <td rowspan="1" colSpan="1">
          <div data-type="p">RegExp 对象</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">　</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">RegExp ( 正则表达式 ) 对象 </div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">　</div>
        </td>
      </tr>
      <tr>
        <td rowspan="1" colSpan="1">
          <div data-type="p">Error 对象</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">　</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">Error Objects 不支持</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">　</div>
        </td>
      </tr>
      <tr>
        <td rowspan="1" colSpan="1">
          <div data-type="p">JSON 对象</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">JSON 语法</div>
          <div data-type="p">parse (text)</div>
          <div data-type="p">stringify(value)</div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p"></div>
        </td>
        <td rowspan="1" colSpan="1">
          <div data-type="p">可根据需求扩充</div>
        </td>
      </tr>
    </tbody>
  </table>
</div>


# 参考例程

 