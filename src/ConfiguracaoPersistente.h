#ifndef CONFIGURACAOPERSISTENTE_H
#define CONFIGURACAOPERSISTENTE_H

#include <Arduino.h>
#include "FS.h" // Biblioteca base do File System
#include "SPIFFS.h" // Sistema de Arquivos SPIFFS
#include <ArduinoJson.h> // Biblioteca necessária para serialização JSON

// Define o tamanho do buffer JSON (ajuste se necessário)
#define JSON_BUFFER_SIZE 256

/**
 * @brief Classe para armazenar a configuração e gerenciar a persistência na Flash (SPIFFS).
 */
class ConfiguracaoPersistente {
private:
    // Atributos de Configuração
    int dia = 0;
    int mes = 0;
    int ano = 0;
    int hora = 0;
    int minuto = 0;
    int segundo = 0;
    int duracao = 0; 
    String ciclo = "unico"; 

    const char* ARQUIVO_CONFIG = "/config.json"; // Nome do arquivo na Flash

    // Função privada para inicializar o SPIFFS
    bool initSPIFFS(); 

public:
    // Construtor
    ConfiguracaoPersistente();

    // Métodos de Persistência
    bool carregar();
    bool salvar();

    // Métodos de Controle (Setters)
    void salvarTemporariamente(int d, int m, int a, int h, int min, int s, int dur, String c);
    
    // Métodos de Acesso (Getters)
    void imprimir(); // Imprime a config atual no Serial
    int getAno() const { return ano; }
    String getCiclo() const { return ciclo; }
};

#endif // CONFIGURACAOPERSISTENTE_H