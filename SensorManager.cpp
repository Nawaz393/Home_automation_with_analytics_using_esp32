#include "SensorManager.h"
#include "FirebaseManager.h"
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>
#include "Globals.h"




#define DHTPIN 14
#define DHTTYPE DHT11
bool isFirstTime = true;
float temp=0;
float hum=0;
float light=0;

DHT dht(DHTPIN, DHTTYPE);
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);
// unsigned long previousMillis = 0;  
const long interval = 5000;

void setupSensors() {
timeClient.begin();
    // Set your time offset from UTC in seconds
    timeClient.setTimeOffset(3600); 


    dht.begin();
    if (!tsl.begin()) {
        Serial.println("Failed to initialize TSL2561 sensor");
    }
}

void sendSensorDataToFirebase() {
    FirebaseJson json;
    sensors_event_t event;
    tsl.getEvent(&event); 

    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    float lightIntensity = event.light;

    if (!isFirstTime && temp == temperature && hum == humidity && light == lightIntensity) {
        Serial.println("Data is the same; skipping Firebase update.");
        return;
    }

    temp = temperature;
    hum = humidity;
    light = lightIntensity;

    // Update the NTP client and get the current time
    timeClient.update();
    long epochTime = timeClient.getEpochTime();

    // Add sensor data to JSON
    json.add("temperature", temperature);
    json.add("humidity", humidity);
    json.add("lightIntensity", lightIntensity);
    json.add("timestamp", epochTime);

    if (!Firebase.pushJSON(fbdo, "/sensorData", json)) {
        Serial.println("Failed to send sensor data to Firebase");
    }
    isFirstTime=false;
    
}

// void sendSensorDataPeriodically() {



//     unsigned long currentMillis = millis();
//     if (currentMillis - previousMillis >= interval) {
//         previousMillis = currentMillis;
//         if (Firebase.ready()) {
//             sendSensorDataToFirebase();
//         }
//     }
// }


float getTemperature(){
  return dht.readTemperature();
}

float getLightIntensity(){
  sensors_event_t event;
    tsl.getEvent(&event); 
    return event.light;
}


float getHumidity(){
  return dht.readHumidity();

}