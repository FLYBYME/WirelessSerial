#ifndef PINS_H
#define PINS_H

#include <Arduino.h>

// Define pin configurations for different boards
#if defined(ARDUINO_AVR_PRO)
    #define NRF24_ENABLED true
    #define NRF24_CS_PIN 10
    #define NRF24_CE_PIN 9

    #define RFM22_ENABLED false
    #define RFM22_CS_PIN 8
    #define RFM22_CE_PIN 7

    #define SEND_LED_ENABLED false
    #define SEND_LED_PIN 13

    #define RECV_LED_ENABLED false
    #define RECV_LED_PIN 12

#elif defined(WIO_BUZZER)
    #define NRF24_ENABLED true
    #define NRF24_CS_PIN D4
    #define NRF24_CE_PIN D3

    #define RFM22_ENABLED false
    #define RFM22_CS_PIN 8
    #define RFM22_CE_PIN 7

    #define SEND_LED_ENABLED false
    #define SEND_LED_PIN 13

    #define RECV_LED_ENABLED false
    #define RECV_LED_PIN 12

#else
    #error "Board not supported!"
#endif

#endif  // PINS_H