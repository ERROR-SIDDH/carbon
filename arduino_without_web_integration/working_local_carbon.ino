#include <Arduino.h>

// Define DIP switch input pins
#define DIP1 5  // D1 - Fuel Type Selection
#define DIP2 4  // D2 - Traffic Type Selection
#define DIP3 14 // D5 - Other Conditions (Night Time, Idle, Distance)
#define DIP4 12 // D6 - Unused or additional input

// Define Flash Button (GPIO0, built-in)
#define FLASH_BTN 0

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
    String emissionCategory = "Light Emission";
    if (fuelType == 2) {
        emissionCategory = "Light Emission (EV)";
    } else {
        if (trafficType == 2 || idleTime == 1 || (fuelType == 0 && nightTime == 1)) {
            emissionCategory = "Heavy Emission";
        } else if (fuelType == 1 && trafficType == 1 && distance == 1) {
            emissionCategory = "Moderate Emission";
        }
    }

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

    waitForButtonPress("🔄 Press Flash Button for Next Calculation.");
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

    Serial.println("\n=================================");
    Serial.println("🔌 System Booted");
    Serial.println("=================================\n");

    // Check if Flash Button is being held at boot
    if (digitalRead(FLASH_BTN) == LOW) {
        Serial.println("⚠ Flash Button is held at boot. Waiting for release...");
        while (digitalRead(FLASH_BTN) == LOW) {
            yield();
        }
    }

    waitForButtonPress("🚀 Press Flash Button to Start.");
}

void loop() {
    calculateEmissions();
}
