#include "utils.hpp"
#include "secrets.h"
#include "hardware_pins.h"

void blinkLight(int times, int delayMs) {
  for (int i = 0; i < times; ++i) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(delayMs);
    digitalWrite(LED_BUILTIN, LOW);
    delay(delayMs);
  }
}
