/* ****************************************************************************
 * Company   : Consyst Digital Industries Pvt Ltd
 * Author    : Huwairis Ibnu Kabeer
 * email     : huwairisik@consyst.biz
 * Date      : 14/11/2024
 * File name : Modbus.ino
 * ****************************************************************************/
 
/**********************************MACROS**********************************/

//MODBUS CONFIG
#define MODBUS_MAX_REQUEST_SIZE 20
#define START_OFFSET            0
#define MB_COUNT                20
#define MODBUS_REGISTER_BITS    16
#define MODBUS_READ_DELAY       50

#define ENDIAN_LITTLE   0
#define ENDIAN_BIG      1 

#define DATATYPE_FLOAT  0
#define DATATYPE_INT16  1

#define REGTYPE_INPUT   0
#define REGTYPE_HOLDING 1

/**********************************STRUCT**********************************/

typedef struct
{
  int Register;
  char TagName[10];
  float data;
}Tag_details;

/**********************************GLOBAL**********************************/

// Address of Modbus Slave device (your computer's local IP address if you are using Modbus Slave simulator)
static IPAddress remote(192, 168, 1, 205);
// Declare ModbusTCP instance
static ModbusEthernet mb;

static bool mb_result = false;

/*******************************LOOKUP TABLE*******************************/

static Tag_details Taginfo[] = 
{
 // Reg_Addr, Tag_Name
  { 0,        "REG 0"  },
  { 2,        "REG 2"  },
  { 4,        "REG 4"  },
  { 6,        "REG 6"  },
  { 8,        "REG 8"  },
  { 10,       "REG 10" },
  { 12,       "REG 12" },
  { 14,       "REG 14" },
  { 16,       "REG 16" },
  { 18,       "REG 18" }
};

/*********************************FUNCTION*********************************/

void ModbusTcpClientInit()
{
    // Act as Modbus TCP client
    mb.client();
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**

bool ModbusConnect()
{
    if(!mb.isConnected(remote))
    {
        Serial.print("Modbus not connected, Connecting to : "); Serial.print(remote);
        if (mb.connect(remote))
        {
            Serial.println(" : Success");
            return true;
        }
        else
        {
            Serial.println(" : Failed");
            return false;
        }
    }
    Serial.println("Modbus already connected");
    return true;
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**

static float FloatConversion(uint16_t val[], int endian)
{
    union
    {
        uint32_t x;
        float f;
    }u;

    unsigned long msb;
    unsigned long lsb;

    if (ENDIAN_LITTLE == endian)
    {
        msb = (unsigned long)val[1];
        lsb = (unsigned long)val[0];
    }
    else
    {
        msb = (unsigned long)val[0];
        lsb = (unsigned long)val[1];
    }

    u.x = ((msb << MODBUS_REGISTER_BITS) | lsb);
    return u.f;

}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**

static bool ParseData(uint16_t *resbuff, int datatype, int endian, float *value)
{
    float result;

    // Do data conversion
    switch (datatype)
    {
        case DATATYPE_FLOAT:
            result = FloatConversion(resbuff, endian);
            break;

        case DATATYPE_INT16:
            result = float(resbuff[0]);
            break;

        default:
            Serial.print("ERROR: Unknown datatype : ");
            Serial.println(datatype);
            return false;
    }

    *value = result;
    return true;
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**

static bool ParseAndSaveTags(uint16_t response[])
{
    for(int i = 0; i < NE(Taginfo); i++)
    {      
        int readindex = Taginfo[i].Register;
        if (false == ParseData(&response[readindex], DATATYPE_FLOAT, ENDIAN_LITTLE, &Taginfo[i].data))
        {
            Serial.print("Parse Failed at Reg :");
            Serial.println(Taginfo[i].Register);
            return false;
        }
               
        // Prepare the Tag information
        // Name & Value
        Serial.print(Taginfo[i].TagName);
        Serial.print(" : ");
        Serial.println(Taginfo[i].data);
        Serial.println(response[readindex]);
    }
        
    return true;
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**

// Callback to monitor errors
static bool cb(Modbus::ResultCode event, uint16_t transactionId, void* data)
{
    if (event != Modbus::EX_SUCCESS)
    {
        // Display Modbus error code
        Serial.printf("ERROR: Modbus result: %02X\n", event);
        mb_result = false;
    }
    else
    {
        mb_result = true;
    }
    return true;
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**

static bool ModbusRead(IPAddress serverip, int regtype, int offset, int count, uint16_t *resbuff)
{
    if (count > MODBUS_MAX_REQUEST_SIZE)
    { 
        Serial.printf_P("Error : Invalid count %d. Max is %d\n\r", count, MODBUS_MAX_REQUEST_SIZE);
        return false;
    }

    // Check if no transaction in progress
    if (mb.isConnected(serverip))
    {
        mb_result = false;

        Serial.println("Going to read registers");
        // REGTYPE_HOLDING 1, REGTYPE_INPUT 0
        switch (regtype) 
        {
            case REGTYPE_HOLDING:
                mb.readHreg(serverip, offset, resbuff, count, cb);
                break; 

            case REGTYPE_INPUT:          
                mb.readIreg(serverip, offset, resbuff, count, cb);
                break;

            default:
                Serial.println("ERROR: Modbus Request Failed");
                return false;
        }

        Serial.println("Read COMPLETED !!");
        // Pulling interval
        delay(100);
        // Common local Modbus task
        mb.task();
        yield(); 
        
        // Check Modbus read status which is set in call back fuction
        return mb_result;
    }
    else
    {
        Serial.println(" ERROR : Modbus server not connected");
        return false;
    }
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**

void Modbus2ReadAllTags()
{
    uint16_t response[MODBUS_MAX_REQUEST_SIZE];

    //Read all Modbus Registers
    if (false == ModbusRead(remote, REGTYPE_HOLDING, START_OFFSET, MB_COUNT, &response[0]))
    {
        // If status is false - then do not proceed - return from here
        Serial.println("ERROR: Modbus Read FAILED!");
        return;
    }
    delay(MODBUS_READ_DELAY); 
    Serial.println("MOdbus Read SUCCESS!");

    //Parse the response Array and Save the each Tags to structure Array of Tags
    if (false == ParseAndSaveTags(response))
    {
        Serial.println("ERROR: Parsing FAILED!");
        return;
    }
    Serial.println("Parsing Response SUCCESS!");
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**

void PublishModbusData()
{
    for(int i = 0; i < NE(Taginfo); i++)
    {
        String msg = String(Taginfo[i].TagName) + " : " + String(Taginfo[i].data);
        debugf("Sending Data : \"%s\" : ", msg.c_str());
        MqttPublish("Modbus/data", msg.c_str());
        delay(100);
    }
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**
