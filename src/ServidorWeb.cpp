#include "ServidorWeb.h"
#include <Arduino.h>

// =========================================================
// CONSTRUTOR
// =========================================================
ServidorWeb::ServidorWeb(const char* s, const char* p)
    : ssid(s), password(p), server(80) {}


// =========================================================
// INICIAR ACCESS POINT
// =========================================================
void ServidorWeb::iniciarAP() {
    Serial.print("Configurando Ponto de Acesso...");
    WiFi.softAP(ssid, password);
    Serial.println("Pronto.");

    Serial.print("Endereço IP do AP: ");
    Serial.println(WiFi.softAPIP());

    server.begin();
    Serial.println("Servidor web iniciado.");
}


// =========================================================
// EXTRAI PARAMETRO DA URL
// =========================================================
String ServidorWeb::getParameterValue(String uri, String param) {
    String searchStr = param + "=";
    int startIndex = uri.indexOf(searchStr);

    if (startIndex < 0) return "";

    startIndex += searchStr.length();
    int endIndex = uri.indexOf('&', startIndex);
    if (endIndex < 0) endIndex = uri.indexOf(' ', startIndex);

    return uri.substring(startIndex, endIndex);
}


// =========================================================
// FUNÇÃO PRINCIPAL DO SERVIDOR WEB
// =========================================================
void ServidorWeb::manusearClientes(
    Rele& valvula,
    ConfiguracaoPersistente& config,
    float temperatura,
    float umidade,
    float fluxoAtual,
    float fluxoTotal,
    String horarioAtual,
    String dataAtual
) {
    WiFiClient client = server.available();

    if (!client) return;

    Serial.println("Novo cliente conectado.");
    String currentLine = "";
    header = "";

    while (client.connected()) {
        if (client.available()) {
            char c = client.read();
            header += c;

            if (c == '\n') {
                if (currentLine.length() == 0) {

                    // =========================================================
                    // CONTROLE DA VÁLVULA
                    // =========================================================
                    if (header.indexOf("GET /valvula/on") >= 0) {
                        valvula.ligar();
                        client.println("HTTP/1.1 303 See Other");
                        client.println("Location: /");
                        client.println();
                        break;
                    }

                    if (header.indexOf("GET /valvula/off") >= 0) {
                        valvula.desligar();
                        client.println("HTTP/1.1 303 See Other");
                        client.println("Location: /");
                        client.println();
                        break;
                    }

                    // =========================================================
                    // SALVAR CONFIGURAÇÕES
                    // =========================================================
                    if (header.indexOf("GET /save") >= 0) {

                        int startUri = header.indexOf("GET /save");
                        int endUri = header.indexOf(" HTTP/1.1");
                        String uri = header.substring(startUri + 4, endUri);

                        config.salvarTemporariamente(
                            getParameterValue(uri, "dia").toInt(),
                            getParameterValue(uri, "mes").toInt(),
                            getParameterValue(uri, "ano").toInt(),
                            getParameterValue(uri, "hora").toInt(),
                            getParameterValue(uri, "minuto").toInt(),
                            getParameterValue(uri, "segundo").toInt(),
                            getParameterValue(uri, "duracao").toInt(),
                            getParameterValue(uri, "ciclo")
                        );

                        config.salvar();

                        client.println("HTTP/1.1 303 See Other");
                        client.println("Location: /");
                        client.println();
                        break;
                    }

                    // =========================================================
                    // PÁGINA PRINCIPAL
                    // =========================================================
                    String estado = valvula.estaLigado() ? "on" : "off";

                    gerarPaginaHTML(
                        client,
                        estado,
                        temperatura,
                        umidade,
                        fluxoAtual,
                        fluxoTotal,
                        horarioAtual,
                        dataAtual
                    );

                    client.println();
                    break;
                } else {
                    currentLine = "";
                }
            } else if (c != '\r') {
                currentLine += c;
            }
        }
    }

    client.stop();
    Serial.println("Cliente desconectado.");
}


// =========================================================
// GERAR HTML COMPLETO
// =========================================================
void ServidorWeb::gerarPaginaHTML(
    WiFiClient client,
    String valvulaEstado,
    float temperatura,
    float umidade,
    float fluxoAtual,
    float fluxoTotal,
    String horarioAtual,
    String dataAtual
) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println("Connection: close");
    client.println(); 
    
    client.println("<!DOCTYPE html><html lang=\"pt-br\">");
    client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
    client.println("<meta charset=\"UTF-8\">");
    client.println("<title>Controle da Horta</title>");
    
    // CSS
    client.println("<style>");
    client.println("body { font-family: Arial; background-color: #f4f7f6; color: #333; margin: 0; padding: 0; }");
    client.println(".container { max-width: 600px; margin: 20px auto; padding: 20px; }");
    client.println(".status-box, .card { background: white; border-radius: 12px; padding: 20px; margin-top: 20px; box-shadow: 0 4px 12px rgba(0,0,0,0.05); }");
    client.println(".status-text-on { color: #28a745; font-weight: bold; }");
    client.println(".status-text-off { color: #dc3545; font-weight: bold; }");
    client.println("</style>");
    
    client.println("</head><body><div class='container'>");
    
    // === Título ===
    client.println("<h1>Controle Remoto da Horta</h1>");

    // === STATUS DA VÁLVULA ===
    client.println("<div class='status-box'>");
    if (valvulaEstado == "off") {
        client.println("<p>Status da válvula: <span class='status-text-off'>Desligada</span></p>");
        client.println("<p><a href='/valvula/on'>Ligar</a></p>");
    } else {
        client.println("<p>Status da válvula: <span class='status-text-on'>Ligada</span></p>");
        client.println("<p><a href='/valvula/off'>Desligar</a></p>");
    }
    client.println("</div>");

    // === DATA E HORA ===
    client.println("<div class='card'>");
    client.println("<h2>Data e Hora</h2>");
    client.printf("<p><b>Data:</b> %s</p>", dataAtual.c_str());
    client.printf("<p><b>Hora:</b> %s</p>", horarioAtual.c_str());
    client.println("</div>");

    // === SENSORES ===
    client.println("<div class='card'>");
    client.println("<h2>Sensores</h2>");
    client.printf("<p><b>Temperatura:</b> %.1f °C</p>", temperatura);
    client.printf("<p><b>Umidade:</b> %.1f %%</p>", umidade);
    client.printf("<p><b>Fluxo Atual:</b> %.2f L/min</p>", fluxoAtual);
    client.printf("<p><b>Total Irrigado:</b> %.2f L</p>", fluxoTotal);
    client.println("</div>");

    // === FORMULÁRIO ANTIGO — SEM ALTERAÇÕES ===
    client.println("<div class='card'>");
    client.println("<h2>Programar Irrigação</h2>");
    client.println("<form action='/save' method='GET'>");
    
    client.println("<label>Dia:</label><input type='text' name='dia'>");
    client.println("<label>Mês:</label><input type='text' name='mes'>");
    client.println("<label>Ano:</label><input type='text' name='ano'>");
    client.println("<label>Hora:</label><input type='text' name='hora'>");
    client.println("<label>Minuto:</label><input type='text' name='minuto'>");
    client.println("<label>Segundo:</label><input type='text' name='segundo'>");
    client.println("<label>Duração (min):</label><input type='text' name='duracao'>");

    client.println("<label>Ciclo:</label>");
    client.println("<select name='ciclo'>");
    client.println("<option value='diario'>Diário</option>");
    client.println("<option value='unico'>Único</option>");
    client.println("</select>");

    client.println("<button type='submit'>Salvar</button>");
    client.println("</form></div>");

    client.println("</div></body></html>");
}
