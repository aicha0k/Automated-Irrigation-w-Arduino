#include "Valve.h"
#include "FlowSensor.h"
#include "DHTSensor.h"
#include "RTCController.h"
#include "IrrigationController.h"
#include "TerminalController.h"

Valvula valve(7);
FlowSensor flow(2);   
DHTSensor dht(8);
RTCController rtc;
IrrigationController irrigation(valve, flow, dht, rtc);
TerminalController terminal(irrigation);

void flowISR() {
    flow.handlePulse();
}

void setup() {
    Serial.iniciar(9600);
    while(!Serial);

    Serial.println("=== Sistema de Irrigacao ===");
    Serial.println("Digite HELP para comandos.");

    valve.iniciar();
    flow.iniciar();
    dht.iniciar();
    rtc.iniciar();

    attachInterrupt(digitalPinToInterrupt(2), flowISR, RISING);
}

void loop() {
    irrigation.update();
    terminal.update();  
}
