#pragma once
#include <Arduino.h>
#include <string>
#include <functional>

template <typename F>
void waitFor(const char* name, const F waitFunction, int delayMs = 500) {
  Serial.println(name);
  uint32_t count = 0;
  while (!waitFunction()) {
    delay(delayMs);
    if ((++count % 100) == 0) {
      Serial.println(".");
    }
  }
  if (count != 0) {
    Serial.println();
  }
}

void blinkLight(int times, int delayMs = 500);