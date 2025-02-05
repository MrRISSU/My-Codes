/* ****************************************************************************
 * Company   : Consyst Digital Industries Pvt Ltd
 * Author    : Huwairis Ibnu Kabeer
 * email     : huwairisik@consyst.biz
 * Date      : 10/01/2025
 * File name : Timestamp.ino
 * ****************************************************************************/

/*******************************HEADER FILES*******************************/

#include "certificates.h"

/**********************************MACROS**********************************/

#define HTTPS_SERVER        "loot5pcssa.execute-api.ap-south-1.amazonaws.com"
#define HTTPS_PATH          "/v1/epochtime"
#define HTTPS_PORT          443
#define DOC_BUF_SIZE        100
#define HTTP_REQ_TIMOUT_MS  10000L

/**********************************GLOBAL**********************************/

const int rand_pin = 1;

// Base Client Ethernet
EthernetClient EthClientTS;
// Secure Ethernet Client
SSLClient SecureClientEth(EthClientTS, TAs, (size_t)TAs_NUM, rand_pin);

uint32_t timeStamp;

/*********************************FUNCTION*********************************/

bool BeginApiServerTS()
{
    return SyncApiTime();
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**

String GetJsonTS()
{
    String response;
    // configure time
//    configTime(3 * 3600, 0, "pool.ntp.org");

    debugf("Starting connection to server...");
    if (!SecureClientEth.connect(HTTPS_SERVER, HTTPS_PORT))
    {
        debugf("Connection failed!\r\n");
        return "\0";
    }
    else
    {
        debugf("Connected to server!\r\n");
        // Make a HTTP request:
        SecureClientEth.println("GET " + String(HTTPS_PATH) + " HTTP/1.0");
        SecureClientEth.println("Host: " + String(HTTPS_SERVER));
        SecureClientEth.println("Connection: close");
        SecureClientEth.println();

        // if there are incoming bytes available
        // from the server, read them and print them:
        uint32_t start = millis();
        while (SecureClientEth.connected() && ((millis() - start) < HTTP_REQ_TIMOUT_MS))
        {
            // Skip the Header from the response
            String line = SecureClientEth.readStringUntil('\n');
            // A CRLF line separates the header from the body in the incoming response
            // If the first character of the line is a carriage return ('\r')
            // It indicates the end of the header
            if (line == "\r")
            {
                debugf("headers received\r\n");
                break;
            }
        }
        // Read the remaining response body
        start = millis();
        while (SecureClientEth.available() && ((millis() - start) < HTTP_REQ_TIMOUT_MS))
        {
            response += (char)SecureClientEth.read();
        }

        response.trim();
        debugf("Response: %s\r\n", response.c_str());
        SecureClientEth.stop();
    }
    return response;
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**

time_t ExtractTime(String jsonResponse)
{
    time_t timestamp = 0;
    DynamicJsonDocument doc(DOC_BUF_SIZE);
    deserializeJson(doc, jsonResponse);
    if(doc.containsKey("unix_time") && doc["unix_time"].is<int>())
    {
        timestamp = doc["unix_time"];
    }
    return timestamp;
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**

bool SyncApiTime()
{
    timeStamp = ExtractTime(GetJsonTS());
    if(timeStamp == 0)
    {
        return false;
    }
    return true;
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**

void PrintTime()
{
    debugf("\r\n--------------------\r\n");
    debugf("TimeStamp = %d\r\n", timeStamp);
    debugf("--------------------\r\n\r\n");
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**
