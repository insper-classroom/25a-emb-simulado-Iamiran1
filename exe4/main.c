/*
 * LED blink with FreeRTOS
 */
#include "pico/stdlib.h"
#include <stdio.h>

#include "hardware/gpio.h"


const int LED_PIN_R = 2;
const int LED_PIN_G = 4 ;
const int BTN_PIN_R = 7;
const int BTN_PIN_G = 6;
volatile int btn_flag_red = 0;
volatile int btn_flag_green = 0;


void btn_callback(uint gpio, uint32_t events ){
  if (events == 0x4) { // fall edge
    if (gpio == BTN_PIN_R)
      btn_flag_red = 1;
    if (gpio == BTN_PIN_G)
      btn_flag_green = 1;
  }



}


int main() {
  stdio_init_all();
  gpio_init(LED_PIN_G);
  gpio_init(LED_PIN_R);
  gpio_init(BTN_PIN_R);
  gpio_init(BTN_PIN_G);


  gpio_set_dir(LED_PIN_R,GPIO_OUT);
  gpio_set_dir(LED_PIN_G,GPIO_OUT);
  gpio_set_dir(BTN_PIN_R,GPIO_IN);
  gpio_set_dir(BTN_PIN_G,GPIO_IN);

  gpio_pull_up(BTN_PIN_R);
  gpio_pull_up(BTN_PIN_G);

  gpio_set_irq_enabled_with_callback(BTN_PIN_R, GPIO_IRQ_EDGE_FALL, true, &btn_callback);
  gpio_set_irq_enabled(BTN_PIN_G, GPIO_IRQ_EDGE_FALL, true);
  
  while (true) {
    if(btn_flag_red){
            gpio_put(LED_PIN_R,!gpio_get(LED_PIN_R));
            sleep_ms(300);
            gpio_put(LED_PIN_R,!gpio_get(LED_PIN_R));
            btn_flag_red = 0;
    }
    if(btn_flag_green){
      gpio_put(LED_PIN_G,!gpio_get(LED_PIN_G));
      sleep_ms(300);
      gpio_put(LED_PIN_G,!gpio_get(LED_PIN_G));
      printf("A");
      btn_flag_green = 0;

    }
  }
}


