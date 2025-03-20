/* ****************************************************************************
 * Company   : Consyst Digital Industries Pvt Ltd
 * Author    : Huwairis Ibnu Kabeer
 * email     : huwairisik@consyst.biz
 * Date      : 20/03/2025
 * File name : gsm_location.ino
 * ****************************************************************************/

/*******************************HEADER FILES*******************************/

/**********************************MACROS**********************************/


#define GSM_MAX_SERVING_CELLS  10
/**********************************STRUCT**********************************/

typedef struct CellTower
{
    String mcc;
    String mnc;
    long lac;
    long cellId;
}CellTower;

typedef struct Location
{
    float lat;
    float lon;
    bool valid;
}Location;

/**********************************GLOBAL**********************************/

// OpenCellID API key (replace with your actual API key)
const char* api_key = "pk.ab2157ad0d5bc052b1dc25edb9ae896a";

/*******************************LOOKUP TABLE*******************************/

/*********************************FUNCTION*********************************/

bool ParseCellTower(const String& line, CellTower& tower)
{
    // Split the string by commas
    char buffer[50];
    line.toCharArray(buffer, 50);
    char* token = strtok(buffer, ",");
    int index = 0;
    String fields[7];

    while (token != NULL && index < 7)
    {
        fields[index++] = String(token);
        token = strtok(NULL, ",");
    }

    if (index < 7)
    {
        debugf("Invalid gsmLocParamLines format: %s\r\n", line.c_str());
        return false;
    }

    // Assign fields based on assumed format: "0709,38,05,4bb6,404,95,c1c0"
    tower.lac = strtol(fields[0].c_str(), NULL, 16);    // LAC (hex to decimal)
    tower.cellId = strtol(fields[3].c_str(), NULL, 16); // Cell ID (hex to decimal)
    tower.mcc = fields[4];                              // MCC
    tower.mnc = fields[5];                              // MNC

    return true;
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**

Location GetLocationFromCellTower(const CellTower& tower)
{
    Location loc = {0.0, 0.0, false};
    String apiKey = "YOUR_OPENCELLID_API_KEY"; // Replace with your API key
    String url = "https://opencellid.org/cell/get?key=" + apiKey +
                 "&mcc=" + tower.mcc +
                 "&mnc=" + tower.mnc +
                 "&lac=" + String(tower.lac) +
                 "&cellid=" + String(tower.cellId) +
                 "&format=json";

    debugf("Querying OpenCellID: %s\r\n", url.c_str());

    if (!httpsClient.connect("opencellid.org", HTTPS_PORT))
    {
        debugf("Connection to OpenCellID failed!\r\n");
        return loc;
    }

    // Send HTTP GET request
    httpsClient.print(String("GET ") + url + " HTTP/1.0\r\n" +
                      "Host: opencellid.org\r\n" +
                      "Connection: close\r\n\r\n");

    // Skip headers
    while (httpsClient.connected())
    {
        String line = httpsClient.readStringUntil('\n');
        if (line == "\r") break;
    }

    // Read response
    String response;
    while (httpsClient.available())
    {
        response += (char)httpsClient.read();
    }
    httpsClient.stop();

    // Parse JSON response
    DynamicJsonDocument doc(200);
    deserializeJson(doc, response);
    if (doc.containsKey("lat") && doc.containsKey("lon"))
    {
        loc.lat = doc["lat"].as<float>();
        loc.lon = doc["lon"].as<float>();
        loc.valid = true;
        debugf("Location: lat=%f, lon=%f\r\n", loc.lat, loc.lon);
    }
    else
    {
        debugf("No valid location in response: %s\r\n", response.c_str());
    }

    return loc;
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**

bool PrepGsmLocInfo()
{
    doc_out.clear();
    doc_out["info_gsm_mode"] = currentMode;

    // Process each cell tower entry
    for (int i = 0; (i < lineCount) && (i < NE(gsmLocParamLines)); i++)
    {
        CellTower tower;
        if (ParseCellTower(gsmLocParamLines[i], tower))
        {
            Location loc = GetLocationFromCellTower(tower);
            if (loc.valid)
            {
                // Store raw data and coordinates
                doc_out["info_gsm_loc"][i]["raw"] = gsmLocParamLines[i];
                doc_out["info_gsm_loc"][i]["lat"] = loc.lat;
                doc_out["info_gsm_loc"][i]["lon"] = loc.lon;
            }
            else
            {
                // Store raw data only if location retrieval fails
                doc_out["info_gsm_loc"][i]["raw"] = gsmLocParamLines[i];
                debugf("Failed to get location for: %s\r\n", gsmLocParamLines[i].c_str());
            }
        }
        else
        {
            doc_out["info_gsm_loc"][i]["raw"] = gsmLocParamLines[i];
            debugf("Failed to parse: %s\r\n", gsmLocParamLines[i].c_str());
        }
    }

    return true;
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**
