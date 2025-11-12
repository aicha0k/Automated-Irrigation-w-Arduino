#ifndef DHTSENSOR_H
#define DHTSENSOR_H

class DHTSensor {
private:
    int pin;
    uint8_t data[5];

    bool lerSensor();

public:
    DHTSensor(int pin);
    void iniciar();
    float getTemperatura();
    float getUmidade();
};

#endif
