# arduino-ancs

## Pinouts

| UNO | Description | Shield |
| --- | ----------- | ------ |
| | | **LCD** |
| A4 | I2C Data (SDA) | A4 |
| A5 | I2C Clock (SCL) | A5 |
| GND | | GND |
| +5V | | +5V |
| | | **BLE** |
| D13 or SCK | SPI SCK | SCK |
| D12 or MISO|  SPI MISO | MISO |
| D11 or MOSI | SPI MOSI | MOSI |
| D10 | S/W Configurable | REQ |
| | _not used_ | ACT |
| D2 | S/W Configurable, _interrupt required_ | RDY |
| D9 | S/W Configurable | RST |
| GND | | GND |
| +5V | | VIN |
| | | **Optional** |
| D6 | _optional_, PWM required | Buzzer |
| D5 | _optional_ | Led |
