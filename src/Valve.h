// Valve.h
#ifndef VALVE_H
#define VALVE_H

class Valvula {
private:
    int pin;
    bool state;

public:
    Valvula(int pin);
    void iniciar();
    void open();
    void close();
    bool isOpen();
};

#endif
