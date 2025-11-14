#include <Wire.h>
#include <RTClib.h>
#include "RTC.h"

RTC_DS3231 rtc;
RTC moduloRTC;


void setup() {
    Serial.begin(9600);
    delay(2000);

    moduloRTC.iniciar();

    DateTime agora = moduloRTC.getNow();


    Serial.println("Testando DS3231...");

    if (!rtc.begin()) {
        Serial.println("ERRO: RTC nao encontrado!");
        while (1);
    }

    if (rtc.lostPower()) {
        Serial.println("RTC sem dados, ajustando para data/hora da compilacao...");
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }

    rtc.disableAlarm(1);
    rtc.disableAlarm(2);
    rtc.clearAlarm(1);
    rtc.clearAlarm(2);

    Serial.println("RTC OK! Iniciando leitura...");
    moduloRTC.atualizarHora();

    moduloRTC.agendarAcionamento(2025, 11, 14, 15, 10, 0, 1); // duração 1 minuto
  
    //moduloRTC.ajustarHorario(2025, 11, 14, 15, 36, 0); // funcao para ajustar horario manualmente
    moduloRTC.agendarCicloDiario(8, 0, 3);

  }

void loop() {
    DateTime agora = rtc.now();

    Serial.print("Hora: ");
    Serial.print(agora.hour());
    Serial.print(":");
    Serial.print(agora.minute());
    Serial.print(":");
    Serial.print(agora.second());

    Serial.print(" | Data: ");
    Serial.print(agora.day());
    Serial.print("/");
    Serial.print(agora.month());
    Serial.print("/");
    Serial.print(agora.year());

    Serial.print(" | Temp: ");
    Serial.print(rtc.getTemperature());
    Serial.println(" C");

    delay(1000);

    if (moduloRTC.alarmeLigou()) {
      Serial.println(">>> ALARME 1 DISPAROU — LIGAR VÁLVULA");
  }

    if (moduloRTC.alarmeDesligou()) {
        Serial.println(">>> ALARME 2 DISPAROU — DESLIGAR VÁLVULA");
  }

}
