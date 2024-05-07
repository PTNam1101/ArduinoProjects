#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <esp_now.h>
#include <WiFi.h>
#include <MQ135.h>
#include "ThingSpeak.h"

#define DHTPIN 4      // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11 // DHT 11
#define PIN_MQ135 35
const char *ssid = "Butter";       // your network SSID (name)
const char *password = "Tu1den8!"; // your network password

WiFiClient client;

// unsigned long myChannelNumber = 2240463;
// const char *myWriteAPIKey = "XVYZXA42VRG5IL9J";
// put function declarations here:
MQ135 mq135_sensor(PIN_MQ135);
DHT dht(DHTPIN, DHTTYPE);

uint8_t broadcastAddress[] = {0x58, 0xBF, 0x25, 0x33, 0x41, 0xA0};

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message
{
  int id;
  int AQ;
  float TP;
  float HU;
} struct_message;

// Create a struct_message called myData
struct_message myData;

esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup()
{
  Serial.begin(115200);
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return;
  }
}

void loop()
{
  float temp = dht.readTemperature();
  float humid = dht.readHumidity();
  if (isnan(humid) || isnan(temp) || temp < 0 || humid < 0)
  {
    Serial.println(F("Failed to read from DHT sensor!"));
    delay(200);
    return;
  }
  float rzero = mq135_sensor.getRZero();
  float correctedRZero = mq135_sensor.getCorrectedRZero(temp, humid);
  float resistance = mq135_sensor.getResistance();
  float ppm = mq135_sensor.getPPM();
  float correctedPPM = mq135_sensor.getCorrectedPPM(temp, humid);
  Serial.print("MQ135 RZero: ");
  Serial.print(rzero);
  Serial.print("\t Corrected RZero: ");
  Serial.print(correctedRZero);
  Serial.print("\t Resistance: ");
  Serial.print(resistance);
  Serial.print("\t PPM: ");
  Serial.print(ppm);
  Serial.print("ppm");
  Serial.print("\t Corrected PPM: ");
  Serial.print(correctedPPM);
  Serial.println("ppm");
  Serial.println("Temp: " + String(temp) + " C");
  Serial.println("Humid: " + String(humid) + " %");

  myData.id = 1;
  myData.AQ = int(correctedPPM);
  myData.TP = temp;
  myData.HU = humid;
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&myData, sizeof(myData));
  if (result == ESP_OK)
  {
    Serial.println("Sent with success");
  }
  else
  {
    Serial.println("Error sending the data");
  }

  delay(1000); // put your main code here, to run repeatedly:
}

// put function definitions here:
