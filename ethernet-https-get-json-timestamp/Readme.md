# Get Time From AWS Lambda API via Ethernet  

**Author**: Huwairis Ibnu Kabeer | CS DIGITAL  
**Date**: 10/01/2025  
**MCU**: ESP32-S3-WROOM-1U  

---  

## Overview  

This code retrieves a timestamp from the AWS Lambda API. It receives a JSON response, which is then deserialized and printed. The primary goal is to obtain the response using an **Ethernet network**.  

---  

## Libraries Used  

### 1. SPI  
- **Version**: Hardware Library  

### 2. Ethernet  
- **Version**: 2.0.2  
- **Link**: [Ethernet Library Arduino Reference](https://docs.arduino.cc/libraries/ethernet/)  

### 3. SSLClient  
- **Version**: 1.6.11  
- **Link**: [SSLClient GitHub Repository](https://github.com/OPEnSLab-OSU/SSLClient)  

### 4. ArduinoJson  
- **Version**: 6.21.3  
- **Link**: [ArduinoJson Library Reference](https://arduinojson.org/?utm_source=meta&utm_medium=library.properties)  

---  

## Usage of `pycert_bearssl` Tool  

The `pycert_bearssl` tool is located in the `SSLClient` library folder. Within the `tools/pycert_bearssl` directory, you will find a Python script named `pycert_bearssl.py`. This script is used to download the SSL certificate required for secure communication with the AWS Lambda API.  

### Steps to Execute the Script  

1. **Navigate to the Script Directory**:  
   Open your terminal and navigate to the `SSLClient/tools/pycert_bearssl` folder.  

2. **Run the Script**:  
   Execute the script with the desired URL as a command line argument:  
   ```bash  
   python3 pycert_bearssl.py download loot5pcssa.execute-api.ap-south-1.amazonaws.com
3. **Confirmation Message**:
   Upon successful execution, you should see the following message in the terminal:
   *Retrieved certificate for loot5pcssa.execute-api.ap-south-1.amazonaws.com.*
4. **Certificate Location**:
   The retrieved SSL certificate will be updated in the `certificates.h` file in `tools/pycert_bearssl` directory. You can include this file in your code to establish a secure connection.