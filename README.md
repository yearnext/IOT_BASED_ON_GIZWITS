<<<<<<< HEAD
=======
# 设备通讯协议介绍
## 一、数据格式介绍

|通讯类型|事件序号|数据发送/接收方设备类型|数据发送/接收方MAC地址|命令|数据长度|数据|校验和|
|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
|1 Byte|1 Byte|1 Byte|8 Byte|1 Byte|1 Byte|18 Byte|1 Byte|

### 1.1 通讯类型
|通讯代号|意义|简写|
|:-:|:-:|:-:|
|0x00|结束此次通讯流程|END_COMM|
|0x01|APP请求与设备通讯|W2D|
|0x02|设备应答APP通讯|W2D_ACK|
|0x03|设备请求与APP通讯|D2W|
|0x04|APP应答设备通讯|D2W_ACK|
|0x05|网关请求与设备通讯|H2S|
|0x06|设备应答网关请求|H2S_ACK|
|0x07|设备请求与网关通讯|S2H|
|0x08|网关应答设备请求|S2H_ACK|
|0x09|校验码错误|ERROR_CODE|

### 1.2 事件序号
等待应答的事件序号应与应答的事件序号保持一致。

### 1.3 设备类型
|设备类型|设备代码|
|:-:|:-:|
|网关|0x00|
|APP|0x00|
|灯|0x01|
|插座|0x02|
|窗帘|0x03|
|温湿度传感器|0x04|

**若数据从APP发向设备(W2D、D2W_ACK通讯类型)，设备类型填写数据接收端的设备类型。**

**若数据从设备发向APP(W2D_ACK、D2W通讯类型)，设备类型填写数据发送方的设备类型。**

**若数据从协调器发向终端(H2S、S2H_ACK通讯类型)，设备类型填写数据接收端的设备类型。**

**若数据从终端发向协调器(H2S_ACK、S2H通讯类型)，设备类型填写数据发送方的设备类型。**

### 1.4 设备MAC地址
设备的MAC地址。

**若数据从APP发向设备(W2D、D2W_ACK通讯类型)，设备类型填写数据接收端的MAC地址。**

**若数据从设备发向APP(W2D_ACK、D2W通讯类型)，设备类型填写数据发送方的MAC地址。**

**若数据从协调器发向终端(H2S、S2H_ACK通讯类型)，设备类型填写数据接收端的MAC地址。**

**若数据从终端发向协调器(H2S_ACK、S2H通讯类型)，设备类型填写数据发送方的MAC地址。**

### 1.5 命令
见设备数据点介绍。

### 1.6 数据长度
数据长度表示数据的具体长度，不能超过最大数据长度。

### 1.7 数据
见设备数据点介绍。

### 1.8 校验和
校验和的计算方式为把数据包从通讯类型为开始按字节求和得出的结果对256求余。

## 二、设备命令介绍
### 2.1 设备通用基本命令
|控制命令|控制命令介绍|权限|数据点|数据点大小|参数枚举|
|:-:|:-:|:-:|:-:|:-:|:-:|
|0x00|设备心跳命令|只读|-|-|-|
|0x01|恢复设备出厂设备命令|只读|-|-|-|
|0x02|设备重启|只读|-|-|-|
|0x03|刷新设备时间|只读|-|-|-|
|0x04|读取设备时间数据|只读|时间数据|9 Byte|-|
|-|-|-|时间-秒钟数据|1 Byte|0~59|
|-|-|-|时间-分钟数据|1 Byte|0~59|
|-|-|-|时间-小时数据|1 Byte|0~23|
|-|-|-|时间-日数据|1 Byte|1~31|
|-|-|-|时间-月数据|1 Byte|1~12|
|-|-|-|时间-星期数据|1 Byte|1~7|
|-|-|-|时间-年数据|2 Byte|0~65535|
|-|-|-|数据有效位|1 Byte|1:该数据为有效数据|
|-|-|-|-|-|0:该数据为无效数据|
|0x05|写入设备时间数据|只写|时间数据|9 Byte|-|
|-|-|-|时间-秒钟数据|1 Byte|0~59|
|-|-|-|时间-分钟数据|1 Byte|0~59|
|-|-|-|时间-小时数据|1 Byte|0~23|
|-|-|-|时间-日数据|1 Byte|1~31|
|-|-|-|时间-月数据|1 Byte|1~12|
|-|-|-|时间-星期数据|1 Byte|1~7|
|-|-|-|时间-年数据|2 Byte|0~65535|
|-|-|-|数据有效位|1 Byte|1:该数据为有效数据|
|-|-|-|-|-|0:该数据为无效数据|

### 2.2 网关命令介绍
|控制命令|控制命令介绍|权限|数据点|数据点大小|参数枚举|
|:-:|:-:|:-:|:-:|:-:|:-:|
|0x10|设备列表变化|只读|设备的数量|1 Byte|0~255|
|0x11|APP通过该命令获取目前设备的数量|只读|设备的数量|1 Byte|0~255|
|0x12|APP通过该命令获取目前设备的信息|只读|读取的设备信息|10 Byte|-|
|-|-|-|设备编号（第x台设备）|1 Byte|0 ~ 255|
|-|-|-|设备类型|1 Byte|见设备类型|
|-|-|-|设备MAC地址|8 Byte|见设备MAC地址|
|0x13|设备列表读取失败|只读|-|-|-|

**APP获取设备信息流程介绍**
1. APP向网关设备请求获取在线设备数量。
2. 网关应答APP在线设备数量。
3. APP向网关发送需要获取设备信息的设备编号。
4. 网关回复APP请求的设备编号的设备信息(如果网关返回读取失败命令则APP应该重新执行获取设备信息流程)。
5. APP依次读完所有设备的设备信息。

**注：当设备列表有变化时，设备会向APP发送设备列表变化命令并等待APP读取设备信息。**

### 2.3 电灯命令介绍
|设备命令|设备命令介绍|权限|数据点|数据点大小|参数枚举|
|:-:|:-:|:-:|:-:|:-:|:-:|
|0x10|灯的亮度|只读|Status|1 Byte|0~100(0为关闭，100为开启)|
|0x11|灯的亮度|只写|Status|1 Byte|0~100(0为关闭，100为开启)|
|0x12|定时器单次运行设置|只读|详见定时器数据类型介绍|详见定时器数据类型介绍|详见定时器数据类型介绍|
|0x13|定时器单次运行设置|只写|详见定时器数据类型介绍|详见定时器数据类型介绍|详见定时器数据类型介绍|
|0x14|定时器重复运行设置|只读|详见定时器数据类型介绍|详见定时器数据类型介绍|详见定时器数据类型介绍|
|0x15|定时器重复运行设置|只写|详见定时器数据类型介绍|详见定时器数据类型介绍|详见定时器数据类型介绍|
|0x16|电灯上电运行模式|只读|Mode|1 Byte|0(上电时电灯状态为关闭状态)|
|-|-|-|-|-|1(上电时电灯状态为上一次掉电前的状态)|
|0x17|电灯上电运行模式|只写|Mode|1 Byte|0(上电时电灯状态为关闭状态)|
|-|-|-|-|-|1(上电时电灯状态为上一次掉电前的状态)|

### 2.4 插座命令介绍
|设备命令|设备命令介绍|权限|数据点|数据点大小|参数枚举|
|:-:|:-:|:-:|:-:|:-:|:-:|
|0x10|插座状态|只读|Status|1 Byte|0~1(0为插座关闭，1为插座开启)|
|0x11|插座状态|只写|Status|1 Byte|0~1(0为插座关闭，1为插座开启)|
|0x12|定时器单次运行设置|只读|详见定时器数据类型介绍|详见定时器数据类型介绍|详见定时器数据类型介绍|
|0x13|定时器单次运行设置|只写|详见定时器数据类型介绍|详见定时器数据类型介绍|详见定时器数据类型介绍|
|0x14|定时器重复运行设置|只读|详见定时器数据类型介绍|详见定时器数据类型介绍|详见定时器数据类型介绍|
|0x15|定时器重复运行设置|只写|详见定时器数据类型介绍|详见定时器数据类型介绍|详见定时器数据类型介绍|
|0x16|消耗电能|只读|读取插座消耗的电能|1 Byte|0~100|
|0x17|插座上电运行模式|只读|Mode|1 Byte|0(上电时插座状态为关闭状态)|
|-|-|-|-|-|1(上电时插座状态为上一次掉电前的状态)|
|0x18|插座上电运行模式|只写|Mode|1 Byte|0(上电时插座状态为关闭状态)|
|-|-|-|-|-|1(上电时插座状态为上一次掉电前的状态)|

### 2.5 窗帘命令介绍
|设备命令|设备命令介绍|权限|数据点|数据点大小|参数枚举|
|:-:|:-:|:-:|:-:|:-:|:-:|
|0x10|读取窗帘当前状态|只读|Status|1 Byte|窗帘当前状态|
|-|-|-|-|-|窗帘初始化状态(0x00)|
|-|-|-|-|-|窗帘正在开启状态(0x01)|
|-|-|-|-|-|窗帘完成开启状态(0x02)|
|-|-|-|-|-|窗帘全部开启状态(0x03)|
|-|-|-|-|-|窗帘正在关闭状态(0x04)|
|-|-|-|-|-|窗帘完成关闭状态(0x05)|
|-|-|-|-|-|窗帘全部关闭状态(0x06)|
|0x11|控制窗帘操作命令|只写|Status|1 Byte|窗帘操作命令|
|-|-|-|-|-|打开窗帘(0x01)|
|-|-|-|-|-|合上窗帘(0x02)|
|0x12|定时器单次运行设置|只读|详见定时器数据类型介绍|详见定时器数据类型介绍|详见定时器数据类型介绍|
|0x13|定时器单次运行设置|只写|详见定时器数据类型介绍|详见定时器数据类型介绍|详见定时器数据类型介绍|
|0x14|定时器重复运行设置|只读|详见定时器数据类型介绍|详见定时器数据类型介绍|详见定时器数据类型介绍|
|0x15|定时器重复运行设置|只写|详见定时器数据类型介绍|详见定时器数据类型介绍|详见定时器数据类型介绍|
|0x16|APP通过该命令读取窗帘的工作模式|只读|智能工作模式开关|1 Byte|0~1|
|-|-|-|设定自动关闭窗帘的亮度|1 Byte|0~100|
|0x17|APP通过该命令设置窗帘的工作模式|只写|智能工作模式开关|1 Byte|0~1|
|-|-|-|设定自动关闭窗帘的亮度|1 Byte|0~100|
|0x18|下雨警报|只读|窗帘向APP发送下雨警报|1 Byte|0~1|
|0x19|窗帘检测到的光照前的强度|只读|窗帘检测到的亮度信息|1 Byte|0~100|
|0x1A|窗帘上电运行模式|只读|窗帘上电运行状态|1 Byte|-|
|-|-|-|-|-|保持现有状态(0x00)|
|-|-|-|-|-|关闭窗帘(0x01)|
|-|-|-|-|-|开启窗帘(0x02)|
|0x1B|窗帘上电运行模式|只写|窗帘上电运行状态|1 Byte|-|
|-|-|-|-|-|保持现有状态(0x00)|
|-|-|-|-|-|关闭窗帘(0x01)|
|-|-|-|-|-|开启窗帘(0x02)|

### 2.6 温湿度传感器命令介绍
|设备命令|设备命令介绍|权限|数据点|数据点大小|参数介绍|
|:-:|:-:|:-:|:-:|:-:|:-:|
|0x10|上报当前环境温湿度数据|只读|上报的温湿度数据|2 Byte|-|
|-|-|-|TEMP|1 Byte|0~50|
|-|-|-|HUMI|1 Byte|0~100|
|0x11|读取当前环境温湿度数据|只读|读取的温湿度数据|2 Byte|-|
|-|-|-|TEMP|1 Byte|0~50|
|-|-|-|HUMI|1 Byte|0~100|
注：由于温湿度传感器精度问题，只能检测到0~50摄氏度的温度且温度误差在正负2摄氏度内，检测到的湿度范围为20%~90%，且湿度误差在正负5%范围内。

### 2.7 定时器数据格式介绍
|数据名称|成员|成员介绍|权限|占用内存|参数介绍|
|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
|定时器工作模式|-|-|只读|1 Byte|-|
|-|-|-|-|-|TIMER_SLEEP_MODE（定时器休眠模式） = 0x00|
|-|-|-|-|-|TIMER_DOWNCNT_MODE（定时器倒计时运行模式） = 0x01|
|-|-|-|-|-|TIMER_PERIOD_MODE（定时器时间段运行模式） = 0x02|
|-|-|-|-|-|TIMER_CIRCUL_MODE（定时器重复运行模式） = 0x03|
|自定义工作参数|-|-|读/写|1 Byte|配置定时器自定义运行模式的工作参数|
|-|保留|保留|保留|Bit 0(占用该字节的第0位)|0~1|
|-|monday|周一工作状态|读/写|Bit 1(占用该字节的第1位)|0~1|
|-|tuesday|周二工作状态|读/写|Bit 2(占用该字节的第2位)|0~1|
|-|wednesday|周三工作状态|读/写|Bit 3(占用该字节的第3位)|0~1|
|-|thursday|周四工作状态|读/写|Bit 4(占用该字节的第4位)|0~1|
|-|friday|周五工作状态|读/写|Bit 5(占用该字节的第5位)|0~1|
|-|saturday|周六工作状态|读/写|Bit 6(占用该字节的第6位)|0~1|
|-|sunday|周日工作状态|读/写|Bit 7(占用该字节的第7位)|0~1|
|定时器工作时间|-|-|读/写|4 Byte|配置定时器的启动时间和停止时间|
|-|Start_Hour|启动时间-小时|读/写|1 Byte|0~23|
|-|Start_Minute|启动时间-分钟|读/写|1 Byte|0~59|
|-|Stop_Hour|启动时间-小时|读/写|1 Byte|0~23|
|-|Stop_Minute|停止时间-分钟|读/写|1 Byte|0~59|
|设备控制状态|-|-|读/写|2 Byte|配置定时器启动、停止时的设备状态|
|-|device_start_status|设备在定时器启动时的状态|读/写|1 Byte|根据设备而定|
|-|device_end_status|设备在定时器停止时的状态|读/写|1 Byte|根据设备而定|

#### 2.7.1 倒计时模式参数配置
例：倒计时8小时35分钟后关闭

    Timer_Mode = TIMER_DOWNCNT_MODE;
    Start_Hour = Now_Time_Hour;
    Start_Minute = Now_Time_Minute;
    Stop_Hour = 8;
    Start_Minute = 35;
    device_start_status = now_status;
    device_end_status = off_status;

**注：**
**1. 当设定定时器工作在倒计时模式时，定时器工作一次，不会重复运行。**
**2. 定时器工作在倒计时模式时不允许出现倒计时时间大于24小时的情况，请注意检查。**

#### 2.7.2 时间段模式参数配置
例：8:50~9:45开启

    Timer_Mode = TIMER_SIGNAL_MODE;
    Start_Hour = 8;
    Start_Minute = 50;
    Stop_Hour = 9;
    Start_Minute = 45;
    device_start_status = start_status;
    device_end_status = end_status;

**注：**
**1. 当设定定时器工作在时间段模式时，定时器工作一次，不会重复运行。**
**2. 定时器工作在时间段模式时不允许出现启动时间大于停止时间的情况，请注意检查。**

#### 2.7.3 重复运行模式参数配置
例：每周一、周二、周三8:50~9:45开启

    Timer_Mode = TIMER_CUSTOM_MODE;
    monday = 1;
    tuesday = 1;
    wednesday = 1;
    Start_Hour = 8;
    Start_Minute = 50;
    Stop_Hour = 9;
    Start_Minute = 45;
    device_start_status = start_status;
    device_end_status = end_status;

**注：**
**1. 当设定定时器工作在重复运行模式时，定时器会重复运行。**
**2. 定时器工作在重复运行模式时不允许出现启动时间大于停止时间的情况，请注意检查。**
>>>>>>> ec588fe... 修复了温湿度数据读取失败的BUG
