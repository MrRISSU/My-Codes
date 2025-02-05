
/*******************************HEADER FILES*******************************/

#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

/**********************************MACROS**********************************/

#define HTTPS_SERVER "loot5pcssa.execute-api.ap-south-1.amazonaws.com"
#define HTTPS_PORT 443

/**********************************GLOBAL**********************************/

WiFiClientSecure httpsClient;
// WiFi credentials
const char* ssid = "digitalwifi2";
const char* password = "123456789";

// "loot5pcssa.execute-api.ap-south-1.amazonaws.com" root certificate authority, to verify the server
const char* test_root_ca= \
  "-----BEGIN CERTIFICATE-----\n" \
  "MIIEXjCCA0agAwIBAgITB3MSSkvL1E7HtTvq8ZSELToPoTANBgkqhkiG9w0BAQsF\n" \
  "ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\n" \
  "b24gUm9vdCBDQSAxMB4XDTIyMDgyMzIyMjUzMFoXDTMwMDgyMzIyMjUzMFowPDEL\n" \
  "MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEcMBoGA1UEAxMTQW1hem9uIFJT\n" \
  "QSAyMDQ4IE0wMjCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALtDGMZa\n" \
  "qHneKei1by6+pUPPLljTB143Si6VpEWPc6mSkFhZb/6qrkZyoHlQLbDYnI2D7hD0\n" \
  "sdzEqfnuAjIsuXQLG3A8TvX6V3oFNBFVe8NlLJHvBseKY88saLwufxkZVwk74g4n\n" \
  "WlNMXzla9Y5F3wwRHwMVH443xGz6UtGSZSqQ94eFx5X7Tlqt8whi8qCaKdZ5rNak\n" \
  "+r9nUThOeClqFd4oXych//Rc7Y0eX1KNWHYSI1Nk31mYgiK3JvH063g+K9tHA63Z\n" \
  "eTgKgndlh+WI+zv7i44HepRZjA1FYwYZ9Vv/9UkC5Yz8/yU65fgjaE+wVHM4e/Yy\n" \
  "C2osrPWE7gJ+dXMCAwEAAaOCAVowggFWMBIGA1UdEwEB/wQIMAYBAf8CAQAwDgYD\n" \
  "VR0PAQH/BAQDAgGGMB0GA1UdJQQWMBQGCCsGAQUFBwMBBggrBgEFBQcDAjAdBgNV\n" \
  "HQ4EFgQUwDFSzVpQw4J8dHHOy+mc+XrrguIwHwYDVR0jBBgwFoAUhBjMhTTsvAyU\n" \
  "lC4IWZzHshBOCggwewYIKwYBBQUHAQEEbzBtMC8GCCsGAQUFBzABhiNodHRwOi8v\n" \
  "b2NzcC5yb290Y2ExLmFtYXpvbnRydXN0LmNvbTA6BggrBgEFBQcwAoYuaHR0cDov\n" \
  "L2NydC5yb290Y2ExLmFtYXpvbnRydXN0LmNvbS9yb290Y2ExLmNlcjA/BgNVHR8E\n" \
  "ODA2MDSgMqAwhi5odHRwOi8vY3JsLnJvb3RjYTEuYW1hem9udHJ1c3QuY29tL3Jv\n" \
  "b3RjYTEuY3JsMBMGA1UdIAQMMAowCAYGZ4EMAQIBMA0GCSqGSIb3DQEBCwUAA4IB\n" \
  "AQAtTi6Fs0Azfi+iwm7jrz+CSxHH+uHl7Law3MQSXVtR8RV53PtR6r/6gNpqlzdo\n" \
  "Zq4FKbADi1v9Bun8RY8D51uedRfjsbeodizeBB8nXmeyD33Ep7VATj4ozcd31YFV\n" \
  "fgRhvTSxNrrTlNpWkUk0m3BMPv8sg381HhA6uEYokE5q9uws/3YkKqRiEz3TsaWm\n" \
  "JqIRZhMbgAfp7O7FUwFIb7UIspogZSKxPIWJpxiPo3TcBambbVtQOcNRWz5qCQdD\n" \
  "slI2yayq0n2TXoHyNCLEH8rpsJRVILFsg0jc7BaFrMnF462+ajSehgj12IidNeRN\n" \
  "4zl+EoNaWdpnWndvSpAEkq2P\n" \
  "-----END CERTIFICATE-----\n";


/*********************************FUNCTION*********************************/

String GetResponse()
{
    String response;
    
    Serial.println("\nStarting connection to server...");
    if (!httpsClient.connect(HTTPS_SERVER, HTTPS_PORT))
    {
        Serial.println("Connection failed!");
        return "\0";
    }
    else
    {
        Serial.println("Connected to server!");
        // Make a HTTP request:
        httpsClient.println("GET https://loot5pcssa.execute-api.ap-south-1.amazonaws.com/v1/epochtime HTTP/1.0");
        httpsClient.println("Host: loot5pcssa.execute-api.ap-south-1.amazonaws.com");
        httpsClient.println("Connection: close");
        httpsClient.println();
    
        while (httpsClient.connected())
        {
            String line = httpsClient.readStringUntil('\n');
            if (line == "\r")
            {
                Serial.printf("headers received\r\n");
                break;
            }
        }
        // if there are incoming bytes available
        // from the server, read them and print them:
        while (httpsClient.available())
        {
            response += (char)httpsClient.read();
        }
        Serial.printf("Response: %s\r\n", response.c_str());
        httpsClient.stop();
    }
    return response;
}

uint64_t ExtractJsonTS(String jsonResponse)
{
    uint64_t timestamp = 0;
    
    DynamicJsonDocument doc(100);
    deserializeJson(doc, jsonResponse);
    if(doc.containsKey("unix_time") && doc["unix_time"].is<int>())
    {
        timestamp = doc["unix_time"];
    }
    return timestamp;
}

/********************************************************************************/

void setup()
{
    //Initialize serial and wait for port to open:
    Serial.begin(115200);
    delay(100);
  
    Serial.printf("Connecting to SSID: %s\r\n", ssid);
    WiFi.begin(ssid, password);
  
    // attempt to connect to Wifi network:
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        // wait 1 second for re-trying
        delay(1000);
    }
  
    Serial.printf("Connected to %s\r\n", ssid);
  
    httpsClient.setCACert(test_root_ca);
}

/********************************************************************************/

void loop()
{
    // do nothing
    uint64_t timestamp = ExtractJsonTS(GetResponse());
    Serial.printf("TS in int: %llu", timestamp);
    delay(1000);
}

/********************************************************************************/
