#include "ServidorWeb.h"
#include <Arduino.h> 

// Construtor (Permanece o mesmo)
ServidorWeb::ServidorWeb(const char* s, const char* p) 
    : ssid(s), password(p), server(80) {}

// Inicialização do AP (Permanece o mesmo)
void ServidorWeb::iniciarAP() {
    Serial.print("Configurando Ponto de Acesso...");
    WiFi.softAP(ssid, password);
    Serial.println("Pronto.");
    Serial.print("Endereço IP do AP: ");
    Serial.println(WiFi.softAPIP()); 
    server.begin(); 
    Serial.println("Servidor web iniciado.");
}

// Função auxiliar para extrair parâmetros da URI (Permanece a mesma)
String ServidorWeb::getParameterValue(String uri, String param) {
    String value = "";
    String searchStr = param + "=";
    int startIndex = uri.indexOf(searchStr);
    
    if (startIndex > 0) {
        startIndex += searchStr.length();
        int endIndex = uri.indexOf('&', startIndex);
        if (endIndex == -1) {
            endIndex = uri.indexOf(' ', startIndex);
        }
        if (endIndex != -1) {
            value = uri.substring(startIndex, endIndex);
        } else {
            value = uri.substring(startIndex);
        }
    }
    return value;
}

// Manuseio principal de clientes (Com as alterações para Redirecionamento)
void ServidorWeb::manusearClientes(ControladorValvula& valvula, ConfiguracaoPersistente& config) {
    WiFiClient client = server.available(); 

    if (client) { 
        Serial.println("Novo cliente conectado.");
        String currentLine = ""; 
        header = ""; 

        while (client.connected()) { 
            if (client.available()) { 
                char c = client.read(); 
                header += c; 
                if (c == '\n') { 
                    if (currentLine.length() == 0) {
                        
                        // --- Processamento de Comandos ---
                        if (header.indexOf("GET /valvula/on") >= 0) {
                            valvula.ligar(); 
                            // Redireciona após o comando para limpar a URI
                            client.println("HTTP/1.1 303 See Other");
                            client.println("Location: /");
                            client.println("Connection: close");
                            client.println();
                            break; 
                            
                        } else if (header.indexOf("GET /valvula/off") >= 0) {
                            valvula.desligar(); 
                            // Redireciona após o comando para limpar a URI
                            client.println("HTTP/1.1 303 See Other");
                            client.println("Location: /");
                            client.println("Connection: close");
                            client.println();
                            break;
                            
                        } else if (header.indexOf("GET /save") >= 0) {
                            
                            int startUri = header.indexOf("GET /save");
                            int endUri = header.indexOf(" HTTP/1.1");
                            String uri = header.substring(startUri + 4, endUri); 
                            
                            // Extração
                            String pDia = getParameterValue(uri, "dia");
                            String pMes = getParameterValue(uri, "mes");
                            String pAno = getParameterValue(uri, "ano");
                            String pHora = getParameterValue(uri, "hora");
                            String pMinuto = getParameterValue(uri, "minuto");
                            String pSegundo = getParameterValue(uri, "segundo");
                            String pDuracao = getParameterValue(uri, "duracao");
                            String pCiclo = getParameterValue(uri, "ciclo");

                            // Salva na RAM e na Flash
                            config.salvarTemporariamente(pDia.toInt(), pMes.toInt(), pAno.toInt(), pHora.toInt(), 
                                                        pMinuto.toInt(), pSegundo.toInt(), pDuracao.toInt(), pCiclo);
                            config.salvar(); 
                            
                            Serial.println("✅ Configuracao Salva e Persistida na Flash. Redirecionando...");

                            // 🚨 AQUI ESTÁ A SOLUÇÃO: REDIRECIONAMENTO HTTP 303
                            client.println("HTTP/1.1 303 See Other"); // Código de status para redirecionamento
                            client.println("Location: /");           // Redireciona para a página inicial
                            client.println("Connection: close");
                            client.println();
                            break;
                            
                        }
                        // --- Fim do Processamento ---

                        // Se a requisição for para a página inicial ('/'), gera o HTML normalmente.
                        if (header.indexOf("GET / ") >= 0 || header.indexOf("GET /favicon.ico") < 0) {
                            gerarPaginaHTML(client, valvula.getEstado()); 
                        }

                        client.println();
                        break;
                    } else { 
                        currentLine = "";
                    }
                } else if (c!= '\r') { 
                    currentLine += c;
                }
            }
        }
        client.stop();
        Serial.println("Cliente desconectado.");
    }
}

// Método para gerar e enviar a página HTML (Permanece o mesmo, mas só será chamado no '/' agora)
void ServidorWeb::gerarPaginaHTML(WiFiClient client, String valvulaEstado) {
    // ... (O código de geração do HTML permanece inalterado)
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println("Connection: close");
    client.println(); 
    
    // Início do Documento HTML
    client.println("<!DOCTYPE html><html lang=\"pt-br\">");
    client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
    client.println("<meta charset=\"UTF-8\">");
    client.println("<title>Controle da Horta</title>");
    
    // CSS Embutido para estilizar a página
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
    
    // Título
    client.println("<h1><span>&#9881;</span>Controle Remoto da Horta</h1>");

    // --- Bloco de Status da Válvula (Dinâmico) ---
    client.println("<div class='status-box'>");
    if (valvulaEstado == "off") {
        client.println("<p>Status atual da válvula: <span class='status-text-off'>Desligada</span></p>");
        client.println("<p><a href='/valvula/on' class='button button-primary'>Ligar Válvula</a></p>");
    } else {
        client.println("<p>Status atual da válvula: <span class='status-text-on'>Ligada</span></p>");
        client.println("<p><a href='/valvula/off' class='button button-secondary'>Desligar Válvula</a></p>");
    }
    client.println("</div>");

    // --- Bloco do Formulário de Acionamento ---
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
    
    // Fim do Documento
    client.println("</div></body></html>");
}