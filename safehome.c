// Projeto de sistema embarcado EmbarcaTech
// Sistema de segurança residencial com múltiplos sensores e atuadores
// Patryck Lopes
// patrycklopes762@gmail.com

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"

#define MOTION_SENSOR_PIN 16
#define DOOR_SENSOR_PIN 17
#define SMOKE_SENSOR_PIN 26
#define TEMP_SENSOR_PIN 27
#define BUZZER_PIN 18
#define LED_R_PIN 19
#define LED_G_PIN 20
#define LED_B_PIN 21
#define DOOR_LOCK_PIN 22
#define ALERT_LED_PIN 28

bool system_armed = true; // Sistema começa armado
uint16_t smoke_threshold = 2000; // Limiar ajustável para fumaça
uint16_t temp_threshold = 2500;  // Limiar ajustável para temperatura

// Função para debounce
bool debounce(int pin) {
    bool stable_state = gpio_get(pin);
    sleep_ms(50); // Aguarda 50ms para verificar se o estado permanece o mesmo
    if (gpio_get(pin) == stable_state) {
        return stable_state;
    }
    return !stable_state; // Retorna o estado oposto se houver flutuação
}

void activate_alarm() {
    gpio_put(BUZZER_PIN, 1); // Ligar sirene
    gpio_put(LED_R_PIN, 0);  // Ligar LED vermelho (nível baixo)
    gpio_put(LED_G_PIN, 1);  // Desligar LED verde (nível alto)
    gpio_put(LED_B_PIN, 1);  // Desligar LED azul (nível alto)
    gpio_put(ALERT_LED_PIN, 1); // Ligar luzes de alerta
    gpio_put(DOOR_LOCK_PIN, 1); // Bloquear portas
    printf("Alarme ativado!\n");
}

void deactivate_alarm() {
    gpio_put(BUZZER_PIN, 0); // Desligar sirene
    gpio_put(LED_R_PIN, 1);  // Desligar LED vermelho (nível alto)
    gpio_put(LED_G_PIN, 0);  // Ligar LED verde (nível baixo)
    gpio_put(LED_B_PIN, 1);  // Desligar LED azul (nível alto)
    gpio_put(ALERT_LED_PIN, 0); // Desligar luzes de alerta
    gpio_put(DOOR_LOCK_PIN, 0); // Desbloquear portas
    printf("Alarme desativado.\n");
}

void indicate_system_status(bool alert_condition) {
    if (alert_condition) {
        gpio_put(LED_R_PIN, 0);  // Ligar LED vermelho (nível baixo)
        gpio_put(LED_G_PIN, 1);  // Desligar LED verde (nível alto)
        gpio_put(LED_B_PIN, 1);  // Desligar LED azul (nível alto)
    } else {
        gpio_put(LED_R_PIN, 1);  // Desligar LED vermelho (nível alto)
        gpio_put(LED_G_PIN, 0);  // Ligar LED verde (nível baixo)
        gpio_put(LED_B_PIN, 1);  // Desligar LED azul (nível alto)
    }
}

void check_sensors() {
    adc_select_input(0);
    uint16_t smoke_level = adc_read(); // Nível de fumaça/gás
    adc_select_input(1);
    uint16_t temperature = adc_read(); // Temperatura

    bool motion_detected = !debounce(MOTION_SENSOR_PIN); // Usa debounce para movimento
    bool door_opened = !debounce(DOOR_SENSOR_PIN);       // Usa debounce para porta

    printf("Smoke: %d, Temp: %d, Motion: %d, Door: %d\n", smoke_level, temperature, motion_detected, door_opened);

    bool alert_condition = motion_detected || door_opened || smoke_level > smoke_threshold || temperature > temp_threshold;

    if (system_armed) {
        if (alert_condition) {
            activate_alarm(); // Ativar alarme em caso de intrusão ou condições críticas
        } else {
            deactivate_alarm(); // Desativar alarme se tudo estiver normal
        }
    }

    indicate_system_status(alert_condition); // Atualiza o estado do LED RGB
}

int main() {
    stdio_init_all(); // Inicializa a comunicação serial
    adc_init();
    adc_gpio_init(SMOKE_SENSOR_PIN);
    adc_gpio_init(TEMP_SENSOR_PIN);

    gpio_init(MOTION_SENSOR_PIN);
    gpio_init(DOOR_SENSOR_PIN);
    gpio_set_dir(MOTION_SENSOR_PIN, GPIO_IN);
    gpio_set_dir(DOOR_SENSOR_PIN, GPIO_IN);
    gpio_pull_up(MOTION_SENSOR_PIN); // Ativa resistor de pull-up interno
    gpio_pull_up(DOOR_SENSOR_PIN);   // Ativa resistor de pull-up interno

    gpio_init(BUZZER_PIN);
    gpio_init(LED_R_PIN);
    gpio_init(LED_G_PIN);
    gpio_init(LED_B_PIN);
    gpio_init(DOOR_LOCK_PIN);
    gpio_init(ALERT_LED_PIN);
    gpio_set_dir(BUZZER_PIN, GPIO_OUT);
    gpio_set_dir(LED_R_PIN, GPIO_OUT);
    gpio_set_dir(LED_G_PIN, GPIO_OUT);
    gpio_set_dir(LED_B_PIN, GPIO_OUT);
    gpio_set_dir(DOOR_LOCK_PIN, GPIO_OUT);
    gpio_set_dir(ALERT_LED_PIN, GPIO_OUT);

    // Inicializa o LED verde (sistema normal)
    gpio_put(LED_R_PIN, 1);  // Desligar LED vermelho
    gpio_put(LED_G_PIN, 0);  // Ligar LED verde
    gpio_put(LED_B_PIN, 1);  // Desligar LED azul

    printf("Sistema inicializado. Aguardando eventos...\n");

    while (true) {
        check_sensors();
        sleep_ms(100); // Verificar sensores a cada 100ms
    }

    return 0;
}