#include <WiFi.h>  
#include <WiFiClient.h>  
#include <WebServer.h>  
#include <ESPmDNS.h>  

#define debugf(...)     Serial.printf(__VA_ARGS__)

char WIFI_SSID[32] = "digitalwifi2";
char WIFI_PASS[32] = "123456789";


char** ssid_list;
uint32_t scancount;


void setup()
{
    Serial.begin(115200);
    WifiScanNetworks();
    BeginWebServer();
}

void loop()
{
    HandleWeb();
}
