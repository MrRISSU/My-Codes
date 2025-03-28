
/********************************PRE MACROS********************************/

#define TINY_GSM_MODEM_SIM7070

/*******************************HEADER FILES*******************************/

#include <TinyGsmClient.h>
#include <SSLClientESP32.h>
#include <SPI.h>
#include <ArduinoJson.h>

/**********************************MACROS**********************************/

#define Println(x)      Serial.println(x)
#define Println_f(x,y)  Serial.println(x, y)
#define Print(x)        Serial.print(x)
#define debugf(...)  Serial.printf(__VA_ARGS__)
// Set serial for AT commands (to the module)
#define SerialAT     Serial1
//Num of Elements
#define NE(x)        (sizeof(x)/sizeof(x[0]))
#define HTTPS_SERVER "loot5pcssa.execute-api.ap-south-1.amazonaws.com"
#define HTTPS_PORT   443

//Network Modes
#define MODE_GSM     13
#define MODE_LTE_GSM 51
#define MODE_LTE     38
#define MODE_AUTO     2
#define GSM_MODE_NONE 0

// GSM APNs
#define APN_THINGSMOBILE  "TM"
#define APN_BSNL          "bsnlnet"
#define APN_AIRTEL        "airtelgprs.com"
#define GSM_APN           APN_AIRTEL

#define WAIT    1
#define NO_WAIT 0

/**********************************GLOBAL**********************************/

// Create GSM client object
TinyGsm gsm_modem(SerialAT);
TinyGsmClient httpClient(gsm_modem, 0);
SSLClientESP32 httpsClient(&httpClient);

DynamicJsonDocument doc_out(1024);

const char *openCellIdCaCert = 
"-----BEGIN CERTIFICATE-----\n"
"MIICnzCCAiWgAwIBAgIQf/MZd5csIkp2FV0TttaF4zAKBggqhkjOPQQDAzBHMQsw\n"
"CQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2VzIExMQzEU\n"
"MBIGA1UEAxMLR1RTIFJvb3QgUjQwHhcNMjMxMjEzMDkwMDAwWhcNMjkwMjIwMTQw\n"
"MDAwWjA7MQswCQYDVQQGEwJVUzEeMBwGA1UEChMVR29vZ2xlIFRydXN0IFNlcnZp\n"
"Y2VzMQwwCgYDVQQDEwNXRTEwWTATBgcqhkjOPQIBBggqhkjOPQMBBwNCAARvzTr+\n"
"Z1dHTCEDhUDCR127WEcPQMFcF4XGGTfn1XzthkubgdnXGhOlCgP4mMTG6J7/EFmP\n"
"LCaY9eYmJbsPAvpWo4H+MIH7MA4GA1UdDwEB/wQEAwIBhjAdBgNVHSUEFjAUBggr\n"
"BgEFBQcDAQYIKwYBBQUHAwIwEgYDVR0TAQH/BAgwBgEB/wIBADAdBgNVHQ4EFgQU\n"
"kHeSNWfE/6jMqeZ72YB5e8yT+TgwHwYDVR0jBBgwFoAUgEzW63T/STaj1dj8tT7F\n"
"avCUHYwwNAYIKwYBBQUHAQEEKDAmMCQGCCsGAQUFBzAChhhodHRwOi8vaS5wa2ku\n"
"Z29vZy9yNC5jcnQwKwYDVR0fBCQwIjAgoB6gHIYaaHR0cDovL2MucGtpLmdvb2cv\n"
"ci9yNC5jcmwwEwYDVR0gBAwwCjAIBgZngQwBAgEwCgYIKoZIzj0EAwMDaAAwZQIx\n"
"AOcCq1HW90OVznX+0RGU1cxAQXomvtgM8zItPZCuFQ8jSBJSjz5keROv9aYsAm5V\n"
"sQIwJonMaAFi54mrfhfoFNZEfuNMSQ6/bIBiNLiyoX46FohQvKeIoJ99cx7sUkFN\n"
"7uJW\n"
"-----END CERTIFICATE-----\n";

int hardwareRev;

/*********************************FUNCTION*********************************/

void connect_wan()       
{
    GprsBegin(GSM_APN); 
}

void hdlWanDisconn()
{
    gsmReconnect();
    debugf("GSM MODEM COMM STATUS = %d\r\n", isGsmOk()); 
    WaitForGPRS();   
}

void setup()
{
    Serial.begin(115200);
    debugf("\r\n");
    hardwareRev = FindHardwareRev();
    debugf("HW Rev : %d\r\n", hardwareRev);
    prepare_GSM(GSM_MODE_NONE);
    connect_wan();
    httpsClient.setCACert(openCellIdCaCert);
}

void loop()
{
    if(!isNetworkReady(NO_WAIT))
    {
        hdlWanDisconn();
    }
    debugf("----------------------------------------------\r\n");
    if (PrepGsmLocInfo())
    {
        serializeJsonPretty(doc_out, Serial);
        Serial.println();
    }
    delay(5000); // Check every 5 seconds
}
