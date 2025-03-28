#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

// WiFi Credentials
const char* ssid = "<chnage_to_ssid>";
const char* password = "<ssid-pass>";
const char* serverUrl = "https://carbon-mu-five.vercel.app/log_emission";  // Update IP if needed

// Define DIP switch input pins
#define DIP1 5  // D1 -  connect first DIP switch here
#define DIP2 4  // D2 -  connect second DIP switch here
#define DIP3 14 // D5 -  connect third DIP switch here
#define DIP4 12 // D6 -  connect fourth DIP switch here

// Define Flash Button (GPIO0, built-in)
#define FLASH_BTN 0

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 19800, 60000);  // UTC+5:30 (IST)

// Function to wait for button press
void waitForButtonPress(const char* message) {
    Serial.println("\n---------------------------------");
    Serial.println(message);
    Serial.println("---------------------------------\n");

    while (digitalRead(FLASH_BTN) == HIGH) {
        yield();
    }
    delay(300);
    while (digitalRead(FLASH_BTN) == LOW);
    delay(300);
}

String getFormattedTimestamp() {
    timeClient.update();
    time_t epochTime = (time_t)timeClient.getEpochTime(); // Explicit cast to time_t

    struct tm *ptm = gmtime(&epochTime); // Convert to UTC struct
    char buffer[30];
    sprintf(buffer, "%04d-%02d-%02dT%02d:%02d:%02d.000000",
            ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday,
            ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
    return String(buffer);
}

// Function to read DIP switches with timeout
int readDIPInput(const char* prompt, const char* options[], int numOptions) {
    Serial.println("\n---------------------------------");
    Serial.println(prompt);
    Serial.println("---------------------------------");

    unsigned long startTime = millis();
    int selection = -1;

    while (selection == -1) {
        if (digitalRead(DIP1) == LOW) selection = 0;
        else if (digitalRead(DIP2) == LOW) selection = 1;
        else if (digitalRead(DIP3) == LOW) selection = 2;

        yield();
        if (millis() - startTime > 10000) {
            Serial.println("⚠ No selection made. Skipping...");
            return -1;
        }
    }

    Serial.print("✅ Selected: ");
    Serial.println(options[selection]);

    waitForButtonPress("Press Flash Button to Confirm Selection.");

    // Ensure the user turns off the DIP switch before proceeding
    Serial.println("🔄 Turn off the selected DIP switch...");
    startTime = millis();
    while (digitalRead(DIP1) == LOW || digitalRead(DIP2) == LOW || digitalRead(DIP3) == LOW) {
        yield();
        if (millis() - startTime > 10000) {
            Serial.println("⚠ DIP switch still ON. Proceeding anyway.");
            break;
        }
    }

    return selection;
}

// Function to calculate carbon emissions
void calculateEmissions() {
    Serial.println("\n=================================");
    Serial.println("🚀 Starting Carbon Emission Calculation...");
    Serial.println("=================================\n");

    // Fuel Type Selection
    const char* fuelOptions[] = { "Petrol", "Diesel", "EV" };
    int fuelType = readDIPInput("Select Fuel Type (DIP1: Petrol, DIP2: Diesel, DIP3: EV)", fuelOptions, 3);

    // Traffic Type Selection
    const char* trafficOptions[] = { "Light Traffic", "Moderate Traffic", "Heavy Traffic" };
    int trafficType = readDIPInput("Select Traffic Type (DIP1: Light, DIP2: Moderate, DIP3: Heavy)", trafficOptions, 3);

    // Night Time Selection
    const char* nightOptions[] = { "Daytime", "Nighttime" };
    int nightTime = readDIPInput("Select Time (DIP1: Daytime, DIP2: Nighttime)", nightOptions, 2);

    // Idle Time Selection
    const char* idleOptions[] = { "Low Idle Time", "High Idle Time" };
    int idleTime = readDIPInput("Select Idle Time (DIP1: Low, DIP2: High)", idleOptions, 2);

    // Distance Selection
    const char* distanceOptions[] = { "Short Distance", "Long Distance" };
    int distance = readDIPInput("Select Distance (DIP1: Short, DIP2: Long)", distanceOptions, 2);

    // Determine Carbon Emission Category
    String emissionCategory = "LOW";
    if (fuelType == 2) {
        emissionCategory = "LOW";
    } else {
        if (trafficType == 2 || idleTime == 1 || (fuelType == 0 && nightTime == 1)) {
            emissionCategory = "HEAVY";
        } else if (fuelType == 1 && trafficType == 1 && distance == 1) {
            emissionCategory = "MODERATE";
        }
    }

    // Get timestamp
    timeClient.update();
    String timestamp = getFormattedTimestamp();

    // Display Final Emission Category
    Serial.println("\n=================================");
    Serial.println("🌍 Carbon Emission Report");
    Serial.println("=================================");
    Serial.printf("🚗 Fuel Type: %s\n", fuelOptions[fuelType]);
    Serial.printf("🚦 Traffic Type: %s\n", trafficOptions[trafficType]);
    Serial.printf("🌙 Time of Day: %s\n", nightOptions[nightTime]);
    Serial.printf("⏳ Idle Time: %s\n", idleOptions[idleTime]);
    Serial.printf("📏 Travel Distance: %s\n", distanceOptions[distance]);
    Serial.println("---------------------------------");
    Serial.printf("🔥 Carbon Emission Type: %s\n", emissionCategory.c_str());
    Serial.println("=================================\n");

    // Send Data to Server
    sendEmissionData(fuelOptions[fuelType], trafficOptions[trafficType], nightOptions[nightTime],
                     idleOptions[idleTime], distanceOptions[distance], emissionCategory, timestamp);

    waitForButtonPress("🔄 Press Flash Button for Next Calculation.");
}

// Function to send emission data to the server
void sendEmissionData(String fuel, String traffic, String timeOfDay, String idle, String distance, String emission, String timestamp) {
    if (WiFi.status() == WL_CONNECTED) {
        WiFiClientSecure client;
        client.setInsecure();  // Use insecure mode for HTTPS
        
        HTTPClient http;
        http.begin(client, serverUrl);
        http.addHeader("Content-Type", "application/json");

        String payload = "{"
                         "\"emission\":\"" + emission + "\","
                         "\"timestamp\":\"" + timestamp + "\""
                         "}";

        Serial.println("📡 Sending data to server: " + payload);
        int httpResponseCode = http.POST(payload);

        if (httpResponseCode > 0) {
            Serial.println("✅ Server Response: " + String(httpResponseCode));
            Serial.println("📩 Response Payload: " + http.getString());
        } else {
            Serial.println("❌ Error sending data. HTTP Code: " + String(httpResponseCode));
        }

        http.end();
    } else {
        Serial.println("❌ WiFi Disconnected! Can't send data.");
    }
}


void setup() {
    Serial.begin(115200);
    delay(100);

    // Set pin modes
    pinMode(DIP1, INPUT_PULLUP);
    pinMode(DIP2, INPUT_PULLUP);
    pinMode(DIP3, INPUT_PULLUP);
    pinMode(DIP4, INPUT_PULLUP);
    pinMode(FLASH_BTN, INPUT_PULLUP);

    Serial.print("connecting to wifi..");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(500);

    }
    timeClient.begin();
    waitForButtonPress("🚀 Press Flash Button to Start.");
}

void loop() {
    calculateEmissions();
}
