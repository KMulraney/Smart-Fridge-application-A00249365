#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <HX711.h>

 
// WiFi
const char *ssid = "eir95882939"; // Enter your WiFi name
const char *password = "VggE8tE5Jh";  // Enter WiFi password

// MQTT Broker
const char* mqtt_broker = "broker.hivemq.com";
const char* topic = "PhoneStatusServo/A00249365";
const char* topic2 = "servo/A00249365";
const int mqtt_port = 1883;

const int PINONE = D2;
const int PINTWO = D3;

 String weightstring;


HX711 scale;

 
WiFiClient espClient;
PubSubClient client(espClient);
 
void setup() {
 
  Serial.begin(115200);
  scale.begin(PINONE, PINTWO);
  scale.set_scale(125);// <- set here calibration factor!!!
  scale.tare();
  Serial.print("it has got this far");
 
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  //connecting to a mqtt broker
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  while (!client.connected()) {
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    Serial.println("Connecting to public emqx mqtt broker.....");
    if (client.connect(clientId.c_str())) {
      Serial.println("Public emqx mqtt broker connected");
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
  float weight = scale.get_units(1);
  client.subscribe(topic);
 
}
 
void callback(char* topic, byte* payload, unsigned int length) {
 
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
 
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
 
  Serial.println();
  Serial.println("-----------------------");
 
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
       client.subscribe(topic);
      client.subscribe(topic2);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

 
void loop() {

   if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
   float weight = scale.get_units(1);
  Serial.println(String(weight, 2));
  delay(2000);
  weightstring = String(weight, 2);
  client.publish(topic, weightstring.c_str());
  Serial.println(weightstring);
}
