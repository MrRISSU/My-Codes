/* ****************************************************************************
 * Company   : Consyst Digital Industries Pvt Ltd
 * Author    : Huwairis Ibnu Kabeer
 * email     : huwairisik@consyst.biz
 * Date      : 07/01/2025
 * File name : Modbus_read.ino
 * ****************************************************************************/
 
/**********************************MACROS**********************************/

#define MODBUS_MAX_REQUEST_SIZE 30
#define REGBUFFERSIZE           64
#define MODBUS_READ_DELAY       50

// For Micredge-101
//#define MODBUS_RX        4
//#define MODBUS_TX        5
//#define ENABLEPIN        16

// For NodeMCU-32S
//#define MODBUS_RX        4
//#define MODBUS_TX        5
//#define ENABLEPIN        16

// For Micredge-300
#define MODBUS_RX        18
#define MODBUS_TX        17
#define ENABLEPIN        8

#define MODBUSREGISTERBITS 16
#define WARNINGLESS        1

/**********************************GLOBAL**********************************/

SoftwareSerial modSerial(MODBUS_RX, MODBUS_TX); 
ModbusRTU mb;
static bool mb_result = false;

/*********************************FUNCTION*********************************/

void ModMasterBegin(int buadrate, SoftwareSerialConfig swSerial)
{    
    modSerial.begin(buadrate, swSerial);
    mb.begin(&modSerial, ENABLEPIN, HIGH);
    mb.master();
}
//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**

// Swaps bytes in a 16 bit data
uint16_t swapBytes(uint16_t value)
{
    return (value << 8) | (value >> 8);
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**

int32_t Int32Conversion(uint16_t val[], int endian)
{   
    int32_t result;
    int32_t msb = 0;
    int32_t lsb = 0;

    switch(endian)
    {
        case ENDIAN_LITTLE:
            debugf("Byte Order: %s\r\n", "Little Endian");
            msb = (int32_t)val[1];
            lsb = (int32_t)val[0];
            break;
  
        case ENDIAN_BIG:
            debugf("Byte Order: %s\r\n", "Big Endian");
            msb = (int32_t)val[0];
            lsb = (int32_t)val[1];
            break;
  
        case ENDIAN_LITTLE_BYTE_SWAP:
            debugf("Byte Order: %s\r\n", "Little Endian Byte Swapped");
            msb = val[1];
            debugf("MSB: 0x0%X\r\n", msb);
            msb = swapBytes(msb);
            debugf("MSB (swped): 0x0%X\r\n", msb);

            lsb = val[0];
            debugf("LSB: 0x0%X\r\n", lsb);
            lsb = swapBytes(lsb);
            debugf("LSB (swped): 0x0%X\r\n", lsb);
            break;
  
        case ENDIAN_BIG_BYTE_SWAP:
            debugf("Byte Order: %s\r\n", "Big Endian Byte Swapped");
            msb = (int32_t)swapBytes(val[0]);
            lsb = (int32_t)swapBytes(val[1]);
            break;
        default:
            Serial.printf("Unknown Byte Order\r\n");
            break;
    }
    return result = (int32_t)((msb << MODBUSREGISTERBITS) | lsb);
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**

float FloatConversion(uint16_t val[], int endian)
{
    union
    {
        uint32_t x;
        float f;
    }u;

    unsigned long msb = 0;
    unsigned long lsb = 0;
    switch(endian)
    {
        case ENDIAN_LITTLE:
            debugf("Byte Order: %s\r\n", "Little Endian");
            msb = (unsigned long)val[1];
            lsb = (unsigned long)val[0];
            break;
  
        case ENDIAN_BIG:
            debugf("Byte Order: %s\r\n", "Big Endian");
            msb = (unsigned long)val[0];
            lsb = (unsigned long)val[1];
            break;
  
        case ENDIAN_LITTLE_BYTE_SWAP:
            debugf("Byte Order: %s\r\n", "Little Endian Byte Swapped");
            msb = (unsigned long)swapBytes(val[1]);
            lsb = (unsigned long)swapBytes(val[0]);
            break;
  
        case ENDIAN_BIG_BYTE_SWAP:
            debugf("Byte Order: %s\r\n", "Big Endian Byte Swapped");
            msb = (unsigned long)swapBytes(val[0]);
            lsb = (unsigned long)swapBytes(val[1]);
            break;
        default:
            debugf("Unknown Byte Order\r\n");
            break;
    }
    u.x = ((msb << MODBUSREGISTERBITS) | lsb);
    return u.f;
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**

// Callback to monitor errors
bool cb(Modbus::ResultCode event, uint16_t transactionId, void* data)
{
#if WARNINGLESS == 1
    if (WARNINGLESS)
    {
        Serial.println(transactionId);
        Serial.println((const char*) data);
    }       
#endif

    if (event != Modbus::EX_SUCCESS)
    {
        Serial.print("Request result: 0x");
        Serial.println(event, HEX);
        mb_result = false;
    }
    else
    {
        mb_result = true;
    }
    return true;
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**

bool ParseData(uint16_t *resbuff, int datatype, int endian, float *value)
{
    float result;

    // Do data conversion
    switch (datatype)
    {
        case DATATYPE_FLOAT:
            debugf("Data Type: %s\r\n", "Float 32");
            result = FloatConversion(resbuff, endian);
            break;

        case DATATYPE_INT32:
            debugf("Data Type: %s\r\n", "Int 32");
            result = (float)Int32Conversion(resbuff, endian);
            break;

        case DATATYPE_INT16:
            debugf("Data Type: %s\r\n", "Int 16");
            result = float(resbuff[0]);
            break;

        default:
            debugf("ERROR: Unknown datatype : %d\r\n", datatype);
            return false;
    }

    *value = result;
    return true;
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**

bool ModbusRead(int slaveid, int regtype, int offset, int count, uint16_t resbuff[4])
{
    memset(resbuff, 0, (2 * 4));

    if (count > REGBUFFERSIZE)
    { 
        Serial.printf_P("Error : Invalid count %d. Max is %d\n\r", count, REGBUFFERSIZE);
        return false;
    }
    
    // Check if no transaction in progress
    if (!mb.slave())
    {
        mb_result = false;

        switch (regtype) // REGTYPE_HOLDING 1, REGTYPE_INPUT 0
        {

            case REGTYPE_HOLDING:
                debugf("Register Type: %s\r\n", "Holding Register");
                mb.readHreg(slaveid, offset, resbuff, count, cb);
                break;

            case REGTYPE_INPUT:
                debugf("Register Type: %s\r\n", "input Register");
                mb.readIreg(slaveid, offset, resbuff, count, cb);
                break;

            default:
                debugf("ERROR: Modbus Request Failed\r\n");
                return false;
        }

        // Check if transaction is active
        while (mb.slave())
        {
            mb.task();
            delay(10);
        }
        // Check Modbus read status
        // If status is false - then do not proceed - return from here
        if (mb_result == false)
        {
            debugf("Modbus Read Failed\r\n");
            return false;
        }

        debugf("Res Buf:\r\n");
        for(int i =0; i < 4; i++)
        {
          debugf("Res Buf[%d]: %X\r\n", i, resbuff[i]);
        }
        
        // Modbus Read is Successful!!
        return true;
    }
    else
    {
        debugf(" ERROR : Modbus slave is busy\r\n");
        return false;
    }
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**

void ModbusReadTags(int slaveID, int regType, int dataType, int byteOreder, int addr, const char* ModTag)
{ 
    uint16_t response[MODBUS_MAX_REQUEST_SIZE];
    float val;
    debugf("--------------------------------------\r\n");
    debugf("Slave ID: %d\r\n", slaveID);
    if (false == ModbusRead(slaveID, regType, addr ,2 ,&response[0]))
    {
       debugf("Modbus read Error !!!\r\n");
       return;
    }

    if (false == ParseData(&response[0], dataType, byteOreder, &val))
    {
       debugf("Parsing Failed !!!\r\n");
       return;
    }
    debugf("%s: %lf\r\n", ModTag, val);
    debugf("--------------------------------------\r\n");
    delay(MODBUS_READ_DELAY); 
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**
