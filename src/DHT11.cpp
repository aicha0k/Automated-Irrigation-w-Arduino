#include "DHT11.h"

DHT11::DHT11(uint8_t pino)
    : pin(pino), temperatura(0), umidade(0) {}

void DHT11::iniciar() {
    pinMode(pin, INPUT_PULLUP);
    Serial.println("DHT11 inicializado.");
}

bool DHT11::atualizar() {
    uint8_t dados[5] = {0,0,0,0,0};
    uint8_t cont = 7;
    uint8_t idx = 0;

    // start signal
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
    delay(20);
    digitalWrite(pin, HIGH);
    delayMicroseconds(40);
    pinMode(pin, INPUT_PULLUP);

    // aguarda resposta
    unsigned long t = micros();
    while(digitalRead(pin)==HIGH) if(micros()-t > 80) return false;
    t = micros();
    while(digitalRead(pin)==LOW)  if(micros()-t > 80) return false;
    t = micros();
    while(digitalRead(pin)==HIGH) if(micros()-t > 80) return false;

    // lê 40 bits
    for(int i=0;i<40;i++){
        while(digitalRead(pin)==LOW);
        unsigned long inicio = micros();
        while(digitalRead(pin)==HIGH);
        unsigned long duracao = micros() - inicio;

        if (duracao > 50) dados[idx] |= (1 << cont);

        if (cont==0) { cont=7; idx++; }
        else cont--;
    }

    // checksum
    if (dados[4] != uint8_t(dados[0] + dados[1] + dados[2] + dados[3]))
        return false;

    // 👍 leitura válida!
    umidade     = dados[0];
    temperatura = dados[2];
    return true;
}

float DHT11::getTemperatura() { return temperatura; }
float DHT11::getUmidade()     { return umidade; }
