// Valve.cpp
#include "Valve.h"
#include <Arduino.h>

Valvula::Valvula(int pin) : pin(pin), state(false) {}

void Valvula::iniciar() {
    pinMode(pin, OUTPUT);
    close();
}

void Valvula::open() {
    digitalWrite(pin, HIGH);
    state = true;
}

void Valvula::close() {
    digitalWrite(pin, LOW);
    state = false;
}

bool Valvula::isOpen() {
    return state;
}
