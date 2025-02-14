#ifndef MODEM_H
#define MODEM_H

#include <Arduino.h>
#include <stdint.h>

/**
 * Modem class definition
 *
 * Modems can send and receive byte streams
 */
class Modem : public Stream {
 public:
  enum PacketFlags {
    PACKET_FLAG_ACK = (1 << 0),
    PACKET_FLAG_RETRY = (1 << 1),
    PACKET_FLAG_ERROR = (1 << 2),
    PACKET_FLAG_BROADCAST = (1 << 3),
    PACKET_FLAG_PRIORITY = (1 << 4),
    PACKET_FLAG_ENCRYPTED = (1 << 5),
    PACKET_FLAG_FRAGMENT = (1 << 6),
    PACKET_FLAG_RESERVED = (1 << 7)
  };

  enum PacketType {
    PACKET_TYPE_DATA,
    PACKET_TYPE_ACK,
    PACKET_TYPE_NACK,
    PACKET_TYPE_CHANGE_MODE,
    PACKET_TYPE_CHANGE_BITE_RATE,
    PACKET_TYPE_CHANGE_POWER,
    PACKET_TYPE_CHANGE_CHANNEL
  };

  enum ModemMode { MODEM_MODE_IDLE, MODEM_MODE_ACTIVE, MODEM_MODE_SCAN };

  enum ModemBiteRate {
    MODEM_BITE_RATE_SLOW,
    MODEM_BITE_RATE_MEDIUM,
    MODEM_BITE_RATE_FAST,
    MODEM_BITE_RATE_REALTIME
  };

  enum ModemPower {
    MODEM_POWER_LOW,
    MODEM_POWER_MEDIUM,
    MODEM_POWER_HIGH,
    MODEM_POWER_MAX
  };

  enum ModemChannel {
    MODEM_CHANNEL_1,
    MODEM_CHANNEL_2,
    MODEM_CHANNEL_3,
    MODEM_CHANNEL_4
  };

  virtual ~Modem() {}  // Important: Virtual destructor for polymorphism

  virtual bool init() = 0;

  virtual void update();

  virtual void setMode(ModemMode mode);
  virtual void setBiteRate(ModemBiteRate biteRate);
  virtual void setPower(ModemPower power);
  virtual void setChannel(ModemChannel channel);

  virtual uint8_t getMaxDataLength() = 0;

 protected:
  ModemMode mode = MODEM_MODE_IDLE;
  ModemBiteRate biteRate = MODEM_BITE_RATE_SLOW;
  ModemPower power = MODEM_POWER_LOW;
  ModemChannel channel = MODEM_CHANNEL_1;
};

#endif  // MODEM_H