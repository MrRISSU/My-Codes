// Hardware Revision ADC Pin
#define HARDWARE_REV_PIN 1

typedef struct 
{
    uint16_t Revision;
    uint16_t RawMaxVal;
}HWRevTable;

HWRevTable hardwares[] =
{  
    { 1, 820  },
    { 2, 1640 },  
    { 3, 2460 }, 
    { 4, 3278 },
    { 5, 4095 }    
};

static void SetHardwareRevPin()
{
    pinMode(HARDWARE_REV_PIN, INPUT);
}

static uint16_t ReadHardwareRevPin()
{
    return analogRead(HARDWARE_REV_PIN);
}

uint16_t FindHardwareRev()
{   
    SetHardwareRevPin();    
    uint16_t RawVal = ReadHardwareRevPin();

    int i;
    for (i = 0; i < NE(hardwares); i++)
    {
        if (RawVal <= hardwares[i].RawMaxVal)
        {
            break;  
        }
    } 
      
    return hardwares[i].Revision;     
}
