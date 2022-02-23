#include "hardware_pins.h"

void setupNetwork();
void setupCamera();

void setupWeb();
void loopWeb();

void setupFeeder();
void loopFeeder();

void setup() {
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    Serial.println();
    Serial.print(String("Start PetFeeder on ") + ARDUINO_BOARD);
    pinMode(LED_BUILTIN, OUTPUT);
    setupCamera();
    setupNetwork();
    setupWeb();
    setupFeeder();
    Serial.println("Setup complete");
}

// using namespace feeder;
void loop() {
    loopWeb();
    loopFeeder();
    //  client.loop();
    //  feeder::loop();
    //  if (readSwitch()) {
    //    enableMotor();
    //  } else {
    //    enableMotor(false);
    //  }
    // Serial.println(" Tick");

    // digitalWrite(LED_BUILTIN, digitalRead(SWITCH_GPIO_NUM));
    // delay(100);
    // //Serial.println(" Tock");
    // //Serial.println(ARDUINO_BOARD);
    // digitalWrite(LED_BUILTIN, LOW);
    // delay(1000);
}
