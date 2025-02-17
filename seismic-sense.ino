
#pragma PARTICLE_NO_PREPROCESSOR

#define ATTEMPT_FREQUENCY 2000 * 1000
unsigned int lastPublish;
//bool gotLocation = false;
unsigned int lastLocationRequest = 0;



struct MDM_CELL_INFO {
    int cellId;
    int locationAreaCode;
    int mobileCountryCode;
    int mobileNetworkCode;

    int signalStrength;

    int tAdv;
    int scramblingCode;
    int dlFrequency;
    int rscp_lev;
    int ecn_lev;

    int count;
    bool ok;
    int size;

    MDM_CELL_INFO()
    {
        memset(this, 0, sizeof(*this));
        size = sizeof(*this);
    }
};

void RequestTowerCellID();
void OnCellTowerFound(MDM_CELL_INFO *towerInfo);
MDM_CELL_INFO ourTowerInfo;

void onLocationReceived(const char *event, const char *data);
void onErrorReceived(const char *event, const char *data);

int _cbCELLINFO(int type, const char* buf, int len, MDM_CELL_INFO* data);

// end cell location global variables
int vibe = D2;
/*int led2 = D1;*/

float mag;
float dis;
unsigned long previousMillis;        // will store last time LED was updated
long interval;
int ledState = LOW;
bool dataRecieved;
int vibrationParrtern(int length, int strength);
int dataParse(String incomingData);
String locationData;
// Last time, we only needed to declare pins in the setup function.
// This time, we are also going to register our Particle function

void setup()
{
   Serial.begin(9600);
   Serial.write('setup');
   pinMode(vibe, OUTPUT);
   Particle.function("data", dataParse);
   //location code
   /*Particle.subscribe("hook-response/get_location", onLocationReceived, MY_DEVICES);*/
   /*Particle.subscribe("hook-error/get_location", onErrorReceived, MY_DEVICES);*/
   //end location code
   Particle.variable("location", locationData);
   dataRecieved = false;

   analogWrite(vibe, 255);

   Serial.println();

   /*cellCall();*/
}

void loop(){
  //if (!gotLocation) {
      unsigned int now = millis();

      if (((now - lastLocationRequest) > ATTEMPT_FREQUENCY) || (lastLocationRequest == 0)) {
          dataRecieved = false;
          lastLocationRequest = now;
          RequestTowerCellID();
      }else if(!dataRecieved) {
          // any updates?
          Cellular.command(_cbCELLINFO, &ourTowerInfo, 1000, "");
      }
  //}
}

int dataParse(String incomingData) {
    Serial.write("incoming Data: ");
    Serial.println(incomingData);
    int substringIndex = incomingData.indexOf('n');
    int stringLength = incomingData.length();
    String distance = incomingData.substring(substringIndex+1,stringLength);
    String magnitude = incomingData.substring(0,substringIndex);
    float dis = distance.toFloat();
    float mag = magnitude.toFloat();
    Serial.println(mag);
    Serial.println(dis);
    Serial.write("out Going Data: ");
    Serial.println(mag);
    Serial.println(dis);
    Serial.write("vibrationPattern: ");
    Serial.println();
    interval = mag;
    unsigned long currentMillis = millis();
    Serial.printlnf("currentMillis: %f", currentMillis);
    analogWrite(vibe, dis);
    delay(interval);
    analogWrite(vibe, 255);
    ledState = HIGH;
    Serial.write("vibe off");
    Serial.println();
}
/**
 * Asks the Cellular module for info about the tower it's connected to.
 **/
void RequestTowerCellID() {
    Serial.println("requesting cell tower info...");
    int ret = Cellular.command(_cbCELLINFO, &ourTowerInfo, 15000, "AT+UCELLINFO=1\r\n");
}



/**
 * Called after we've parsed a good response from the cellular module about our tower
 **/
void OnCellTowerFound(MDM_CELL_INFO *towerInfo) {
    /*if (dataRecieved) {
        return;
    }*/

    Serial.println("Current Cell Tower Information:");
    Serial.println("Tower ID:" + String(towerInfo->cellId));
    Serial.println("Location Area Code:" + String(towerInfo->locationAreaCode));
    Serial.println("Mobile Country Code:" + String(towerInfo->mobileCountryCode));
    Serial.println("Mobile Network Code:" + String(towerInfo->mobileNetworkCode));

    Serial.println("Publishing data so the hook can hit the browser location API");

    String json = String::format("{\"id\":\"%d\",\"lac\":\"%d\",\"mcc\":\"%d\",\"mnc\":\"%d\"}",
        towerInfo->cellId,
        towerInfo->locationAreaCode,
        towerInfo->mobileCountryCode,
        towerInfo->mobileNetworkCode
    );
    locationData = json;
    Particle.publish("get_location", json, PRIVATE);
}



void onErrorReceived(const char *event, const char *data) {
    Serial.println("The server sent us an error: " + String(data));
    Serial.println("Lets keep trying!  Sometimes the geolocation server wants us to wait a bit.");
}

/**
 *
 *  Once our webhook responds with our approximate location, lets log it out!
 * But we could also do something with this cloud side as well, since our location will be published
 * as a normal event.
 *
 **/
void onLocationReceived(const char *event, const char *data) {
    if ((data == NULL) || (strlen(data) == 0)) {
        return;
    }

    Serial.println("got location back " + String(data));


    char one[32], two[32], three[32];
    float latitude, longitude, uncertainty;
    //sscanf(someString, "%f,%f,%f", &latitude, &longitude, &altitude);
    sscanf(data, "%[^,],%[^,],%[^,]", one, two, three);

    latitude = String(one).toFloat();
    longitude = String(two).toFloat();
    uncertainty = String(three).toFloat();

    if (latitude ==  0) {
        Serial.println("nevermind");
        return;
    }

    //lets stop asking for location updates (so we don't burn a ton of data)
    //TODO: we could check cell tower id every few minutes, and request a location update
    // when that changes.


    Serial.println(String::format("Parsed into %f, %f, %f ", latitude, longitude, uncertainty));
    Serial.println(String::format("You're on a map! https://www.google.com/maps/place/%f,%f", latitude, longitude));

    String dataJson = String("{")
        + "\"lat\":" + String::format("%f", latitude)
        + ",\"lng\":" + String::format("%f", longitude)
        + ",\"unc\":" + String::format("%f", uncertainty)
        + "}";

    Particle.publish("current_location", dataJson, 60, PRIVATE);

    //gotLocation = true;
}



/* Cell Info Callback */
int _cbCELLINFO(int type, const char* buf, int len, MDM_CELL_INFO* data){
    if ((type != TYPE_PLUS) || !data) {
        return WAIT;
    }

    //Serial.println("Heard: " + String(buf));

    int mode;

    //  +UCELLINFO:       1,     2,  310,  410, 7f05, 57b14e2, 395, 1032, 255, 255
    //+UCELLINFO: <mode>,<type>,<MCC>,<MNC>,<LAC>,<CI>,<scrambling_code>,<dl_frequency>,<rscp_lev>,<ecn0_lev>

    MDM_CELL_INFO localData;
    int count = sscanf(buf, "\r\n+UCELLINFO: %d, %d, %d, %d, %x, %x, %d, %d, %d, %d\r\n",
        &mode,
        &type,

        &localData.mobileCountryCode,
        &localData.mobileNetworkCode,

        // hex
        &localData.locationAreaCode,
        &localData.cellId,

        &localData.scramblingCode,
        &localData.dlFrequency,
        &localData.rscp_lev,
        &localData.ecn_lev
    );

    if ( count <= 0 )
    {
        // bail if nothing matched
        return WAIT;
    }


    if ((localData.mobileCountryCode != 0) && (localData.mobileCountryCode != 65535)) {

        Serial.println("got good cellinfo response " + String(buf));

        data->mobileCountryCode = localData.mobileCountryCode;
        data->mobileNetworkCode = localData.mobileNetworkCode;
        data->locationAreaCode = localData.locationAreaCode;
        data->cellId = localData.cellId;

        data->scramblingCode = localData.scramblingCode;
        data->dlFrequency = localData.dlFrequency;
        data->rscp_lev = localData.rscp_lev;
        data->ecn_lev = localData.ecn_lev;

        data->count = count;
        data->ok = true;

        dataRecieved = true;
        OnCellTowerFound(data);
    }

    return WAIT;
}
