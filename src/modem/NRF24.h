#ifndef MODEM_NRF24_H
#define MODEM_NRF24_H

#include <RH_NRF24.h>
#include <SPI.h>

#include "Modem.h"

#define NRF24_PACKET_SIZE 32
#define RING_BUFFER_SIZE 128  // Define the size of the ring buffer

/**
 * @class ModemNRF24
 * @brief A class for handling communication using the NRF24L01+ radio module.
 */
class ModemNRF24 : public Modem {
 public:
  /**
   * @struct PacketFormat
   * @brief A structure representing the format of a packet.
   */
  struct PacketFormat {
    uint16_t sequenceNumber; /**< The sequence number of the packet. */
    uint8_t length; /**< The length of the packet data. */
    uint8_t stream; /**< The stream ID, supporting multiple streams. */
    uint8_t flags; /**< The flags associated with the packet. */
    uint8_t* data; /**< The data contained in the packet. */
    uint16_t checksum; /**< The checksum of the packet. */
    static const uint8_t headerLength = 7; /**< The length of the packet header. */
    static const uint8_t maxDataLength = NRF24_PACKET_SIZE - 7; /**< The maximum length of the packet data. */
  };

  /**
   * @brief Constructor for the ModemNRF24 class.
   * @param ce_pin The chip enable (CE) pin for the NRF24L01+ module.
   * @param csn_pin The chip select (CSN) pin for the NRF24L01+ module.
   */
  ModemNRF24(uint8_t ce_pin, uint8_t csn_pin);

  /**
   * @brief Destructor for the ModemNRF24 class.
   */
  ~ModemNRF24();

  /**
   * @brief Initializes the ModemNRF24 object.
   * @return Returns true if initialization is successful; otherwise, returns false.
   */
  bool init();

  /**
   * @brief Updates the state of the ModemNRF24 object.
   */
  void update();

  /**
   * @brief Sets the mode of the modem.
   * @param mode The mode to set.
   */
  void setMode(ModemMode mode);

  /**
   * @brief Sets the bitrate of the modem.
   * @param biteRate The bitrate to set.
   */
  void setBiteRate(ModemBiteRate biteRate);

  /**
   * @brief Sets the power of the modem.
   * @param power The power to set.
   */
  void setPower(ModemPower power);

  /**
   * @brief Sets the channel of the modem.
   * @param channel The channel to set.
   */
  void setChannel(ModemChannel channel);

  /**
   * @brief Gets the maximum data length of a packet.
   * @return The maximum data length of a packet.
   */
  uint8_t getMaxDataLength() { return PacketFormat::maxDataLength; }

  /**
   * @brief Gets the number of bytes available to read.
   * @return The number of bytes available to read.
   */
  int available();

  /**
   * @brief Reads a byte from the receive buffer.
   * @return The byte read from the receive buffer.
   */
  int read();

  /**
   * @brief Peeks at the next byte in the receive buffer without removing it.
   * @return The next byte in the receive buffer.
   */
  int peek();

  /**
   * @brief Writes a byte to the transmit buffer.
   * @param data The byte to write.
   * @return The number of bytes written.
   */
  size_t write(uint8_t data);

  /**
   * @brief Writes a buffer of data to the transmit buffer.
   * @param buffer The buffer of data to write.
   * @param size The number of bytes to write.
   * @return The number of bytes written.
   */
  size_t write(const uint8_t* buffer, size_t size);

  /**
   * @brief Gets the number of bytes available for writing.
   * @return The number of bytes available for writing.
   */
  int availableForWrite();

  /**
   * @brief Flushes the receive buffer.
   */
  void flush();

 private:
  RH_NRF24* radio; /**< The RH_NRF24 radio instance. */
  uint8_t ce_pin; /**< The chip enable (CE) pin for the NRF24L01+ module. */
  uint8_t csn_pin; /**< The chip select (CSN) pin for the NRF24L01+ module. */
  uint16_t sequenceNumber = 0; /**< The sequence number for packets. */

  /**
   * @struct RingBuffer
   * @brief A structure representing a ring buffer.
   */
  struct RingBuffer {
    uint8_t buffer[RING_BUFFER_SIZE]; /**< The buffer array. */
    volatile uint16_t head; /**< The head index of the buffer. */
    volatile uint16_t tail; /**< The tail index of the buffer. */
  };

  RingBuffer rxBuffer; /**< The receive buffer. */
  RingBuffer txBuffer; /**< The transmit buffer. */

  /**
   * @brief Creates a packet with the given buffer and length.
   * @param buf The buffer containing the data.
   * @param len The length of the data.
   * @param packet The packet to create.
   * @return Returns true if the packet is created successfully; otherwise, returns false.
   */
  bool createPacket(uint8_t* buf, int len, PacketFormat* packet);

  /**
   * @brief Sends a packet.
   * @param packet The packet to send.
   * @return Returns true if the packet is sent successfully; otherwise, returns false.
   */
  bool sendPacket(PacketFormat* packet);

  /**
   * @brief Receives a packet.
   * @param packet The packet to receive.
   * @return Returns true if the packet is received successfully; otherwise, returns false.
   */
  bool receivePacket(PacketFormat* packet);

  /**
   * @brief Processes a received packet.
   * @param packet The packet to process.
   * @return Returns true if the packet is processed successfully; otherwise, returns false.
   */
  bool processPacket(PacketFormat* packet);

  /**
   * @brief Sends an acknowledgment packet.
   * @param sequenceNumber The sequence number of the packet to acknowledge.
   */
  void sendAck(uint16_t sequenceNumber);

  /**
   * @brief Handles a received packet.
   */
  void handleReceivedPacket();

  /**
   * @brief Handles a packet to be transmitted.
   */
  void handleTransmitPacket();

  /**
   * @brief Validates the checksum of a packet.
   * @param packet The packet to validate.
   * @return Returns true if the checksum is valid; otherwise, returns false.
   */
  bool validateChecksum(PacketFormat* packet);

  /**
   * @brief Calculates the checksum of a packet.
   * @param packet The packet to calculate the checksum for.
   * @return The calculated checksum.
   */
  uint16_t calculateChecksum(PacketFormat* packet);

  /**
   * @brief Gets the transmit power value for the given modem power.
   * @param power The modem power.
   * @return The transmit power value.
   */
  RH_NRF24::TransmitPower getPowerValue(ModemPower power);

  /**
   * @brief Gets the data rate value for the given modem bitrate.
   * @param biteRate The modem bitrate.
   * @return The data rate value.
   */
  RH_NRF24::DataRate getDataRateValue(ModemBiteRate biteRate);

  /**
   * @brief Stores a byte in the ring buffer.
   * @param byte The byte to store.
   * @param buffer The ring buffer to store the byte in.
   */
  void storeByte(uint8_t byte, RingBuffer* buffer);

  /**
   * @brief Reads a byte from the ring buffer.
   * @param buffer The ring buffer to read the byte from.
   * @return The byte read from the ring buffer.
   */
  int readByte(RingBuffer* buffer);

  /**
   * @brief Gets the number of bytes available in the ring buffer.
   * @param buffer The ring buffer to check.
   * @return The number of bytes available in the ring buffer.
   */
  int bytesAvailable(RingBuffer* buffer);
};

#endif  // MODEM_NRF24_H
