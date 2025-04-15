// Patryck Lopes
// patrycklopes762@gmail.com

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

// Definir pinos para as linhas e colunas do teclado matricial
#define ROWS 4
#define COLS 4
uint row_pins[ROWS] = {2, 3, 4, 5};  // Pinos das linhas
uint col_pins[COLS] = {6, 7, 8, 9};  // Pinos das colunas

// Definir pinos para os LEDs
#define RED_LED 10
#define GREEN_LED 11
#define BLUE_LED 12

// Mapa de teclas do teclado matricial
const char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

// Inicializar os pinos do teclado
void init_keypad() {
    for (int i = 0; i < ROWS; i++) {
        gpio_init(row_pins[i]);
        gpio_set_dir(row_pins[i], GPIO_OUT);
        gpio_put(row_pins[i], 1);  // Inicialmente desativar as linhas
    }
    for (int j = 0; j < COLS; j++) {
        gpio_init(col_pins[j]);
        gpio_set_dir(col_pins[j], GPIO_IN);
        gpio_pull_up(col_pins[j]);  // Ativar pull-up interno
    }
}

// Inicializar os LEDs
void init_leds() {
    gpio_init(RED_LED);
    gpio_set_dir(RED_LED, GPIO_OUT);
    gpio_put(RED_LED, 0);

    gpio_init(GREEN_LED);
    gpio_set_dir(GREEN_LED, GPIO_OUT);
    gpio_put(GREEN_LED, 0);

    gpio_init(BLUE_LED);
    gpio_set_dir(BLUE_LED, GPIO_OUT);
    gpio_put(BLUE_LED, 0);
}

// Ler a tecla pressionada
char read_keypad() {
    for (int i = 0; i < ROWS; i++) {
        gpio_put(row_pins[i], 0);  // Ativar a linha atual
        for (int j = 0; j < COLS; j++) {
            if (!gpio_get(col_pins[j])) {  // Verificar se a coluna está ativada
                while (!gpio_get(col_pins[j]));  // Aguardar a liberação da tecla
                gpio_put(row_pins[i], 1);  // Desativar a linha atual
                return keys[i][j];
            }
        }
        gpio_put(row_pins[i], 1);  // Desativar a linha atual
    }
    return '\0';  // Nenhuma tecla pressionada
}

// Controlar os LEDs com base na tecla pressionada
void control_leds(char key) {
    gpio_put(RED_LED, 0);
    gpio_put(GREEN_LED, 0);
    gpio_put(BLUE_LED, 0);

    switch (key) {
        case '1': case '4': case '7': case '*':
            gpio_put(RED_LED, 1);
            break;
        case '2': case '5': case '8': case '0':
            gpio_put(GREEN_LED, 1);
            break;
        case '3': case '6': case '9': case '#':
            gpio_put(BLUE_LED, 1);
            break;
        case 'A': case 'B': case 'C': case 'D':
            // Nenhum LED aceso
            break;
    }
}

int main() {
    stdio_init_all();
    init_keypad();
    init_leds();

    while (1) {
        char key = read_keypad();
        if (key != '\0') {
            printf("Tecla pressionada: %c\n", key);
            control_leds(key);
        }
        sleep_ms(100);  // Pequeno atraso para evitar leitura múltipla
    }
}
