#pragma once
#include "Arduino.h"

class TwoWire {
public:
    void begin() {}
    void setClock(long) {}
    void beginTransmission(uint8_t) {}
    size_t write(const uint8_t *, size_t n) { return n; }
    uint8_t endTransmission() { return 0; }
    int requestFrom(int, int) { return 0; }
    int available() { return 0; }
    int read() { return -1; }
};
