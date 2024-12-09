#include "WIFIManager.h"
#include "FirebaseManager.h"
#include "SensorManager.h"
#include "RelayManager.h"
#include "esp_task_wdt.h"

void setup() {
    Serial.begin(115200);
    setupWiFi();
    setupFirebase();
    setupSensors();
    if (!fetchAndSetRelayStates(fbdo)) {
        Serial.println("Failed to fetch initial relay states from Firebase.");
    }
    setupRelays();


}

void loop() {
    maintainConnection();
    // sendSensorDataPeriodically();
    handleFirebaseStream();
   accumulatePowerAndSensorDataAndSend(fbdo);
   esp_task_wdt_reset();

    // for (int i=0;i<4;i++){
    //   updatePowerConsumption(i,fbdo);
    // }

  

  
}
