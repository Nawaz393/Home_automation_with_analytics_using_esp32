#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

void setupSensors();
void sendSensorDataPeriodically();
float getHumidity();
float getLightIntensity();
float getTemperature();
extern bool isFirstTime;
extern float temp;
extern float hum;
extern float light;

#endif
