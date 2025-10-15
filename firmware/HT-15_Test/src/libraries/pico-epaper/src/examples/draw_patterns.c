/* Example program for the pico-epaper library */

#include <math.h>
#include <memory.h>
#include <hardware/spi.h>
#include <pico/epaper.h>
#include <pico/stdlib.h>


int
main()
{
	struct epaper display = {
		.spi = spi0,
		.cs_pin = 6,
		.rst_pin = 8,
		.dc_pin = 7,
		.busy_pin = 9,
		.width = 400,
		.height = 300,
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
	gpio_init(display.dc_pin);
	gpio_init(display.busy_pin);

	gpio_set_dir(display.cs_pin, GPIO_OUT);
	gpio_set_dir(display.rst_pin, GPIO_OUT);
	gpio_set_dir(display.dc_pin, GPIO_OUT);
	gpio_set_dir(display.busy_pin, GPIO_IN);

	memset(display_buffer, 0xff, buffer_size);
	memset(prev_display_buffer, 0xff, buffer_size);

	/* Display contents of the buffer (clear display to white) */
	epaper_update(&display);

	/* Draw diagonal lines */
	for (int x = 0; x < display.width; ++x) {
		int y = (int) ((float) display.height * (float) x
			/ (float) display.width);

		display.buffer[y * display.width / 8 + x / 8]
			&= ~(0x80 >> (x & 0x07));
		display.buffer[(display.height - y) * display.width / 8 + x / 8]
			&= ~(0x80 >> (x & 0x07));
	}

	/* Display contents of the buffer */
	epaper_update(&display);

	/* Draw cricles */
	for (int x0 = 10; x0 < display.width; x0 += 20) {
		for (int i = 0; i < 100; ++i) {
			int x = x0 + 10 * sinf(2.0f * M_PI * i / 100);
			int y = 10 + 10 * cosf(2.0f * M_PI * i / 100);
			display.buffer[y * display.width / 8 + x / 8]
				&= ~(0x80 >> (x & 0x07));
		}

		/* Perform a pratial refresh */
		epaper_update(&display, true);
	}

	/* Draw sine wave */
	for (int x = 0; x < display.width; ++x) {
		int y = (display.height / 2) + 100 * sin(4.0 * M_PI * x
			/ display.width);

		display.buffer[y * display.width / 8 + x / 8]
			&= ~(0x80 >> (x & 0x07));
	}

	/* Display contents of the buffer */
	epaper_update(&display);

	while (true) {
		tight_loop_contents();
	}
}
