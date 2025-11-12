#include "TerminalController.h"

TerminalController::TerminalController(IrrigationController &ctrl)
: controller(ctrl), buffer("") {}

void TerminalController::update() {
    while (Serial.available()) {
        char c = Serial.read();

        if (c == '\n' || c == '\r') {
            if (buffer.length() > 0) {
                parseCommand(buffer);
                buffer = "";
            }
        } else {
            buffer += c;
        }
    }
}

void TerminalController::parseCommand(const String &cmd) {
    if (cmd.startsWith("SET")) {
        int h, m, d;
        sscanf(cmd.c_str(), "SET %d %d %d", &h, &m, &d);

        controller.setSchedule(h, m, d);

        Serial.println("Novo horario definido:");
        controller.printSchedule();
        return;
    }

    if (cmd.startsWith("GET")) {
        controller.printSchedule();
        return;
    }

    if (cmd.startsWith("HELP")) {
        Serial.println("COMANDOS DISPONIVEIS:");
        Serial.println("SET HH MM DURATION  => define horario (DURATION em segundos)");
        Serial.println("GET                 => mostra configuracao atual");
        Serial.println("HELP                => lista comandos");
        return;
    }

    Serial.print("Comando nao reconhecido: ");
    Serial.println(cmd);
}
