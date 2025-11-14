#include "DHT11.h"

DHT11::DHT11(uint8_t pino) {
    pin = pino;
    temperatura = 0.0;
    umidade = 0.0;
}

void DHT11::iniciar() {
    pinMode(pin, INPUT_PULLUP);
    Serial.println("Sensor DHT11 inicializado (sem biblioteca externa).");
}

bool DHT11::lerDados(uint8_t dados[5]) {
    uint8_t cont = 7;
    uint8_t idx = 0;

    for (int i = 0; i < 5; i++) dados[i] = 0;

    // Inicia comunicação
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
    delay(20); // >18ms para iniciar
    digitalWrite(pin, HIGH);
    delayMicroseconds(40);
    pinMode(pin, INPUT);

    // Confirma resposta do sensor
    unsigned long tempo = micros();
    while (digitalRead(pin) == HIGH)
        if ((micros() - tempo) > 80) return false;

    tempo = micros();
    while (digitalRead(pin) == LOW)
        if ((micros() - tempo) > 80) return false;

    tempo = micros();
    while (digitalRead(pin) == HIGH)
        if ((micros() - tempo) > 80) return false;

    // Leitura dos 40 bits
    for (int i = 0; i < 40; i++) {
        while (digitalRead(pin) == LOW); // espera início do bit
        unsigned long duracao = micros();
        while (digitalRead(pin) == HIGH);
        duracao = micros() - duracao;

        // Bit 0: ~26-28us | Bit 1: ~70us
        if (duracao > 40) dados[idx] |= (1 << cont);

        if (cont == 0) {
            cont = 7;
            idx++;
        } else cont--;
    }

    // Verifica checksum
    if (dados[4] == uint8_t(dados[0] + dados[1] + dados[2] + dados[3]))
        return true;
    else
        return false;
}

float DHT11::lerTemperatura() {
    uint8_t dados[5];
    if (lerDados(dados)) {
        temperatura = dados[2] + (dados[3] / 10.0);
    } else {
        Serial.println("Erro ao ler temperatura do DHT11!");
    }
    return temperatura;
}

float DHT11::lerUmidade() {
    uint8_t dados[5];
    if (lerDados(dados)) {
        umidade = dados[0] + (dados[1] / 10.0);
    } else {
        Serial.println("Erro ao ler umidade do DHT11!");
    }
    return umidade;
}

void DHT11::mostrarLeituras() {
    float t = lerTemperatura();
    float u = lerUmidade();
    Serial.print("Temperatura: ");
    Serial.print(t);
    Serial.print(" °C  |  Umidade: ");
    Serial.print(u);
    Serial.println(" %");
}
