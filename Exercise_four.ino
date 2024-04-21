#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Vux";
const char* password = "opal18032001";

const char* mqtt_server = "192.168.7.220";
const char* topic = "nt533/o22/group4/led";
const char* mqtt_server_username = "mqtt_client";
const char* mqtt_server_password = "1";
const int mqtt_port = 1883;

const char* PAYLOAD_ON = "1";
const char* PAYLOAD_OFF = "0";

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received on topic: ");
  Serial.println(topic);

  Serial.print("Payload: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }

  // Set LED state based on the last payload byte received
  if (length > 0) {
    if ((char)payload[length - 1] == '1') {
      digitalWrite(D2, HIGH);
    } else {
      digitalWrite(D2, LOW);
    }
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  pinMode(D2, OUTPUT);

  WiFi.begin(ssid, password);
  Serial.println("");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  while (!client.connected()) {
    Serial.println("Connecting to MQTT broker...");
    if (client.connect("ESP8266Client", mqtt_server_username, mqtt_server_password)) {
      Serial.println("Connected to MQTT broker");
      client.subscribe(topic);
    } else {
      Serial.print("Failed to connect to MQTT broker, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 5 seconds");
      delay(5000);
    }
  }
  Serial.print("Subscribed topic: ");
  Serial.print(topic);
  client.subscribe(topic);

  client.publish(topic, "Hello this messenger from group 4 ");
}

void loop() {
  client.loop();
  // Turn on LED
  client.publish(topic, PAYLOAD_ON);
  delay(1000);
  // Turn off LED
  client.publish(topic, PAYLOAD_OFF);
}