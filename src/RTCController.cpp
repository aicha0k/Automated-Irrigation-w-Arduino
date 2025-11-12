// RTCController.cpp
#include "RTCController.h"

void RTCController::iniciar() {
    rtc.iniciar();
}

DateTime RTCController::now() {
    return rtc.now();
}

bool RTCController::verificarHorario(int hora, int minuto) {
    DateTime t = rtc.now();
    return (t.hora() == hora && t.minuto() == minuto);
}
