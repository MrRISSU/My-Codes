/*
 * ---------------------------------------------------------
 *                     Modbus TCP Data Read
 *                           with GSM
 * ---------------------------------------------------------
 * Author: Huwairis Ibnu Kabeer
 * Date:   07/11/2024
 *
 * Overview:
 * This code facilitates reading data from both Holding
 * Registers (HRegs) and Input Registers (IRegs) of a
 * Modbus TCP slave device. Along with Network connectivity
 * using GSM Network
 *
 * Key Features:
 * - Supports reading from Holding and Input Registers.
 * - Offers flexible network configurations: Static IP
 *   and DHCP.
 * - Controlled by the `IS_ETH_STATIC` macro for seamless
 *   switching between Static IP and DHCP modes.
 *
 * How to Use:
 * - To read data from registers, pass the specific macro
 *   (REGTYPE_INPUT or REGTYPE_HOLDING) to the `ModbusRead()`
 *   function.
 */
/*
 * ---------------------------------------------------------
 *                      Libraries used
 * ---------------------------------------------------------
 * 
 * Library: TinyGSM
 * Version: 0.11.5
 * Link   : https://github.com/vshymanskyy/TinyGSM
 * 
 * --------------------------------------------------------- 
 * 
 * Library: SSLClientESP32
 * Version: 2.0.0
 * Link   : https://github.com/alkonosst/SSLClientESP32
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

#include <TinyGsmClient.h>
#include <SSLClientESP32.h>
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
#define HTTPS_SERVER    "loot5pcssa.execute-api.ap-south-1.amazonaws.com"
#define HTTPS_PORT      443

//Network Modes
#define MODE_GSM        13
#define MODE_LTE_GSM    51
#define MODE_LTE        38
#define MODE_AUTO       2
#define GSM_MODE_NONE   0

#define WAIT            1
#define NO_WAIT         0

// GSM APNs
#define APN_THINGSMOBILE  "TM"
#define APN_BSNL          "bsnlnet"
#define GSM_DEFAULT_APN   "airtelgprs.com"

/**********************************GLOBAL**********************************/

int hardwareRev;

/*********************************FUNCTION*********************************/

void setup()
{
    Serial.begin(115200);
    debugf("\r\n");
    hardwareRev = FindHardwareRev();
    debugf("HW Rev : %d\r\n", hardwareRev);
    // Initialize ethernet for TCP communication
    ethernetBegin();
    printEthInfo();
    delay(1000);
    // Set modbus TCP as client
    ModbusTcpClientInit();
    // Initialize GSM for Network connectivity
    prepare_GSM(GSM_MODE_NONE);
    // Connect GPRS
    connect_wan();
    // Initialize MQTT connection for sending data
    BeginMqtt();
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**

void loop()
{
    if(!isNetworkReady(NO_WAIT))
    {
        // Handle WAN disconnection
        hdlWanDisconn();
    }
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
    if(isNetworkReady(NO_WAIT) && IsMqttConnected())
    {
        PublishModbusData();
    }
    delay(5000);
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**
