#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

// Define GPIO pins
#define LED_PIN_VD 13
#define LED_PIN_VM 12
#define LED_PIN_AZ 11
#define BUTTON_PIN_A 5
#define BUTTON_PIN_B 6

int main() {
  // Inicializa LED como saída
  gpio_init(LED_PIN_VM);
  gpio_set_dir(LED_PIN_VM, GPIO_OUT);

  gpio_init(LED_PIN_VD);
  gpio_set_dir(LED_PIN_VD, GPIO_OUT);

  gpio_init(LED_PIN_AZ);
  gpio_set_dir(LED_PIN_AZ, GPIO_OUT);

  // Inicializa o botão como entrada com resistor de pull-down
  gpio_init(BUTTON_PIN_A);
  gpio_set_dir(BUTTON_PIN_A, GPIO_IN);
  gpio_pull_down(BUTTON_PIN_A); // Ativar resistor de pull-down

  gpio_init(BUTTON_PIN_B);
  gpio_set_dir(BUTTON_PIN_B, GPIO_IN);
  gpio_pull_down(BUTTON_PIN_B); // Ativar resistor de pull-down

  while (true) {
    // Verificar o estado do botão (ativo baixo com pull-down)
    if (gpio_get(BUTTON_PIN_A)) {
      // Se botão A pressionado, liga o LED AZ
      gpio_put(LED_PIN_AZ, 1);
      gpio_put(LED_PIN_VM, 0);
      gpio_put(LED_PIN_VD, 0);
      sleep_ms(100); // Delay de 100 milissegundos
    } else if (gpio_get(BUTTON_PIN_B)) {
      // Se botão B pressionado, liga o LED VM
      gpio_put(LED_PIN_AZ, 0);
      gpio_put(LED_PIN_VM, 1);
      gpio_put(LED_PIN_VD, 0);
    } else {
      // Se nenhum botão pressionado, liga o LED VD
      gpio_put(LED_PIN_AZ, 0);
      gpio_put(LED_PIN_VM, 0);
      gpio_put(LED_PIN_VD, 1);
    }
  }

}