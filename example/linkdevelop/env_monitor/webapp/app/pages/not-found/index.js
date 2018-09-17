import React, { Component } from "react";
// 以CSS Modules方式引入404页样式
import style from "./index.css";

// 导出404页组件
export default class NotFound extends Component {
    render(){
        return <div className="container">
            <p className={ style.notFound }>404</p>
        </div>
    }
}