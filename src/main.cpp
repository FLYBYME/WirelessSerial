#include <Arduino.h>
#include <SPI.h>

#include "modem/NRF24.h"
#include "pins.h"

ModemNRF24 modem(NRF24_CE_PIN, NRF24_CS_PIN);

void setup();
void loop();
