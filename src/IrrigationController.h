#ifndef IRRIGATIONCONTROLLER_H
#define IRRIGATIONCONTROLLER_H

#include "Valve.h"
#include "FlowSensor.h"
#include "DHTSensor.h"
#include "RTCController.h"

class IrrigationController {
private:
    Valvula &valvula;
    SensorVazao &vazao;
    SensorDHT &dht;
    RTCController &rtc;

    int agendaHora;
    int agendaMinuto;
    unsigned long duracaoIrrigacao; // em segundos

    bool acionadoHoje;

public:
    IrrigationController(
        Valvula &v,
        SensorVazao &f,
        SensorDHT &d,
        RTCController &r
    );

    void programarIrrigacao(int hora, int minuto, unsigned long duration_seconds);
    void printProgramacao();
    void update();  // chamada constante no loop

};

#endif
