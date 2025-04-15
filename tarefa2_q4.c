#include "stdio.h"
#include "string.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define BTN_A_PIN 5
#define I2C_SCL_PIN 8
#define I2C_SDA_PIN 9

#define DS1307_ADDR 0x68 
#define DEBOUNCE_TIME 600000

static long last_clicked_btn_a = 1e6; 
static volatile bool setar_hora = false; 

static void BCD_to_ASCI (char *buf, uint8_t val) {
    buf[0] = (char) ((val >> 4) + '0');
    buf[1] = (char) ((val & 0x0F) + '0');
}

static void busca_reg_to_mem(uint8_t addr, uint8_t *buf, uint16_t pos, int tam, int tpos) {
    uint8_t aux[2];
    if (tpos == 2) {
        aux[0] = pos >> 8;
        aux[1] = pos & 0xFF;
        i2c_write_blocking(i2c0, addr, aux, 2, true);
    } else {
        aux[0] = pos & 0xFF; 
        i2c_write_blocking(i2c0, addr, aux, 1, true);
   } 
   // Le a resposta
   i2c_read_blocking(i2c0, addr, buf, tam, false);
}

bool repeating_timer_callback(__unused struct repeating_timer *t)
{
  if(!setar_hora){
    uint8_t regs[7];
    char leitura[] = "Data: xx/xx/xx Hora: xx:xx:xx\n"; 
    busca_reg_to_mem(DS1307_ADDR, regs, 0, 7, 1);

    BCD_to_ASCI(leitura+6, regs[4]);
    BCD_to_ASCI(leitura+9, regs[5]);
    BCD_to_ASCI(leitura+12, regs[6]);
    BCD_to_ASCI(leitura+21, regs[2] & 0x3F);
    BCD_to_ASCI(leitura+24, regs[1]);
    BCD_to_ASCI(leitura+27, regs[0] & 0x7F);

    printf(leitura);
  }
  return true;
}

static void button_click_callback(unsigned int gpio, long unsigned int events) 
{
  if (gpio == BTN_A_PIN && time_us_32() - last_clicked_btn_a > DEBOUNCE_TIME)
  {
    last_clicked_btn_a = time_us_32();
    setar_hora = !setar_hora;
  }
}

static inline uint8_t convert_num_to_bcd(uint8_t num) {
    return ((num / 10) << 4) | (num % 10);
}


static void ajustar_relogio(uint8_t d, uint8_t m, uint8_t a, uint8_t h, uint8_t mt, uint8_t s) {
    uint8_t buf[8];
    buf[0] = 0; 
    buf[1] = convert_num_to_bcd(s);   
    buf[2] = convert_num_to_bcd(mt);    
    buf[3] = convert_num_to_bcd(h);    
    buf[4] = 1;             
    buf[5] = convert_num_to_bcd(d); 
    buf[6] = convert_num_to_bcd(m); 
    buf[7] = convert_num_to_bcd(a); 
    i2c_write_blocking (i2c0, DS1307_ADDR, buf, 8, false);
}

int main() {
    stdio_init_all();
    sleep_ms(1000);
    printf("Serial iniciado\n");

    i2c_init(i2c0, 100 * 1000);
    sleep_ms(100);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C); 
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C); 
    gpio_pull_up(I2C_SDA_PIN); 
    gpio_pull_up(I2C_SCL_PIN);

    gpio_init(BTN_A_PIN);
    gpio_set_dir(BTN_A_PIN, GPIO_IN);
    gpio_pull_up(BTN_A_PIN);
    gpio_set_irq_enabled_with_callback(BTN_A_PIN, GPIO_IRQ_EDGE_FALL, true, &button_click_callback);

    printf("Configurando tempo\n");
    // 24/09/24 13:27:00
    ajustar_relogio(24,9,24,13,27,00);

    struct repeating_timer timer;
    add_repeating_timer_ms(5000, &repeating_timer_callback, NULL, &timer);
    repeating_timer_callback(&timer);

    while(true){
      if(setar_hora){
        printf("Formato: dd/mm/aa-hh:mm:ss:\n");
        uint8_t d, m, a, h, mt, s;
        scanf("%2d/%2d/%2d-%2d:%2d:%2d:",&d,&m,&a,&h,&mt,&s);
        printf("\n");
        char leitura[41]; 
        if(d == NULL || m == NULL || a == NULL || h == NULL || mt == NULL || s == NULL){
          printf("Algo de errado não está certo \n");
          continue;
        }
        sprintf(leitura, "Registrando: %02d/%02d/%02d Hora: %02d:%02d:%02d\n",d,m,a,h,mt,s);
        printf(leitura);
        ajustar_relogio(d,m,a,h,mt,s);
        setar_hora = false;
      }
    }

}
