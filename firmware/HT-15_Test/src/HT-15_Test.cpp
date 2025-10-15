#include <stdio.h>
#include <math.h>
#include <memory.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/adc.h"
#include "hardware/spi.h"

#include "pico/epaper.h"

#include "pindefs.c"

//turn audio amp power on or off
void set_audioamp_power(bool state){
    //state = true turns off power, false turns on power (inverted logic)
    if(state){
        printf("Disabling Audio Amp Power\n");
    } else{
        printf("Enabling Audio Amp Power\n");
    }
    gpio_put(AUDIOAMP_POWER, !state);
}

//initialize battery voltage reading
void init_battery_voltage(){
    adc_init();
    adc_gpio_init(V_BAT);
}

//returns battery voltage in volts
float get_battery_voltage(){
    adc_select_input(V_BAT-40); //V_BAT is ADC7, ADC input is 0 indexed
    return (float)adc_read()*.003791; //conversion factor for voltage divider and ADC step size (127/27)*(3.3/4095)
}

//initialize all necessary pins
void init_all(){
    //Audioamp power pin
    gpio_init(AUDIOAMP_POWER);
    gpio_set_dir(AUDIOAMP_POWER, GPIO_OUT);
    set_audioamp_power(true);

    //init status LED
    gpio_init(LED_STATUS);
    gpio_set_dir(LED_STATUS, GPIO_OUT);
    gpio_put(LED_STATUS, 1);

    //init battery voltage reading
    init_battery_voltage();

}

//what will run on core 0
void core_0() {
    printf("Core 0 launched\n");


    bool led_state = false;
    gpio_put(LED_STATUS, 1);
   
    //define button matricies
    uint8_t button_sense_pin[6] = {BTN_MTX_0, BTN_MTX_1, BTN_MTX_2, BTN_MTX_3, BTN_MTX_4, BTN_MTX_5};
    uint8_t button_pwr_pin[4] = {BTN_MTX_A, BTN_MTX_B, BTN_MTX_C, BTN_MTX_D};
    char button_names[6][4][6] = {
        {"Left", "Up", "Right", "Side2"},
        {"Back", "Down", "Enter", "Side1"},
        {"1", "2", "3", "PTT"},
        {"4", "5", "6", "Lock"},
        {"7", "8", "9", "NA"},
        {"*", "0", "#", "NA"}
    };

    //button states [sense][pwr][debounce_pointer] debounce_pointer toggles every loop to store last two states
    bool button_debounce_states[6][4][2] = {0}; //debounce states for each button
    bool button_pressed_last_loop[6][4] = {0}; //used to make sure button press is only registered once, not every loop
    //raw battery voltage ready by pin V_BAT

    //init button pins
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

    bool button_debounce_pointer=0;
    bool a_button_has_been_pressed = false;

    uint16_t counter = 0;
    printf("Starting main loop on core 0\n");
    while (true) {
        //toggle LED
        if(!(counter%50)){
            led_state = !led_state;
            gpio_put(LED_STATUS, led_state);
        }
        
        //read battery voltage every 10 seconds
        if (!counter){
            printf("Battery Voltage: %.2fV\n", get_battery_voltage());
        }

        //scan buttons
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
        //check for button presses with debounce, and only trigger the button action on rising edge
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
        //print new line if any button has been pressed, and reset flag

        //manage counter
        counter++;
        if(counter>=1000){
            counter = 0;
        }
        sleep_ms(10);
    }
}

void core_1() {

    //init display
    struct epaper display = {
        .spi = spi1,
        .cs_pin = DISPLAY_CS,
        .rst_pin = DISPLAY_RESET,
        .busy_pin = DISPLAY_BUSY,
        .width = 200,
        .height = 200,
        .black_border = false,
    };

    const size_t buffer_size = display.height * display.width / 8;

    uint8_t display_buffer[buffer_size];
	uint8_t prev_display_buffer[buffer_size];

	display.buffer = display_buffer;
	display.previous_buffer = prev_display_buffer;

	spi_init(spi0, 4000000);
	gpio_set_function(2, GPIO_FUNC_SPI);
	gpio_set_function(3, GPIO_FUNC_SPI);

	gpio_init(display.cs_pin);
	gpio_init(display.rst_pin);
	gpio_init(display.busy_pin);

	gpio_set_dir(display.cs_pin, GPIO_OUT);
	gpio_set_dir(display.rst_pin, GPIO_OUT);
	gpio_set_dir(display.busy_pin, GPIO_IN);

	memset(display_buffer, 0xff, buffer_size);
	memset(prev_display_buffer, 0xff, buffer_size);

	/* Display contents of the buffer (clear display to white) */
	epaper_update(&display);


}

int main(){
    
    stdio_init_all();
    init_all();
    sleep_ms(5000);

    printf("Device Initalized!\n");

    multicore_reset_core1();
    multicore_launch_core1(core_1);

    core_0();

}