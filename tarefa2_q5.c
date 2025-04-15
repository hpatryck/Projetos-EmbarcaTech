#include "pico/stdlib.h"
#include "hardware/pwm.h"

#define RED_LED 13
#define BLUE_LED 12
#define GREEN_LED 11

#define RED_LED_FREQ 1000
#define BLUE_LED_FREQ 10000

uint red_led_slice;
uint blue_led_slice;
volatile uint8_t red_duty_cycle = 5;

void configure_pwm(uint gpio, uint frequency_hz) {
    
    gpio_set_function(gpio, GPIO_FUNC_PWM);

    uint slice_num = pwm_gpio_to_slice_num(gpio);
    
    float clkdiv = (float)125000000 / (frequency_hz * 256);
    
    pwm_set_clkdiv(slice_num, clkdiv);
    pwm_set_wrap(slice_num, 255);
    pwm_set_enabled(slice_num, true);
    
    if (gpio == RED_LED) {
        red_led_slice = slice_num;
    } else if (gpio == BLUE_LED) {
        blue_led_slice = slice_num;
    }
}

bool timer_callback(struct repeating_timer *t) {
    
    red_duty_cycle += 5;
    
    if (red_duty_cycle > 100) {
        red_duty_cycle = 5;
    }
    
    pwm_set_gpio_level(RED_LED, (red_duty_cycle * 255) / 100);
    
    printf("Red: %d%%\n", red_duty_cycle);
    
    return true;
}

int main() {
    stdio_init_all();
    
    configure_pwm(RED_LED, RED_LED_FREQ);
    configure_pwm(BLUE_LED, BLUE_LED_FREQ);
    
    pwm_set_gpio_level(RED_LED, (red_duty_cycle * 255) / 100);
    pwm_set_gpio_level(BLUE_LED, 128);
    
    struct repeating_timer timer;
    add_repeating_timer_ms(2000, timer_callback, NULL, &timer);
    
    while (true) {
        tight_loop_contents();
    }

    return 0;
}
