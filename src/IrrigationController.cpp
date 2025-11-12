#include "IrrigationController.h"
#include <Arduino.h>

IrrigationController::IrrigationController(
    Valvula &v,
    Sensor &f,
    SensorDHT &d,
    RTCController &r
) : valvula(v), vazao(f), dht(d), rtc(r),
    agendaHora(6), agendaMinuto(0), duracaoIrrigacao(30),
    acionadoHoje(false) {}

void IrrigationController::programarIrrigacao(int hora, int minuto, unsigned long duracao) {
    agendaHora = hora;
    agendaMinuto = minuto;
    duracaoIrrigacao = duracao;
}

void IrrigationController::printProgramacao() {
    Serial.print("Irrigacao programada para ");
    Serial.print(agendaHora);
    Serial.print(":");
    if (agendaMinuto < 10) Serial.print("0");
    Serial.print(agendaMinuto);
    Serial.print(" por ");
    Serial.print(duracaoIrrigacao);
    Serial.println(" segundos.");
}

void IrrigationController::update() {
    DateTime t = rtc.now();

    // reset diário
    if (t.hora() == 0 && t.minuto() == 0) {
        acionadoHoje = false;
    }

    // horário de iniciar irrigação
    if (!acionadoHoje &&
        t.hora() == agendaHora &&
        t.minuto() == agendaMinuto) {

        acionadoHoje = true;

        Serial.println(">>> Iniciando irrigacao...");
        valve.open();

        unsigned long start = millis();
        while (millis() - start < duracaoIrrigacao * 1000UL) {

            float temp = dht.getTemperatura();
            float humi = dht.getUmidade();
            float flowRate = flow.getFlowRate();

            Serial.print("Temp: ");
            Serial.print(temp);
            Serial.print(" °C | Umidade: ");
            Serial.print(humi);
            Serial.print(" % | Fluxo: ");
            Serial.print(flowRate);
            Serial.println(" L/min");

            delay(1000);
        }

        valve.close();
        Serial.println(">>> Irrigacao finalizada.");
    }
}
