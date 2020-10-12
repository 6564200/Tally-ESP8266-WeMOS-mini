#include <ESP8266WiFi.h>
//#include <Pinger.h>
//#include <Wire.h>
#include <WiFiUdp.h>
#include "Adafruit_GFX.h"
#include <WEMOS_Matrix_GFX.h>

#define NUMBER 5 //--------- number Tally
static const int CAM_COD[] = {62, 30, 46, 54, 58, 60};
#if NUMBER == 1
   static const uint8_t PROGMEM BMP[] = { B01111000,    B01111000,    B00011000,    B00011000,    B00011000,    B00011000,    B01111110,    B01111110 };
#elif NUMBER == 2
   static const uint8_t PROGMEM BMP[] = { B11111111,    B11111111,    B00000011,    B11111111,    B11111111,    B11000000,    B11111111,    B11111111 };
#elif NUMBER == 3
   static const uint8_t PROGMEM BMP[] = { B11111111,    B11111111,    B00000111,    B00111110,    B00111110,    B00000111,    B11111111,    B11111111 };
#elif NUMBER == 4
   static const uint8_t PROGMEM BMP[] = { B11000011,    B11000011,    B11000011,    B11111111,    B11111111,    B00000011,    B00000011,    B00000011 };
#elif NUMBER == 5
   static const uint8_t PROGMEM BMP[] = { B11111111,    B11111111,    B11000000,    B11111111,    B11111111,    B00000011,    B11111111,    B11111111 };
#endif

MLED matrix(7);

const char *ssid = "TALLY";
const char *pass = "Channel4";

unsigned int localPort = 10000; // port listen
IPAddress SendIP(192,168,50,255); //UDP Broadcast IP 
WiFiUDP udp;

char packetBuffer[9]; //UDP data
float temp;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ SETUP
void setup()
{
  matrix.clear();
  matrix.writeDisplay();
  //pinMode(test_pin, OUTPUT);
  Serial.begin(115200);
  Serial.printf("Connecting to %s ", ssid);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  if (WiFi.getAutoConnect() != true) WiFi.setAutoConnect(true);
  WiFi.setAutoReconnect(true);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass); //Connect 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    }

// Wait for connection
  matrix.drawPixel(0, 0, LED_ON);
  matrix.writeDisplay();
  bool A = LED_ON;
  while (WiFi.status() != WL_CONNECTED)
  { delay(1000);
    A = !A;
    matrix.drawPixel(0, 0, A);
    matrix.writeDisplay();
  }
  matrix.drawPixel(7, 0, LED_ON);
  matrix.writeDisplay();
  

//Start UDP
  Serial.println("Starting UDP");
  udp.begin(localPort);
  Serial.printf("Now listening at IP %s, UDP port %d   \n", WiFi.localIP().toString().c_str(), localPort);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++
void reconnectWifi() {
  WiFi.begin(ssid, pass);
  Serial.printf("RECONECT");
}

//+++++++++++++++++++++++++++++++++++++++++++++++++ MAIN
void loop()
{
  int packetSize = udp.parsePacket();
  if (packetSize) {
     //memset(packetBuffer, 0, sizeof(packetBuffer));
     udp.read(packetBuffer, 1); // read the packet into the buffer, we are reading only one byte
     byte myData = packetBuffer[0];
     Serial.println(myData);

     if (1 == 2) {
        matrix.clear();
        matrix.drawBitmap(0, 0, BMP, 8, 8, LED_ON);
        matrix.writeDisplay();
        }
     else {
        matrix.clear();
        matrix.writeDisplay();
        }
     }

  if (WiFi.status() != WL_CONNECTED){
      matrix.drawPixel(0, 0, LED_ON);
      matrix.writeDisplay();
      WiFi.mode(WIFI_STA);
      WiFi.SSID();
      WiFi.setOutputPower(0);
      WiFi.begin(ssid, pass);
      delay(300);
      }
  delay(50);
}
