#ifndef PICO_EPAPER_EPAPER_H
#define PICO_EPAPER_EPAPER_H

#include <stdbool.h>
#include <stdint.h>

struct spi_inst;
struct critical_section;

/*
 * Epaper display configuraton.
 */
struct epaper {

	/*
	 * SPI bus which has the epaper display.
	 * Probably spi0 or spi1 from hardware/spi.h.
	 * The bus MUST be initialized before calling any epaper_* function.
	 * Use spi_init and gpio_set_function.
	 */
	struct spi_inst *spi;

	/*
	 * Chip Select pin.
	 * This pin MUST be configured as output before calling any epaper_*
	 * function. Use gpio_init and gpio_set_dir.
	 */
	uint8_t cs_pin;

	/*
	 * Reset pin.
	 * This pin MUST be configured as output before calling any epaper_*
	 * function. Use gpio_init and gpio_set_dir.
	 */
	uint8_t rst_pin;

	/*
	 * Data/Command pin.
	 * This pin MUST be configured as output before calling any epaper_*
	 * function. Use gpio_init and gpio_set_dir.
	 */
	uint8_t dc_pin;

	/*
	 * Busy pin.
	 * This pin MUST be configured as input before calling any epaper_*
	 * function. Use gpio_init and gpio_set_dir.
	 */
	uint8_t busy_pin;

	/* Horizontal resolution of the display (px) */
	uint16_t width;

	/* Vertical resolution of the display (px) */
	uint16_t height;

	/*
	 * If set to true, refreshing the display will draw a black border around
	 * the content. Otherwise the border will be white.
	 */
	bool black_border;

	/* Pointer to the buffer representing current contents of the display */
	uint8_t *buffer;

	/* Pointer to the buffer representing previous contents of the display */
	uint8_t *previous_buffer;

	/*
	 * Critical section for IRQ safe mutual exclusion of the epaper device.
	 * If critical_section is set to non-NULL value, the critical section is
	 * entered (blocking) for the time of executing every SPI command.
	 *
	 * If you use this library in a way that execution of a command can be
	 * interrupted (for example: main loop and interrupt service routine),
	 * then set critical_section to an initialized critical section object
	 * (use critical_section_init). Otherwise, remember to set this to NULL.
	 */
	struct critical_section *critical_section;
};

/*
 * Send buffer contents to the display
 *
 * \param partial if true, uses partial update lookup table; full update otherwise
 */
void epaper_update(const struct epaper *display);

enum epaper_commands {
	EPAPER_PSR = 0x00,
	EPAPER_PWR = 0x01,
	EPAPER_POF = 0x02,
	EPAPER_PON = 0x04,
	EPAPER_BTST = 0x06,
	EPAPER_DSLP = 0x07,
	EPAPER_DTM1 = 0x10,
	EPAPER_DSP = 0x12,
	EPAPER_DTM2 = 0x13,
	EPAPER_LUTC = 0x20,
	EPAPER_LUTWW = 0x21,
	EPAPER_LUTBW = 0x22,
	EPAPER_LUTR = 0x22,
	EPAPER_LUTWB = 0x23,
	EPAPER_LUTW = 0x23,
	EPAPER_LUTBB = 0x24,
	EPAPER_LUTB = 0x24,
	EPAPER_PLL = 0x30,
	EPAPER_CDI = 0x50,
	EPAPER_TRES = 0x61,
	EPAPER_VDCS = 0x82,
};

enum epaper_psr {
	EPAPER_RES0 = 0,
	EPAPER_RES1 = 1 << 6,
	EPAPER_RES2 = 1 << 7,
	EPAPER_RES3 = (1 << 7) | (1 << 6),
	EPAPER_REG_EN = 1 << 5,
	EPAPER_BWR = 1 << 4,
	EPAPER_UD = 1 << 3,
	EPAPER_SHL = 1 << 2,
	EPAPER_SHD_N = 1 << 1,
	EPAPER_RST_N = 1 << 0,
};

enum epaper_pwr {
	EPAPER_VDS_EN = 1 << 1,
	EPAPER_VDG_EN = 1 << 0,
	EPAPER_VCOM_HV = 1 << 2,
	EPAPER_VGHL_LV0 = 0,
	EPAPER_VGHL_LV1 = 1 << 0,
	EPAPER_VGHL_LV2 = 1 << 1,
	EPAPER_VGHL_LV3 = (1 << 1) | (1 << 0),
};

#endif
