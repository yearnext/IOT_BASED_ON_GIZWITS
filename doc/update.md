# IOT_BASE_ON_GIZWITS更新日志——mcu_software_debug分支
## 版本命名规则
**1. 版本信息格式介绍**
工程名\_分支名\_主版本号.子版本号.修正版本号\_版本类型\_编译时间
示例：IOTBaseOnGizwits_MCUSoftwareDebug_0.0.1_alpha_201609271352

|格式名称|参数介绍|
|:-:|:-:|
|工程名|IOTBaseOnGizwits|
|分支名|MCUSoftwareDebug|
|主版本号|0|
|子版本号|0|
|修正版本号|1|
|版本类型|alpha|
|编译时间|201609271352|

**2.版本号管理策略**
+ 项目初版本时，版本号可以为 0.1 或 0.1.0, 也可以为 1.0 或 1.0.0。
+ 当项目在进行了局部修改或 bug 修正时，主版本号和子版本号都不变，修正版本号加1。
+ 当项目在原有的基础上增加了部分功能时，主版本号不变，子版本号加1，修正版本号复位为0，因而可以被忽略掉
+ 当项目在进行了重大修改或局部修正累积较多，而导致项目整体发生全局变化时，主版本号加1。

**3.版本类型介绍**

|版本类型|版本名称|版本介绍|
|:-:|:-:|:-:|
|测试版本|-|-|
|-|Alphal|内部测试版|
|-|Beta|外部测试版|
|-|M 版|Milestone， 每个开发阶段的终结点的里程碑版本|
|-|Trail|试用版（含有某些限制，如时间、功能，注册后也有可能变为正式版）|
|-|RC版|Release Candidate，意思是发布倒计时，该版本已经完成全部功能并清除大部分的BUG。到了这个阶段只会除BUG，不会对软件做任何大的更改|
|-|RTM版|Release To Manufactur，意思是发布到生产商，这基本就是最终的版本|
|-|GA版|Generally Available，最终版|
|正式版本|-|-|
|-|Enhance|增强版或者加强版|
|-|Full version|完全版|
|-|Release|发行版，有时间限制|
|-|Upgrade|升级版|
|-|Retail|零售版|
|-|Plus|增强版，不过这种大部分是在程序界面及多媒体功能上增强|

## 更新日志
**固件版本：IOTBaseOnGizwits_MCUSoftwareDebug_0.2.12_alpha_201612252330**
+ 修复了窗帘定时器回调存在递归的BUG。
+ 修改通讯协议标注错误的问题。

**固件版本：IOTBaseOnGizwits_MCUSoftwareDebug_0.2.11_alpha_201612242230**
+ 修复了定时器运行错误的BUG。
+ 修复了网关获取网络时间过长的BUG。
+ 修改网关清除僵尸设备的时间。
+ 延长设备心跳时间。
+ 添加定时器操作设备的状态上报操作。
+ 修复的下雨状态检测不正常的BUG。
+ 修复通讯协议部分内容。
+ 添加电灯设备的路由分支。
+ 解决了定时器无法在启动后立即停止设备的BUG。

**固件版本：IOTBaseOnGizwits_MCUSoftwareDebug_0.2.10_alpha_201612240022**
+ 修复了电动窗帘上电自动运行的BUG。
+ 修复了电动窗帘按键失灵的BUG。
+ 修复了按键过于灵敏的BUG。

**固件版本：IOTBaseOnGizwits_MCUSoftwareDebug_0.2.9_alpha_201612212337**
+ 完成ZSTACK的NV操作调试。
+ 添加更新日志。
+ 添加设备时钟芯片使能/禁用开关。
+ 添加设备数据保存使能/禁用开关。
+ 修改电灯默认上电状态为关闭状态。
+ 解决了电动窗帘无法实现远程和按键连续控制的BUG。

**固件版本：IOTBaseOnGizwits_MCUSoftwareDebug_0.2.8_alpha_201612201138**
+ 修复倒计时定时器不能正常工作的BUG。

**固件版本：IOTBaseOnGizwits_MCUSoftwareDebug_0.2.7_alpha_201612192312**
+ 修复设备从网关获取时间不正确的BUG。
+ 修复了读取设备列表不正确的BUG。
+ 禁用除COORD设备的串口2驱动。
+ 修改工程清理的批处理文件。
+ 修复设备无法上报消息的BUG。

**固件版本：IOTBaseOnGizwits_MCUSoftwareDebug_0.2.6_alpha_201612181939**
+ 进入DEBUG阶段。

**固件版本：IOTBaseOnGizwits_MCUSoftwareDebug_0.2.5_alpha_201612172346**
+ 解决电动窗帘电机运行时单片机死机的BUG。

**固件版本：IOTBaseOnGizwits_MCUSoftwareDebug_0.2.4_alpha_201612162350**
+ 发现BUG：电动窗帘电机运行时单片机死机。

**固件版本：IOTBaseOnGizwits_MCUSoftwareDebug_0.2.3_alpha_201612142327**
+ 修复zigbee设备底层通讯错误的BUG。
+ 准备进入DEBUG阶段。

**固件版本：IOTBaseOnGizwits_MCUSoftwareDebug_0.2.2_alpha_201612132303**
+ 重写设备列表服务。
+ 分离COORD组件。
+ 增加COORD设备控制程序。
+ 修改空应答机制。
+ 解决抓包失败导致接收缓冲区错误的BUG。
+ 发现：按键反应迟钝BUG。
+ 提高通讯稳定性。

**固件版本：IOTBaseOnGizwits_MCUSoftwareDebug_0.2.1_alpha_201612121622**
+ 修改设备类型判断代码。
+ 使用标准库进行结构体偏移量的计算。
+ 新建工程模版。

**固件版本：IOTBaseOnGizwits_MCUSoftwareDebug_0.2.0_alpha_201612102211**
+ 重写MYPROTOCOL程序框架。

**固件版本：IOTBaseOnGizwits_MCUSoftwareDebug_0.1.27_alpha_201612072222**
+ 修复温湿度数据读取错误的BUG。
+ 降低温湿度数据的读取时间。

**固件版本：IOTBaseOnGizwits_MCUSoftwareDebug_0.1.26_alpha_201612062254**
+ 添加智能插座初始化程序。
+ 修改工程目录结构。

**固件版本：IOTBaseOnGizwits_MCUSoftwareDebug_0.1.25_alpha_201612022237**
+ 添加自动校时服务。

**固件版本：IOTBaseOnGizwits_MCUSoftwareDebug_0.1.24_alpha_201611282112**
+ 添加电动窗帘应答服务。

**固件版本：IOTBaseOnGizwits_MCUSoftwareDebug_0.1.23_alpha_201611271452**
+ 修改电动窗帘模块功能，添加对环境亮度的检测。

**固件版本：IOTBaseOnGizwits_MCUSoftwareDebug_0.1.22_alpha_201611252216**
+ 添加ADC转换程序。
+ 修改定时器组件。

**固件版本：IOTBaseOnGizwits_MCUSoftwareDebug_0.1.21_alpha_201611242229**
+ 修改硬件电路。
+ 完善电动窗帘的硬件抽象层。
+ 修改DS1302驱动程序。

**固件版本：IOTBaseOnGizwits_MCUSoftwareDebug_0.1.20_alpha_201611222241**
+ 修改数据存储服务。

**固件版本：IOTBaseOnGizwits_MCUSoftwareDebug_0.1.19_alpha_201611212228**
+ 修改硬件IO配置。
+ 修改电动窗帘的按键检测处理策略。
+ 修改MYPROTOCOL底层通讯程序。
+ 增加帮助文档。
+ 添加星期计算程序。
+ 完善获取网络时间服务。

**固件版本：IOTBaseOnGizwits_MCUSoftwareDebug_0.1.18_alpha_201611202354**
+ 整合硬件资源。
+ 完善MYPROTOCOL框架。
+ 添加电动窗帘的处理程序。

**固件版本：IOTBaseOnGizwits_MCUSoftwareDebug_0.1.17_alpha_201611042210**
+ 修改网关回复APP请求控制设备应答处理。

**固件版本：IOTBaseOnGizwits_MCUSoftwareDebug_0.1.16_alpha_201611032241**
+ 修复了网关未连接APP却仍然向APP发送命令请求BUG。

**固件版本：IOTBaseOnGizwits_MCUSoftwareDebug_0.1.15_alpha_201611022235**
+ 添加对温湿度传感器设备的处理程序。
+ 添加对智能插座设备的处理程序。
+ 修改温湿度传感器设备的控制命令。

**固件版本：IOTBaseOnGizwits_MCUSoftwareDebug_0.1.14_alpha_201610311910**
+ 更新通讯协议定时器章节的说明。
+ 修改定时器处理程序。
+ 定时器程序支持通讯协议标准。

**固件版本：IOTBaseOnGizwits_MCUSoftwareDebug_0.1.13_alpha_201610302259**
+ 修复组网状态指示灯显示异常的BUG。
+ 修改通讯协议定时器章节说明。
+ 修改通讯协议介绍。

**固件版本：IOTBaseOnGizwits_MCUSoftwareDebug_0.1.12_alpha_201610292001**
+ 修复了添加设备失败的BUG。
+ 修改MYPROTOCOL的日志打印配置。

**固件版本：IOTBaseOnGizwits_MCUSoftwareDebug_0.1.11_alpha_201610282359**
+ 禁用MYPROTOCOL框架的数据发送缓冲机制。
+ 修改发包函数。
+ 解决主机无法接收到从机发送的数据的BUG。
+ 修改USE_GIZWITS_MOD宏为MYPROTOCOL_DEVICE_COORD宏。

**固件版本：IOTBaseOnGizwits_MCUSoftwareDebug_0.1.10_alpha_201610272246**
+ 修改设备状态变化处理策略。
+ 修改通讯协议通讯类型注释内容。
+ 修改通讯协议定时器模块注释内容。
+ 优化数据包解析框架。
+ 修正底层通讯程序发送目标设设置不正确的BUG。
+ 整合MYPROTOCOL模块。
+ 已知BUG：主机无法接收到从机发送的数据。

**固件版本：IOTBaseOnGizwits_MCUSoftwareDebug_0.1.9_alpha_201610252302**
+ 修改按键检测处理策略。

**固件版本：IOTBaseOnGizwits_MCUSoftwareDebug_0.1.8_alpha_201610241122**
+ 添加智能电灯定时器后台服务。
+ 修复了DS1302数据读写错误的BUG。
+ 修复了DHT11读取数据时校验和计算错误的BUG。

**固件版本：IOTBaseOnGizwits_MCUSoftwareDebug_0.1.7_alpha_201610222328**
+ 修改部分通讯协议内容，增加通讯流程说明。
+ 完成智能电灯的开发工作，进入DEBUG阶段。
+ 添加机智云固件初始化程序。
+ 已知问题：终端设备无法加入网络。

**固件版本：IOTBaseOnGizwits_MCUSoftwareDebug_0.1.6_alpha_201610211726**
+ 修改设备通讯类型。
+ 修改底层通讯程序。

**固件版本：IOTBaseOnGizwits_MCUSoftwareDebug_0.1.5_alpha_201610202301**
+ 添加自动检测设备类型代码。
+ 完善通讯协议的介绍。

**固件版本：IOTBaseOnGizwits_MCUSoftwareDebug_0.1.4_alpha_201610191105**
+ 修改通讯协议定时器部分内容
+ 添加定时器任务的处理程序。

**固件版本：IOTBaseOnGizwits_MCUSoftwareDebug_0.1.3_alpha_201610181027**
+ 修改应用定时器的结构。

**固件版本：IOTBaseOnGizwits_MCUSoftwareDebug_0.1.2_alpha_201610171858**
+ 完成应用定时器工作状态检测。
+ 整合通讯协议。

**固件版本：IOTBaseOnGizwits_MCUSoftwareDebug_0.1.1_alpha_201610161038**
+ 整合数据包发送程序。

**固件版本：IOTBaseOnGizwits_MCUSoftwareDebug_0.1.0_alpha_201610150748**
+ 修改程序框架，整合各个资源模块进行集中控制。

**固件版本：IOTBaseOnGizwits_MCUSoftwareDebug_0.0.13_alpha_201610141627**
+ 添加按键检测程序。
+ 禁用ZSTACK自带的定时器驱动程序。
+ 完成CC2530的FLASH测试。
+ 修改定时器驱动。

**固件版本：IOTBaseOnGizwits_MCUSoftwareDebug_0.0.12_alpha_201610131528**
+ 完成呼吸灯的测试。

**固件版本：IOTBaseOnGizwits_MCUSoftwareDebug_0.0.11_alpha_201610122228**
+ 添加定时器PWM驱动程序。

**固件版本：IOTBaseOnGizwits_MCUSoftwareDebug_0.0.10_alpha_201610110946**
+ 禁用ZSTACK自带的LED驱动和KEY驱动程序。
+ 设备心跳通讯测试成功。

**固件版本：IOTBaseOnGizwits_MCUSoftwareDebug_0.0.9_alpha_201610101634**
+ 修复了终端设备串口波特率调整不当的BUG。

**固件版本：IOTBaseOnGizwits_MCUSoftwareDebug_0.0.8_alpha_201610092303**
+ 添加获取网络时间数据包。
+ 数据包解析框架开发中。

**固件版本：IOTBaseOnGizwits_MCUSoftwareDebug_0.0.7_alpha_201610051154**
+ 完成Gizwits通讯与Zigbee之间的兼容测试。

**固件版本：IOTBaseOnGizwits_MCUSoftwareDebug_0.0.6_alpha_201610040803**
+ 添加错误数据包的处理机制。
+ 添加结束通讯机制。

**固件版本：IOTBaseOnGizwits_MCUSoftwareDebug_0.0.5_alpha_201610031635**
+ 完成CC2530网关和ESP8266通讯。

**固件版本：IOTBaseOnGizwits_MCUSoftwareDebug_0.0.4_alpha_201609291115**
+ 更新工程文件，未解决UART1工作异常的问题。

**固件版本：IOTBaseOnGizwits_MCUSoftwareDebug_0.0.3_alpha_201609282133**
+ 开启ZSTACK的双串口，已知问题为UART1无法正常工作。

**固件版本：IOTBaseOnGizwits_MCUSoftwareDebug_0.0.2_alpha_201609280955**
+ 完成机智云固件的移植工作。

**固件版本：IOTBaseOnGizwits_MCUSoftwareDebug_0.0.1_alpha_201609271352**
+ 新建工程，启动项目开发。
+ 实现Zigbee设备的P2P通讯。