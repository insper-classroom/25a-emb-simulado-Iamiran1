/*
 * LED blink with FreeRTOS
 */
#include <stdio.h>
#include <string.h> 
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/irq.h"


static volatile bool fired = false;
const int ECHO_PIN = 12;
const int TRIG_PIN = 13;

volatile bool echo_got = false;
volatile int start_us;
volatile int end_us = 0;
volatile alarm_id_t alarm;

int64_t alarm_callback(alarm_id_t id, void *user_data) {
    fired = true;
    return 0;
 }

void gpio_callback(uint gpio, uint32_t events){
    if (gpio == ECHO_PIN){
        if (events & GPIO_IRQ_EDGE_RISE) {
            echo_got = true;
            start_us =  get_absolute_time();
        }
        else if (events & GPIO_IRQ_EDGE_FALL) {
            end_us = get_absolute_time();
            cancel_alarm(alarm);

        }
    }
}

void send_trig_pulse(){   
    gpio_put(TRIG_PIN,1);
    sleep_ms(30);
    gpio_put(TRIG_PIN,0);
}

int main() {
    stdio_init_all();
    printf("oi \n");
    gpio_init(ECHO_PIN);
    gpio_set_dir(ECHO_PIN, GPIO_IN);
    gpio_set_irq_enabled_with_callback(ECHO_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    gpio_init(TRIG_PIN);
    gpio_set_dir(TRIG_PIN, GPIO_OUT);
    sleep_us(80);

    while(1){
        while(true){
            alarm = add_alarm_in_ms(5000, alarm_callback, NULL, false);    
            send_trig_pulse();
            if (fired) {
                fired = 0;
                printf("Sensor 1 - dist: falha");
                }
            if(!fired){
                    while ((alarm == 0) && (end_us == 0)) {}
                    int delta_t = end_us - start_us;
                    int distancia = (int)((delta_t * 0.0343) / 2.0);
                    printf("Sensor 1 - dist: %d cm\n",distancia);
                    echo_got = false;
            }

     }
    }
}
