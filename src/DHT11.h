#ifndef DHT11_H
#define DHT11_H

#include <Arduino.h>

class DHT11 {
private:
    uint8_t pin;
    float temperatura;
    float umidade;

public:
    DHT11(uint8_t pino);
    void iniciar();
    bool atualizar();          // <--- Lê uma única vez
    float getTemperatura();    
    float getUmidade();
};

#endif