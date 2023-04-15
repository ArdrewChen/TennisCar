
#include "ESP8266WiFi.h"
 
const char* ssid = "one";//连接的WiFi名称
const char* password =  "12345678";//连接的wifi密码

//静态地址、网关、子网掩码
IPAddress local_IP(192, 168, 121, 200);
IPAddress gateway(192, 168, 121, 1);
IPAddress subnet(255, 255, 255, 0);
 
WiFiServer wifiServer(8080);//使用8080端口进行连接
 
void setup() {
  Serial.begin(115200);
  wifi_station_dhcpc_stop();
  delay(1000);
  WiFi.config(local_IP, gateway, subnet);//设置静态IP
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
//    Serial.println("Connecting..");
  }
//  Serial.println(WiFi.localIP());
  wifiServer.begin();
}
 
void loop() {
  WiFiClient client = wifiServer.available();
  String msgs = "";
  if (client) {
    while (client.connected()) {
      while (client.available()>0) {
        char c = client.read();
        Serial.write(c);
        msgs += c;
      }
      msgs = "";
      delay(10);
    }
    client.stop();
//    Serial.println("Client disconnected");
  }
}
