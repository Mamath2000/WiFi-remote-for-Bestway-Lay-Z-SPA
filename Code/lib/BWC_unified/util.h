#pragma once

#include <Arduino.h>


float C2F(float c);
float F2C(float f);

#ifdef ESP8266
const int D_pin[9] = {D0, D1, D2, D3, D4, D5, D6, D7, D8};
#else
// Waveshare ESP32-S3-Zero : mapping GPIO direct (pas de macros D0..D8)
const int D_pin[9] = {4, 5, 6, 7, 8, 9, 10, 11, 12};
#endif
int gpio2dp(int gpio);
int dp2gpio(int dp);