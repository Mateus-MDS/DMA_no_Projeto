/**
 * Controle de ar condicionado via IR com DMA
 * Versão simplificada - apenas UART
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "custom_ir.h"

// Configurações
#define IR_PIN 11          // Pino para saída IR
#define LED_PIN 25         // LED onboard do Pico
#define LED_EXTERNO 15     // LED extra para demonstrar CPU livre

// Menu de comandos via UART
void show_menu() {
    printf("\n??????????????????????????????????????????\n");
    printf("?  CONTROLE IR COM DMA - EMBARCATECH    ?\n");
    printf("?????????????????????????????????????????????\n");
    printf("?  1 - Ligar AC - Led verde pica - cpu livre?\n");
    printf("?  2 - Desligar AC                          ?\n");
    printf("?  3 - Temperatura 22°C                     ?\n");
    printf("?  4 - Temperatura 20°C                     ?\n");
    printf("?  5 - Ventilador Nível 1                   ?\n");
    printf("?  6 - Ventilador Nível 2                   ?\n");
    printf("?  7 - Demonstração automática              ?\n");
    printf("?  0 - Mostrar este menu                    ?\n");
    printf("?????????????????????????????????????????????\n");
    printf("Digite uma opção: ");
}

// Processa comandos do teclado
void process_uart_input() {
    int ch = getchar_timeout_us(0);
    if (ch == PICO_ERROR_TIMEOUT) {
        return;
    }
    
    printf("%c\n", ch);
    
    switch (ch) {
        case '1':
            printf("\n?? DEMONSTRANDO CPU LIVRE COM DMA!\n");
            printf("   Transmitindo IR E piscando LED ao mesmo tempo...\n");
            
            gpio_put(LED_PIN, 1);
            
            // ? INICIA TRANSMISSÃO IR VIA DMA
            // A função turn_on_ac() usa DMA, então retorna rapidamente
            turn_on_ac();
            
            // ?? PROVA: Enquanto DMA transmite, CPU pode fazer outras coisas!
            // Vamos piscar um LED 10 vezes durante a transmissão
            printf("   CPU livre: piscando LED externo 10x...\n");
            for(int i = 0; i < 10; i++) {
                gpio_put(LED_EXTERNO, 1);
                sleep_ms(50);
                gpio_put(LED_EXTERNO, 0);
                sleep_ms(50);
                printf("   . ");
            }
            printf("\n   ? CPU estava LIVRE durante transmissão IR!\n\n");
            
            gpio_put(LED_PIN, 0);
            break;
            
        case '2':
            gpio_put(LED_PIN, 1);
            turn_off_ac();
            gpio_put(LED_PIN, 0);
            break;
            
        case '3':
            gpio_put(LED_PIN, 1);
            set_temp_22c();
            gpio_put(LED_PIN, 0);
            break;
            
        case '4':
            gpio_put(LED_PIN, 1);
            set_temp_20c();
            gpio_put(LED_PIN, 0);
            break;
            
        case '5':
            gpio_put(LED_PIN, 1);
            set_fan_level_1();
            gpio_put(LED_PIN, 0);
            break;
            
        case '6':
            gpio_put(LED_PIN, 1);
            set_fan_level_2();
            gpio_put(LED_PIN, 0);
            break;
            
        case '7':
            printf("\n>>> DEMONSTRAÇÃO AUTOMÁTICA <<<\n");
            ir_demo();
            break;
            
        case '0':
            show_menu();
            break;
            
        default:
            printf("? Opção inválida! Digite '0' para ver o menu.\n");
            break;
    }
}

int main() {
    // Inicializar stdio
    stdio_init_all();
    sleep_ms(2000);
    
    printf("\n\n");
    printf("????????????????????????????????????????????????????\n");
    printf("?                                                  ?\n");
    printf("?    SISTEMA IR COM DMA - PROJETO EMBARCATECH     ?\n");
    printf("?                                                  ?\n");
    printf("?    Raspberry Pi Pico (RP2040)                   ?\n");
    printf("?    Transmissão IR otimizada com DMA             ?\n");
    printf("?                                                  ?\n");
    printf("????????????????????????????????????????????????????\n");
    printf("\n");
    
    // Configurar LED
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0);
    
    // Configurar LED externo (para demonstração de CPU livre)
    gpio_init(LED_EXTERNO);
    gpio_set_dir(LED_EXTERNO, GPIO_OUT);
    gpio_put(LED_EXTERNO, 0);
    
    // Inicializar sistema IR com DMA
    printf("? Inicializando sistema IR com DMA...\n");
    
    if (!custom_ir_init(IR_PIN)) {
        printf("? ERRO: Falha ao inicializar sistema IR!\n");
        printf("   Piscando LED de erro...\n");
        while (1) {
            gpio_put(LED_PIN, 1);
            sleep_ms(100);
            gpio_put(LED_PIN, 0);
            sleep_ms(100);
        }
    }
    
    printf("? Sistema IR inicializado com sucesso!\n");
    printf("   Pino IR: GPIO %d\n", IR_PIN);
    printf("   Carrier: 38kHz via PWM\n");
    printf("   DMA: Ativo e configurado\n");
    printf("\n");
    
    // Mostrar menu inicial
    show_menu();
    
    // Loop principal
    while (1) {
        // Processar comandos UART
        process_uart_input();
        
        // Pequeno delay
        sleep_ms(10);
    }
    
    return 0;
}