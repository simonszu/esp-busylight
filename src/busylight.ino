#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* SSID = "Devicefunk";
const char* PSK = "freewaycola";
const char* MQTT_BROKER = "192.168.10.4";

WiFiClient espClient;
PubSubClient client(espClient);

int blueLED = D6;
int redLED = D5;

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println('\n');
  setup_wifi();
  client.setServer(MQTT_BROKER, 1883);
  client.setCallback(mqtt_callback);
  connect_mqtt();
  client.publish("/busylight/status", "online");

  pinMode(blueLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  digitalWrite(blueLED, LOW);
  digitalWrite(redLED, LOW);

}

void setup_wifi() {
  Serial.print("Connecting to ");
  Serial.print(SSID);
  Serial.println("...");
  
  WiFi.mode(WIFI_STA);
  WiFi.hostname("simon-busylight");
  WiFi.begin(SSID, PSK);

  int i = 0;
  while(WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(++i);
    Serial.print(' ');
  }

  Serial.println('\n');
  Serial.println("Connection established!");
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());
}
void mqtt_callback(String topic, byte* message, unsigned int length) {
  Serial.print("Received message [");
  Serial.print(topic);
  Serial.print("] ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
  
  if (topic=="/busylight/red") {
    if (messageTemp == "on"){
      digitalWrite(blueLED, LOW);
      digitalWrite(redLED, HIGH);
    } else if (messageTemp == "off"){
      digitalWrite(redLED, LOW);
    }
  }
  if (topic=="/busylight/blue") {
    if (messageTemp == "on"){
      digitalWrite(redLED, LOW);
      digitalWrite(blueLED, HIGH);
    } else if (messageTemp == "off") {
      digitalWrite(blueLED, LOW);
    }
  } 
  if (topic=="/busylight/purple") {
    if (messageTemp == "on"){
      digitalWrite(redLED, HIGH);
      digitalWrite(blueLED, HIGH);
    } else if (messageTemp == "off") {
      digitalWrite(redLED, LOW);
      digitalWrite(blueLED, LOW);
    }
  }   
}
void connect_mqtt() {
  while (!client.connected()) {
    Serial.print("Reconnecting MQTT...");
    if (!client.connect("BusylightClient")) {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 5 seconds");
      delay(5000);
    }
  }
  client.subscribe("/busylight/red");
  client.subscribe("/busylight/blue");
  client.subscribe("/busylight/purple");
  Serial.println("MQTT connected!");
}

void loop() {
  if (!client.connected()) {
    connect_mqtt();
  }
  client.loop();
}
