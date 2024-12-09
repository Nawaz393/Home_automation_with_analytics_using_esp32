#include "HardwareSerial.h"
#include "RelayManager.h"
#include "SensorManager.h"
#include "Globals.h"


const int relayPins[] = {2, 4, 5, 18};
bool relayStates[] = {false, false, false, false};
float numOfWatts[] = {13, 45, 45, 100};
const int numRelays = sizeof(relayPins) / sizeof(relayPins[0]);
unsigned long startTime[numRelays] = {0};
unsigned long elapsedTime[numRelays] = {0};
bool status[]={false,false,false,false};
const long interval = 120000;
unsigned long previousMillis = 0;
float accumulatedPower=0.0;

void setupRelays() {
    for (int i = 0; i < numRelays; i++) {
        pinMode(relayPins[i], OUTPUT);
        digitalWrite(relayPins[i], relayStates[i] ? LOW : HIGH);
    }
}

void streamCallback(StreamData data) {
    Serial.println("Stream Data received...");

    // Print the path and type of data received
    Serial.println("Path: " + data.streamPath());
    Serial.println("Type: " + data.dataType());

    if (data.dataTypeEnum() == firebase_rtdb_data_type_boolean) {
        bool value = data.to<bool>();
        // Mapping dataPath to relay index
        const String relayPaths[] = {"/relay1", "/relay2", "/relay3", "/relay4"};
        for (int i = 0; i < numRelays; i++) {
            if (data.dataPath() == relayPaths[i]) {
                relayStates[i] = value;
                digitalWrite(relayPins[i], relayStates[i] ? LOW : HIGH);
                Serial.println(relayPaths[i] + ": ");
                Serial.println(relayStates[i] ? "true" : "false");
                break; // Exit the loop once the matching path is found
            }
        }
    }
}

void streamTimeoutCallback(bool timeout) {
    if (timeout) {
        Serial.println("Stream timeout, attempting to reconnect...");
    }
}

bool fetchAndSetRelayStates(FirebaseData &fbdo) {
    const String relayNodes[] = {"/relay1", "/relay2", "/relay3", "/relay4"};
    bool success = true;

    for (int i = 0; i < numRelays; i++) {
        if (!Firebase.getBool(fbdo, "relays" + relayNodes[i])) {
            Serial.println("Failed to get data for relays");
            Serial.println(relayNodes[i]);
            Serial.println(": ");
            Serial.println(fbdo.errorReason());
            success = false;
        } else {
            relayStates[i] = fbdo.to<bool>();
            digitalWrite(relayPins[i], relayStates[i] ? HIGH : LOW);
        }
    }

    return success;
}

void updatePowerConsumption(int relayIndex,FirebaseData &fbdo) {
    if (relayStates[relayIndex] && !status[relayIndex]) {
        // Relay turned on, start timing
        status[relayIndex]=true;
        startTime[relayIndex] = millis();
    } else if(!relayStates[relayIndex] && status[relayIndex]) {
        // Relay turned off, calculate power consumption
        status[relayIndex]=false;
        elapsedTime[relayIndex] = millis() - startTime[relayIndex];
        if(elapsedTime[relayIndex]>0){
        float powerConsumption = calculatePowerConsumption(numOfWatts[relayIndex], elapsedTime[relayIndex]);
        // Send data to Firebase
        sendPowerConsumptionToFirebase(fbdo, powerConsumption);
        }
        startTime[relayIndex] = 0;
        elapsedTime[relayIndex] = 0;
    }
}

void sendPowerConsumptionToFirebase(FirebaseData &fbdo, float powerConsumption) {
  FirebaseJson json;

  // Update time client (assuming it's necessary)
  timeClient.update();
  long epochTime = timeClient.getEpochTime();

  String path = "/power_consumption";

  // Set values directly as their data types
  json.set("consumption", powerConsumption);
  json.set("timestamp", epochTime);

  if (Firebase.pushJSON(fbdo, path, json)) {
    Serial.println("Power consumption data sent to Firebase successfully.");
  } else {
    Serial.println("Failed to send power consumption data to Firebase: " + fbdo.errorReason());
    // Add more robust error handling here (optional)
  }
}

float calculatePowerConsumption(float applianceWattage, unsigned long operatingTimeInMs) {
  // Convert milliseconds to hours
  float operatingTimeInHours = operatingTimeInMs / (float)(3600 * 1000);
  
  // Calculate energy consumption in watt-hours
  float energyConsumedInWh = applianceWattage * operatingTimeInHours;
  
  return energyConsumedInWh;
}



float calculatePowerConsumptionOfAppliances(){
float consumption=0.0;
  for (int i=0;i<4;i++){
 if (relayStates[i]) {
  consumption+=numOfWatts[i];
 }
  }
return consumption;
}
void accumulatePowerAndSensorDataAndSend( FirebaseData &fbdo) {
    static unsigned long lastCalcMillis = 0;
    unsigned long currentMillis = millis();
    
    // Calculate power consumption every second
    if (currentMillis - lastCalcMillis >= 1000) {
        lastCalcMillis = currentMillis;
        float powerConsumption = calculatePowerConsumptionOfAppliances();
        accumulatedPower += powerConsumption / 3600.0; // Convert power to watt-hours (Wh)
    }

    // Check if 5 minutes have passed to send data to Firebase
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;

        // Assume these functions are defined elsewhere to get the current sensor values
        float lightIntensity = getLightIntensity();
        float humidity = getHumidity();
        float temperature = getTemperature();

        // Assume FirebaseData is defined and properly set up elsewhere
       
        sendAllDataToFirebase(lightIntensity, humidity, temperature, accumulatedPower, fbdo);

        // Reset accumulated power after sending
        accumulatedPower = 0.0;
    }
}




void sendAllDataToFirebase(float lightIntensity,float humidity,float temprature,float powerConsumption,FirebaseData &fbdo){
   FirebaseJson json;

  // Update time client (assuming it's necessary)
  timeClient.update();
  long epochTime = timeClient.getEpochTime();

  String path="/room_data";
  


  Serial.print("powerConsumption: ");
  Serial.println( powerConsumption);
  Serial.print("lightIntensity: ");
  Serial.println(lightIntensity);
  Serial.print("temprature: ");
  Serial.println(temprature);
  Serial.print("humidity: ");
  Serial.println(humidity);
  Serial.print("timestamp: ");
  Serial.println(epochTime);

  json.set("power_consumption",powerConsumption);
  json.set("light_intensity",lightIntensity);
  json.set("temprature",temprature);
  json.set("humidity",humidity);
  json.set("timestamp",epochTime);
  if (Firebase.pushJSON(fbdo, path, json)) {
    Serial.println("Power consumption data sent to Firebase successfully.");
  } else {
    Serial.println("Failed to send power consumption data to Firebase: " + fbdo.errorReason());
    // Add more robust error handling here (optional)
  }

}


