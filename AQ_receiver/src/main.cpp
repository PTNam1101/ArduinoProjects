#include <Arduino.h>
#include <HTTPClient.h>
#include <LiquidCrystal_I2C.h>
#include <esp_now.h>
#include <WiFi.h>
#include <string.h>
// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;
#define led_w 25
#define led_b 27
#define led_g 33
#define led_r1 12
#define led_r2 13
const char *ssid = "Butter";          // your network SSID (name)
const char *password = "Tu1den8!"; // your network password
WiFiClient client;


// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

// Structure example to receive data
// Must match the sender structure
typedef struct struct_message
{
  int id;
  int AQ;
  float TP;
  float HU;
} struct_message;

// Create a struct_message called myData
struct_message myData;
String AirQ(int AQ)
{
  String ans = "---";
  if (AQ == 0){
    digitalWrite(led_g, LOW);
    digitalWrite(led_b, LOW);
    digitalWrite(led_w, LOW);
    digitalWrite(led_r1, LOW);
    digitalWrite(led_r2, LOW);
    ans = "C";
  }
  else if (0 < AQ && AQ <= 50)
  {
    digitalWrite(led_g, HIGH);
    digitalWrite(led_b, LOW);
    digitalWrite(led_w, LOW);
    digitalWrite(led_r1, LOW);
    digitalWrite(led_r2, LOW);
    ans = "G";
  }
  else if (51 <= AQ && AQ <= 100)
  {
    ans = "N";
    digitalWrite(led_g, LOW);
    digitalWrite(led_w, HIGH);
    digitalWrite(led_b, LOW);
    digitalWrite(led_r1, LOW);
    digitalWrite(led_r2, LOW);
  }
  else if (101 <= AQ && AQ <= 150)
  {
    ans = "P";
    digitalWrite(led_g, LOW);
    digitalWrite(led_w, LOW);
    digitalWrite(led_b, HIGH);
    digitalWrite(led_r1, LOW);
    digitalWrite(led_r2, LOW);
  }
  else if (151 <= AQ && AQ <= 200)
  {
    ans = "B";
    digitalWrite(led_g, LOW);
    digitalWrite(led_b, HIGH);
    digitalWrite(led_w, HIGH);
    digitalWrite(led_r1, LOW);
    digitalWrite(led_r2, LOW);
  }
  else if (201 <= AQ && AQ <= 300)
  {
    ans = "V";
    digitalWrite(led_g, LOW);
    digitalWrite(led_b, LOW);
    digitalWrite(led_w, LOW);
    digitalWrite(led_r1, HIGH);
    digitalWrite(led_r2, LOW);
  }
  else if (301 <= AQ)
  {
    ans = "D";
    digitalWrite(led_g, LOW);
    digitalWrite(led_b, LOW);
    digitalWrite(led_w, LOW);
    digitalWrite(led_r1, HIGH);
    digitalWrite(led_r2, HIGH);
  }
  return ans;
}
// callback function that will be executed when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("ID: ");
  Serial.println(myData.id);
  Serial.print("AQ: ");
  Serial.println(myData.AQ);
  Serial.print("TP: ");
  Serial.println(myData.TP);
  Serial.print("HU: ");
  Serial.println(myData.HU);
  if(myData.AQ > 2000) myData.AQ = 2000;
  String AQ = AirQ(myData.AQ);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Area " + String(myData.id) + " " + String(myData.AQ) + " " + AQ);
  lcd.setCursor(0, 1);
  lcd.print( String(myData.TP) + "C" + " " + String(myData.HU) + "%");
}
// void setup_wifi() {
//   Serial.print("Connecting to ");
//   Serial.println(ssid);
//   WiFi.begin(ssid, password);
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }
//   Serial.println("");
//   Serial.println("WiFi connected");
//   Serial.println("IP address: ");
//   Serial.println(WiFi.localIP());
// }

void setup()
{
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // initialize LCD
  lcd.init();
  // turn on LCD backlight
  lcd.backlight();
  //connect to network
  // setup_wifi();

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);
  pinMode(led_g, OUTPUT);
  pinMode(led_w, OUTPUT);
  pinMode(led_b, OUTPUT);
  pinMode(led_r1, OUTPUT);
  pinMode(led_r2, OUTPUT);
}

void loop()
{
  
}
