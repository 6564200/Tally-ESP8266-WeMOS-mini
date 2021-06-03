#include <ESP8266WiFi.h>
//#include <Pinger.h>
//Pinger pinger;
#include <WiFiUdp.h>
#include "Adafruit_GFX.h"
#include <WEMOS_Matrix_GFX.h>


int ERR = 0;

//static const uint8_t PROGMEM
//  A_bmp[] =  { B01111000,    B01111000,    B00011000,    B00011000,    B00011000,    B00011000,    B01111110,    B01111110 },
//  B_bmp[] =  { B11111111,    B11111111,    B00000011,    B11111111,    B11111111,    B11000000,    B11111111,    B11111111 },
//  C_bmp[] =  { B11111111,    B11111111,    B00000111,    B00111110,    B00111110,    B00000111,    B11111111,    B11111111 },
//  D_bmp[] =  { B11000011,    B11000011,    B11000011,    B11111111,    B11111111,    B00000011,    B00000011,    B00000011 },
//  E_bmp[] =  { B11111111,    B11111111,    B11000000,    B11111111,    B11111111,    B00000011,    B11111111,    B11111111 };

  
#define NUMBER 5 //--------- number Tally
static const int CAM_COD[] = {128, 64, 32, 16, 8, 4, 2, 1};  // Panasonic
//static const int CAM_COD[] = {62, 30, 46, 54, 58, 60}; 
#if NUMBER == 1
   static const uint8_t PROGMEM BMP[] = { B00000000,    B00000011,    B00000011,    B11111111,    B11111111,    B11000011,    B11000011,    B00000000 };
#elif NUMBER == 2
   static const uint8_t PROGMEM BMP[] = { B11111011,    B11111011,    B11011011,    B11011011,    B11011011,    B11011011,    B11011111,    B11011111 };
#elif NUMBER == 3
   static const uint8_t PROGMEM BMP[] = { B11100111,    B11111111,    B11111111,    B11011011,    B11011011,    B11011011,    B11000011,    B11000011};
#elif NUMBER == 4
   static const uint8_t PROGMEM BMP[] = { B11111111,    B11111111,    B00011000,    B00011000,    B00011000,    B00011000,    B11111000,    B11111000 };
#elif NUMBER == 5
   static const uint8_t PROGMEM BMP[] = { B11011111,    B11011111,    B11011011,    B11011011,    B11011011,    B11011011,    B11111011,    B11111011 };
#elif NUMBER == 6
   static const uint8_t PROGMEM BMP[] = { B11111111,    B11111111,    B11111111,    B11011111,    B11011111,    B11011111,    B11111111,    B11111111 };
#elif NUMBER == 7
   static const uint8_t PROGMEM BMP[] = { B11111111,    B11111111,    B11111111,    B11011111,    B11011111,    B11011111,    B11111111,    B11111111 };
#elif NUMBER == 8
   static const uint8_t PROGMEM BMP[] = { B11111111,    B11111111,    B11111111,    B11011111,    B11011111,    B11011111,    B11111111,    B11111111 };
#endif



MLED matrix(7); //set intensity

//  extern "C" {
//    #include <cstdint>
//    #include "user_interface.h"
//    const  uint8_t mac[] = {0xFC, 0x95, 0xF2, 0x4E, 0x98, 0x0D};
//    }
//  bool changeMac(const uint8_t mac[6]) {
//    return wifi_set_macaddr(STATION_IF, const_cast<uint8*>(mac));
//    }

const char* ssid = "TALLY";
const char* password = "Channel4";
//#define test_pin 2
const IPAddress remote_ip(192, 168, 50, 10);
int LIP;
//IPAddress ip(169,168,50,103);  //Node static IP
//IPAddress gateway(192,168,50,10);
//IPAddress subnet(255,255,255,0);

WiFiUDP Udp;
unsigned int localUdpPort = 8888;  // local port to listen on
char incomingPacket[255];  // buffer for incoming packets
char  replyPacket[] = "ok";  // a reply string to send back

const int packetSize = 1;
byte packetBuffer[packetSize];

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
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
  WiFi.begin(ssid, password);
  //WiFi.config(ip, gateway, subnet);
  //pinMode(2, OUTPUT);
  Serial.println(WiFi.macAddress());
  int x, y;
  float t = 0;
  bool A = LED_ON;
  while (WiFi.status() != WL_CONNECTED)
  { //digitalWrite(2, HIGH);   
    //Serial.println("HIGH");
    delay(300);                      
    //digitalWrite(2, LOW);  
    //Serial.println("LOW");
    delay(300);
    t += 0.2;
    if (t > 6.3) { A = !A; t = 0;}
    x = round(3.4 + 5 * cos(t));
    y = round(3.5 + 5 * sin(t));
    if (x < 0) {x=0;} else if (x>7) {x=7;}
    if (y < 0) {y=0;} else if (y>7) {y=7;}
    matrix.drawPixel(x, y, A);
    matrix.writeDisplay();
  }
  
  delay(1000);
  IPAddress ipLIP = WiFi.localIP();
  LIP = ipLIP[3];
  
  Udp.begin(localUdpPort+LIP);
  Serial.printf("Now listening at IP %s, UDP port %d  - %d   \n", WiFi.localIP().toString().c_str(), LIP, localUdpPort+LIP);
  pinMode(13, OUTPUT);
}






void reconnectWifi() {
  WiFi.begin(ssid, password);
  Serial.printf("RECONECT");

}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//bool STAT = false;
void loop()
{ 
  //bool A = LED_ON;
  matrix.clear();
  int packetSize = Udp.parsePacket();
  //Serial.print("IP "); 
  //Serial.println(String((WiFi.localIP())[3]));
  //Serial.println(WiFi.localIP().toString().c_str());
  //bool ret = pinger.Ping(remote_ip);
  if (packetSize)
  {
    Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    
    byte myData = packetBuffer[0];

      //Serial.println(myData);
      myData = ~myData & CAM_COD[NUMBER-1]; //Panas
      Serial.println(myData);
//    if (myData == CAM_COD[NUMBER]) {
      if (myData > 0) { 
         
         matrix.drawBitmap(0, 0, BMP, 8, 8, LED_ON);
         matrix.writeDisplay();
         //STAT = !STAT;
         digitalWrite(13, LOW);
         //digitalWrite(2, HIGH);
      }
    else {
      matrix.clear();
      matrix.writeDisplay();
      //STAT = !STAT;
      digitalWrite(13, HIGH);
      //digitalWrite(2, LOW);
    }
    
    ERR = 0;

    if (WiFi.status() != WL_CONNECTED){
      
      matrix.drawPixel(0, 0, LED_ON);
      matrix.writeDisplay();
      WiFi.mode(WIFI_STA);
      WiFi.SSID();
      WiFi.setOutputPower(0);
      WiFi.begin(ssid, password);
      delay(300);
      }
  }
  else
  {
    ERR += 1;
    if (ERR > 2000) {
      ESP.restart();
    
    }
  }
  delay(30);
}
