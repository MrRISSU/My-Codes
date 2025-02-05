# Modbus RTU Byte Order Testing  

---  

**Author**: Huwairis Ibnu Kabeer  
**Date**: 07/01/2025  
**MCU**: ESP32-S3-WROOM-1U  

---  

## Overview  
This trial code aims to facilitate the reading of data from both Holding Registers (HRegs) and Input Registers (IRegs) of a Modbus RTU slave device. It supports various byte orders and data types to ensure compatibility and ease of use in different applications.  

---  

## Supported Byte Orders  
- **Little Endian**: Data stored with the least significant byte first.  
- **Big Endian**: Data stored with the most significant byte first.  
- **Little Endian Byte Swapped**: Option for applications requiring byte-swapped representations.  
- **Big Endian Byte Swapped**: Alternative for handling byte-swapped data with big-endian formats.  

## Supported Data Types  
- **INT 16**: 16-bit signed integer.  
- **INT 32**: 32-bit signed integer.  
- **FLOAT 32**: 32-bit floating point.  

---  

## Libraries Used  
### 1. Modbus-ESP8266  
- **Version**: Custom Library  
- **Link**: Located in `iiotnext-edge\Code\Libraries` under the **DLMS branch**.  

### 2. EspSoftwareSerial  
- **Version**: 7.0.0  
- **Link**: [EspSoftwareSerial GitHub Repository](https://github.com/plerup/espsoftwareserial/)  

---  

## Usage Instructions  
To utilize this code for reading data from Modbus RTU devices:  
1. Set up the ESP32-S3-WROOM-1U with the necessary libraries.  
2. Configure the Modbus parameters (slave address, baud rate, etc.).  
3. Choose the desired byte order and data type for your readings.  
4. Implement the reading functions as demonstrated in the code.  

---  

## Further Information  
For further details and in-depth examples, refer to the comments in the code and the documentation for each library. Ensure that the connections and wiring for the Modbus devices are correctly configured according to the respective specifications.  

---  

## Conclusion  
With the modbus RTU byte order testing implementation, this code provides a flexible solution for reading various data types efficiently from Modbus RTU slaves, accommodating diverse byte order requirements.  


___________________________________________END_OF_FILE__________________________________________________