#include <Wire.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <Adafruit_MLX90640.h>
#include <ArduinoOTA.h>

Adafruit_MLX90640 mlx;
float frame[32*24]; // buffer for full frame of temperatures

const char* ssid = "xxx";
const char* password = "xxx";

WiFiClient wclient;
PubSubClient client(wclient);

int regions[24];
float tempDiff = 5;
int highestRegion = 0;
int highestRegionOld = 0;



void setup() {
  Serial.begin(9600);
  
  WiFi.mode(WIFI_STA);
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
  
  if (MDNS.begin("thermalcontrol")) {
  Serial.println("MDNS responder started");
  }

  client.setServer("192.168.1.2", 1883);
  

  Serial.println("Adafruit MLX90640 Simple Test");
  if (! mlx.begin(MLX90640_I2CADDR_DEFAULT, &Wire)) {
    Serial.println("MLX90640 not found!");
    while (1) delay(10);
  }
  Serial.println("Found Adafruit MLX90640");

  Serial.print("Serial number: ");
  Serial.print(mlx.serialNumber[0], HEX);
  Serial.print(mlx.serialNumber[1], HEX);
  Serial.println(mlx.serialNumber[2], HEX);
  
  mlx.setMode(MLX90640_CHESS);
  mlx.setResolution(MLX90640_ADC_18BIT);
  mlx.setRefreshRate(MLX90640_8_HZ);
  Wire.setClock(1000000); // max 1 MHz


  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  
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



void loop() {

    if (!client.connected()) {
      reconnect();
    }
    client.loop();

    ArduinoOTA.handle();
    
    if (mlx.getFrame(frame) != 0) {
      Serial.println("Failed");
      return;
    }
    float average=0;

    for(int i=0; i<=768; i++){
      average = average + frame[i];
    }

    average=average/768;
    
    Serial.println(average);

    for(int i=0; i<=768; i++){
      if (frame[i]-average > tempDiff) {
        highestRegion=i%32;
      }
    }
    
    if (highestRegion != highestRegionOld) {
      Serial.println(highestRegion);
      char cstr[16];
      itoa((highestRegion*6)+50, cstr, 10);
      client.publish("videowindow", cstr);
    }


    highestRegionOld=highestRegion;

}
