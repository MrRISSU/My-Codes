/* ****************************************************************************
 * Company   : Consyst Digital Industries Pvt Ltd
 * Author    : Huwairis Ibnu Kabeer
 * email     : huwairisik@consyst.biz
 * Date      : 07/11/2024
 * File name : ethernet.ino
 * ****************************************************************************/

/**********************************MACROS**********************************/

// ETHERNET SPI PINS
#define ETHERNET_PIN_SCK   36
#define ETHERNET_PIN_MISO  37
#define ETHERNET_PIN_MOSI  35
#define ETHERNET_PIN_CS    38
// 
#define IS_ETH_STATIC      0

/**********************************GLOBAL**********************************/

// Enter a MAC address and IP address for your controller below.
static byte emac[6];
#if IS_ETH_STATIC == 1
// The IP address will be dependent on your local network:
// ping the ip in command prompt. Then only use only if Destination host unreachable.
static IPAddress staticIP(192, 168, 1, 253);
#endif

/*********************************FUNCTION*********************************/

static void getEthernetMac(byte *eMacAddr)
{
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_ETH);
   
    for (int i = 0; i < 6; i++) 
    {
        eMacAddr[i] = (byte)mac[i];
    }    
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**
#if IS_ETH_STATIC == 1
static void ethBeginStatic()
{
    Ethernet.begin(emac, staticIP);
    // Check for Ethernet hardware present
    bool status = true;
    if (Ethernet.hardwareStatus() == EthernetNoHardware) 
    {
        Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware.");
        status = false;
    }
    else if (Ethernet.linkStatus() == LinkOFF) 
    {
        Serial.println("Ethernet cable is not connected.");
        status = false;
    }
    // no point in carrying on, so do nothing forevermore:
    while (status == false) 
    {
        Serial.println("Check Hardware and Retart");
        delay(3000);
    }
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**

#elif IS_ETH_STATIC == 0
static void ethBeginDHCP()
{
    if (Ethernet.begin(emac) == 0)
    {
        Serial.println("Failed to configure Ethernet using DHCP");
        // Check for Ethernet hardware present
        if (Ethernet.hardwareStatus() == EthernetNoHardware) 
        {
            Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware.");
        }
        else if (Ethernet.linkStatus() == LinkOFF) 
        {
            Serial.println("Ethernet cable is not connected.");
        }
        // no point in carrying on, so do nothing forevermore:
        while (true) 
        {
            Serial.println("Check Hardware and Retart");
            delay(3000);
        }
    }
}
#endif

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**

void ethernetBegin()
{
    // Begin SPI
    SPI.begin(ETHERNET_PIN_SCK, ETHERNET_PIN_MISO, ETHERNET_PIN_MOSI, ETHERNET_PIN_CS);
    // init() Not required in EDGEBOX
    Ethernet.init(ETHERNET_PIN_CS);
    // Get Mac address from chip
    getEthernetMac(emac);
    Serial.printf_P("Ethernet Mac Addr : %02X:%02X:%02X:%02X:%02X:%02X\n", emac[0],emac[1],emac[2],emac[3],emac[4],emac[5]);
#if IS_ETH_STATIC == 1
    ethBeginStatic();
#elif IS_ETH_STATIC == 0
    ethBeginDHCP();
#endif
    Serial.println("Ethernet Successfully Initialized");
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**

void printEthInfo()
{
    //Print Network info
    Serial.print("Local IP          : ");
    Serial.println(Ethernet.localIP());
    Serial.print("Subnet Mask       : ");
    Serial.println(Ethernet.subnetMask());
    Serial.print("Gateway IP        : ");
    Serial.println(Ethernet.gatewayIP()); 
    Serial.print("DNS Server        : ");
    Serial.println(Ethernet.dnsServerIP());
}
