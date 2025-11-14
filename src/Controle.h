#ifndef CONTROLE_H
#define CONTROLE_H

#include "DHT11.h"
#include "RTC.h"
#include "Rele.h"

// ==== CONFIGURAÇÃO DE PINOS ====
#define PINO_DHT 7
#define PINO_RELE 8

class Controle {
private:
    DHT11* sensorDHT;   // ponteiro para sensor DHT
    RTC*   rtc;         // ponteiro para módulo de relógio
    Rele*  rele;        // ponteiro para módulo de relé

    float temperatura;
    float umidade;

public:
    Controle(DHT11* sensorDHT, RTC* rtc, Rele* rele);

    void lerTemperatura();
    void lerUmidade();
    void controlarIrrigacao();
    void testarSistema();
    void exibirStatus();
};

#endif
