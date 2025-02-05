#define HTML_BODY_SIZE          2048
#define OPTIONS_HTML            "<option value=\"%s\" %s>%s</option>\n"
#define HTML_OPTIONS_EXTRA_SIZE sizeof(OPTIONS_HTML)

// Define the web serverwith the port
WebServer server(80);
// Buffer for the HTML page
char body[HTML_BODY_SIZE];

static void handleHome()
{
    debugf("\r\n");
    debugf("homepage Opened\r\n");
    snprintf(body,  sizeof(body),  "<html><head>\n<meta name='viewport' content='width=device-width, initial-scale=1.0'>\n</head><body>"
             "<a href=\"wifisetup\">Wifi Setup</a><br>"
             "<a href=\"settings\">Settings</a><br>"
             "<a href=\"factory_reset\">Factory Reset</a><br>"

             "\n</body>\n</html>" );

    //Send web page
    server.send(200, "text/html", body);
}

String isSelect(char *ssid1, const char *ssid2)
{
        if (strcmp(ssid1, ssid2) == 0)
        {  
            return String("selected");
        }
        return String("");
}

// Function to generate dropdown options for SSIDs (Pass ssid list, NE of ssid list and current ssid)
const char* genSSIDOptions(char **ssid_list, int numOfOptions, const char *current_ssid)
{
    size_t options_size = 0;
    // Calculate the size needed for this option, including the HTML tags 
    for (int i = 0; i < numOfOptions; i++)
    {
        options_size += 2 * strlen(ssid_list[i]) + HTML_OPTIONS_EXTRA_SIZE;
    }
    debugf("options_size: %d\r\n", options_size);

    char *options = (char *)malloc(options_size + 1);
    if (options == NULL)
    {
        // Handle memory allocation failure
        debugf("Memmory allocation failed!\r\n");
        return NULL;
    }
    // Initialize buffer  
    options[0] = '\0';

    for (int i = 0; i < numOfOptions; i++)
    {
        // Skip iteration if buffer is empty
        if(*ssid_list[i] == 0)
        {
            continue;
        }
        // If the current SSID matches the configured SSID, make it selected by defualt.
        int currentOptLen = strlen(options);
        int remainingOptLen = options_size - currentOptLen;
        char *ssid = ssid_list[i];
        // If the current SSID matches the configured SSID, make it selected by defualt.
        String selectStr = isSelect(ssid_list[i], current_ssid);
        
        snprintf(options + currentOptLen, remainingOptLen, OPTIONS_HTML,
                  ssid,
                  selectStr.c_str(),
                  ssid  );
    }
    return options;
}

// Function to handle the Wifi setup
void handleWifi()
{
    debugf("\r\n");
    debugf("Wifi page Opened\r\n");
    int spfwifi = snprintf(body, sizeof(body), "<!DOCTYPE HTML>\n<html>\n<head>\n"
                           "<title>MrRISSU</title>\n"
                           "<meta name='viewport' content='width=device-width, initial-scale=1.0'>\n"
                           "<style> body{font-family:Arial;} h1{text-align:center;} </style>\n</head>\n<body>\n"
                           "<p style=\"text-align:right;font-size:.85em;padding-right:1em;\"><b>MrRISSU</b></p>\n"
                           "<h1>WIFI CREDENTIALS</h1><br><br>\n"

                           "<form method='get' action='wifi'>\n"

                           "<label for=\"wIdInput\">Enter SSID:</label><br>\n"

                           "<div class=\"input-container\">\n"

                           "<input type=\"text\" id=\"wIdInput\" name=\"customSSID\" value=\"%s\" placeholder=\"Default SSID\">\n"

                           "<select id=\"wIdSelect\" name=\"wId\">\n"
                           "%s"
                           "</select>\n"
                           "</div><br>\n"

                           "<label for=\"wPw\">Password:</label><br>\n"
                           "<input type=\"text\" id=\"wPw\" name=\"wPw\" value=\"%s\"><br><br>\n"

                           "<input type=\"submit\" value=\"Submit\">\n</form>"
                           "</form>\n"
                           
                           "<form method='get' action='scan'>\n"  
                           "<input type=\"submit\" value=\"Re-scan\">\n"  
                           "</form>\n"

                           "\n</body>\n</html>",
                           WIFI_SSID,
                           genSSIDOptions(ssid_list, scancount, WIFI_SSID),
                           WIFI_PASS  );

    Serial.printf("%d\r\n", spfwifi);
    // Send response to client
    server.send(200, "text/html", body);

    // Handler for saving settings
    server.on("/wifi", []() {
        String Cssid = server.arg("wId");
        String Cpass = server.arg("wPw");
        debugf("Selected SSID : %s\r\n", Cssid.c_str());
        debugf("Selected PASS : %s\r\n", Cpass.c_str());
    } );

    // Handler for scanning networks  
    server.on("/scan", []() {  
        // Notify user that scanning is in progress
        snprintf(body, HTML_BODY_SIZE, "<html><body><h1>Scanning networks...</h1><p>Please reconnect once the scanning is complete.</p></body></html>");
        server.send(200, "text/html", body);
      
        // Call the function to scan networks
        WifiScanNetworks();
        
        // After scanning, again get into Webserver for choosing settings
        BeginWebServer();
    } ); 
}

void BeginWebServer()
{
    // Set WiFi mode to Access Point
    WiFi.mode(WIFI_AP);
    // Access point Name
    WiFi.softAP("~MrRISSU~");

    // Start server  
    server.on("/", handleHome);
    server.on("/wifisetup", handleWifi);
    
    server.begin();  
    debugf("HTTP server started\r\n");  

    // Print the Access Point Name
    debugf("Access Point Name       : %s\r\n", "~MrRISSU~");
    // Print the Access Point IP address
    debugf("Access Point IP address : %s\r\n", WiFi.softAPIP().toString().c_str());  
}

void HandleWeb()
{
    server.handleClient();
}
