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

volatile int start_us;
volatile int end_us = 0;

static volatile bool fired_2 = false;
const int ECHO_PIN_2 = 18;
const int TRIG_PIN_2 = 19;

volatile int start_us_2;
volatile int end_us_2 = 0;

int64_t alarm_callback(alarm_id_t id, void *user_data) {
    fired = true;
    return 0;
 }
int64_t alarm_callback2(alarm_id_t id, void *user_data) {
    fired_2 = true;
    return 0;
 }
void gpio_callback(uint gpio, uint32_t events){
    if (gpio == ECHO_PIN){
        if (events & GPIO_IRQ_EDGE_RISE) {
            start_us =  get_absolute_time();
        }
        else if (events & GPIO_IRQ_EDGE_FALL) {
            end_us = get_absolute_time();

        }
    }
    if (gpio == ECHO_PIN_2){
        if (events & GPIO_IRQ_EDGE_RISE) {
            start_us_2 =  get_absolute_time();
        }
        else if (events & GPIO_IRQ_EDGE_FALL) {
            end_us_2 = get_absolute_time();

        }
        
    }
}

void send_trig_pulse(){   
    gpio_put(TRIG_PIN,1);
    gpio_put(TRIG_PIN_2,1);

    sleep_ms(30);
    gpio_put(TRIG_PIN,0);
    gpio_put(TRIG_PIN_2,0);
}


int main() {
    stdio_init_all();
    printf("oi \n");
    gpio_init(ECHO_PIN);
    gpio_set_dir(ECHO_PIN, GPIO_IN);
    gpio_set_irq_enabled_with_callback(ECHO_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    gpio_init(TRIG_PIN);
    gpio_set_dir(TRIG_PIN, GPIO_OUT);
    gpio_put(TRIG_PIN, 0);

    gpio_init(ECHO_PIN_2);
    gpio_set_dir(ECHO_PIN_2, GPIO_IN);
    gpio_set_irq_enabled_with_callback(ECHO_PIN_2, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    gpio_init(TRIG_PIN_2);
    gpio_set_dir(TRIG_PIN_2, GPIO_OUT);
    gpio_put(TRIG_PIN_2, 0);


    while(1){
        while(true){
            alarm_id_t alarm = add_alarm_in_ms(5000, alarm_callback, NULL, false);    
            alarm_id_t alarm_2 = add_alarm_in_ms(5000, alarm_callback2, NULL, false);    
            sleep_us(100);
            send_trig_pulse();

            while ((alarm == 0) && (end_us == 0)) {}
            while ((alarm_2 == 0) && (end_us_2 == 0)) {}

            if (fired_2) {
                fired_2 = 0;
                printf("Sensor 2 - dist: falha");
                }
            else if(!fired_2){
                    int delta_t_2 = end_us_2- start_us_2;
                    int distancia_2 = (int)((delta_t_2 * 0.0343) / 2.0);
                    printf("Sensor 2 - dist: %d cm\n",distancia_2);
                    cancel_alarm(alarm_2);

            }
            if (fired) {
                fired = 0;
                printf("Sensor 1 - dist: falha");
                }
            else if(!fired){
                    int delta_t = end_us - start_us;
                    int distancia = (int)((delta_t * 0.0343) / 2.0);
                    printf("Sensor 1 - dist: %d cm\n",distancia);
                    cancel_alarm(alarm);

            }
            

     }
    }
}
