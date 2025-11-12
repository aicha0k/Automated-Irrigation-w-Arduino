#ifndef TERMINALCONTROLLER_H
#define TERMINALCONTROLLER_H

#include <Arduino.h>
#include "IrrigationController.h"

class TerminalController {
private:
    IrrigationController &controller;
    String buffer;

public:
    TerminalController(IrrigationController &ctrl);
    void update();
    void parseCommand(const String &cmd);
};

#endif
