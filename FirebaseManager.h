#ifndef FIREBASE_MANAGER_H
#define FIREBASE_MANAGER_H

#include <FirebaseESP32.h>

extern FirebaseData fbdo;
extern FirebaseData fbdos;

void setupFirebase();
void handleFirebaseStream();

#endif
