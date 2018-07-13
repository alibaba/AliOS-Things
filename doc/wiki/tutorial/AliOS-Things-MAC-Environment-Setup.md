
感谢 @dupig  
   
也可以尝试爱好者提供的**一键安装脚本**：[Setup Script for Mac](http://p28phe5s5.bkt.clouddn.com/setup_osx.zip)。下载解压后在Terminal里运行以下命令即可完成环境安装配置。
```bash
cd ~/Downloads/setup_osx && bash setup_osx.sh
```
*Note: 若下载解压目录不是~/Downloads/setup_osx，请注意修改上面命令*
  

作为一个搞交互设计的小白，对AliOS-Things的应用前景感到很兴奋，很想利用它做一些场景性的探索。于是花了一点时间和怡然讨论整理出了AliOS-Things在MAC上的配置，供大家参考。MAC跟Windows的步骤其实差不多，主要是一些安装包的配置上会有些区别。
1. 首先下载visual studio code，然后安装
![pasted graphic](https://user-images.githubusercontent.com/8230103/34241779-d206aab4-e652-11e7-909f-15ac8e54bd37.jpg)
2. 安装之后打开，点击红圈进入安装扩展页面
![pasted graphic 2](https://user-images.githubusercontent.com/8230103/34241783-d6f71fae-e652-11e7-8c9c-26bfe6438154.jpg)
3. 在应用搜索中输入alios，唯一一个的扩展就是它
![pasted graphic 3](https://user-images.githubusercontent.com/8230103/34241789-e0c93c88-e652-11e7-9a27-13a48885ff85.jpg)
4. 然后下载安装alios-studio，需要重启一次VS Code。重启后扩展生效，回到欢迎页面点击右下角create project，或者用键盘command+P打开命令窗口输入“>alios-studio：Create project”
![pasted graphic 4](https://user-images.githubusercontent.com/8230103/34241795-e4ae2840-e652-11e7-8e79-fa153931f3a8.jpg)
![pasted graphic 5](https://user-images.githubusercontent.com/8230103/34241813-fa0577ca-e652-11e7-95dc-befc129b686d.jpg)
5. 第一次创立项目的时候需要配置SDK，注意这时对比windows的界面是多了一个tool-chain path的
![pasted graphic 6](https://user-images.githubusercontent.com/8230103/34241817-03449a64-e653-11e7-86c7-b564d35943c6.jpg)
6. SDK path需要从GitHub上下载alios-things的[开发包](https://github.com/alibaba/AliOS-Things)
![pasted graphic 7](https://user-images.githubusercontent.com/8230103/34241833-23347b8c-e653-11e7-9ca8-e50fde85464c.jpg)
7. 我个人把它放到VS Code的extension目录下然后解压，注意这个.vscode是隐藏的，需要按command+shift+. 显示
![pasted graphic 8](https://user-images.githubusercontent.com/8230103/34241845-355c59ce-e653-11e7-9832-dbab4dd3331c.jpg)
8. 除此之外还需要下载[交叉工具链](https://launchpad.net/gcc-arm-embedded/+download)
![pasted graphic 9](https://user-images.githubusercontent.com/8230103/34241870-64781e50-e653-11e7-9dd0-df3ddc719ac1.jpg)
9. 下载解压后复制路径到tool-chain path里面
![pasted graphic 10](https://user-images.githubusercontent.com/8230103/34241891-70fd1518-e653-11e7-946e-d86601c22452.jpg)
然后就可以create project了，之后流程跟windows一样了。