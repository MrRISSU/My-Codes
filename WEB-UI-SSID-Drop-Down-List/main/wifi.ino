/* ****************************************************************************
 * Company   : Consyst Digital Industries Pvt Ltd
 * Author    : Huwairis Ibnu Kabeer
 * email     : huwairisik@consyst.biz
 * Date      : 08/01/2025
 * File name : WiFi.ino
 * ****************************************************************************/

/**********************************MACROS**********************************/

#define WAIT_TIME_WIFI (20 * 2)

/**********************************STRUCT**********************************/

/**********************************GLOBAL**********************************/

/*******************************LOOKUP TABLE*******************************/

/*********************************FUNCTION*********************************/

void WifiScanNetworks()
{
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    debugf("Scanning WiFi Networks...\r\n");
    scancount = WiFi.scanNetworks();
    debugf("Scan Done\r\n");
    debugf("WiFi Networks Found : %d\n\r", scancount);
    if(scancount > 0)
    {
        // Allocate memory for the ssid_list (number of SSIDs)
        ssid_list = (char**)malloc(scancount * sizeof(char*));
        // Handle allocation failure
        if (ssid_list == NULL)
        {
            debugf("Memory allocation for ssid_list failed!");
            return;
        }
        
        debugf("%-5s : %-11s : %-4s : %-4s : %-20s : %-48s\n\r", "SlNo", "Encription", "RSSI", "Channel", "BSSID", "SSID");
        for (int i = 0; i < scancount; ++i)
        {
            //-----------------------------------------------------------------//
            uint16_t ssid_length = strlen( WiFi.SSID(i).c_str()) + 1;
            // Allocate memory for each SSID
            ssid_list[i] = (char*)malloc(ssid_length);
            // handle allocation failure
            if (ssid_list[i] == NULL)
            {
                debugf("Memory allocation for ssid_list[%d] failed!", i);
                // cut off remaining SSIDs
                scancount = i;
                assert(0);
                break;
            }
            //-----------------------------------------------------------------//
            strncpy(ssid_list[i], WiFi.SSID(i).c_str(), ssid_length);
            long rssi          = WiFi.RSSI(i);
            int enc            = WiFi.encryptionType(i);
            const char *encstr = getEncryptionType(enc);
            int32_t ch   = WiFi.channel(i); 
            
            // Print SSID and RSSI for each network found
            debugf("%-5d : %-11s : %-4ld : %-4d : %-20s : %-48s\n\r", (i + 1), encstr, rssi, ch, WiFi.BSSIDstr(i).c_str(), ssid_list[i]);
        }
    }

}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**

bool IsWifiConnected()
{
    return (WiFi.status() == WL_CONNECTED) ? true : false;
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**

static const char* getEncryptionType(int thisType)
{
    // read the encryption type and return the name:
    switch (thisType)
    {
        case WIFI_AUTH_WEP:             return "WEP";
        case WIFI_AUTH_WPA_PSK:         return "WPA";
        case WIFI_AUTH_WPA2_PSK:        return "WPA2";
        case WIFI_AUTH_OPEN:            return "None";
        case WIFI_AUTH_WPA_WPA2_PSK:    return "Auto";
        case WIFI_AUTH_WPA2_ENTERPRISE: return "WPA2-ent";
        default:                        return "Unknown";
    }  
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**

bool WaitForWifi()
{
    int c = 0;
    debugf("Waiting for Wifi to connect\r\n");
    while (c < WAIT_TIME_WIFI)
    {
        if (IsWifiConnected())
        {

            debugf("\r\n");
            debugf("Connected!!!\r\n");
            debugf("IP    : %s\r\n", WiFi.localIP().toString().c_str());
            debugf("Mask  : %s\r\n", WiFi.subnetMask().toString().c_str());
            debugf("GW    : %s\r\n", WiFi.gatewayIP().toString().c_str());
            debugf("DNS 1 : %s\r\n", WiFi.dnsIP(0).toString().c_str());
            debugf("DNS 2 : %s\r\n", WiFi.dnsIP(1).toString().c_str());
            return true;
        }
        debugf(" .");
        delay(500);
        c++;
    }
    debugf("\r\n");
    debugf("ERROR: Wifi Connection timed out\r\n");
    ShowWifiError();
    return false;
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**

void WifiConnect()
{
    debugf("Connecting to: %s\r\n", WIFI_SSID);
    for (int i = 0; i < scancount; ++i)
    {        
        if (strcmp(WiFi.SSID(i).c_str(), WIFI_SSID) != 0)
        {
            continue;
        }
        debugf("Found matching WiFi Access Point\r\n");
                
        uint8_t bssid[6];
        uint8_t *bssidptr = WiFi.BSSID(i);
        memcpy(bssid, bssidptr, 6);
        int32_t ch   = WiFi.channel(i);
        debugf("SSID     : %s\r\n", WiFi.SSID(i).c_str());
        debugf("CHANNEL  : %d\r\n", WiFi.channel(i));
        debugf("BSSID    : %s\r\n", WiFi.BSSIDstr(i).c_str());                           
        debugf("RSSI     : %d\r\n", WiFi.RSSI(i));                                
        debugf("ENC TYPE : %s\r\n", getEncryptionType(WiFi.encryptionType(i)));                     
                 
        WiFi.mode(WIFI_STA);
        WiFi.begin(WIFI_SSID, WIFI_PASS, ch, bssid);
  
        if(WaitForWifi())
        {
            // WIFI Connected
            return;
        }
        else 
        {
            //Wifi Timeout, connection failed
            //continue for loop and check for AP with same SSID in the scan list
            debugf("\r\n");
            debugf("Wifi Connection Failed\r\n");
            WiFi.disconnect();
        }
    }
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**

void WifiReconnect()
{
    wl_status_t status = WiFi.status();
    if(status == WL_DISCONNECTED)
    {
        debugf("Reconnecting to WiFi...\r\n");
        WiFi.mode(WIFI_STA);
        WiFi.disconnect();
        // try to reconnect to the previously connected access point
        WiFi.reconnect();
    }
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**

void ShowWifiError()
{
    wl_status_t status = WiFi.status();
    debugf("WiFi status (%d) : ", status);
    switch(status)
    {
            case WL_CONNECTED:      debugf("CONNECTED\r\n"); break;
            case WL_IDLE_STATUS:    debugf("IDLE\r\n"); break;
            case WL_NO_SSID_AVAIL:  debugf("NO_SSID\r\n"); break;
            case WL_CONNECT_FAILED: debugf("CONNECT_FAILED\r\n"); break;
            case WL_DISCONNECTED:   debugf("DISCONNECTED\r\n"); break;
            default:                debugf("Unknown\r\n"); break;
    }
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**
