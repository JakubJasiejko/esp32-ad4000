# esp32-ad4000

Sterownik przetwornika ADC `AD4000` dla ESP32. Biblioteka obsługuje konfigurację układu i szybki odczyt surowych próbek przez SPI.

## Co zawiera
- inicjalizację układu i busa
- konfigurację rejestru sterującego
- przygotowanie toru przed pomiarem
- szybki odczyt `uint16_t`

## Zależności
- `esp32-spi2-bus`

## Pliki
- `include/ad4000.h`
- `src/ad4000.c`

## Szybki start
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

Pełny przykład znajdziesz w `examples/basic_usage/basic_usage.c`.
