#include <Arduino.h>
#include <SPI.h>

#include "modem/NRF24.h"
#include "pins.h"

extern ModemNRF24 modem;

void loop() {
  modem.update();
  while (Serial.available() > 0) {
    modem.write(Serial.read());
  }
}
