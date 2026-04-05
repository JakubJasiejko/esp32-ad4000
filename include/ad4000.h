#pragma once

#include <stdint.h>
#include "driver/gpio.h"
#include "spi2.h"

/* Public defines (bez zmian) */
#define AD4000_PIN_SDI    ((gpio_num_t)19)
#define AD4000_PIN_SDO    ((gpio_num_t)21)
#define AD4000_PIN_SCLK   ((gpio_num_t)18)
#define AD4000_PIN_CNV    ((gpio_num_t)5)

#define AD4000_SPI_INDEX  0

#define AD4000_WREG   0b00010100
#define AD4000_RREG   0b01010100

#define AD4000_STATUS_BIT_DISABLE 0b0
#define AD4000_STATUS_BIT_ENABLE  0b1
#define AD4000_SPAN_DISABLE       0b0
#define AD4000_SPAN_ENABLE        0b1
#define AD4000_MODE_HI_Z_DISABLE  0b0
#define AD4000_MODE_HI_Z_ENABLE   0b1
#define AD4000_MODE_TURBO_DISABLE 0b0
#define AD4000_MODE_TURBO_ENABLE  0b1
#define AD4000_OV_FLAG_ALWAYS_0   0b0


#define AD4000_SPI_CLK_HZ 40000000



/**
 * @brief Inicjalizacja AD4000: dodaje urządzenie do wybranego busa.
 *
 * Uwaga: magistralę inicjalizujesz wcześniej przez spi2_init_bus().
 *
 * @param bus            SPI2_BUS0 / SPI2_BUS1
 * @param out_index      zwrócony globalny index urządzenia (do użycia w pozostałych funkcjach)
 * @param local_index    0..MAX_SPI_DEVICES-1 w ramach wybranego busa
 * @param cs_pin         CS (jeśli brak, daj GPIO_NUM_NC)
 * @param cnv_pin        pin CNV
 * @param clock_speed_hz częstotliwość SPI dla tego urządzenia
 * @param spi_mode       tryb SPI 0..3
 */
uint8_t ad4000_init(spi2_bus_t bus,
                    int *out_index,
                    int local_index,
                    gpio_num_t cs_pin,
                    gpio_num_t cnv_pin,
                    int clock_speed_hz,
                    uint8_t spi_mode);

uint8_t ad4000_configure(int index, gpio_num_t cnv_pin,
                         uint8_t status_bits_enable,
                         uint8_t turbo_enable,
                         uint8_t hiz_enable,
                         uint8_t span_enable);

uint8_t ad4000_readConfig(int index, gpio_num_t cnv_pin);

/**
 * @brief Przygotowanie SDI do pomiaru (ustawienie w stan wysoki).
 */
uint8_t ad4000_prepareBeforeMeasurement(int index, gpio_num_t sdi_pin);

uint16_t ad4000_getRawData(int index, gpio_num_t cnv_pin);
void ad4000_trigger(gpio_num_t cnv_pin);
uint16_t ad4000_getRawDataHandle(spi_device_handle_t handle, gpio_num_t cnv_pin);
