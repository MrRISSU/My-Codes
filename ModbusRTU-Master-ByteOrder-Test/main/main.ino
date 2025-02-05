/* ****************************************************************************
 * Company   : Consyst Digital Industries Pvt Ltd
 * Author    : Huwairis Ibnu Kabeer
 * email     : huwairisik@consyst.biz
 * Date      : 07/01/2025
 * File name : main.ino
 * ****************************************************************************/

/*******************************HEADER FILES*******************************/

#include <ModbusRTU.h>
#include <SoftwareSerial.h>

/**********************************MACROS**********************************/

#define debugf(...)     Serial.printf(__VA_ARGS__)
//// Modbus Config ////
#define SLAVE_ID                1
// Byte order
#define ENDIAN_LITTLE           0
#define ENDIAN_BIG              1
#define ENDIAN_LITTLE_BYTE_SWAP 2
#define ENDIAN_BIG_BYTE_SWAP    3
// Register Type
#define REGTYPE_HOLDING         1
#define REGTYPE_INPUT           0
// Data Type
#define DATATYPE_INT16          0
#define DATATYPE_INT32          1
#define DATATYPE_FLOAT          2


/*********************************FUNCTION*********************************/

void setup()
{
    Serial.begin(115200);
    ModMasterBegin(9600, SWSERIAL_8N1);
}

void loop()
{
    debugf("--------------- READING ALL TAGS ----------------\r\n");
    // Test with diffrent byte order for INT 32
    ModbusReadTags(SLAVE_ID, REGTYPE_INPUT, DATATYPE_INT32, ENDIAN_LITTLE,           0, "Data_1");
    ModbusReadTags(SLAVE_ID, REGTYPE_INPUT, DATATYPE_INT32, ENDIAN_LITTLE_BYTE_SWAP, 2, "Data_2");
    ModbusReadTags(SLAVE_ID, REGTYPE_INPUT, DATATYPE_INT32, ENDIAN_BIG,              4, "Data_3");
    ModbusReadTags(SLAVE_ID, REGTYPE_INPUT, DATATYPE_INT32, ENDIAN_BIG_BYTE_SWAP,    6, "Data_4");

    // Test with diffrent byte order for FLOAT
    ModbusReadTags(SLAVE_ID, REGTYPE_INPUT, DATATYPE_FLOAT, ENDIAN_LITTLE,           8, "Data_1");
    ModbusReadTags(SLAVE_ID, REGTYPE_INPUT, DATATYPE_FLOAT, ENDIAN_LITTLE_BYTE_SWAP, 10, "Data_2");
    ModbusReadTags(SLAVE_ID, REGTYPE_INPUT, DATATYPE_FLOAT, ENDIAN_BIG,              12, "Data_3");
    ModbusReadTags(SLAVE_ID, REGTYPE_INPUT, DATATYPE_FLOAT, ENDIAN_BIG_BYTE_SWAP,    14, "Data_4");
    delay(1000);
}

/**************************************************************************/
