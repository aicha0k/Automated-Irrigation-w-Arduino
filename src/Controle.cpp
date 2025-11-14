#include "Controle.h"
#include <Arduino.h>

Controle::Controle(DHT11* sensorDHT, RTC* rtc, Rele* rele) {
    this->sensorDHT = sensorDHT;
    this->rtc = rtc;
    this->rele = rele;
    temperatura = 0.0;
    umidade = 0.0;
}

void Controle::lerTemperatura() {
    temperatura = sensorDHT->lerTemperatura();
    Serial.print("Temperatura: ");
    Serial.println(temperatura);
}

void Controle::lerUmidade() {
    umidade = sensorDHT->lerUmidade();
    Serial.print("Umidade: ");
    Serial.println(umidade);
}

void Controle::controlarIrrigacao() {
    // Exemplo simples de regra
    if (umidade < 40.0) {
        Serial.println("Umidade baixa, irrigando...");
        rele->ligar();
    } else {
        Serial.println("Umidade adequada, desligando irrigação.");
        rele->desligar();
    }
}

void Controle::testarSistema() {
    Serial.println("=== Teste de Sistema ===");
    lerTemperatura();
    lerUmidade();
    rtc->mostrarHora();
    controlarIrrigacao();
}

void Controle::exibirStatus() {
    Serial.print("Status atual: Temp=");
    Serial.print(temperatura);
    Serial.print("°C  Umid=");
    Serial.print(umidade);
    Serial.println("%");
}
