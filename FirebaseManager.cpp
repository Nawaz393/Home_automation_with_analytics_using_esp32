#include "FirebaseManager.h"
#include "RelayManager.h" 

#define API_KEY "************************"
#define USER_EMAIL "**********@gmail.com"
#define USER_PASSWORD "**********"
#define DATABASE_URL "https://***********************"

FirebaseData fbdo;
FirebaseData fbdos;
FirebaseAuth auth;
FirebaseConfig config;

void setupFirebase() {
    config.api_key = API_KEY;
    config.database_url = DATABASE_URL;
    auth.user.email = USER_EMAIL;
    auth.user.password = USER_PASSWORD;
    Firebase.begin(&config, &auth);
    Firebase.setMaxRetry(fbdo, 3);

    if (!Firebase.ready()) {
        Serial.println("Failed to connect to Firebase");
    } else {
        Serial.println("Connected to Firebase");
    }

    if (!Firebase.beginStream(fbdos, "/relays")) {
        Serial.println("Could not begin stream");
        Serial.println(fbdos.errorReason());
    }

    Firebase.setStreamCallback(fbdos, streamCallback, streamTimeoutCallback);
}

void handleFirebaseStream() {
    if (Firebase.ready()) {
        if (!Firebase.readStream(fbdos)) {
            Serial.println(fbdos.errorReason());
        }

        if (fbdos.streamTimeout()) {
            Serial.println("Stream timeout, resume streaming...");
        }
    }
}
