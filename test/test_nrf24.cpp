#include <Arduino.h>
#include <unity.h>
#include "modem/NRF24.h"

class TestModemNRF24 : public ModemNRF24 {
public:
    TestModemNRF24(uint8_t ce_pin, uint8_t csn_pin) : ModemNRF24(ce_pin, csn_pin) {}

    bool init() override {
        return true;
    }

    void update() override {}

    uint8_t getMaxDataLength() override {
        return 32;
    }
};

TestModemNRF24 modem(9, 10);

void test_modem_nrf24_initialization() {
    TEST_ASSERT_TRUE(modem.init());
}

void test_modem_nrf24_mode_setting() {
    modem.setMode(Modem::MODEM_MODE_ACTIVE);
    TEST_ASSERT_EQUAL(Modem::MODEM_MODE_ACTIVE, modem.mode);
}

void test_modem_nrf24_data_transmission() {
    modem.write('A');
    TEST_ASSERT_EQUAL(1, modem.availableForWrite());
    TEST_ASSERT_EQUAL('A', modem.read());
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_modem_nrf24_initialization);
    RUN_TEST(test_modem_nrf24_mode_setting);
    RUN_TEST(test_modem_nrf24_data_transmission);
    UNITY_END();
}

void loop() {
    // Empty loop
}
