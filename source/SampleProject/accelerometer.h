#pragma once
#include <stdint.h>

struct AccData {
  int8_t x; 
  int8_t y;
  int8_t z;
};

AccData readAccelerometer();
