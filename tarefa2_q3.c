#include <stdio.h>
#include "pico/stdlib.h"
#include <stdio.h> 
#include <string.h>
#include "hardware/uart.h" 

#define UART0_TX_PIN 0
#define UART0_RX_PIN 1
#define UART1_TX_PIN 4
#define UART1_RX_PIN 5

char palavra[50]; 

int main() {
  stdio_init_all();

  
  uart_init(uart0, 115200); 
  gpio_set_function(UART0_TX_PIN, GPIO_FUNC_UART); 
  gpio_set_function(UART0_RX_PIN, GPIO_FUNC_UART); 

  uart_init(uart1, 115200); 
  gpio_set_function(UART1_TX_PIN, GPIO_FUNC_UART); // Configura pino 4 como TX
  gpio_set_function(UART1_RX_PIN, GPIO_FUNC_UART); // Configura pino 5 como RX
  
  uart_set_fifo_enabled(uart0, true);
  uart_set_fifo_enabled(uart1, true);
  
while (true) {
// Recebe a mensagem via console
    printf("\nDigite mensagem para enviar via UART 0:");
    scanf("%s", palavra);

    // Limpa o buffer da UART1 para descartar qualquer dado antigo (como o prompt)
    while (uart_is_readable(uart1)) {
        (void)uart_getc(uart1);
    }

    // Envia a mensagem pela UART0
    uart_puts(uart0, palavra);

    // Aguarda e verifica se há dados disponíveis na UART1
    sleep_ms(100); // Dá um tempo para a transmissão
    if (uart_is_readable(uart1)) {
        char recebido[50] = {0};
        int i = 0;

        // Lê a mensagem recebida da UART1
        while (uart_is_readable(uart1) && i < sizeof(recebido) - 1) {
            recebido[i++] = uart_getc(uart1);
        }
        recebido[i] = '\0'; // Finaliza a string recebida
        sleep_ms(50);
        // Imprime a mensagem recebida no console
        printf("\nMensagem recebida da UART1: %s\n", recebido);
       
              
    } else {
        printf("Nenhuma mensagem recebida da UART1.\n");
    } 
}    
}
