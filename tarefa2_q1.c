#include "pico/stdlib.h"
#include "hardware/timer.h"

#define BUTTON_PIN 2
#define LED_PIN 4
#define BLINK_FREQUENCY_HZ 10
#define BLINK_DURATION_SEC 10

volatile uint8_t button_press_count = 0;
volatile bool led_blinking = false;

// Callback para verificar o estado do botão
bool button_callback(repeating_timer_t *rt) {
    static bool last_button_state = false;
    bool current_button_state = gpio_get(BUTTON_PIN);

    if (current_button_state && !last_button_state) {
        button_press_count++;
    }

    last_button_state = current_button_state;
    return true; // Continue chamando o callback
}

// Função para piscar o LED
void blink_led(uint pin, uint frequency_hz, uint duration_sec) {
    uint interval_us = 1000000 / frequency_hz;
    uint64_t total_blinks = frequency_hz * duration_sec;

    for (uint64_t i = 0; i < total_blinks; i++) {
        gpio_put(pin, 1);
        sleep_us(interval_us / 2);
        gpio_put(pin, 0);
        sleep_us(interval_us / 2);
    }
}

int main() {
    stdio_init_all();

    // Configuração dos pinos
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_down(BUTTON_PIN);

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0);

    // Configuração do temporizador repetitivo para monitorar o botão
    repeating_timer_t button_timer;
    add_repeating_timer_ms(10, button_callback, NULL, &button_timer);

    while (true) {
        if (button_press_count >= 5 && !led_blinking) {
            led_blinking = true; // Evita múltiplas execuções simultâneas
            button_press_count = 0; // Reinicia o contador

            blink_led(LED_PIN, BLINK_FREQUENCY_HZ, BLINK_DURATION_SEC);

            led_blinking = false; // Permite nova contagem após o piscar
        }

        tight_loop_contents(); // Economiza energia enquanto espera
    }

    return 0;
}
