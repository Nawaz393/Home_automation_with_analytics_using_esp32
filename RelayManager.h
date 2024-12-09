#ifndef RELAY_MANAGER_H
#define RELAY_MANAGER_H

#include <FirebaseESP32.h>


void setupRelays();                                 
void streamCallback(StreamData data);
void streamTimeoutCallback(bool timeout);
bool fetchAndSetRelayStates(FirebaseData &fbdo);
void sendPowerConsumptionToFirebase(FirebaseData &fbdo,float powerConsumption);
float calculatePowerConsumption(float applianceWattage, unsigned long operatingTimeInMs);
void updatePowerConsumption(int relayIndex,FirebaseData &fbdo);
void sendAllDataToFirebase(float lightIntensity,float humidity,float temprature,float powerConsumption,FirebaseData &fdbo);
void accumulatePowerAndSensorDataAndSend(FirebaseData &fbdo);
float calculatePowerConsumptionOfAppliances();


#endif
