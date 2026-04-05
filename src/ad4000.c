/**
 * @file ad4000.c
 * @brief Implementacja obsługi ADC AD4000 przy użyciu biblioteki spi2 (dwie magistrale).
 */

#include "ad4000.h"
#include "spi2.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_cpu.h"
#include "esp_rom_sys.h"
#include "driver/gpio.h"
#include "soc/gpio_reg.h"

#define AD4000_CNV_HIGH_CYCLES 240U

static inline void ad4000_cnv_high(gpio_num_t cnv_pin)
{
    REG_WRITE(GPIO_OUT_W1TS_REG, (1UL << (uint32_t)cnv_pin));
}

static inline void ad4000_cnv_low(gpio_num_t cnv_pin)
{
    REG_WRITE(GPIO_OUT_W1TC_REG, (1UL << (uint32_t)cnv_pin));
}

static inline void ad4000_cnv_pulse(gpio_num_t cnv_pin)
{
    uint32_t start_cycles = esp_cpu_get_cycle_count();
    ad4000_cnv_high(cnv_pin);
    while ((uint32_t)(esp_cpu_get_cycle_count() - start_cycles) < AD4000_CNV_HIGH_CYCLES) {
        __asm__ __volatile__("nop");
    }
    ad4000_cnv_low(cnv_pin);
}

void ad4000_trigger(gpio_num_t cnv_pin)
{
    ad4000_cnv_pulse(cnv_pin);
}

uint8_t ad4000_init(spi2_bus_t bus,
                    int *out_index,
                    int local_index,
                    gpio_num_t cs_pin,
                    gpio_num_t cnv_pin,
                    int clock_speed_hz,
                    uint8_t spi_mode)
{
    if (out_index == NULL) return 0xFF;

    // Dodajemy urządzenie do wybranego busa.
    // AD4000 w Twoim kodzie używał GPIO_NUM_NC jako CS — ale jeśli faktycznie masz CS,
    // podaj go. Jeśli nie używasz CS (np. zawsze jedno urządzenie i ręcznie CNV),
    // zostaw GPIO_NUM_NC.
    int idx = spi2_add_device(bus, cs_pin, local_index, clock_speed_hz, spi_mode);
    if (idx < 0) return 0xFF;

    // CNV
    gpio_set_direction(cnv_pin, GPIO_MODE_OUTPUT);
    ad4000_cnv_low(cnv_pin);
    vTaskDelay(pdMS_TO_TICKS(20));

    *out_index = idx;
    return 0x00;
}

uint8_t ad4000_configure(int index, gpio_num_t cnv_pin,
                         uint8_t status_bits_enable,
                         uint8_t turbo_enable,
                         uint8_t hiz_enable,
                         uint8_t span_enable)
{
    uint8_t config_byte =
        (status_bits_enable << 4)
      | (turbo_enable       << 3)
      | (hiz_enable         << 2)
      | (span_enable        << 1)
      | AD4000_OV_FLAG_ALWAYS_0;

    uint8_t tx_w[2] = { AD4000_WREG, config_byte };

    ad4000_cnv_pulse(cnv_pin);

    return spi2_write16(tx_w, index);
}

uint8_t ad4000_readConfig(int index, gpio_num_t cnv_pin)
{
    uint8_t tx_r[2] = { AD4000_RREG, 0x00 };
    uint8_t rx_r[2] = { 0 };

    ad4000_cnv_pulse(cnv_pin);

    if (spi2_read(tx_r, rx_r, 2, index) != 0x00) return 0xFF;

    return rx_r[1];
}

/**
 * @brief Przygotowanie linii SDI w stan wysoki przed pomiarem (Twoja logika).
 *
 * W wersji "2 magistrale" NIE robimy spi_deinit() globalnie, bo to by wywaliło też drugi bus.
 * Zamiast tego:
 * - jeżeli chcesz przełączyć konfigurację dla konkretnego busa, powinieneś zrobić spi2_deinit_bus(bus)
 *   i ponownie spi2_init_bus(bus, ...) poza tym modułem.
 *
 * Dlatego tu zostawiamy tylko operacje GPIO + ewentualnie ponowne dodanie urządzenia,
 * zakładając, że bus jest już skonfigurowany właściwie.
 */
uint8_t ad4000_prepareBeforeMeasurement(int index,
                                        gpio_num_t sdi_pin)
{
    // W Twoim kodzie: ustaw SDI na 1.
    gpio_reset_pin(sdi_pin);
    gpio_set_direction(sdi_pin, GPIO_MODE_OUTPUT);
    gpio_set_level(sdi_pin, 1);

    vTaskDelay(pdMS_TO_TICKS(20));
    return 0x00;
}

uint16_t ad4000_getRawData(int index, gpio_num_t cnv_pin)
{
    ad4000_cnv_pulse(cnv_pin);
    uint16_t raw = 0;
    if (spi2_read16_fast(index, &raw) != 0x00) return 0xFFFF;
    return raw;
}

uint16_t ad4000_getRawDataHandle(spi_device_handle_t handle, gpio_num_t cnv_pin)
{
    ad4000_cnv_pulse(cnv_pin);
    uint16_t raw = 0;
    if (spi2_read16_fast_handle(handle, &raw) != 0x00) return 0xFFFF;
    return raw;
}
