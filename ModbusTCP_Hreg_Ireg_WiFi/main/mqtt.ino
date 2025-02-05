/* ****************************************************************************
 * Company   : Consyst Digital Industries Pvt Ltd
 * Author    : Huwairis Ibnu Kabeer
 * email     : huwairisik@consyst.biz
 * Date      : 14/11/2024
 * File name : mqtt.ino
 * ****************************************************************************/
 
/**********************************MACROS**********************************/

#define MQTT_HOST_NAME  "f64d3fa1a1194397b27310c905c13c92.s1.eu.hivemq.cloud"
#define MQTT_PORT       8883
#define CLIENT_ID       "MicrEdge300Gateway"
#define MQTT_USR_NAME   "micredge"
#define MQTT_PASSWORD   "IIoTNextMicrEdge350"

/**********************************STRUCT**********************************/



/**********************************GLOBAL**********************************/

PubSubClient mqttclient;
static String clientId;
const char root_ca_IIoTNext[] PROGMEM = 
"-----BEGIN CERTIFICATE-----\n"
"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\n"
"-----END CERTIFICATE-----\n";

/*******************************LOOKUP TABLE*******************************/



/*********************************FUNCTION*********************************/

void SetCACert()
{
    WifiClientSecureMqtt.setCACert(root_ca_IIoTNext);
}

void BeginMqtt()
{
    SetCACert();
    // Set client for GSM
    mqttclient.setClient(WifiClientSecureMqtt);
    mqttclient.setServer(MQTT_HOST_NAME, MQTT_PORT);

    // Create a random client ID   
    clientId = String(CLIENT_ID) + "-" + String(random(0xffff), HEX);
    debugf("Completed\r\n");
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**

void showMqttError(int state)
{    
    debugf("\r\n");
    debugf("Failed, state(%d) : \r\n", state);
    switch (state) {
        case MQTT_CONNECTION_TIMEOUT:      debugf("CNT_TMOUT\r\n");     break;
        case MQTT_CONNECTION_LOST:         debugf("CNT_LST\r\n");       break;
        case MQTT_CONNECT_FAILED:          debugf("CNT_FL\r\n");        break;
        case MQTT_DISCONNECTED:            debugf("DSCNT\r\n");         break;
        case MQTT_CONNECTED:               debugf("CNT\r\n");           break;
        case MQTT_CONNECT_BAD_PROTOCOL:    debugf("CNT_BAD_PROTO\r\n"); break;
        case MQTT_CONNECT_BAD_CLIENT_ID:   debugf("CNT_BAD_ID\r\n");    break;
        case MQTT_CONNECT_UNAVAILABLE:     debugf("CNT_UNAVLBL\r\n");   break;
        case MQTT_CONNECT_BAD_CREDENTIALS: debugf("CNT_BAD_CRED\r\n");  break;
        case MQTT_CONNECT_UNAUTHORIZED:    debugf("CNT_UNAUTH\r\n");    break;
        default:                           debugf("Unknown\r\n");       break;
    }
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**

bool MqttReconnect() 
{
    unsigned int retrycount = 3;
    // Loop until we're reconnected
    unsigned int count = 0;
    while (!mqttclient.connected()) 
    {                
        // Attempt to connect
        debugf("clientId %s\n\r", clientId.c_str());
        debugf("Connecting to MQTT.....\r\n");
        if (mqttclient.connect(clientId.c_str(), MQTT_USR_NAME, MQTT_PASSWORD, 0, 0, 0, 0, 1)) 
        {
            debugf("Connected!\r\n");
            return true;
        }
        else 
        {
            count++;
            showMqttError(mqttclient.state());
            debugf("Wait for 1 second\n\r");
            delay(1000);
            if (count >= retrycount)
            {
                return false;
            }
            debugf("Trying Again\n\r");
        }
    }
    return true;
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**

bool IsMqttConnected()
{
    bool mStatus = mqttclient.connected();
    return mStatus; 
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**

bool MqttConnect()
{
    if (!IsMqttConnected()) 
    {
        return MqttReconnect();
    }
    else
    {
        return true;
    }
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**

void HandleMqtt()
{
    // It checks for any messages that need to be handled
    // such as incoming messages that have been published
    // to topics that the client is subscribed to
    mqttclient.loop();
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**

bool MqttPublish(const char *topic, const char *msg)
{
    if (false == mqttclient.publish(topic, msg))
    {
        debugf("PUBLISH ERROR !!\r\n");
        return false;
    }
    debugf("PUBLISH SUCCESS !!\r\n");
    return true;
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**
