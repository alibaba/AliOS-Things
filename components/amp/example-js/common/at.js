var at = require('at');

function atTestCallback() {
    var s = at.atReadLine(-1); /* 从AT串口读一行字符串，参数为读字符串超时时间（单位毫秒，-1表示一直等待） */
    at.atSendNoReply("AT+TEST value is " + s.toString() + "\n", 1);  /* 发送响应命令，参数分别为: 响应字符串，是否发送命令间隔符 */
}

function atWlogpCallback() {
    var s = at.atRead(1, -1);  /* 从AT串口读字符串，第一个参数为读取字符串的长度，
                                  第二个参数为读字符串超时时间（单位毫秒，-1表示一直等待） */

    if (s.toString() == '?') {
        at.atSendNoReply('log print is open \n', 1);
    } else if (s.toString() == '=') {
        var log = at.atRead(1, -1);
        if (log.toString() == '1') {
            at.atSendNoReply('open log print \n', 1);
        } else if (log.toString() == '0') {
            at.atSendNoReply('close log print \n', 1);
        } else {
            at.atSendNoReply('AT Command error\n', 1);
        }
    } else {
        at.atSendNoReply('AT Command error\n', 1);
    }
}

at.atInit(3, 115200, '\r\n') /* 初始化at功能，参数分别为: uart端口号、波特率、命令间隔字符串 */
at.atSetEcho(1) /* 设置是否显示回显：0表示关闭回显，1表示打开回显 */
at.atRegisterCmdCallback('AT+TEST=', atTestCallback)  /* 注册AT命令，参数分别为: AT命令字符串，回调函数 */
at.atRegisterCmdCallback('AT*WLOGP', atWlogpCallback)

/* AT命令运行期间，REPL需要设置为disable，在app.json中增加: "repl": "disable" */
