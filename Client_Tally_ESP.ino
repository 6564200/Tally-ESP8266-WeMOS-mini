#include <ESP8266WiFi.h>
//#include <Pinger.h>
//Pinger pinger;
#include <WiFiUdp.h>
#include "Adafruit_GFX.h"
#include <WEMOS_Matrix_GFX.h>




//static const uint8_t PROGMEM
//  A_bmp[] =  { B01111000,    B01111000,    B00011000,    B00011000,    B00011000,    B00011000,    B01111110,    B01111110 },
//  B_bmp[] =  { B11111111,    B11111111,    B00000011,    B11111111,    B11111111,    B11000000,    B11111111,    B11111111 },
//  C_bmp[] =  { B11111111,    B11111111,    B00000111,    B00111110,    B00111110,    B00000111,    B11111111,    B11111111 },
//  D_bmp[] =  { B11000011,    B11000011,    B11000011,    B11111111,    B11111111,    B00000011,    B00000011,    B00000011 },
//  E_bmp[] =  { B11111111,    B11111111,    B11000000,    B11111111,    B11111111,    B00000011,    B11111111,    B11111111 };

  
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
unsigned int localUdpPort = 4213;  // local port to listen on
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
  
  //wifi_set_macaddr(0, const_cast<uint8*>(mac));
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
  
  delay(1000);
  IPAddress ipLIP = WiFi.localIP();
  LIP = ipLIP[3];
  
  Udp.begin(localUdpPort+LIP);
  Serial.printf("Now listening at IP %s, UDP port %d  - %d   \n", WiFi.localIP().toString().c_str(), LIP, localUdpPort+LIP);
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
  
  int packetSize = Udp.parsePacket();
  //Serial.print("IP "); 
  //Serial.println(String((WiFi.localIP())[3]));
  //Serial.println(WiFi.localIP().toString().c_str());
  //bool ret = pinger.Ping(remote_ip);
  if (packetSize)
  {
    Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    
    byte myData = packetBuffer[0];

//    Serial.println(myData);
    if (myData == CAM_COD[NUMBER]) {
      
         matrix.clear();
         matrix.drawBitmap(0, 0, BMP, 8, 8, LED_ON);
         matrix.writeDisplay();
         //STAT = !STAT;
        
      }
    else {
      matrix.clear();
      matrix.writeDisplay();
      //STAT = !STAT;
      
    }


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
  delay(30);
}
