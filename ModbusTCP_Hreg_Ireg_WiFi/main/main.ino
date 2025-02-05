/*
 * ---------------------------------------------------------
 *                     Modbus TCP Data Read
 *                           with WiFi
 * ---------------------------------------------------------
 * Author: Huwairis Ibnu Kabeer
 * Date:   07/11/2024
 * MCU:    ESP32-S3-WROOM-1U
 *
 * Overview:
 * This code facilitates reading data from both Holding
 * Registers (HRegs) and Input Registers (IRegs) of a
 * Modbus TCP slave device. Along with Network connectivity
 * using WiFi Network
 *
 * Key Features:
 * - Supports reading from Holding and Input Registers.
 * - Offers flexible network configurations: Static IP
 *   and DHCP.
 * - Controlled by the `IS_ETH_STATIC` macro for seamless
 *   switching between Static IP and DHCP modes.
 * - Connects to a secured MQTT using WiFi Netwok for seamless
 *   data monitoring
 *
 * How to Use:
 * - To read data from registers, pass the specific macro
 *   (REGTYPE_INPUT or REGTYPE_HOLDING) to the 'ModbusRead()'
 *   function.
 */
/*
 * ---------------------------------------------------------
 *                      Libraries used
 * ---------------------------------------------------------
 * 
 * Library: WiFi
 * Version: Inbuilt Library
 * 
 * --------------------------------------------------------- 
 * 
 * Library: WiFiClientSecure
 * Version: Inbuilt Library
 * 
 * --------------------------------------------------------- 
 * 
 * Library: SPI
 * Version: Inbuilt Library
 * 
 * --------------------------------------------------------- 
 * 
 * Library: ArduinoJson
 * Version: 6.21.3
 * Link   : https://arduinojson.org/?utm_source=meta&utm_medium=library.properties
 * 
 * --------------------------------------------------------- 
 * 
 * Library: Ethernet
 * Version: 2.0.2
 * Link   : https://docs.arduino.cc/libraries/ethernet/#Usage/Examples
 * 
 * ---------------------------------------------------------
 * 
 * Library: modbus_esp8266 (Customized)
 * Version: 4.0.0
 * Link1  : https://github.com/emelianov/modbus-esp8266 (Real)
 * Link2  : https://dev.azure.com/iiotnext/iiotnext-edge/_git/iiotnext-edge?path=/Code/Libraries/modbus-esp8266 (customized)
 * 
 * ---------------------------------------------------------
 * 
 * Library: PubSubClient
 * Version: 2.8.0
 * Link   : https://pubsubclient.knolleary.net/
 * 
 * --------------------------------------------------------- 
 * 
 */
/********************************PRE MACROS********************************/

#define TINY_GSM_MODEM_SIM7070

/*******************************HEADER FILES*******************************/
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <SPI.h>
#include <ArduinoJson.h>
#include <Ethernet.h>
#include <ModbusEthernet.h>
#include <PubSubClient.h>

/**********************************MACROS**********************************/

#define Println(x)      Serial.println(x)
#define Println_f(x,y)  Serial.println(x, y)
#define Print(x)        Serial.print(x)
#define debugf(...)     Serial.printf(__VA_ARGS__)
// Set serial for AT commands (to the module)
#define SerialAT        Serial1
//Num of Elements
#define NE(x)           (sizeof(x)/sizeof(x[0]))

/**********************************GLOBAL**********************************/

/*********************************FUNCTION*********************************/

void setup()
{
    Serial.begin(115200);
    debugf("\r\n");
    // Initialize ethernet for TCP communication
    ethernetBegin();
    printEthInfo();
    delay(1000);
    // Set modbus TCP as client
    ModbusTcpClientInit();
    WifiScanNetworks();
    WifiConnect();
    // Initialize MQTT connection for sending data
    BeginMqtt();
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**

void loop()
{
    WifiReconnect();
    if(!IsMqttConnected())
    {
        // Handle MQTT disconnection
        MqttConnect();
    }
    if(ModbusConnect() == false)
    {
       // this will act like countinue in a loop
       return;
    }
    
    HandleMqtt();
    // Read All Modbus Tags
    Modbus2ReadAllTags();
    // If both Network and MQTT connection are good, Publish the data
    if(IsWifiConnected() && IsMqttConnected())
    {
        PublishModbusData();
    }
    delay(5000);
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**
