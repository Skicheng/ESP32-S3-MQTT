#include <WiFi.h>
#include <PubSubClient.h>

// WiFi配置
const char* ssid = "******";
const char* password = "******";

// MQTT配置
const char* mqtt_server = "******";   // 服务器地址/IP
const int mqtt_port = 1883;                     // 默认端口1883（无加密）
const char* mqtt_user = "******";        // MQTT用户名
const char* mqtt_pass = "******";        // MQTT密码
const char* clientID = "ESP32S3_Client";        // 客户端ID

// 订阅主题和发布主题
const char* pub_topic = "esp32s3/data";
const char* sub_topic = "esp32s3/commands";

WiFiClient espClient;
PubSubClient client(espClient);

const int ANALOG_PIN = 1;// 使用GPIO1作为模拟输入


// 连接WiFi
void setup_wifi() {
  Serial.println();
  Serial.print("连接WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi连接成功");
  Serial.print("IP地址: ");
  Serial.println(WiFi.localIP());
}

// 收到MQTT消息时的回调函数
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("收到消息 [");
  Serial.print(topic);
  Serial.print("]: ");
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

// 重连MQTT
void reconnect() {
  while (!client.connected()) {
    Serial.print("尝试MQTT连接...");
    
    if (client.connect(clientID, mqtt_user, mqtt_pass)) {
      Serial.println("已连接");
      
      // 连接成功后订阅主题
      client.subscribe(sub_topic);
      Serial.print("已订阅主题: ");
      Serial.println(sub_topic);
      
    } else {
      Serial.print("失败，rc=");
      Serial.print(client.state());
      Serial.println(" 5秒后重试");
      delay(5000);
    }
  }
}
void testServerConnection() {
  WiFiClient testClient;
  Serial.print("测试服务器端口连接...");
  if (testClient.connect(mqtt_server, mqtt_port)) {
    Serial.println("端口可访问!");
    testClient.stop();
  } else {
    Serial.println("连接失败! 检查:");
    Serial.printf("- 地址: %s\n", mqtt_server);
    Serial.printf("- 端口: %d\n", mqtt_port);
    Serial.println("- 防火墙设置");
    Serial.println("- 网络隔离策略");
  }
}

// 读取并发布模拟值
void publishSensorData() {
  // 读取模拟值 (0-4095对应0-3.3V)
  int analogValue = analogRead(ANALOG_PIN);
  
  char payload[256];
  snprintf(payload, sizeof(payload),
           "{\"device\":\"%s\",\"sensor\":\"analog\",\"value\":%d,\"voltage\":%.2f,\"rssi\":%d}",
           clientID,
           analogValue,
           analogValue * 3.3 / 4096.0, // 转换为电压值
           WiFi.RSSI());
  
  // 发布消息
  if (client.publish(pub_topic, payload)) {
    Serial.print("发布成功: ");
    Serial.println(payload);
  } else {
    Serial.println("发布失败!");
  }
}

void setup() {
  analogReadResolution(12); // 设置为12位分辨率 (0-4095)
  pinMode(ANALOG_PIN, INPUT);
  Serial.begin(115200);
  setup_wifi();
  
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  testServerConnection();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  // 示例：每10秒发布一次数据
  static unsigned long lastMsgTime = 0;
  unsigned long now = millis();
  
  if (now - lastMsgTime > 1000) {
    lastMsgTime = now;
    publishSensorData();
  }
}
