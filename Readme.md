
# WirelessSerial

WirelessSerial is a project that implements a wireless communication system using the NRF24L01+ radio module. The project is designed to provide reliable and efficient wireless communication between devices.

## Features

- Initialization and configuration of the NRF24L01+ radio module
- Sending and receiving packets with acknowledgment
- Configurable bitrate, power, and channel settings
- Ring buffer implementation for handling incoming and outgoing data
- Packet checksum validation for data integrity

## Getting Started

### Prerequisites

- [PlatformIO](https://platformio.org/) installed on your development environment
- [Arduino](https://www.arduino.cc/) framework
- NRF24L01+ radio modules

### Installation

1. Clone the repository to your local machine:

   ```sh
   git clone https://github.com/FLYBYME/WirelessSerial.git
   ```

2. Open the project in PlatformIO.

3. Connect the NRF24L01+ radio module to your microcontroller.

### Usage

1. Initialize the `ModemNRF24` object with the appropriate CE and CSN pins:

   ```cpp
   ModemNRF24 modem(9, 10); // Example pins
   ```

2. Call the `init` method to initialize the radio module:

   ```cpp
   if (!modem.init()) {
     Serial.println("Failed to initialize modem");
     while (1);
   }
   ```

3. Use the `write` method to send data:

   ```cpp
   modem.write("Hello, world!", 13);
   ```

4. Use the `read` method to receive data:

   ```cpp
   if (modem.available()) {
     int byte = modem.read();
     Serial.write(byte);
   }
   ```

### Example

Here is a simple example of how to use the `ModemNRF24` class:

```cpp
#include <Arduino.h>
#include "modem/NRF24.h"

ModemNRF24 modem(9, 10);

void setup() {
  Serial.begin(9600);
  if (!modem.init()) {
    Serial.println("Failed to initialize modem");
    while (1);
  }
}

void loop() {
  modem.update();

  if (modem.available()) {
    int byte = modem.read();
    Serial.write(byte);
  }

  if (Serial.available()) {
    int byte = Serial.read();
    modem.write(byte);
  }
}
```

## Contributing

Contributions are welcome! Please open an issue or submit a pull request for any improvements or bug fixes.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
