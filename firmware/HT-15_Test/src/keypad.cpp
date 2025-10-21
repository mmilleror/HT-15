// keypad.c
#include <stdio.h>
#include <math.h>
#include <memory.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

#include "pindefs.c"
#include "keypad.h"

// internal state

void Keypad::keypad_init(){
    button_sense_pin[6] = {BTN_MTX_0, BTN_MTX_1, BTN_MTX_2, BTN_MTX_3, BTN_MTX_4, BTN_MTX_5};
    button_pwr_pin[4] = {BTN_MTX_A, BTN_MTX_B, BTN_MTX_C, BTN_MTX_D};
    button_locations[6][4] = {
        {KEY_LEFT, KEY_UP, KEY_RIGHT, KEY_SIDE2},
        {KEY_BACK, KEY_DOWN, KEY_ENTER, KEY_SIDE1},
        {KEY_1, KEY_2, KEY_3, KEY_PTT},
        {KEY_4, KEY_5, KEY_6, KEY_LOCK},
        {KEY_7, KEY_8, KEY_9, KEY_NULL},
        {KEY_STAR, KEY_0, KEY_HASH, KEY_NULL}
    };
    button_names[6][4][6] = {
        {"Left", "Up", "Right", "Side2"},
        {"Back", "Down", "Enter", "Side1"},
        {"1", "2", "3", "PTT"},
        {"4", "5", "6", "Lock"},
        {"7", "8", "9", "NA"},
        {"*", "0", "#", "NA"}
    };

    button_debounce_states[6][4][2] = {0};
    button_pressed_last_loop[6][4] = {0};
    button_debounce_pointer = 0;

    buttons_pressed_since_last_poll[5] = {KEY_NULL};
    buttons_pressed_count = 0;
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

void Keypad::keypad_poll(){
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
                    if(buttons_pressed_count < 5){
                        buttons_pressed_since_last_poll[buttons_pressed_count] = button_locations[i][j];
                        buttons_pressed_count++;
                    }
                }
                button_pressed_last_loop[i][j] = true;
            } else{
                button_pressed_last_loop[i][j] = false;
            }
        }
    }
}

void Keypad::get_buttons_pressed(Keys* out_buttons){
// out_buttons will be filled with the buttons pressed since last poll, size is 5 keys

    //copy the buttons pressed to the output array
    memcpy(out_buttons, buttons_pressed_since_last_poll, sizeof(buttons_pressed_since_last_poll));
    //reset for next poll
    buttons_pressed_count = 0;
}