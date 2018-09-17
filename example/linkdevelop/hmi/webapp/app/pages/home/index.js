import React, { Component } from "react";
// 以CSS Modules方式引入Home页样式
import style from "./index.css";

import { Input, Button, Notice } from '@bone/bone-web-ui';

import { APIGateway } from '@bone/linkdevelop-sdk';

const config = {
    domain: 'http://api.xiaxiaowen.top'
}

let xAxisData = [];
let yAxisTemp = [];
let yAxisHumi = [];
for(let i =0 ; i < 60; i++)
{
    xAxisData.push(i + 's');
}

// 导出Home页组件
export default class Home extends Component {
    constructor() {
        super();
        this.state = {
            downStream: '',
            upStream: '',
            notice: '',
            noticeDisplay: false
        };
    }

    getUpStream = () => {
        APIGateway.request(`${config.domain}/upStream`, {
            version: "1.0.2",
            data: {
                // 接口参数
            }
        }).then(response => {
            //console.log(response);
            if(response.code == 200)
            {
                this.setState({
                    upStream: response.data.value
                })
            }
        }).catch(error => {
            console.log(error);
        });
    }

    onInputMsg = (msg) => {
        //console.log(msg);
        this.setState({
            downStream: msg
        });
    }
    onSendMsg = () => {
        APIGateway.request(`${config.domain}/downStream`, {
            version: "1.0.2",
            data: {
                // 接口参数
            "down": this.state.downStream
            }
        }).then(response => {
            console.log(response);
            if(response.code == 200)
            {
                console.log("set message ok");
                this.setState({
                    notice: "设置成功",
                    noticeDisplay: true
                });
                setTimeout(() => {
                    this.setState({
                        noticeDisplay: false
                    });
                }, 2000);
            }
            else{
                console.log("set message error");
                this.setState({
                    notice: "设置失败" + response,
                    noticeDisplay: true
                });
                setTimeout(() => {
                    this.setState({
                        noticeDisplay: false
                    });
                }, 2000);
            }
            
        }).catch(error => {
            console.log(error);
        });
    }

    componentDidMount() {
        setInterval(this.getUpStream, 1000);
    };
    
    render(){
        return (
            <div>
                <Input addonBefore="设备端" size="medium" value={this.state.upStream} />
                <br />
                <Input addonBefore="发送数据" size="medium" onChange={this.onInputMsg} />
                <Button type="primary" onClick={this.onSendMsg}>发送</Button>
                {
                    this.state.noticeDisplay ? 
                            <Notice >
                                {this.state.notice}
                            </Notice>
                        : <div></div>
                }
            </div>
        )
            
    }
}