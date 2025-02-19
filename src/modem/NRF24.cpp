#include "modem/NRF24.h"

#include <SPI.h>

ModemNRF24::ModemNRF24(uint8_t _ce_pin, uint8_t _csn_pin)
    : radio(nullptr), ce_pin(_ce_pin), csn_pin(_csn_pin) {}

ModemNRF24::~ModemNRF24() {
  if (radio != nullptr) {
    delete radio;
  }
}

/**
 * Initializes the ModemNRF24 object.
 *
 * This function sets up the ModemNRF24 instance by checking the validity of
 * the CE and CSN pins, creating an RH_NRF24 radio instance, and initializing
 * the radio module. It configures the default mode, bitrate, power, and
 * channel settings for the radio.
 *
 * @return Returns true if initialization is successful; otherwise, returns false.
 */
bool ModemNRF24::init() {
  // Check if the chip enable (CE) pin and chip select (CSN) pin are set.
  // We need these pins to communicate with the radio module.
  if (ce_pin == 0 || csn_pin == 0) {
    Serial.println("Invalid CE or CSN pin");
    return false;
  }

  // If the pins are set, create a new instance of the RH_NRF24 radio with
  // the specified CE and CSN pins.
  radio = new RH_NRF24(ce_pin, csn_pin);

  // Ensure radio is not nullptr before calling init
  if (radio == nullptr) {
    Serial.println("Failed to create RH_NRF24 instance");
    return false;
  }

  // Initialize the radio module.
  // This will set the radio up to use the specified CE and CSN pins,
  // and make sure everything is set up properly.
  if (!radio->init()) {
    // If the initialization fails, print an error message and return false.
    Serial.println("NRF24 init failed");
    return false;
  }

  // Set the default mode, bitrate, power, and channel for the radio.
  // This will make sure the radio is set up the way we want it to be.
  setMode(MODEM_MODE_IDLE);
  setBiteRate(MODEM_BITE_RATE_SLOW);
  setPower(MODEM_POWER_LOW);
  setChannel(MODEM_CHANNEL_1);

  Serial.println("NRF24 initialized");

  // Return true if the initialization was successful.
  return true;
}

/**
 * Check for received packets and send any packets that need to be sent.
 *
 * If the radio is not initialized, print an error message and return.
 *
 * If there are any packets available, call handleReceivedPacket() to process them.
 *
 * If the radio is not in transmit mode, call handleTransmitPacket() to send any
 * packets that need to be sent.
 */
void ModemNRF24::update() {
  if (radio == nullptr) {
    Serial.println("Radio is not initialized");
    return;
  }

  if (radio->available()) {
    handleReceivedPacket();
  }

  if (radio->mode() != RH_NRF24::RHModeTx) {
    handleTransmitPacket();
  }
}

void ModemNRF24::setMode(ModemMode mode) { this->mode = mode; }

/**
 * Set the bitrate for the radio module.
 *
 * This will set the bitrate for the radio to the specified value.
 * The available bitrates are MODEM_BITE_RATE_SLOW, MODEM_BITE_RATE_MEDIUM, MODEM_BITE_RATE_FAST, and
 * MODEM_BITE_RATE_REALTIME.
 *
 * @param _bitrate The bitrate to set the radio to.
 * 
 * @return void
 */
void ModemNRF24::setBiteRate(ModemBiteRate _bitrate) {
  RH_NRF24::TransmitPower powerValue = getPowerValue(power);
  RH_NRF24::DataRate dataRateValue = getDataRateValue(_bitrate);
  radio->setRF(dataRateValue, powerValue);
  biteRate = _bitrate;
}

/**
 * Set the transmit power for the radio module.
 *
 * This function sets the transmit power for the radio module. The power is
 * specified as an enumeration of type ModemPower, which can take on the values
 * MODEM_POWER_LOW, MODEM_POWER_MEDIUM, or MODEM_POWER_HIGH.
 *
 * @param _power The transmit power to set for the radio module.
 * 
 * @return void
 */
void ModemNRF24::setPower(ModemPower _power) {
  RH_NRF24::TransmitPower powerValue = getPowerValue(_power);
  RH_NRF24::DataRate dataRateValue = getDataRateValue(biteRate);
  radio->setRF(dataRateValue, powerValue);
  power = _power;
}

/**
 * Set the channel for the radio module.
 *
 * This function sets the channel for the radio module. The channel is
 * specified as an enumeration of type ModemChannel, which can take on the
 * values MODEM_CHANNEL_1, MODEM_CHANNEL_2, MODEM_CHANNEL_3, or MODEM_CHANNEL_4.
 *
 * @param _channel The channel to set for the radio module.
 * 
 * @return void
 */
void ModemNRF24::setChannel(ModemChannel _channel) {
  switch (_channel) {
    case MODEM_CHANNEL_1:
      radio->setChannel(10);
      break;
    case MODEM_CHANNEL_2:
      radio->setChannel(20);
      break;
    case MODEM_CHANNEL_3:
      radio->setChannel(30);
      break;
    case MODEM_CHANNEL_4:
      radio->setChannel(40);
      break;
  }
  channel = _channel;
}

int ModemNRF24::available() {
  return (RING_BUFFER_SIZE + rxBuffer.head - rxBuffer.tail) % RING_BUFFER_SIZE;
}

int ModemNRF24::read() {
  if (rxBuffer.head == rxBuffer.tail) {
    return -1;
  } else {
    return readByte(&rxBuffer);
  }
}

int ModemNRF24::peek() {
  if (rxBuffer.head == rxBuffer.tail) {
    return -1;
  } else {
    return rxBuffer.buffer[rxBuffer.tail];
  }
}

size_t ModemNRF24::write(uint8_t data) {
  storeByte(data, &txBuffer);
  return 1;
}

/**
 * Writes data to the transmit buffer.
 *
 * @param buffer The data to write to the transmit buffer.
 * @param size The number of bytes to write from the buffer.
 *
 * @return The number of bytes written to the transmit buffer.
 */
size_t ModemNRF24::write(const uint8_t* buffer, size_t size) {
  for (size_t i = 0; i < size; i++) {
    storeByte(buffer[i], &txBuffer);
  }
  return size;
}

/**
 * Returns the number of bytes available for writing in the transmit buffer.
 *
 * This value is always a positive integer, and may be zero if the buffer is
 * full.
 *
 * @return The number of bytes available for writing in the transmit buffer.
 */
int ModemNRF24::availableForWrite() {
  return (RING_BUFFER_SIZE + txBuffer.tail - txBuffer.head - 1) %
         RING_BUFFER_SIZE;
}

/**
 * Empties the receive buffer.
 *
 * This function repeatedly reads data from the receive buffer until it is empty.
 * It is useful for clearing out any old data that may be in the buffer after
 * initialization or when switching to a different communication mode.
 */
void ModemNRF24::flush() {
  while (read() != -1) {
    // empty the buffer
  }
  
}

/**
 * Creates a packet with the given buffer and length.
 *
 * @param buffer The data buffer to be included in the packet.
 * @param len The length of the data buffer.
 * @param packet The PacketFormat structure where the packet will be created.
 * 
 * @return true if the packet was created successfully, false if the length exceeds the maximum allowable data length.
 * 
 * The packet is initialized with a sequence number, flags, and a checksum calculated from the data.
 */
bool ModemNRF24::createPacket(uint8_t* buffer, int len, PacketFormat* packet) {
  if (len > PacketFormat::maxDataLength) {
    return false;
  }
  packet->sequenceNumber = sequenceNumber++;
  packet->length = len;
  // set default flags
  packet->flags = PACKET_FLAG_ACK | PACKET_FLAG_PRIORITY;
  packet->data = buffer;
  packet->checksum = calculateChecksum(packet);
  return true;
}

/**
 * Send a packet over the radio.
 * 
 * @param packet The packet to be sent.
 * 
 * @return true if the packet was sent successfully, false otherwise.
 */
bool ModemNRF24::sendPacket(PacketFormat* packet) {
  uint8_t len = PacketFormat::headerLength + packet->length;
  return radio->send((uint8_t*)packet, len);
}

/**
 * Receive a packet from the radio module.
 *
 * @param packet The packet to be received.
 *
 * @return boolean True if the packet was received successfully, false
 * otherwise.
 */
bool ModemNRF24::receivePacket(PacketFormat* packet) {
  uint8_t len = sizeof(*packet);
  return radio->recv((uint8_t*)packet, &len);
}

/**
 * Process a received packet.
 *
 * @param packet The packet to be processed.
 *
 * @return void
 */
bool ModemNRF24::processPacket(PacketFormat* packet) {
  // Store the packet in the receive buffer
  for (int i = 0; i < packet->length; i++) {
    if (bytesAvailable(&rxBuffer) > 0) {
      storeByte(packet->data[i], &rxBuffer);
    } else {
      break;
    }
  }

  return false;
}

/**
 * Send an acknowledgment packet with the provided sequence number.
 *
 * @param sequenceNumber The sequence number of the packet to be acknowledged.
 *
 * @return void
 */
void ModemNRF24::sendAck(uint16_t sequenceNumber) {
  // Create a new packet of type PacketFormat
  PacketFormat packet;

  // Set the sequence number of the packet to the provided sequence number
  packet.sequenceNumber = sequenceNumber;

  // Set the length of the packet data to 0, since this is an acknowledgment
  // packet
  packet.length = 0;

  // Set the packet flags to indicate that this is an acknowledgment packet
  packet.flags = PACKET_FLAG_ACK;

  // Calculate and set the checksum for the packet
  packet.checksum = calculateChecksum(&packet);

  // Send the packet using the sendPacket function
  sendPacket(&packet);
}

/**
 * Handle a received packet from the radio module.
 *
 * This function attempts to receive a packet from the radio module.
 * If a packet is successfully received, it validates the checksum.
 * If the packet is an acknowledgment (ACK) packet, it logs the receipt
 * of the ACK. If the packet is successfully processed, it sends an
 * acknowledgment for the packet.
 *
 * @return void
 */
void ModemNRF24::handleReceivedPacket() {
  PacketFormat packet;
  if (receivePacket(&packet)) {
    Serial.println("Received packet");
    if (validateChecksum(&packet)) {
      if (packet.flags & PACKET_FLAG_ACK) {
        Serial.println("ACK received");
      }
      if (processPacket(&packet)) {
        sendAck(packet.sequenceNumber);
      }
    }
  }
}

/**
 * Handle transmission of a packet.
 *
 * This function is called when a packet needs to be sent.
 * It reads a packet from the transmit buffer, creates a new packet
 * and sends it using the sendPacket function.
 *
 * @return void
 */
void ModemNRF24::handleTransmitPacket() {
  PacketFormat packet;
  uint8_t buffer[PacketFormat::maxDataLength];
  uint8_t len = 0;

  int availableBytes = bytesAvailable(&txBuffer);
  int bytesToRead = min(availableBytes, PacketFormat::maxDataLength);

  for (int i = 0; i < bytesToRead; i++) {
    buffer[len++] = readByte(&txBuffer);
  }

  if (createPacket(buffer, len, &packet)) {
    Serial.println("Sending packet");
    sendPacket(&packet);
  }
}

bool ModemNRF24::validateChecksum(PacketFormat* packet) {
  return packet->checksum == calculateChecksum(packet);
}

/**
 * Calculates the checksum of a packet.
 *
 * The checksum is calculated by summing up the fields of the packet
 * structure.
 *
 * @param packet The packet to calculate the checksum of.
 *
 * @return uint16_t The calculated checksum.
 */
uint16_t ModemNRF24::calculateChecksum(PacketFormat* packet) {
  uint16_t checksum = 0;
  checksum += packet->sequenceNumber;
  checksum += packet->length;
  checksum += packet->stream;
  checksum += packet->flags;
  for (uint8_t i = 0; i < packet->length; i++) {
    checksum += packet->data[i];
  }
  return checksum;
}

/**
 * Converts a ModemPower enum to a RH_NRF24::TransmitPower enum.
 *
 * @param power The power to convert.
 *
 * @return The corresponding RH_NRF24::TransmitPower enum value.
 */
RH_NRF24::TransmitPower ModemNRF24::getPowerValue(ModemPower power) {
  switch (power) {
    case MODEM_POWER_LOW:
      return RH_NRF24::TransmitPower0dBm;
    case MODEM_POWER_MEDIUM:
      return RH_NRF24::TransmitPowerm6dBm;
    case MODEM_POWER_HIGH:
      return RH_NRF24::TransmitPowerm12dBm;
    case MODEM_POWER_MAX:
      return RH_NRF24::TransmitPowerm18dBm;
    default:
      return RH_NRF24::TransmitPower0dBm;
  }
}

/**
 * Converts a ModemBiteRate enum to a RH_NRF24::DataRate enum.
 *
 * @param bitrate The bitrate to convert.
 *
 * @return The corresponding RH_NRF24::DataRate enum value.
 */
RH_NRF24::DataRate ModemNRF24::getDataRateValue(ModemBiteRate bitrate) {
  switch (bitrate) {
    case MODEM_BITE_RATE_SLOW:
      return RH_NRF24::DataRate250kbps;
    case MODEM_BITE_RATE_MEDIUM:
      return RH_NRF24::DataRate1Mbps;
    case MODEM_BITE_RATE_FAST:
      return RH_NRF24::DataRate2Mbps;
    case MODEM_BITE_RATE_REALTIME:
      return RH_NRF24::DataRate2Mbps;
    default:
      return RH_NRF24::DataRate250kbps;
  }
}

/**
 * Stores a byte into the ring buffer.
 *
 * This function adds a byte to the ring buffer at the current head position
 * and then updates the head position. If the buffer is full (i.e., the next
 * position equals the tail), the byte is not added, and the buffer remains
 * unchanged.
 *
 * @param byte The byte to be stored in the ring buffer.
 * @param buffer The ring buffer where the byte will be stored.
 * 
 * @return void
 */

void ModemNRF24::storeByte(uint8_t byte, RingBuffer* buffer) {
  uint16_t next = (buffer->head + 1) % RING_BUFFER_SIZE;
  if (next != buffer->tail) {
    buffer->buffer[buffer->head] = byte;
    buffer->head = next;
  }
}

/**
 * Retrieves a byte from the ring buffer.
 *
 * This function checks if the ring buffer is empty, and if so, returns -1.
 * Otherwise, it retrieves the byte at the current tail position, updates
 * the tail position, and returns the byte.
 *
 * @param buffer The ring buffer to retrieve the byte from.
 *
 * @return The byte retrieved from the ring buffer, or -1 if the buffer is
 * empty.
 */
int ModemNRF24::readByte(RingBuffer* buffer) {
  if (buffer->head == buffer->tail) {
    return -1;
  } else {
    uint8_t byte = buffer->buffer[buffer->tail];
    buffer->tail = (buffer->tail + 1) % RING_BUFFER_SIZE;
    return byte;
  }
}

/**
 * Returns the number of bytes available to be read from the ring buffer.
 *
 * @param buffer The ring buffer to query.
 *
 * @return The number of bytes available to be read from the ring buffer.
 */
int ModemNRF24::bytesAvailable(RingBuffer* buffer) {
  return (RING_BUFFER_SIZE + buffer->head - buffer->tail) % RING_BUFFER_SIZE;
}
