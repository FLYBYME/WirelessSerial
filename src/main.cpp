#include <Arduino.h>
#include <SPI.h>

#include "modem/NRF24.h"
#include "pins.h"


ModemNRF24 modem(NRF24_CE_PIN, NRF24_CS_PIN);

void setup() {
  Serial.begin(115200);
  while(!Serial) {}
  Serial.print("NRF24_CE_PIN: "); Serial.println(NRF24_CE_PIN);
  Serial.print("NRF24_CS_PIN: "); Serial.println(NRF24_CS_PIN);
  modem.init();
}

void loop() {
  modem.update();
  while (Serial.available() > 0) {
    modem.write(Serial.read());
  }
}
