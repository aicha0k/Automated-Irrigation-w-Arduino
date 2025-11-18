// 1. INCLUSÃO DE BIBLIOTECAS E CLASSES
#include "ControladorValvula.h"
#include "ServidorWeb.h"
//#include "Configuracao.h"
#include "ConfiguracaoPersistente.h"

// 2. DEFINIÇÕES GLOBAIS
const char* WIFI_SSID = "ESP32-Configuracao";
const char* WIFI_PASSWORD = "123456789"; 

// Pinos de hardware
const int VALVULA_PIN = 26;
const int LED_BUILTIN_PIN = 2; // Pino 2 é comumente usado como LED interno em DevKits

// 3. OBJETOS GLOBAIS
// Instanciação das Classes
ControladorValvula valvula(VALVULA_PIN, LED_BUILTIN_PIN);
ServidorWeb servidor(WIFI_SSID, WIFI_PASSWORD);
ConfiguracaoPersistente configAtual;

// Declaração da rotina principal
void minhaRotinaDeExecucao(); 

// 4. FUNÇÃO DE CONFIGURAÇÃO (SETUP)
void setup() {
    Serial.begin(115200); 
    servidor.iniciarAP();

    configAtual.carregar();
}

// 5. FUNÇÃO DE LOOP
void loop() {
    // Manuseio de clientes (Acessa e modifica os objetos 'valvula' e 'configAtual')
    servidor.manusearClientes(valvula, configAtual); 

    // Rotina principal do sistema embarcado
    minhaRotinaDeExecucao(); 
    delay(10); 
}

// 6. SUA ROTINA PRINCIPAL (Executada em segundo plano)
void minhaRotinaDeExecucao() {
    // Exemplo de controle e feedback usando os objetos
    digitalWrite(LED_BUILTIN_PIN, HIGH); // Indica que a rotina está livre

    static long lastMsg = 0;
    if (millis() - lastMsg > 5000) { 
        lastMsg = millis();
        
        Serial.println("\n----------------------------------------------");
        Serial.println("Executando rotina principal...");
        
        // Imprime a configuração salva (chama o método da classe Configuracao)
        configAtual.imprimir(); 
        
        Serial.println("Status da Válvula: " + valvula.getEstado());
        Serial.println("----------------------------------------------");
        
        // Lógica de Agendamento: Aqui você usaria 'configAtual' para checar a hora atual (via RTC, por exemplo)
        // e chamar valvula.ligar() ou valvula.desligar() no momento certo.
    }
}