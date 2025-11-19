// TODO:
// - ACRESCENTAR a memoria flash o reagendamento causado pelo alarme 2
//#include "ControladorValvula.h"
#include "ServidorWeb.h"
//#include "Configuracao.h"
#include "ConfiguracaoPersistente.h"
#include "RTC.h"
#include "Controle.h"
#include "Rele.h"
#include "DHT11.h"

// ==== Configuracao Servidor Web ====
const char* WIFI_SSID = "ESP32-Servidor";
const char* WIFI_PASSWORD = "123456789";

// 3. OBJETOS GLOBAIS
// Instanciação das Classes
ServidorWeb servidor(WIFI_SSID, WIFI_PASSWORD);
ConfiguracaoPersistente configAtual;
RTC rtc;
Rele rele(18); // Pino 18 para o relé
DHT11 dht11(4); // Pino 23 para o DHT11
Controle controle(&dht11, &rtc, &rele);

// Declaração da rotina principal
void minhaRotinaDeExecucao(); 

// 4. FUNÇÃO DE CONFIGURAÇÃO (SETUP)
void setup() {
    Serial.begin(115200); 
    servidor.iniciarAP();
    configAtual.carregar();

    dht11.iniciar(); // Inicialização do DHT
    rele.iniciar();  // Inicialização do Relé

     if (!rtc.iniciar()) {
         Serial.println("Falha ao inicializar o RTC. O agendamento nao funcionará.");
     }

    if (configAtual.getAno() != 0) {
        configAtual.salvarTemporariamente(
            configAtual.getDia(), 
            configAtual.getMes(), 
            configAtual.getAno(), 
            configAtual.getHora(), 
            configAtual.getMinuto(), 
            configAtual.getSegundo(), 
            configAtual.getDuracao(), 
            configAtual.getCiclo());
    }
    //controle.testarSistema();
}

// 5. FUNÇÃO DE LOOP
void loop() {
    // Manuseio de clientes (Acessa e modifica os objetos 'valvula' e 'configAtual')
    servidor.manusearClientes(rele, configAtual); 

    // Rotina principal do sistema embarcado
    minhaRotinaDeExecucao(); 
    delay(10); 
}

// 6. SUA ROTINA PRINCIPAL (Executada em segundo plano)
void minhaRotinaDeExecucao() {

    if (configAtual.isAtualizada()) {
        Serial.println("Detectada nova configuracao. Agendando no RTC...");
        
        // Passa a nova configuração para a classe RTC
        String ciclo = configAtual.getCiclo();
        
        // Você pode refinar a lógica de agendamento diário ou único aqui
        if (ciclo.equalsIgnoreCase("unico") || ciclo.equalsIgnoreCase("diario")) {
            
            rtc.agendarAcionamento(
                configAtual.getAno(), 
                configAtual.getMes(), 
                configAtual.getDia(),
                configAtual.getHora(), 
                configAtual.getMinuto(), 
                configAtual.getSegundo(),
                configAtual.getDuracao() 
            );
        }
        
        // Limpa a flag para que o agendamento não seja refeito a cada loop
        configAtual.clearAtualizada(); 
    }
    
    if (rtc.alarmeLigou()) {
        Serial.println(">>> ALARME 1 DISPAROU — LIGAR RELE <<<");
        rele.ligar();
    }

    // Evento de Alarme 2 (DESLIGAR e RE-AGENDAR)
    if (rtc.alarmeDesligou()) {
        Serial.println(">>> ALARME 2 DISPAROU — DESLIGAR RELE E RE-AGENDAR <<<");
        rele.desligar();

        // Lógica de re-agendamento cíclico do colega (1 minuto para frente)
        DateTime agora = rtc.getNow();
        rtc.agendarAcionamento(
            agora.year(), agora.month(), agora.day(),
            agora.hour(), agora.minute() + 1, agora.second(),
            configAtual.getDuracao() // Usando a duração salva pelo usuário
        );
    }
    
    //static long lastControlTime = 0;
    // O controle automático baseado em umidade deve ser executado periodicamente
    // if (millis() - lastControlTime > 10000) { // 10 segundos
    //     lastControlTime = millis();
        
    //     //controle.lerTemperatura(); // Lê sensores (Temp e Umid)
    //     //controle.controlarIrrigacao(); // Toma decisão de controle
    // }

    static long lastMsg = 0;
    if (millis() - lastMsg > 5000) { 
        lastMsg = millis();
        
        Serial.println("\n----------------------------------------------");

        if (dht11.atualizar()) {
            Serial.print("Temp: ");
            Serial.print(dht11.getTemperatura());
            Serial.print(" °C | Umidade: ");
            Serial.print(dht11.getUmidade());
            Serial.println(" %");
        } else {
            Serial.println("Falha ao ler DHT11.");
        }
        
        rtc.mostrarHora();
        Serial.println("Executando rotina principal...");
        
        // Imprime a configuração salva (chama o método da classe Configuracao)
        configAtual.imprimir(); 
        
        Serial.print("Status da Válvula: "); 
        Serial.println(rele.estaLigado() ? "LIGADO" : "DESLIGADO");
        Serial.println("----------------------------------------------");
        
        // Lógica de Agendamento: Aqui você usaria 'configAtual' para checar a hora atual (via RTC, por exemplo)
        // e chamar valvula.ligar() ou valvula.desligar() no momento certo.
    }
}