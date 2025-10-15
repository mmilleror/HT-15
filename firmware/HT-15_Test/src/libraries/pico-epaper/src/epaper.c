#include <malloc.h>
#include <memory.h>
#include <stdarg.h>
#include <hardware/gpio.h>
#include <hardware/spi.h>
#include <pico/critical_section.h>
#include <pico/epaper.h>

extern const uint8_t lut_faster_vcom[], lut_faster_w2w[], lut_faster_b2w[],
	lut_faster_w2b[], lut_faster_b2b[];
extern const uint8_t lut_fastest_vcom[], lut_fastest_w2w[], lut_fastest_b2w[],
	lut_fastest_w2b[], lut_fastest_b2b[];

void
epaper_write_array(const struct epaper *display, bool data, const uint8_t *src,
		size_t len)
{
	if (display->critical_section != NULL) {
		critical_section_enter_blocking(display->critical_section);
	}
	gpio_put(display->dc_pin, data);
	gpio_put(display->cs_pin, 0);
	spi_write_blocking(display->spi, src, len);
	gpio_put(display->cs_pin, 1);
	if (display->critical_section != NULL) {
		critical_section_exit(display->critical_section);
	}
}

void
epaper_write(const struct epaper *display, bool data, int len, ...)
{
	va_list valist;
	va_start(valist, len);
	uint8_t src[len];
	for (int i = 0; i < len; i++) {
		src[i] = va_arg(valist, int);
	}
	va_end(valist);

	epaper_write_array(display, data, src, len);
}

void
epaper_command(const struct epaper *display, uint8_t command, int len, ...)
{
	va_list valist;
	va_start(valist, len);
	uint8_t src[len];
	for (int i = 0; i < len; i++) {
		src[i] = va_arg(valist, int);
	}
	va_end(valist);

	epaper_write_array(display, 0, &command, 1);
	epaper_write_array(display, 1, src, len);
}

void
epaper_wait(const struct epaper *display)
{
	do {
		sleep_us(10);  /* after 10 uS busy should be low */
	} while (!gpio_get(display->busy_pin));
}

void
epaper_update(const struct epaper *display)
{
	gpio_put(display->rst_pin, 0);
	sleep_ms(1);  /* 10 uS reset signal should be enough */
	gpio_put(display->rst_pin, 1);
	epaper_wait(display);

	epaper_command(display, EPAPER_BTST, 3, 0x17, 0x17, 0x17);
	epaper_command(display, EPAPER_PWR, 4, EPAPER_VDS_EN | EPAPER_VDG_EN,
		EPAPER_VGHL_LV0, 0x2b, 0x2b);
	epaper_command(display, EPAPER_PON, 0);
	epaper_wait(display);

	epaper_command(display, EPAPER_PSR, 1, EPAPER_RES0 | EPAPER_REG_EN
		| EPAPER_BWR | EPAPER_UD | EPAPER_SHL | EPAPER_SHD_N
		| EPAPER_RST_N);
	epaper_command(display, EPAPER_PLL, 1, 0x3c);
	epaper_command(display, EPAPER_TRES, 4, 0x01, 0x90, 0x01, 0x2c);
	epaper_command(display, EPAPER_VDCS, 1, 0x28);
	epaper_command(display, EPAPER_CDI, 1,
		display->black_border ? 0x77 : 0x97);

	epaper_write(display, 0, 1, EPAPER_LUTC);
	epaper_write_array(display, 1, lut_faster_vcom, 44);
	epaper_write(display, 0, 1, EPAPER_LUTWW);
	epaper_write_array(display, 1, lut_faster_w2w, 42);
	epaper_write(display, 0, 1, EPAPER_LUTBW);
	epaper_write_array(display, 1, lut_faster_b2w, 42);
	epaper_write(display, 0, 1, EPAPER_LUTWB);
	epaper_write_array(display, 1, lut_faster_w2b, 42);
	epaper_write(display, 0, 1, EPAPER_LUTBB);
	epaper_write_array(display, 1, lut_faster_b2b, 42);

	epaper_write(display, 0, 1, EPAPER_DTM1);
	epaper_write_array(display, 1, display->previous_buffer,
		display->height * display->width / 8);

	epaper_write(display, 0, 1, EPAPER_DTM2);
	epaper_write_array(display, 1, display->buffer,
		display->height * display->width / 8);

	memcpy(display->previous_buffer, display->buffer,
		display->height * display->width / 8);

	epaper_command(display, EPAPER_DSP, 0);
	epaper_wait(display);

	epaper_command(display, EPAPER_CDI, 1, 0x17);
	epaper_command(display, EPAPER_POF, 0);
	epaper_command(display, EPAPER_DSLP, 1, 0xa5);
}
