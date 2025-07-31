# ESP32-S3-MQTT

## 项目说明

### 功能概述

本ESP32S3程序实现以下功能：

1. 连接指定WiFi网络
2. 使用MQTT协议连接消息服务器
3. 读取GPIO1引脚的模拟数值
4. 每秒上传数据至MQTT服务器的指定主题
5. 接收来自MQTT服务器的控制指令
6. 提供详细的系统状态报告

### 硬件要求

- 支持WiFi的ESP32开发板
- 传感器/模拟信号源连接到GP IO1引脚

### 配置说明

#### WiFi配置
```cpp
const char* ssid = "******";        // WiFi名称
const char* password = "******";     // WiFi密码
```

#### MQTT配置
```cpp
const char* mqtt_server = "******"; // MQTT服务器地址
const int mqtt_port = 1883;                // MQTT端口(默认1883)
const char* mqtt_user = "******";            // MQTT用户名
const char* mqtt_pass = "******";         // MQTT密码
```

#### 其他配置
```cpp
const char* pub_topic = "sensors/ESP32S3/analog"; // 数据发布主题
const int PUBLISH_INTERVAL = 5000;          // 数据上传间隔(毫秒)
const int ANALOG_PIN = 1;                   // 模拟输入引脚
```



### MQTT数据格式

设备上传JSON格式数据到指定的主题：

```json
{
  "device": "ESP32S3_Sensor_Node",
  "sensor": "analog",
  "value": 2048,
  "voltage": 1.65,
  "rssi": -65
}
```

- **device**: 设备标识符
- **sensor**: 传感器类型
- **value**: 原始ADC值(0-4095)
- **voltage**: 转换后的电压值(0-3.3V)
- **rssi**: WiFi信号强度(dBm)

### 错误处理

代码中增加了完善的错误检测和处理机制：

1. WiFi连接失败时自动重启
2. MQTT连接失败时提供详细的错误原因
3. 服务器端连接验证
4. 数据发布失败检测与报告

## 部署说明

1. 在Arduino IDE中导入项目
2. 根据实际情况修改配置参数
3. 使用USB数据线上传程序至ESP32S3开发板
4. 打开串口监视器(115200波特率)查看运行状态
5. 使用MQTT客户端订阅指定主题接收数据

