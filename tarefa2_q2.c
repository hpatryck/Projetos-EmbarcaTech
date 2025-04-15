#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "hardware/pwm.h"

#define LED_PIN 10
#define BUTTON 1
#define BUTTON2 2

volatile uint count = 0;
volatile bool isLedBlinking = false;
volatile uint frequency = 10;

void timerCallback(uint gpio, uint32_t events) {
  //Tratando o botão 1
  if (gpio == BUTTON && !gpio_get(BUTTON)) {
    count++;
    printf("Botão 1 apertado %d vezes!\n", count);
    if (count == 5) {
      isLedBlinking = true;
      count = 0;
    }
  }
  
  //Tratando o botão 2
  if (gpio == BUTTON2 && !gpio_get(BUTTON2)) {
    if (frequency == 10){
      printf("Botão 2 apertado, mudando frequência para 1Hz\n");
      frequency = 1;

    } else {
      printf("Botão 2 apertado, retornando para frequência de 10Hz\n");
      frequency = 10;
    }
  }
}

//Inicializa o led e o botão
void initPins(){
  // Inicialização do LED
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);

  // Inicialização do botão 1
  gpio_init(BUTTON);
  gpio_set_dir(BUTTON, GPIO_IN);
  gpio_pull_up(BUTTON);

  // Inicialização do botão 2
  gpio_init(BUTTON2);
  gpio_set_dir(BUTTON2, GPIO_IN);
  gpio_pull_up(BUTTON2);
  
}

int main() {
  stdio_init_all();
  initPins();
  
  // Configuração das interrupções
  gpio_set_irq_enabled_with_callback(BUTTON, GPIO_IRQ_EDGE_FALL, true, &timerCallback);
  gpio_set_irq_enabled(BUTTON2, GPIO_IRQ_EDGE_FALL, true);

  printf("Aperte o botão 1 cinco vezes para piscar o LED em 10Hz\nAperte o botão 2 para alternar entre 10Hz e 1Hz\n");

  while (true) {
    if (isLedBlinking) {
      uint32_t startTime = time_us_32();
      uint32_t blinkDuration = 10000000; // 10 segundos em microssegundos
      
      while ((time_us_32() - startTime) < blinkDuration) {
        gpio_put(LED_PIN, 1);
        sleep_ms(1000/(frequency)); //tempo = 1000 milissegundos /frequencia 
        gpio_put(LED_PIN, 0);
        sleep_ms(1000/(frequency));
      }
      isLedBlinking = false;
    }
      
      sleep_ms(10);
  }
}
