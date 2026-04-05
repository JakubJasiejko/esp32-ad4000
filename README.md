# esp32-ad4000

Driver for the `AD4000` ADC on ESP32. The library handles device configuration and fast raw sample reads over SPI.

## Features
- initialize the device and SPI bus
- configure the control register
- prepare the interface before measurement
- fast `uint16_t` raw data reads

## Dependencies
- `esp32-spi2-bus`

## Files
- `include/ad4000.h`
- `src/ad4000.c`

## Quick start
```c
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
```

See `examples/basic_usage/basic_usage.c` for a complete example.
