#include "Controle.h"
#include <Arduino.h>

Controle::Controle(DHT11* sensorDHT, RTC* rtc, Rele* rele) {
    this->sensorDHT = sensorDHT;
    this->rtc = rtc;
    this->rele = rele;
    temperatura = 0.0;
    umidade = 0.0;
}

// Agora lerTemperatura() faz a leitura COMPLETA do DHT uma vez só
void Controle::lerTemperatura() {

    if (sensorDHT->atualizar()) {      // lê temperatura E umidade num ciclo só
        temperatura = sensorDHT->getTemperatura();
        umidade     = sensorDHT->getUmidade();
    } else {
        Serial.println("Falha ao atualizar DHT11!");
    }

    Serial.print("Temperatura: ");
    Serial.println(temperatura);
}

void Controle::lerUmidade() {
    // NÃO ler de novo — DHT11 não aguenta leituras seguidas.
    Serial.print("Umidade: ");
    Serial.println(umidade);
}

void Controle::controlarIrrigacao() {
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
    lerTemperatura(); // Já atualiza tudo
    lerUmidade();     // Só imprime
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
