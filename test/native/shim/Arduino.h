// Minimal host-side stand-in for Arduino.h -- just enough for hcr.cpp to
// compile and run natively. Not a general Arduino mock.
#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#define DEC 10

typedef uint8_t byte;

// Tests drive the clock by assigning mock_millis.
extern unsigned long mock_millis;
inline unsigned long millis() { return mock_millis; }
inline void delay(unsigned long) {}

class String {
public:
    String() {}
    String(const char *s) : _s(s ? s : "") {}
    String(char c) : _s(1, c) {}
    String(int v) : _s(std::to_string(v)) {}
    String(int v, int) : _s(std::to_string(v)) {}
    String(unsigned int v) : _s(std::to_string(v)) {}
    String(long v) : _s(std::to_string(v)) {}
    String(unsigned long v) : _s(std::to_string(v)) {}
    String(double v) : _s(std::to_string(v)) {}

    unsigned int length() const { return (unsigned int)_s.size(); }
    const char *c_str() const { return _s.c_str(); }
    int toInt() const { return atoi(_s.c_str()); }
    float toFloat() const { return (float)atof(_s.c_str()); }
    bool equals(const String &o) const { return _s == o._s; }
    int indexOf(char c, unsigned int from = 0) const {
        size_t p = _s.find(c, from);
        return p == std::string::npos ? -1 : (int)p;
    }
    String substring(unsigned int from) const { return String(_s.substr(from).c_str()); }
    String substring(unsigned int from, unsigned int to) const {
        return String(_s.substr(from, to - from).c_str());
    }
    char charAt(unsigned int i) const { return _s[i]; }
    char operator[](unsigned int i) const { return _s[i]; }

    String &operator+=(const String &o) { _s += o._s; return *this; }
    String &operator+=(char c) { _s += c; return *this; }
    bool operator==(const String &o) const { return _s == o._s; }
    bool operator==(const char *o) const { return _s == o; }
    bool operator!=(const String &o) const { return _s != o._s; }
    bool operator!=(const char *o) const { return _s != o; }

    friend String operator+(const String &a, const String &b) { String r(a); r._s += b._s; return r; }
    friend String operator+(const char *a, const String &b) { return String(a) + b; }
    friend String operator+(char a, const String &b) { return String(a) + b; }
    friend String operator+(const String &a, const char *b) { return a + String(b); }
    friend String operator+(const String &a, char b) { return a + String(b); }

private:
    std::string _s;
};

class HardwareSerial {
public:
    void begin(long) {}
    bool available() { return false; }
    int read() { return -1; }
    void print(const String &) {}
    void print(const char *) {}
    void print(char) {}
    void print(int) {}
    void println(const String &) {}
    void println(const char *) {}
    void println(int) {}
    operator bool() { return true; }
};

extern HardwareSerial Serial;
