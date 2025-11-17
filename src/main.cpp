#include <Arduino.h>
#include "Controle.h"
#include "DHT11.h"
#include "RTC.h"
#include "Rele.h"

// ==== OBJETOS DOS MÓDULOS ====
DHT11 sensorDHT(PINO_DHT);
RTC    moduloRTC;
Rele   moduloRele(PINO_RELE);

// ==== OBJETO PRINCIPAL DO SISTEMA ====
Controle sistema(&sensorDHT, &moduloRTC, &moduloRele);

void setup() {
    Serial.begin(9600);
    delay(1000);

    Serial.println("\n=== Sistema de Irrigação — Teste Completo ===");

    sensorDHT.iniciar();
    moduloRTC.iniciar();
    moduloRele.iniciar();

    // Ajusta RTC para hora do computador
    // moduloRTC.ajustarParaHoraDoComputador();

    // Agendar alarme de 1 minuto para frente
    DateTime agora = moduloRTC.getNow();
    moduloRTC.agendarAcionamento(
        agora.year(), agora.month(), agora.day(),
        agora.hour(), agora.minute() + 1, agora.second(),
        1
    );
}

void loop() {

  if (sensorDHT.atualizar()) {

      Serial.print("Temp: ");
      Serial.print(sensorDHT.getTemperatura());
      Serial.print(" °C  |  Umidade: ");
      Serial.print(sensorDHT.getUmidade());
      Serial.println(" %");

  } else {
      Serial.println("Falha ao ler DHT11.");
  }

  moduloRTC.mostrarHora();

  if (moduloRTC.alarmeLigou()) {
    Serial.println(">>> ALARME 1 DISPAROU — LIGAR LED <<<");
    moduloRele.ligar();    // <<< LIGA O LED
}

// ======== EVENTO DE ALARME 2 (DESLIGAR LED) ========
if (moduloRTC.alarmeDesligou()) {
    Serial.println(">>> ALARME 2 DISPAROU — DESLIGAR LED <<<");
    moduloRele.desligar(); // <<< DESLIGA O LED
}

  delay(5000);
}
