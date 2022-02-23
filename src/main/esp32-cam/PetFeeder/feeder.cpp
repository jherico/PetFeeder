#include <atomic>
#include "hardware_pins.h"
#include "utils.hpp"

namespace feeder {

constexpr int MOTOR_PIN{MOTOR_GPIO_NUM};
constexpr int SWITCH_PIN{SWITCH_GPIO_NUM};

bool error{false};
std::atomic<uint16_t> scoopCount{0};
bool switchClosed{false};
bool motorOn{false};

bool readSwitch() {
    return digitalRead(SWITCH_PIN) != LOW;
}

void enableMotor(boolean enable = true) {
    motorOn = enable;
    // digitalWrite(LED_BUILTIN, enable ? HIGH : LOW);
    digitalWrite(MOTOR_PIN, enable ? HIGH : LOW);
}

void setup() {
    pinMode(SWITCH_PIN, INPUT);
    pinMode(MOTOR_PIN, OUTPUT);
    switchClosed = readSwitch();
    if (!switchClosed) {
        const auto endMillis = millis() + 5000;
        enableMotor();
        waitFor(
            "Resetting state", [&]() { return switchClosed = readSwitch() || (millis() > endMillis); }, 50);
        enableMotor(false);
        if (!readSwitch()) {
            error = true;
            blinkLight(3);
            digitalWrite(LED_BUILTIN, readSwitch() ? HIGH : LOW);
            Serial.println("Switch not closed, aborting");
        }
    }
}

void loop() {
    if (error) {
        // if we're in an error state, read the switch but do nothing with it and use the state to set the LED on or off
        digitalWrite(LED_BUILTIN, readSwitch() ? HIGH : LOW);
        return;
    }

    if (motorOn) {
        auto newSwitchClosed = readSwitch();
        if (switchClosed ^ newSwitchClosed) {
            Serial.println(newSwitchClosed ? "Switch is closed" : "Switch is open");
            if (newSwitchClosed) {
                if (--scoopCount <= 0) {
                    scoopCount = 0;
                    Serial.println("Turning off motor");
                    enableMotor(false);
                }
            }
            switchClosed = newSwitchClosed;
        }
    } else if (scoopCount.load() != 0) {
        Serial.println("Turning on motor for scoop delivery");
        enableMotor();
    }
}

void feed(uint8_t count = 1) {
    Serial.println("Calling feedscoops with ");
    Serial.print("\t");
    Serial.println(count);
    feeder::scoopCount.fetch_add(count);
}

}  // namespace feeder

bool readFeederSwitch() {
    return feeder::readSwitch();
}

void feedScoops(uint8_t count = 1) {
    feeder::feed(count);
}

void setupFeeder() {
    feeder::setup();
}

void loopFeeder() {
    feeder::loop();
}
