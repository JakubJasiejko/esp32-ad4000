#include "ad4000.h"

void app_main(void)
{
    spi2_init_bus(SPI2_BUS1, SPI3_HOST, AD4000_PIN_SDI, AD4000_PIN_SDO, AD4000_PIN_SCLK);

    int adc = -1;
    ad4000_init(SPI2_BUS1, &adc, 0, GPIO_NUM_NC, AD4000_PIN_CNV, AD4000_SPI_CLK_HZ, 0);
    ad4000_prepareBeforeMeasurement(adc, AD4000_PIN_SDI);
    uint16_t raw = ad4000_getRawData(adc, AD4000_PIN_CNV);
    (void)raw;
}
