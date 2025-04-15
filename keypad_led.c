#include <stdio.h>
#include <pico/stdlib.h>
#include "pico/stdio.h"

// Define os pinos GPIO para as linhas e para as colunas
#define L1 2
#define L2 3
#define L3 4
#define L4 5
#define C1 6
#define C2 7
#define C3 8
#define C4 9

// Mapeamento das teclas em uma matriz 4x4
char teclas[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

// Inicializando os pinos de GPIO pins
void init_teclado() {
    // Configuração das linhas como saídas
    gpio_init(L1);
    gpio_set_dir(L1, GPIO_OUT);
    gpio_put(L1, 0);

    gpio_init(L2);
    gpio_set_dir(L2, GPIO_OUT);
    gpio_put(L2, 0);

    gpio_init(L3);
    gpio_set_dir(L3, GPIO_OUT);
    gpio_put(L3, 0);

    gpio_init(L4);
    gpio_set_dir(L4, GPIO_OUT);
    gpio_put(L4, 0);

    gpio_init(C1);
    gpio_set_dir(C1, GPIO_IN);
    gpio_pull_down(C1);

    gpio_init(C2);
    gpio_set_dir(C2, GPIO_IN);
    gpio_pull_down(C2);

    gpio_init(C3);
    gpio_set_dir(C3, GPIO_IN);
    gpio_pull_down(C3);

    gpio_init(C4);
    gpio_set_dir(C4, GPIO_IN);
    gpio_pull_down(C4);
}

char leitura_teclado() {
    for (int row = 0; row < 4; row++) {
        // Coloca a linha atual em nível alto
        gpio_put(L1 + row, 1);

        for (int col = 0; col < 4; col++) {
            if (gpio_get(C1 + col)) {
                // Espera um tempo para estabilização da tecla pressionada
                sleep_ms(50);
                while (gpio_get(C1 + col)); // Espera até a tecla ser solta
                gpio_put(L1 + row, 0); // Reseta a linha atual
                return teclas[row][col];
            }
        }

        // Coloca a linha atual novamente para nível baixo
        gpio_put(L1 + row, 0);
    }
    return 0;
}

int main(){
  stdio_init_all();
  init_teclado();

  printf("Pressione uma tecla!\n");

  while(1){
    char key = leitura_teclado();
    if(key){
      printf("Tecla pressionada: %c\n", key);
    }
    sleep_ms(100);
  }
}


