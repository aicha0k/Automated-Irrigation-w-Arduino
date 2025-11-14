#ifndef DHT11_H
#define DHT11_H

#include <Arduino.h>

class DHT11 {
private:
    uint8_t pin;
    float temperatura;
    float umidade;

    bool lerDados(uint8_t dados[5]); // função auxiliar para capturar os 40 bits

public:
    DHT11(uint8_t pino);
    void iniciar();
    float lerTemperatura();
    float lerUmidade();
    void mostrarLeituras();
};

#endif
