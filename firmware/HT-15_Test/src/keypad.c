// keypad.c
#include <stdio.h>
#include <math.h>
#include <memory.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

#include "pindefs.c"
#include "keypad.h"

// internal state
static uint8_t button_sense_pin[6] = {BTN_MTX_0, BTN_MTX_1, BTN_MTX_2, BTN_MTX_3, BTN_MTX_4, BTN_MTX_5};
static uint8_t button_pwr_pin[4] = {BTN_MTX_A, BTN_MTX_B, BTN_MTX_C, BTN_MTX_D};
static char button_names[6][4][6] = {
    {"Left", "Up", "Right", "Side2"},
    {"Back", "Down", "Enter", "Side1"},
    {"1", "2", "3", "PTT"},
    {"4", "5", "6", "Lock"},
    {"7", "8", "9", "NA"},
    {"*", "0", "#", "NA"}
};

static bool button_debounce_states[6][4][2] = {0};
static bool button_pressed_last_loop[6][4] = {0};
static bool button_debounce_pointer = 0;

void keypad_init(void){
    // init button pins
    for (int i = 0; i < 6; i++) {
        gpio_init(button_sense_pin[i]);
        gpio_set_dir(button_sense_pin[i], GPIO_IN);
        gpio_pull_down(button_sense_pin[i]);
    }
    for (int i = 0; i < 4; i++) {
        gpio_init(button_pwr_pin[i]);
        gpio_set_dir(button_pwr_pin[i], GPIO_OUT);
        gpio_put(button_pwr_pin[i], 0);
    }

    // ADC init for battery & pot
    adc_init();
    adc_gpio_init(V_BAT);
    adc_gpio_init(POT_VOLUME);
}

void keypad_poll(void){

        for (int j = 0; j < 4; j++) {
            gpio_put(button_pwr_pin[j], 1);
            sleep_us(1);
            for (int i = 0; i < 6; i++) {
                button_debounce_states[i][j][button_debounce_pointer] = gpio_get(button_sense_pin[i]);
            }
            gpio_put(button_pwr_pin[j], 0);
            sleep_us(1);
        }
        button_debounce_pointer = !button_debounce_pointer;
        for (int i = 0; i < 6; i++) {
            for (int j = 0; j < 4; j++) {
                if(button_debounce_states[i][j][0] && button_debounce_states[i][j][1]){
                    if(!button_pressed_last_loop[i][j]){
                        printf("%s\n", button_names[i][j]);
                    }
                    button_pressed_last_loop[i][j] = true;
                } else{
                    button_pressed_last_loop[i][j] = false;
                }
            }
        }
    }

    // process encoder
    encoder_state = ((encoder_state<<1) | gpio_get(BTN_ENC_A)) & 0b111; //store the last three states of the encoder A pin
    if(encoder_state==0b100){
        if(gpio_get(BTN_ENC_B)){
            printf("Channel +\n");
        } else{
            printf("Channel -\n");
        }
        encoder_state=0;
    }

    // read volume pot every 1000 ticks
    if (counter % 1000 == 0){
        current_volume = get_volume_pot();
        if(abs(current_volume-last_volume)>2){
            last_volume = current_volume;
            printf("Volume: %d\n", current_volume);
        }
    }

    counter++;
    if(counter>=10000){
        counter = 0;
    }
}

// ADC helpers
float get_battery_voltage(){
    adc_select_input(V_BAT-40); //V_BAT is ADC7, ADC input is 0 indexed
    return (float)adc_read()*.003791; //conversion factor for voltage divider and ADC step size (127/27)*(3.3/4095)
}
uint8_t get_volume_pot(){
    adc_select_input(POT_VOLUME-40); //POT_VOLUME is ADC5, ADC input is 0 indexed
    return 99-((uint8_t)((float)adc_read()*0.02442));
}
