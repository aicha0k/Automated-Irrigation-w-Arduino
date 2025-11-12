// RTCController.h
#ifndef RTCCONTROLLER_H
#define RTCCONTROLLER_H

#include <RTClib.h>

class RTCController {
private:
    RTC_DS3231 rtc;

public:
    void iniciar();
    DateTime now();
    bool verificarHorario(int hora, int minuto);
};

#endif
