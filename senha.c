//Projeto Sávio 
#include <stdio.h>
#include "pico/stdlib.h"
#include <string.h>

#define LED_RED 27
#define LED_GREEN 28

#define PIN_A 16
#define PIN_B 17
#define PIN_C 18
#define PIN_D 19
#define PIN_E 20
#define PIN_F 21
#define PIN_G 22

#define CHAVE_1 10
#define CHAVE_2 11
#define CHAVE_3 12
#define CHAVE_4 13

const uint8_t segment_pins[] = {16, 17, 18, 19, 20, 21, 22};
const uint8_t display_pins[] = {CHAVE_1, CHAVE_2, CHAVE_3, CHAVE_4};

const uint8_t display[15][7] = {
    {1, 1, 1, 1, 1, 1, 0},
    {0, 1, 1, 0, 0, 0, 0},
    {1, 1, 0, 1, 1, 0, 1},
    {1, 1, 1, 1, 0, 0, 1},
    {0, 1, 1, 0, 0, 1, 1},
    {1, 0, 1, 1, 0, 1, 1},
    {1, 0, 1, 1, 1, 1, 1},
    {1, 1, 1, 0, 0, 0, 0},
    {1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 0, 0, 1, 1},
    {1, 1, 1, 0, 1, 1, 1},
    {0, 0, 1, 1, 1, 1, 1},
    {1, 0, 0, 1, 1, 1, 0},
    {0, 1, 1, 1, 1, 0, 1},
    {0, 1, 1, 0, 1, 1, 1}
};

#define SET 1
#define RESET 0
#define COMP 4

char password1[COMP+1] = {0};

const uint8_t Linha[] = {2, 3, 4, 5};
const uint8_t Coluna[] ={6, 7, 8, 9};

char teclas[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

void init_display() {
    for (int i = 0; i < 7; i++) {
        gpio_init(segment_pins[i]);
        gpio_set_dir(segment_pins[i], GPIO_OUT);
        gpio_put(segment_pins[i], 0);
    }
    for (int i = 0; i < 4; i++) {
        gpio_init(display_pins[i]);
        gpio_set_dir(display_pins[i], GPIO_OUT);
        gpio_put(display_pins[i], 0);
    }
}

void clear_display() {
    int segmentos[] = {PIN_A, PIN_B, PIN_C, PIN_D, PIN_E, PIN_F, PIN_G};
    for (int i = 0; i < 7; i++) {
        gpio_put(segmentos[i], 0);
    }
}

void display_caractere(char c, int display_num) {
    int index = -1;
    if (c >= '0' && c <= '9') {
        index = c - '0';
    } else {
        switch (c) {
            case 'A': index = 10; break;
            case 'B': index = 11; break;
            case 'C': index = 12; break;
            case 'D': index = 13; break;
            case '#': index = 14; break;
            default: return; 
        }
    }
    for (int i = 0; i < 4; i++) {
        gpio_put(display_pins[i], i == display_num ? 1 : 0);
    }
    gpio_put(display_pins[display_num], 1);
    int segmentos[] = {PIN_A, PIN_B, PIN_C, PIN_D, PIN_E, PIN_F, PIN_G};
    for (int i = 0; i < 7; i++) {
        gpio_put(segmentos[i], display[index][i]);
    }
    sleep_ms(5);
}

void init_teclado() {
    for (int i = 0; i < 4; i++) {
        gpio_init(Linha[i]);
        gpio_set_dir(Linha[i], GPIO_OUT);
        gpio_put(Linha[i],RESET);
        gpio_init(Coluna[i]);
        gpio_set_dir(Coluna[i], GPIO_IN);
        gpio_pull_down(Coluna[i]);
    }
}

char leitura_teclado() {
    for (int row = 0; row < 4; row++) {
        gpio_put(Linha[row], SET);
        for (int col = 0; col < 4; col++) {
            if (gpio_get(Coluna[col])) {
                sleep_ms(100);
                while (gpio_get(Coluna[col]));
                gpio_put(Linha[row], RESET);
                return teclas[row][col];
            }
        }
        gpio_put(Linha[row], RESET);
    }
    return 0; 
}

void get_senha(char *senha, int tam) {
    int index = 0;
    while (index < tam) {
        char key = leitura_teclado();
        if (key != 0) {
            printf("%c", key);
            senha[index] = key;
            display_caractere(key, index - '0');
            index = index + 1;
            sleep_ms(100); 
        }
    }
    senha[tam] = '\0'; 
}

bool compare_senhas(const char *password1, const char *password2) {
    return strcmp(password1, password2) == 0; 
}

int main() {
    stdio_init_all();
    init_teclado();
    init_display();
    gpio_init(LED_RED);
    gpio_init(LED_GREEN);
    gpio_set_dir(LED_RED,GPIO_OUT);
    gpio_set_dir(LED_GREEN,GPIO_OUT);
    printf("Insira a senha de 4 digitos desejada!\n");
    get_senha(password1, COMP);
    printf("\nSenha gravada!\n");
    int cont = 0;
    bool acesso = 1;
    int count = 3;
    while (acesso) {
        printf("Digite a senha do cofre de 4 digitos!\n");
        char input[COMP+1] = {0};
        get_senha(input, COMP);
        if (compare_senhas(input, password1)) {
            printf("\nSenha Correta: Acesso ao cofre!\n");
            acesso = !acesso;
            gpio_put(LED_GREEN, true);
        }
        else{
            printf("\nSenha Incorreta: Acesso ao cofre negado !\n");
            cont = cont +1;
            count = count - 1;
            gpio_put(LED_RED,true);
            sleep_ms(500);
            gpio_put(LED_RED,false);
            sleep_ms(500);
            gpio_put(LED_RED,true);
            sleep_ms(500);
            gpio_put(LED_RED,false);
            sleep_ms(500);
            gpio_put(LED_RED,true);
            sleep_ms(500);
            gpio_put(LED_RED,false);
            sleep_ms(250);
            printf("O numero de tentativas restantes é: %d\n", count);
        }
        sleep_ms(50);
        if(cont>2){
            printf("\n Três tentativas bloqueadas por 5 segundos!\n");
            gpio_put(LED_RED,true);
            sleep_ms(5000);
            gpio_put(LED_RED,false);
            cont= 0;
        }
    }
}