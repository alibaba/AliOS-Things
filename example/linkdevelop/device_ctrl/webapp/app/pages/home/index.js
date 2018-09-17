import React, { Component } from "react";
// 以CSS Modules方式引入Home页样式
import style from "./index.css";

import { Switch } from '@bone/bone-web-ui';

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
            checked: false
        };
    }

    onChange = (checked) => {
        APIGateway.request(`${config.domain}/switchCtrl`, {
            version: "1.0.2",
            data: {
                // 接口参数
            "switch": checked == false ? 1: 0
            }
        }).then(response => {
            console.log(response);
            if(response.code == 200)
            {
                console.log("set switch ok");
                this.setState({
                    checked,
                });
            }
            
        }).catch(error => {
            console.log(error);
        });
    }

    componentDidMount() {
        APIGateway.request(`https://api.link.aliyun.com/thing/device/properties/set`, {
            version: "1.1.2",
            data: {
                "productKey": "a1y3ZVasdt4",
                "deviceName": "simple",
                "properties": {
                    "cmd_led": 1
                }
            }
        }).then(response => {
            console.log(response);
            
            
        }).catch(error => {
            console.log(error);
        });
    };
    
    render(){
        var oMeta = document.createElement('meta');
        oMeta.name = "viewport";
        oMeta.content = 'width=device-width, initial-scale=1, maximum-scale=1, minimum-scale=1, user-scalable=no';
        document.getElementsByTagName('head')[0].appendChild(oMeta);
        return (
            <div>
                <span style={{fontSize: "20px"}}>设置开关</span>
                <Switch checked={this.state.checked} onChange={this.onChange} />
            </div>
        )
            
    }
}