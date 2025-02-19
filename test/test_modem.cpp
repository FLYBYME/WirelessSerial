#include <Arduino.h>
#include <unity.h>
#include "modem/Modem.h"

class TestModem : public Modem {
public:
    bool init() override {
        return true;
    }

    void update() override {}

    uint8_t getMaxDataLength() override {
        return 32;
    }
};

TestModem modem;

void test_modem_initialization() {
    TEST_ASSERT_TRUE(modem.init());
}

void test_modem_mode_setting() {
    modem.setMode(Modem::MODEM_MODE_ACTIVE);
    TEST_ASSERT_EQUAL(Modem::MODEM_MODE_ACTIVE, modem.mode);
}

void test_modem_data_transmission() {
    modem.write('A');
    TEST_ASSERT_EQUAL(1, modem.availableForWrite());
    TEST_ASSERT_EQUAL('A', modem.read());
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_modem_initialization);
    RUN_TEST(test_modem_mode_setting);
    RUN_TEST(test_modem_data_transmission);
    UNITY_END();
}

void loop() {
    // Empty loop
}
