#ifndef SERVIDORWEB_H
#define SERVIDORWEB_H

#include <WiFi.h>
#include "ControladorValvula.h"
#include "ConfiguracaoPersistente.h"
#include "Rele.h"

/**
 * @brief Classe para gerenciar a interface web, Wi-Fi e processamento de requisições.
 */
class ServidorWeb {
private:
    const char* ssid;
    const char* password;
    WiFiServer server;
    String header;

    // Métodos privados
    String getParameterValue(String uri, String param);
    void gerarPaginaHTML(WiFiClient client, String valvulaEstado);

public:
    // Construtor
    ServidorWeb(const char* s, const char* p);

    // Métodos públicos
    void iniciarAP();
    void manusearClientes(Rele& valvula, ConfiguracaoPersistente& config);
};

#endif // SERVIDORWEB_H