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
    float fluxoTotal
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

                        Serial.println("Configuração salva.");

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
                        fluxoTotal
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
    float fluxoTotal
) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println("Connection: close");
    client.println(); 
    
    client.println("<!DOCTYPE html><html lang=\"pt-br\">");
    client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
    client.println("<meta charset=\"UTF-8\">");
    client.println("<title>Controle da Horta</title>");
    
    // === CSS ORIGINAL PRESERVADO ===
    client.println("<style>");
    client.println("body { font-family: Arial, sans-serif; background-color: #f4f7f6; color: #333; margin: 0; padding: 0; }");
    client.println(".container { max-width: 600px; margin: 20px auto; padding: 20px; }");
    client.println("h1 { color: #005A9C; font-size: 28px; display: flex; align-items: center; }");
    client.println("h1 span { font-size: 32px; margin-right: 10px; }"); 
    client.println("h2 { color: #333; border-bottom: 2px solid #eee; padding-bottom: 5px; }");
    client.println(".status-box { text-align: center; padding: 20px; background-color: #fff; border-radius: 12px; box-shadow: 0 4px 12px rgba(0,0,0,0.05); }");
    client.println(".status-box p { font-size: 1.2rem; margin-top: 0; }");
    client.println(".status-text-on { color: #28a745; font-weight: bold; }"); 
    client.println(".status-text-off { color: #dc3545; font-weight: bold; }"); 
    client.println(".card { background-color: white; border-radius: 12px; box-shadow: 0 4px 12px rgba(0,0,0,0.05); padding: 24px; margin-top: 30px; }");
    client.println("form label { font-weight: bold; color: #555; display: block; margin-top: 15px; margin-bottom: 5px; }");
    client.println("input[type=\"text\"], select { width: 95%; padding: 12px; border: 1px solid #ddd; border-radius: 6px; font-size: 16px; box-sizing: content-box; }"); 
    client.println(".button { display: inline-block; text-decoration: none; border: none; color: white; padding: 14px 28px; font-size: 16px; border-radius: 8px; cursor: pointer; text-align: center; }");
    client.println(".button-primary { background-color: #005A9C; }"); 
    client.println(".button-secondary { background-color: #6c757d; }"); 
    client.println(".button-save { background-color: #ffc107; color: #212529; font-weight: bold; width: 100%; margin-top: 20px; }"); 
    client.println(".button-save span { margin-right: 8px; }"); 
    client.println("</style>");
    
    client.println("</head><body><div class='container'>");
    
    // === Título ===
    client.println("<h1><span>&#9881;</span>Controle Remoto da Horta</h1>");

    // === Status da válvula ===
    client.println("<div class='status-box'>");
    if (valvulaEstado == "off") {
        client.println("<p>Status atual da válvula: <span class='status-text-off'>Desligada</span></p>");
        client.println("<p><a href='/valvula/on' class='button button-primary'>Ligar Válvula</a></p>");
    } else {
        client.println("<p>Status atual da válvula: <span class='status-text-on'>Ligada</span></p>");
        client.println("<p><a href='/valvula/off' class='button button-secondary'>Desligar Válvula</a></p>");
    }
    client.println("</div>");

    // === NOVA SEÇÃO: SENSORES ===
    client.println("<div class='card'>");
    client.println("<h2>Sensores</h2>");
    client.printf("<p><b>Temperatura:</b> %.1f °C</p>", temperatura);
    client.printf("<p><b>Umidade:</b> %.1f %%</p>", umidade);
    client.printf("<p><b>Fluxo Atual:</b> %.2f L/min</p>", fluxoAtual);
    client.printf("<p><b>Total Irrigado:</b> %.2f L</p>", fluxoTotal);
    client.println("</div>");

    // === FORMULÁRIO ANTIGO (NÃO ALTERADO) ===
    client.println("<div class='card'>");
    client.println("<h2>Definir acionamento</h2>");
    client.println("<form action='/save' method='GET'>");
    
    client.println("<label for='dia'>Dia:</label>");
    client.println("<input type='text' id='dia' name='dia'>");
    
    client.println("<label for='mes'>Mês:</label>");
    client.println("<input type='text' id='mes' name='mes'>");

    client.println("<label for='ano'>Ano:</label>");
    client.println("<input type='text' id='ano' name='ano'>");

    client.println("<label for='hora'>Hora:</label>");
    client.println("<input type='text' id='hora' name='hora'>");

    client.println("<label for='minuto'>Minuto:</label>");
    client.println("<input type='text' id='minuto' name='minuto'>");

    client.println("<label for='segundo'>Segundo:</label>");
    client.println("<input type='text' id='segundo' name='segundo'>");

    client.println("<label for='duracao'>Duração da Irrigação (min):</label>");
    client.println("<input type='text' id='duracao' name='duracao'>");

    client.println("<label for='ciclo'>Ciclo:</label>");
    client.println("<select id='ciclo' name='ciclo'>");
    client.println("<option value='diario'>Diário</option>");
    client.println("<option value='unico'>Único</option>");
    client.println("</select>");
    
    client.println("<button type='submit' class='button button-save'><span>&#128190;</span>Salvar</button>");
    
    client.println("</form></div>");
    
    client.println("</div></body></html>");
}
