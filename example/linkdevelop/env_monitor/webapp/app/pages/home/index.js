import React, { Component } from "react";
// 以CSS Modules方式引入Home页样式
import style from "./index.css";

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
            sensor: {},
            devices: []
        };
    }

    getSensor = () => {
        APIGateway.request(`${config.domain}/getSensor`, {
            version: "1.0.0",
            data: {
                // 接口参数
            }
        }).then(response => {
            console.log(response);
            this.setState({
                sensor: response.data.value
            })
        }).catch(error => {
            console.log(error);
        });
    }

    componentDidMount() {
        setInterval(this.getSensor, 1000);

        APIGateway.request(`${config.domain}/getDevices`, {
            version: "1.0.0",
            data: {
                // 接口参数
            }
        }).then(response => {
            console.log(response);
            this.setState({
                devices: response.data.items
            });
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
                <div>
                    <span style={{fontSize: "20px", marginRight: "10px"}}>设备列表</span>
                    <br />
                {
                    this.state.devices.map((e, idx) => (
                        <span key={idx} style={{fontSize: "20px", marginRight: "10px"}}>{e.name}</span>
                        ))
                }</div>
                <div>
                    <span>温度：{this.state.sensor.Temp}</span>
                    <br />
                    <span>湿度：{this.state.sensor.Humi}</span>
                </div>
                
            </div>
        )
            
    }
}