/* ****************************************************************************
 * Company   : Consyst Digital Industries Pvt Ltd
 * Author    : Huwairis Ibnu Kabeer
 * email     : huwairisik@consyst.biz
 * Date      : 10/01/2025
 * File name : Ethernet.ino
 * ****************************************************************************/

/**********************************MACROS**********************************/

#define ETHERNET_RETRY_COUNT  5
#define EXPANDIP(arr)         arr[0], arr[1], arr[2], arr[3]

#define WAIT_TIME_ETHERNET    (5 * 1000)

// Ethernet pin macros
#define ETHERNET_PIN_SCK   36
#define ETHERNET_PIN_MISO  37
#define ETHERNET_PIN_MOSI  35
#define ETHERNET_PIN_CS    38

// Ethernet Connection Status macros
#define CHIP_NOT_FOUND       0 
#define CABLE_NOT_CONNECTED  1
#define CONNECTION_STATUS_OK 3

#define STATIC          0
#define IPADDRESS_SIZE  4

/**********************************GLOBAL**********************************/

static byte eMac[6];

int ipaddress    [IPADDRESS_SIZE] = {0,0,0,0};
int gatewayStatic[IPADDRESS_SIZE] = {0,0,0,0};
int subnetStatic [IPADDRESS_SIZE] = {0,0,0,0};
int dnsStatic    [IPADDRESS_SIZE] = {0,0,0,0};

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

bool isEthernetHardwareOK()
{
    return (Ethernet.hardwareStatus() != EthernetNoHardware);
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**

bool isEthernetCableConnected()
{
    return (Ethernet.linkStatus() != LinkOFF);
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**

bool isEthernetConnected()
{
    return (isEthernetCableConnected() && (getEthernetIP() != IPAddress(0,0,0,0)) );
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**

IPAddress getEthernetIP()
{
    return Ethernet.localIP();
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**

IPAddress getEthernetSubnetMask()
{
    return Ethernet.subnetMask();
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**

IPAddress getEthernetGatewayIP()
{
    return Ethernet.gatewayIP();
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**

IPAddress getEthernetDnsIP()
{
    return Ethernet.dnsServerIP();
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**

void showEthStat()
{
    debugf("ETHERNET STATUS\r\n");
    debugf("ETH Hardware : ");
    switch(Ethernet.hardwareStatus())
    {
        case EthernetNoHardware : debugf("No Hardware\r\n"); break;
        case EthernetW5500      : debugf("W5500\r\n"); break;
        default                 : debugf("Unknown\r\n"); break;
    }
    
    debugf("ETH Cable    : ");
    switch(Ethernet.linkStatus())
    {
        case LinkOFF  : debugf("Not Connected\r\n"); break;
        case LinkON   : debugf("Connected\r\n"); break;
        case Unknown  : debugf("Unknown\r\n"); break;
        default       : debugf("Error\r\n"); break;
    }
    
    debugf("\r\n");
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**

void printInfo()
{
    //Print Network info
    debugf("IP    : %s\r\n", getEthernetIP().toString().c_str());
    debugf("Mask  : %s\r\n", getEthernetSubnetMask().toString().c_str());
    debugf("GW    : %s\r\n", getEthernetGatewayIP().toString().c_str()); 
    debugf("DNS   : %s\r\n", getEthernetDnsIP().toString().c_str());
    showEthStat(); 
    debugf("\r\n");
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**

bool WaitForEthernet()
{
    debugf("Obtaining IP Address ");
    unsigned long startTime = millis();
    while(getEthernetIP() == IPAddress(0,0,0,0))
    {
        delay(50);
        debugf(" .");
        if(millis() - startTime > WAIT_TIME_ETHERNET)
        {
            debugf("\r\n");
            debugf("DHCP Error : IP Not Assigned\r\n");
            return false;
        }
    }
    debugf(" . . .SUCCESS !\r\n");
    printInfo();
    return true;
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**

void prepare_Ethernet()
{
    //Iinitialize SPI Bus for Communicating with W5500 Ethernet Chip
    SPI.begin(ETHERNET_PIN_SCK, ETHERNET_PIN_MISO, ETHERNET_PIN_MOSI, ETHERNET_PIN_CS);

    debugf("\r\n");
    debugf("Initializing Ethernet\r\n");
    Ethernet.init(ETHERNET_PIN_CS);
    
    getEthernetMac(eMac);
    debugf("\r\n");
    debugf("Ethernet Mac Addr = %02X:%02X:%02X:%02X:%02X:%02X\n\r", eMac[0],eMac[1],eMac[2],eMac[3],eMac[4],eMac[5]); 
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**

uint8_t EthernetConnectionStatus()
{
    // Check for Ethernet hardware present
    if (!isEthernetHardwareOK()) 
    {
        return CHIP_NOT_FOUND;
    } 
    // Check weather Ethernet Cable is Connected or not
    else if (!isEthernetCableConnected()) 
    {
        return CABLE_NOT_CONNECTED;
    }
    else
    {
        return CONNECTION_STATUS_OK;
    }
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**

void EthernetBegin()
{  
    uint8_t retryCount = 0;
    uint8_t ethernetConnectionStatus;
    debugf("Connecting to Ethernet Network . . .\r\n");
    if (STATIC == 1)
    {
        //Static IP  Configuration
        IPAddress staticIP(EXPANDIP(ipaddress));
        IPAddress dns(EXPANDIP(dnsStatic));
        IPAddress gateway(EXPANDIP(gatewayStatic));
        IPAddress subnet(EXPANDIP(subnetStatic));
        
        //Manual Configuration. Return type void
        Ethernet.begin(eMac, staticIP, dns, gateway, subnet);
        ethernetConnectionStatus = EthernetConnectionStatus();
        while(ethernetConnectionStatus != CONNECTION_STATUS_OK)
        {
            retryCount++;
            ethernetConnectionStatus = EthernetConnectionStatus();
            //if Connection Status Not ok
            debugf("Failed\r\n");
            //Then print the Reason
            if(ethernetConnectionStatus == CHIP_NOT_FOUND)
            {
                //if Ethernet hardware is not present
                debugf("ERROR : Ethernet Chip Not Found\r\n");
                return;
            }
            else if(ethernetConnectionStatus == CABLE_NOT_CONNECTED)
            {
                //if Ethernet cable is not connected
                debugf("ERROR : Ethernet cable not connected\r\n");
            }
            if(retryCount >= ETHERNET_RETRY_COUNT)
            {
                return;        
            }
            debugf("Retrying . . .\r\n");
            delay(100);
        }
    }
    else
    {
        //DHCP configuration
        // Initialise the Ethernet shield to use the provided MAC address and
        // gain the rest of the configuration through DHCP.
        // Returns 0 if the DHCP configuration failed, and 1 if it succeeded
        while(Ethernet.begin(eMac) == 0)
        {
            retryCount++;
            ethernetConnectionStatus = EthernetConnectionStatus();
            debugf("Failed\r\n");
            
            if(ethernetConnectionStatus == CHIP_NOT_FOUND)
            {
                //if Ethernet hardware is not present
                debugf("ERROR : Ethernet Chip Not Found\r\n");
                return;
            }
            else if(ethernetConnectionStatus == CABLE_NOT_CONNECTED)
            {
                //if Ethernet cable is not connected
                debugf("ERROR : Ethernet cable not connected\r\n");
            }
            if(retryCount >= ETHERNET_RETRY_COUNT)
            {
                return;        
            }
            debugf("Retrying . . .\r\n");
            delay(50);
        }
    }
    debugf("SUCCESS\r\n");
    return;
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**
