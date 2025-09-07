# OBD2_data_export

Arduino sketch for exporting vehicle telemetry over Serial via the OBD-II CAN bus.

## Features
- Initializes MCP2515-based CAN interface at 500 kbps and sets masks/filters for ECU responses.
- Functions to request and parse standard PIDs:
  - Vehicle speed
  - Engine RPM
  - Accelerator pedal position
- Reads brake status from an analog input.
- `loop()` queries these values and prints formatted data `/S<speed>/R<rpm>/P<pedal>/B<brake>` over Serial.

## Hardware
- Requires a board with an MCP2515 CAN controller (e.g., CANBed, CAN Bus Shield).
- Uses `SPI_CS_PIN` to select the CAN controller (default `9`).
- `BRAKE_PIN` (`A1`) monitors a brake switch.

## Usage
1. Connect the hardware to the vehicle's OBD-II port.
2. Compile and upload `OBD2_data_export.ino` to the microcontroller.
3. Open the Serial Monitor at 115200 baud to view the telemetry output.

## Demo Videos
- [OBD and brake signal test via Arduino IDE Serial Monitor](https://youtu.be/m80BvdzcJ14?si=kJuWkYoUcHwdtPdA)
- [OBD and brake signal test with Python reading Serial data](https://youtu.be/XU-K6cFl0xY?si=2aGPG9y0yyfWvpPZ)
