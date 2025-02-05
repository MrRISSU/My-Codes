/* ****************************************************************************
 * Company   : Consyst Digital Industries Pvt Ltd
 * Author    : Huwairis Ibnu Kabeer
 * email     : huwairisik@consyst.biz
 * Date      : 10/01/2025
 * File name : main.ino
 * ****************************************************************************/

/*******************************HEADER FILES*******************************/

#include <SPI.h>
#include <Ethernet.h>
#include <SSLClient.h>
#include <ArduinoJson.h>

/**********************************MACROS**********************************/

#define debugf(...)     Serial.printf(__VA_ARGS__)

/*********************************FUNCTION*********************************/

void setup()
{
    Serial.begin(115200);
    // Prepare the ethernet chip
    prepare_Ethernet();
    // Connect
    EthernetBegin();
    WaitForEthernet();
    
    BeginApiServerTS();
}

void loop()
{
    if(!isEthernetConnected())
    {
        showEthStat();
        return;
    }
    if(!SyncApiTime())
    {
        return;
    }
    PrintTime();
}
