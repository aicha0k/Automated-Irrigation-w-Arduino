#ifndef SERVIDORWEB_H
#define SERVIDORWEB_H

#include <WiFi.h>
#include "Rele.h"

// Forward declaration: a classe `Configuracao` é usada por referência.
class Configuracao;


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
    void gerarPaginaHTML(WiFiClient client, Rele &valvula);

public:
    // Construtor
    ServidorWeb(const char* s, const char* p);

    // Métodos públicos
    void iniciarAP();
    void manusearClientes(Rele& valvula, Configuracao& config);
};

#endif // SERVIDORWEB_H