#include "DHTSensor.h"
#include <Arduino.h>

DHTSensor::DHTSensor(int pin) : pin(pin) {}

void DHTSensor::iniciar() {
    pinMode(pin, INPUT_PULLUP);
}

bool DHTSensor::lerSensor() {
    uint8_t lastState = HIGH;
    uint8_t counter = 0;
    uint8_t j = 0;

    memset(data, 0, 5);

    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
    delay(20);

    pinMode(pin, INPUT_PULLUP);
    delayMicroseconds(40);

    for (int i = 0; i < 85; i++) {
        counter = 0;
        while (digitalRead(pin) == lastState) {
            counter++;
            delayMicroseconds(1);
            if (counter == 255) return false;
        }
        lastState = digitalRead(pin);

        if (i >= 4 && i % 2 == 0) {
            data[j/8] <<= 1;
            if (counter > 50) data[j/8] |= 1;
            j++;
        }
    }
    return true;
}

float DHTSensor::getTemperatura() {
    if (lerSensor())
        return data[2];
    return NAN;
}

float DHTSensor::getUmidade() {
    if (lerSensor())
        return data[0];
    return NAN;
}
