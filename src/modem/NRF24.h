#ifndef MODEM_NRF24_H
#define MODEM_NRF24_H

#include <RH_NRF24.h>
#include <SPI.h>

#include "Modem.h"

#define NRF24_PACKET_SIZE 32
#define RING_BUFFER_SIZE 128  // Define the size of the ring buffer

class ModemNRF24 : public Modem {
 public:
  struct PacketFormat {
    uint16_t sequenceNumber;
    uint8_t length;
    uint8_t stream;  // Stream ID, supporting multiple streams
    uint8_t flags;
    uint8_t* data;
    uint16_t checksum;
    static const uint8_t headerLength = 7;
    static const uint8_t maxDataLength = NRF24_PACKET_SIZE - 7;
  };

  ModemNRF24(uint8_t ce_pin, uint8_t csn_pin);
  ~ModemNRF24();

  bool init();
  void update();

  void setMode(ModemMode mode);
  void setBiteRate(ModemBiteRate biteRate);
  void setPower(ModemPower power);
  void setChannel(ModemChannel channel);
  uint8_t getMaxDataLength() { return PacketFormat::maxDataLength; }

  int available();
  int read();
  int peek();

  size_t write(uint8_t data);
  size_t write(const uint8_t* buffer, size_t size);
  int availableForWrite();
  void flush();

 private:
  RH_NRF24* radio;
  uint8_t ce_pin;
  uint8_t csn_pin;
  uint16_t sequenceNumber = 0;

  // Ring buffer structure
  struct RingBuffer {
    uint8_t buffer[RING_BUFFER_SIZE];
    volatile uint16_t head;
    volatile uint16_t tail;
  };

  RingBuffer rxBuffer;  // Receive buffer
  RingBuffer txBuffer;  // Transmit buffer

  bool createPacket(uint8_t* buf, int len, PacketFormat* packet);
  bool sendPacket(PacketFormat* packet);
  bool receivePacket(PacketFormat* packet);
  bool processPacket(PacketFormat* packet);
  void sendAck(uint16_t sequenceNumber);

  void handleReceivedPacket();
  void handleTransmitPacket();

  bool validateChecksum(PacketFormat* packet);
  uint16_t calculateChecksum(PacketFormat* packet);

  RH_NRF24::TransmitPower getPowerValue(ModemPower power);
  RH_NRF24::DataRate getDataRateValue(ModemBiteRate biteRate);

  void storeByte(uint8_t byte, RingBuffer* buffer);
  int readByte(RingBuffer* buffer);
  int bytesAvailable(RingBuffer* buffer);
};

#endif  // MODEM_NRF24_H