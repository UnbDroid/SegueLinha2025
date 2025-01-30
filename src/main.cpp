#include <WiFi.h>
#include <ArduinoOTA.h>

const char* ssid = "Mario’s iPhone";
const char* password = "mario123";


const int led = 2;

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    
    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }

    // Print IP address and start OTA service
    Serial.println("Connected to WiFi");
    Serial.print("ESP32 IP Address: ");
    Serial.println(WiFi.localIP());

    ArduinoOTA.begin(); // Initialize OTA
    Serial.println("Ready for OTA updates");

    pinMode(led, OUTPUT);


}

void loop() {
    ArduinoOTA.handle(); // Handle OTA requests
    digitalWrite(led, LOW);
    delay(100);


}
