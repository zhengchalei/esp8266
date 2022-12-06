#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <PubSubClient.h>

ESP8266WiFiMulti wifiMulti;
WiFiClient espClient;
PubSubClient client(espClient);

int relay = 4;

const char* ssid = "Xiaomi_C0AC";
const char* passphrase = "12345678";

const char* mqttServerHost = "broker.emqx.io";
const int mqttServerPort = 1883;
const char* mqttTopic = "relay/command";


void setup(void) {
  Serial.begin(9600);
  pinMode(relay, OUTPUT);

  // 连接WIFI
  Serial.println("will connect WIFI ...");
  connectWifi();
  Serial.println("start connect MQTT ...");
  connectMQTT();
}

void loop(void) {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

void connectMQTT() {
  Serial.println("connect MQTT SERVER ...");
  client.setServer(mqttServerHost, mqttServerPort);
  Serial.println("set MQTT Callback ...");
  client.setCallback(callback);
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);  // 打印主题信息
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if ((int)payload[0] % 2 == 1) {
    Serial.println("open");
    open();
  } else {
    Serial.println("close");
    close();
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("MQTT connection...");
    // Attempt to connect
    if (client.connect("esp8266-01")) {
      Serial.println("connected");
      // 连接成功时订阅主题
      client.subscribe(mqttTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void connectWifi() {
  wifiMulti.addAP(ssid, passphrase);
  Serial.println("Connecting ...");

  while (wifiMulti.run() != WL_CONNECTED) {
    delay(1000);
    Serial.print('Connecting');
  }
  Serial.println('\n');
  Serial.print("Connected to WIFI");
  Serial.println(WiFi.SSID());
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());
}

void open() {
  digitalWrite(relay, HIGH);
}
void close() {
  digitalWrite(relay, LOW);
}