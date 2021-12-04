#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <PubSubClient.h>

const char* ssid = "xxx";
const char* password = "xxx";

WiFiClient wclient;
PubSubClient client(wclient);
int statusArray[]={1,1,1,1,1};
int blinkOn=0;
int blinky=0;


void setup() {
  Serial.begin(9600);
  
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
  
  if (MDNS.begin("worldmap")) {
  Serial.println("MDNS responder started");
  }

  client.setServer("192.168.1.2", 1883);
  client.setCallback(callback);

  pinMode(D0, INPUT);
  pinMode(D1, INPUT);
  pinMode(D2, INPUT);
  pinMode(D3, INPUT);
  pinMode(D4, INPUT);
  pinMode(D5, INPUT);
  pinMode(D6, OUTPUT);

  digitalWrite(D6, HIGH);
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // ... and resubscribe
      client.subscribe("videowindow");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char* topic, byte* data, unsigned int length) {
    String dataString = "";
  for (int i = 0; i < length; i++) {
    dataString=dataString + ((char)data[i]);
  }
  Serial.println(dataString);
  if (dataString == "OK") {
    blinkOn=0;
    digitalWrite(D6, HIGH);
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  delay(500);

  if (digitalRead(D1) != statusArray[0]) {
    if (digitalRead(D1) == 0) {
      client.publish("videowindow", "LOC=LAS");
      blinkOn=1;
      statusArray[0]=0;
    } else {
      client.publish("videowindow", "LOC=XXX");
      statusArray[0]=1;      
    }
  }

  if (digitalRead(D2) != statusArray[1]) {
    if (digitalRead(D2) == 0) {
      client.publish("videowindow", "LOC=FLO");
      blinkOn=1;
      statusArray[1]=0;
    } else {
      client.publish("videowindow", "LOC=XXX");
      statusArray[1]=1;      
    }
  }

if (digitalRead(D3) != statusArray[2]) {
    if (digitalRead(D3) == 0) {
      client.publish("videowindow", "LOC=AMS");
      blinkOn=1;
      statusArray[2]=0;
    } else {
      client.publish("videowindow", "LOC=XXX");
      statusArray[2]=1;      
    }
  }

if (digitalRead(D4) != statusArray[3]) {
    if (digitalRead(D4) == 0) {
      client.publish("videowindow", "LOC=VER");
      blinkOn=1;
      statusArray[3]=0;
    } else {
      client.publish("videowindow", "LOC=XXX");
      statusArray[3]=1;      
    }
  }


  if (digitalRead(D5) != statusArray[4]) {
    if (digitalRead(D5) == 0) {
      client.publish("videowindow", "LOC=JPN");
      blinkOn=1;
      statusArray[4]=0;
    } else {
      client.publish("videowindow", "LOC=XXX");
      statusArray[4]=1;      
    }
  }
  

  if (blinkOn == 1) {
    if (blinky == 0) {
      digitalWrite(D6, HIGH);
      blinky=1;
    } else {
      digitalWrite(D6, LOW);
      blinky=0;
    }
  }

}
