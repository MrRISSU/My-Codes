

#define GSM_CONF_BUFFER_SIZE 1024
#define GSM_MAX_SETTINGS     15

//ME300 PROTOTYPE-2 GSM PINS
#define GSM_TX           48
#define GSM_RX           3
#define GSM_PWR_PIN      39
#define GSM_PWR_ENABLE   11


// Other roaming Profiles of Things Mobile
#define APN_THINGSMOBILE_ROAMING_1  "TM-1"
#define APN_THINGSMOBILE_ROAMING_2  "TM-2"
#define APN_THINGSMOBILE_ROAMING_3  "TM-3"
#define APN_THINGSMOBILE_ROAMING_4  "TM-4"

#define GPRS_USER  ""
#define GPRS_PASS  ""

//TIMEOUT MACROS
#define GPRS_TIMEOUT                 (20 * 1000)
#define GSM_SERIAL_RESPONSE_TIMEOUT  (10 * 1000)
#define GSM_POWERON_TIMEOUT          8000
#define GSM_POWEROFF_TIMEOUT         8000
#define GSM_OPERATOR_CONNECT_TIMEOUT 10*1000
#define GSM_SIGNAL_SEARCH_TIMEOUT    10*1000

//Delay MACROS
#define GSM_PWR_ON_DWELL_TIME        1500
#define GSM_PWR_OFF_DWELL_TIME       2000
#define GSM_PWR_ON_WAIT_TIME         4000
#define GSM_PWR_OFF_WAIT_TIME        4200
#define GSM_RESTART_WAIT_TIME        500
#define GSM_PWR_ON_RETRY_DELAY       1000
#define SERIAL_REINITIALIZE_DELAY    500
#define GSM_POWERCYCLE_DELAY         500

#define GSM_SET_FACTORY_BAUDRATE     1
#define GSM_HIGH_SERIAL_BAUDRATE    921600
#define GSM_FACTORY_SERIAL_BAUDRATE 9600


//Sim Status
#define SIM_ERROR               "SIM ERROR"
#define SIM_READY               "SIM READY"
#define SIM_LOCKED              "SIM LOCKED"
#define SIM_ANTITHEFT_LOCKED    "SIM ANTITHEFT LOCKED" 
#define SIM_STATUS_UNKNOWN      "SIM STATUS UNKNOWN"

// Network Band modes
#define CATM         1
#define NBIOT        2
#define CATM_NBIOT   3

// GSM CNTP Command Modes
#define GSM_CNTP_MODE_UTC_TO_LOCALTIME         0
#define GSM_CNTP_MODE_UTC_TO_SERIAL            1
#define GSM_CNTP_MODE_UTC_TO_LOCALTIME_SERIAL  2

// GSM CID OPTIONS
#define GSM_PDP_CONTEXT_CID_DEFAULT 0

//Other Macros
#define WAIT_TIME_GPRS 7000

#define MODEM_RESTART_TRIES 3
#define GSM_NORMAL_POWERON_RETRY_COUNT   5
#define GSM_SETBAUD_POWERON_RETRY_COUNT  3

#define GSM_MAX_SERVING_CELLS  10
#define CENG_LINE_MAX_LENGTH   45
#define NO_LENGTH_LIMIT        0
#define GSM_LOC_DEBUGG         0

//Buffer size to hold the formatted string
//Buffer size should be large enough to hold the formatted string
#define FMT_STR_BUF_SIZE 256

//------------------------------- Structure & Global Variables ------------------------------

typedef struct ModemInfo
{
    char modemName[20];
    char modemInfo[20];
    char modemImei[20];
}modeminfo_t;

typedef struct SimInfo
{
    char detectedOperator[20];
    char selectedApn[20];
    char simImsi[20];
    char simCcid[20];
    int signalQuality;
}siminfo_t;

typedef struct GsmSettings
{
    char gsmOperator[20];
    char gsmApn[20];
} gsmsettings_t;

gsmsettings_t gsmSettings[GSM_MAX_SETTINGS];
modeminfo_t modemInfo;
siminfo_t simInfo;
// Network modes to Auto Try the modes
uint16_t NetworkModes[] =
{
    GSM_MODE_NONE,  // Reserved for Configured
    MODE_AUTO,      // Auto Selection
    MODE_LTE,       // 4G / LTE
    MODE_LTE_GSM,   // 3G
    MODE_GSM        // 2G
};

static bool gsmStatus = true;
static uint16_t selectedMode;

static String ATresponseBuff;

// Variables for GSM Location infos
static String currentMode;
static uint8_t lineCount = 0;
String gsmLocParamLines[GSM_MAX_SERVING_CELLS];

// *******************************************************************

static void gsm_hardware_power_on()
{
    digitalWrite(GSM_PWR_ENABLE, HIGH);
    return;
}

static void gsm_hardware_power_off()
{
    digitalWrite(GSM_PWR_ENABLE, LOW);
    return;
}

static bool isGsmOFF()
{
    return !isGsmOk();
}

static bool isGsmON()
{
    return isGsmOk();  
}

static bool isGsmOk()
{
    return gsm_modem.testAT();
}

static bool PowerOnGSM()
{
    if(isGsmON())
    {
        Println(" SUCCESS (Already ON)");
        // GSM gsm_modem already ON, So return
        return true;
    }

    if(hardwareRev >= 2)
    {
        gsm_hardware_power_on();
        delay(100);
    }
    digitalWrite(GSM_PWR_PIN, HIGH);
    delay(GSM_PWR_ON_DWELL_TIME);
    digitalWrite(GSM_PWR_PIN, LOW);
    
    unsigned long StartTime = millis();    
    while(isGsmOFF())
    {
        if( (millis() - StartTime) > GSM_POWERON_TIMEOUT)
        {
            return false;
        }       
        Serial.print(" .");
        delay(500);          
    }    
    Println(" SUCCESS");
        
    Serial.print("Waiting for Modem to get ready"); 
    delay(GSM_PWR_ON_WAIT_TIME);
    Serial.println(" OK");   
    return true; 
}

static bool GsmTryPowerON(int baudrate, int8_t retryCount)
{
    bool Status = false;
    delay(SERIAL_REINITIALIZE_DELAY);
    BeginGsmSerial(baudrate);
    
    for(uint8_t i = 0; i < retryCount; i++)
    {
        (i == 0) ? ( Print("Powering ON GSM Modem with baudrate = "), Print(baudrate) ) :  Print("Retrying ") ;  
        
        if(false == PowerOnGSM())
        {
            Println(" FAILED");
            delay(GSM_PWR_ON_RETRY_DELAY);
            continue;
        }
        Status = true;
        break;
    }
    
    if(false == Status)
    {
        debugf("GSM Power ON Retry Count Exceeded\n\r");
    }
    Println("\n\r");
        
    return Status;
}

static bool PowerOffGSM()
{
    if(isGsmOFF())
    {
        // GSM Modem already OFF, So return
        return true;
    }  

    Serial.print("\n\rPowering OFF GSM Modem");

     if(hardwareRev >= 2)
    {
        gsm_hardware_power_off();
        delay(100);
    }
    else
    {
        digitalWrite(GSM_PWR_PIN, HIGH);
        delay(GSM_PWR_OFF_DWELL_TIME);
        digitalWrite(GSM_PWR_PIN, LOW);
    }
    
    unsigned long StartTime = millis();
    while(isGsmON())
    {
        if( (millis() - StartTime) > GSM_POWEROFF_TIMEOUT)
        {
            return false;
        }
        Serial.print(" .");
        delay(500);
    }
    Serial.println(" SUCCESS");

    Serial.print("Waiting for Modem to Shutdown completely");
    delay(GSM_PWR_OFF_WAIT_TIME);
    Serial.println(" OK");
    return true;
}

bool RestartModem()
{
    // restart() generally takes longer than init() but ensures the module doesn't have lingering connections
    
    if (!gsm_modem.restart()) 
    {
        return false;              
    }  
    return true;
}

static bool WaitForGPRS()
{
    if (gsmStatus == false)
    {
        Serial.println("WaitForGPRS() : GSM Status not good. Not doing Gprs Init");
        return false;
    }

    unsigned long StartTime = millis();
    Print("Waiting for GPRS to connect");

    while (millis() - StartTime < WAIT_TIME_GPRS)
    {
        if (gsm_modem.isGprsConnected())
        {
            Println("...CONNECTED!");    
            Print("GSM Local IP   : "); Println(get_gsm_IP());
            Println();
            return true;

        }
        Print(". ");
        gsm_modem.gprsConnect(simInfo.selectedApn, GPRS_USER, GPRS_PASS);       
        delay(500);
    } 
    
    gsmStatus = false;
    Print("ERROR: GPRS Connection timed out -->");
    return false;
}

//void GprsDisconnect()
//{
//    gsm_modem.gprsDisconnect();
//}

static void ConnectGPRS()
{
    gsm_modem.gprsConnect(simInfo.selectedApn, GPRS_USER, GPRS_PASS);
}

static void GprsBegin(const char* configuredApn)
{
    Serial.printf_P("GprsBegin start\n\r");
    if (gsmStatus == false)
    {
        Serial.println("GSM Status not good. Not doing Gprs Init");
        return;      
    }

    if (configuredApn != NULL && strlen(configuredApn) > 0)
    {
        Serial.printf_P("Found Configured APN\n\r");
        strncpy(simInfo.selectedApn, configuredApn, sizeof(simInfo.selectedApn));
        Serial.printf_P("Connecting to %s \n\r", simInfo.selectedApn);
        ConnectGPRS();
        WaitForOperator();
        delay(100);
        if (true == gsm_modem.isGprsConnected())
        {
            Serial.printf_P("GPRS Connected\n\r");
            return;
        }
    }



    if (String(simInfo.detectedOperator).length() != 0)
    {
        Serial.printf_P("Operator Found, let's search for APN\n\r");
        // GPRS connection parameters are usually set after network registration
        for (int i = 0; (i < NE(gsmSettings)) && (strlen(gsmSettings[i].gsmOperator)) > 0; i++)
        { 
            strncpy(simInfo.selectedApn, gsmSettings[i].gsmApn, sizeof(simInfo.selectedApn));

            // if (String(simInfo.detectedOperator).indexOf(gsmSettings[i].gsmOperator) != -1) 
            if (strcasestr(simInfo.detectedOperator, gsmSettings[i].gsmOperator) != NULL) 
            {
                Serial.printf_P("Connecting to %s,  Operator is %s\n\r", simInfo.selectedApn, gsmSettings[i].gsmOperator);
                ConnectGPRS();
            }

            delay(100);
            if (true == gsm_modem.isGprsConnected())
            {
                Serial.printf_P("GPRS Connected\n\r");
                return;
            }
        }
     }



    Serial.printf_P("GPRS Still not connected\n\r");
    Serial.printf_P("Trying all APNs in our list\n\r");
    // GPRS connection parameters are usually set after network registration
    for (int i = 0; i < NE(gsmSettings) && strlen(gsmSettings[i].gsmOperator) > 0; i++)
    { 
        strncpy(simInfo.selectedApn, gsmSettings[i].gsmApn, sizeof(simInfo.selectedApn));
        Serial.printf_P("Connecting to %s \n\r", simInfo.selectedApn);
        ConnectGPRS();

        delay(100);
        if (true == gsm_modem.isGprsConnected())
        {
            Serial.printf_P("GPRS Connected\n\r");
            return;
        }
    }

    Serial.printf_P("GPRS Not Connected\n\r");
    gsmStatus = false;
    return;
}

static void SetNetMode(int Mode)
{
    Serial.print("Setting Network Mode");
    if(gsm_modem.setNetworkMode(Mode))
    {
        Serial.print("...SUCCESS --> ");
        Serial.printf_P("MODE = %d \n\r", Mode);       
    }
    else
    {
        Serial.println("...FAILED");
    }
    return;
}

static void SetNetBandMode(int Mode)
{
    Serial.print("Setting Network Band Mode");
    if(gsm_modem.setPreferredMode(Mode))
    {
        Serial.print("...SUCCESS --> ");
        Serial.printf_P("MODE = %d \n\r", Mode);       
    }
    else
    {
        Serial.println("...FAILED");
    }
    return;  
}

bool isNetworkReady(bool wait)
{
    if (gsm_modem.isNetworkConnected()) 
    {
        return true;
    }

    if(wait)
    {
        Serial.println("Network Not connected");
        Serial.print("Waiting for network...");
        if (!gsm_modem.waitForNetwork()) 
        {
            Serial.println("...FAILED");
            return false;
        }
        Serial.println("...SUCCESS!"); 
        return true;
    }
    return false;
}

static IPAddress get_gsm_IP()
{
    return gsm_modem.localIP();  
}

static int getRSSI(int csq)
{
    return (-109 + ((csq-2) * 2));
}

static bool isSimReady()
{
    return ((gsm_modem.getSimStatus() == 1) ? true : false) ;
}

static String Sim_Status(int Status)
{
    switch(Status)
    {
        case 0 : return SIM_ERROR;
        case 1 : return SIM_READY;
        case 2 : return SIM_LOCKED;
        case 3 : return SIM_ANTITHEFT_LOCKED; 
        default: return SIM_STATUS_UNKNOWN; 
    }
}

static String GetGsmIMEI()
{
    return gsm_modem.getIMEI();  
}

static void GetModemInfo()
{
    strncpy(modemInfo.modemName, gsm_modem.getModemName().c_str(), sizeof(modemInfo.modemName));
    strncpy(modemInfo.modemInfo, gsm_modem.getModemInfo().c_str(), sizeof(modemInfo.modemInfo));
    strncpy(modemInfo.modemImei, GetGsmIMEI().c_str(), sizeof(modemInfo.modemImei)); 
    return;
}

static void GetSimInfo()
{
    strncpy(simInfo.simImsi, gsm_modem.getIMSI().c_str(), sizeof(simInfo.simImsi));
    strncpy(simInfo.simCcid, gsm_modem.getSimCCID().c_str(), sizeof(simInfo.simCcid));
    return;
}

static void GetGsmInfo()
{
    GetModemInfo();
    if(isSimReady())
    {
        GetSimInfo();
    }
    return;
}

static void PrintModemInfo()
{
    Serial.printf_P("%-15s: %s \n\r", "Modem Name", modemInfo.modemName);
    Serial.printf_P("%-15s: %s \n\r", "Modem Info", modemInfo.modemInfo);
    Serial.printf_P("%-15s: %s \n\r", "Modem IMEI", modemInfo.modemImei);  
    return;
}

static void PrintSimInfo()
{
    Serial.printf_P("%-15s: %s \n\r", "Sim status", Sim_Status(gsm_modem.getSimStatus()).c_str());
    Serial.printf_P("%-15s: %s \n\r", "Sim CCID", simInfo.simImsi);
    Serial.printf_P("%-15s: %s \n\r", "Sim IMSI", simInfo.simCcid);
    return;
}

static void PrintGsmInfo()
{
    PrintModemInfo();
    if(isSimReady())
    {
        PrintSimInfo();
    }
    return;
}

static void WaitForOperator()
{
    unsigned long StartTime = millis();
    debugf("%-15s: ", "Sim Operator");   
    while(gsm_modem.getOperator().length() == 0)
    {
        debugf(" .");
        if(millis() - StartTime > GSM_OPERATOR_CONNECT_TIMEOUT)
        {
            Println("ERROR : Timeout!");
            return;
        }        
    }
    strncpy(simInfo.detectedOperator, gsm_modem.getOperator().c_str(), sizeof(simInfo.detectedOperator));
    debugf("\r%-15s: %s \n\r", "Sim Operator", simInfo.detectedOperator); 
       
    return;
}

static void WaitForSignal()
{     
    unsigned long StartTime = millis();
    Serial.printf_P("%-15s: ", "Signal quality");   
    while(getRSSI(gsm_modem.getSignalQuality()) >= 0)
    {
        Serial.print(" .");
        delay(1000);
        if(millis() - StartTime > GSM_SIGNAL_SEARCH_TIMEOUT)
        {
            Println("ERROR : Timeout!");
            return;
        }        
    }
    simInfo.signalQuality = getRSSI(gsm_modem.getSignalQuality());
    Serial.printf_P("\r%-15s: %d dBm\n\r\n", "Signal quality", simInfo.signalQuality);
    return;
}

static void BeginGsmSerial(int baudrate)
{
    // Set GSM module baud rate
    SerialAT.begin(baudrate, SERIAL_8N1, GSM_RX, GSM_TX);
    delay(500);
}

static bool GsmSetDesiredBaudrate(int currentBaudrate, int desiredBaudrate)
{
    Println();
    Print("Checking if Current GSM Modem Baudrate is "); Println(currentBaudrate);

    //PowerOnGSM successfull means GSM modem has Factory Default Baudrate.
    if(GsmTryPowerON(currentBaudrate, GSM_SETBAUD_POWERON_RETRY_COUNT))
    {
        Println("Current Baudrate is Correct");
        gsm_modem.setBaud(desiredBaudrate);
        Print("GSM Baudrate set to desired value : "); Println(desiredBaudrate);
        return true;
    }
    Println("Current Baudrate given Incorrect, Skipping setBaud()");
    return false;
    
}

static bool PowerOnWithDesiredBaudrate(int desiredBaudrate, int fallbackBaudrate)
{
    // Powering ON GSM Module with desiredbaudrate
    if(false == GsmTryPowerON(desiredBaudrate, GSM_NORMAL_POWERON_RETRY_COUNT))
    {
        // Poweron with desired baudrate failed
        // Trying to switch modem baudrate to desiredbaudrate assuming fallbackBaudrate = current baudrate
        
        if(false == GsmSetDesiredBaudrate(fallbackBaudrate, desiredBaudrate))
        {
            return false;              
        }

        // modem baudrate set to desiredbaudrate
        // So Trying again Powering ON GSM Module with desiredbaudrate
        if(false == GsmTryPowerON(desiredBaudrate, GSM_NORMAL_POWERON_RETRY_COUNT))
        {   
            return false;          
        }
    }

    // GSM Poweron success with desired baudrate
    return true;
}

static void SetPowerPin()
{
    pinMode(GSM_PWR_PIN, OUTPUT);
    if(hardwareRev >= 2)
    {
        pinMode(GSM_PWR_ENABLE, OUTPUT);
    }
    return;
}

static void gsmReconnect()
{   
    Print("Restarting modem");
    if (!RestartModem())
    {
      Println("...FAILED!");
      return;
    }
    Println();
    
    if(!isNetworkReady(WAIT))
    {
        Println("Network connection FAILED!");
        return;
    }
    
    return;
}

int8_t prepare_GSM(uint16_t configuredMode)
{   
    
    // Setting GPIO to control GSM Power Key
    SetPowerPin();
    strncpy(simInfo.selectedApn, GSM_APN, sizeof(simInfo.selectedApn));
    Serial.printf_P("Set APN to Default : %s\n\r", simInfo.selectedApn);
    Println("Set Default GSM Modes List.");

    //Powering ON GSM Module
    Println();
    Println("----------------------------------------------------------------------------\n\r");

    int desiredBaudrate;
    int fallbackBaudrate; 
    
    if(GSM_SET_FACTORY_BAUDRATE == true)
    {
        desiredBaudrate  = GSM_FACTORY_SERIAL_BAUDRATE;
        fallbackBaudrate = GSM_HIGH_SERIAL_BAUDRATE;
    }
    else
    {
        desiredBaudrate  = GSM_HIGH_SERIAL_BAUDRATE;
        fallbackBaudrate = GSM_FACTORY_SERIAL_BAUDRATE;
    }

    Println("GSM Modem Baud Rate Config -");
    Print("  * Desired Baud Rate   : "); Println(desiredBaudrate);
    Print("  * Fallback Baud Rate  : "); Println(fallbackBaudrate);
    Println();

    // Initializing Serial com to communicate with GSM modem
    BeginGsmSerial(desiredBaudrate);

    //Powering OFF GSM Module for a fresh Start (effective when changing SIM Card)
    if(false == PowerOffGSM())
    {
       Serial.println(" FAILED");
    }

    delay(GSM_POWERCYCLE_DELAY);
    
    Println();
    bool powerOnStatus = PowerOnWithDesiredBaudrate(desiredBaudrate, fallbackBaudrate);
    
    Println("----------------------------------------------------------------------------\n\r");
    
    if(false == powerOnStatus)
    {
        gsmStatus = false;
        return -1;
    }

    // Writing the configured and conected modes in reserved space
    NetworkModes[0] = configuredMode;
    
    for (int i = 0; i < NE(NetworkModes); i++)
    {
        selectedMode = NetworkModes[i];
        if (selectedMode == GSM_MODE_NONE) {
            Serial.println("Skipping Invalid Mode");
            continue;
        }

        if (setMode(selectedMode))
        {
            Serial.print("Selected Mode : "); Serial.println(selectedMode);
            gsmStatus = true;
            return 1;
        }
    }
    Serial.println("Prepare GSM Failed");
    gsmStatus = false;
    return 0;
}



static bool setMode(uint16_t gsmmode)
{
    //Restart Modem
    Println("Initializing modem...");
    int i;
    for (i = 0; i < MODEM_RESTART_TRIES; i++)
    {
        if(true == RestartModem())
        {
            break;
        }
        
        Print("Initialize modem failed Retrying --> ");
        delay(GSM_RESTART_WAIT_TIME);
    }
    if (i >= MODEM_RESTART_TRIES)
    {
        Println("Initialize modem failed. Giving Up.");
        return false;
    }
    Println("Initialize modem Successful.");  
    
    Print("Setting Mode : "); Println(gsmmode);    
    //Set Network mode to Auto
    SetNetMode(gsmmode);

    //Set Network Band Mode
    SetNetBandMode(CATM_NBIOT);

    //Get Modem & Sim details
    GetGsmInfo();
    
    //Print Modem & Sim details
    PrintGsmInfo(); 

    //Give some time to detect the operator
    WaitForOperator();

    //Give some time to get a signal
    WaitForSignal();        

    //Check the Network
    if(isNetworkReady(WAIT))
    {
        Serial.println("Network Ready");
        return true;     
    }
    Print("Network Not Ready --> ");
    return false;
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**

static String SubString(String str, char startChar, char endChar)
{
    int16_t startIndex  = str.indexOf(startChar);
    int16_t endIndex    = str.lastIndexOf(endChar);

    // Validate indexes
    if(startIndex == -1 || endIndex == -1)
    {
        return String();
    }
    return str.substring(startIndex + 1, endIndex);
}

static String StringTok(String mainStr, String startStr, String endStr, unsigned int sizeLimit = NO_LENGTH_LIMIT)
{
    static String str          = String();
    static int16_t startIndex  = 0;
    static int16_t endIndex    = 0;

    // If mainStr is a different string then this is for a fresh splitting
    if(mainStr != str)
    {
        str         = mainStr;
        startIndex  = 0;
        endIndex    = 0;
    }

    startIndex = str.indexOf(startStr, endIndex);
    endIndex   = str.indexOf(endStr, startIndex);

    // Validate obtained indexes
    if(startIndex == -1 || endIndex == -1 || startIndex > str.length() || endIndex > str.length())
    {
        return String();
    }

    // Indexes are valid so get substring
    String subStr = str.substring(startIndex, endIndex);

    // Validate Substring
    if(subStr.length() == 0 || (subStr.length() > sizeLimit && sizeLimit != NO_LENGTH_LIMIT))
    {
        return String();
    }

    return subStr;
}

//************************ AT COMMAND FUNCTIONS ************************

static bool GetModemResponse()
{
    ATresponseBuff = "";

    if(false == WaitModemResponse(GSM_SERIAL_RESPONSE_TIMEOUT))
    {
        return false;
    }
        
    while (SerialAT.available() != 0)
    {
        ATresponseBuff += (char)SerialAT.read();
    }
    
    ATresponseBuff.trim();
    
    return true;
}

static bool WaitModemResponse(unsigned long timeout)
{
    unsigned long StartTime = millis();
    while (SerialAT.available() == 0) 
    {   
        if( (millis() - StartTime) > timeout)
        {
            return false; 
        }      
        delay(100);
    }
    return true;
}

static void SendAT(const char *cmd, ...) 
{
    va_list args;
    va_start(args, cmd);
    char formattedCmd[FMT_STR_BUF_SIZE];
    vsnprintf(formattedCmd, sizeof(formattedCmd), cmd, args);
    va_end(args);
    
    String ATCommand = "AT" + String(formattedCmd);

    // Uncomment following line to print the Command
    // debugf("Command = %s\r\n", ATCommand.c_str());

    // Send command to GSM Modem
    SerialAT.println(ATCommand);  
}

static bool isResponseContains(String checkString)
{   
    return (ATresponseBuff.indexOf(checkString) != -1);
}

//******************* LOCATION FUNCTIONS **************************

static bool EnableEngMode()
{
    // Send Engineering mode Enable Command
    SendAT("+CENG=1,1");

    // Get Response to buffer
    if(false == GetModemResponse())
    {
        debugf("EnableEngMode(): No AT response\r\n");
        return false;
    }

    if(false == isResponseContains("OK"))
    {
        debugf("EnableEngMode(): AT response Not 'OK'\r\n");
        return false;
    }

    return true;
}

bool UpdateGsmLocInfos()
{
    // Enable Engineering Mode to fetch Network & location info
    if(false == EnableEngMode())
    {
        return false;
    }

    // Reset Line Count before new request
    lineCount = 0;
    
    // SIM7070 AT Command Manual : https://www.waveshare.com/w/upload/0/02/SIM7070_SIM7080_SIM7090_Series_AT_Command_Manual_V1.03.pdf
    // Refer section '5.2.30 AT+CENG' , Page number 140 of SIM7070 AT Command Manual

    // Network Info Request
    SendAT("+CENG?");

    // Get Response to buffer
    if(false == GetModemResponse())
    {
        debugf("UpdateGsmLocInfos(): No AT response\r\n");
        return false;
    }

    if(false == isResponseContains("OK"))
    {
        debugf("UpdateGsmLocInfos(): AT response Not 'OK'\r\n");
        return false;
    }            
    
    /*------------------------------------------------------------------------------------

    Response Format  : +CENG: <mode>,<Ncell>,<cell num>,<SystemMode>

                       +CENG: <cell>,"<bcch>,<rxl>,<bsic>,<cellid>,<mcc>,<mnc>,<lac>"
                       ...
                       ...

                       OK

    Response Example : +CENG: 1,1,6,GSM
                       +CENG: 0,"0665,35,02,4b2c,404,95,c1c0"
                       +CENG: 1,"0662,31,08,48aa,404,95,c1c0"
                       +CENG: 2,"0711,30,54,0e6c,404,95,c1c0"
                       +CENG: 3,"0663,26,61,4b2b,404,95,c1c0"
                       +CENG: 4,"0709,21,50,0e6b,404,95,c1c0"
                       +CENG: 5,"0708,16,50,78f4,404,95,c1c0"

                       OK
    ------------------------------------------------------------------------------------*/

    if(false == isResponseContains("+CENG"))
    {
        debugf("UpdateGsmLocInfos(): AT response Not '+CENG'\r\n");
        return false;
    }

    String line;
    uint8_t paramLineIndex  = 0;

#if GSM_LOC_DEBUGG == 1
    debugf("Obtained Response : ------------------\r\n");

    tracenew(ATresponseBuff.c_str());

    debugf("Done ---------------------------------\n\r\r\n");


    char HEXresponse[300];


    strncpy_safe(HEXresponse, ATresponseBuff.c_str(), sizeof(HEXresponse));
    debugf("HEX of response : ---------------\r\n");


    for (int i =  0; i < strlen(HEXresponse); i++)
    {
        debugf("%X\r\n", HEXresponse[i]);
    }

    debugf("Done ---------------------------------\n\r\r\n");
#endif
    
    // Parsing and Saving responses
    for(int i = 0; i <= NE(gsmLocParamLines); i++)
    {
        line = StringTok(ATresponseBuff, "+CENG:", "\r\n", CENG_LINE_MAX_LENGTH);

        if(line.length() == 0)
        {
            break;
        }

        if(i == 0)
        {
            int16_t lastCommaIndex = line.lastIndexOf(',');
            if(lastCommaIndex != -1)
            {
                currentMode = line.substring(lastCommaIndex + 1);
            }
            continue;
        }

        String paramLine = SubString(line, '\"', '\"');

        if(paramLine.length() != 0)
        {
            gsmLocParamLines[paramLineIndex] = paramLine;
            paramLineIndex++;
        }
    }
    
    lineCount = paramLineIndex;
    SerialAT.flush();

#if GSM_LOC_DEBUGG == 1
    debugf("line Count   = %u\r\n", lineCount);

    debugf("String array Elements are:------------\r\n");

    for (int i =  0; i < lineCount; i++)
    {
        debugf("%s\r\n", gsmLocParamLines[i].c_str());
    }

    debugf("Done ---------------------------------\n\r\r\n");
#endif

    return true;
}
