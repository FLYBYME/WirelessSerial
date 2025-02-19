#include <Arduino.h>
#include <SPI.h>

#include "modem/NRF24.h"
#include "pins.h"

extern ModemNRF24 modem;

void setup() {
  Serial.begin(115200);
  while(!Serial) {}
  Serial.print("NRF24_CE_PIN: "); Serial.println(NRF24_CE_PIN);
  Serial.print("NRF24_CS_PIN: "); Serial.println(NRF24_CS_PIN);
  modem.init();
}
