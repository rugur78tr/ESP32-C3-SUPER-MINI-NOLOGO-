# ESP32-C3-SUPER-MINI-NOLOGO-


<img width="708" height="194" alt="image" src="https://github.com/user-attachments/assets/535eac38-1884-4138-a135-12988a035ec4" />

<img width="718" height="305" alt="image" src="https://github.com/user-attachments/assets/f1fed180-b253-4e0a-86b6-a46838049ab5" />

This is a fantastic project! Using the ESP32-C3 Super Mini as a Bluetooth HID (Human Interface Device) for a volume knob is a perfect application.

The ESP32-C3 supports Bluetooth Low Energy (BLE) HID, which is ideal for this purpose, as it doesn't require extra drivers on Windows 10.

Here is the breakdown for your project:

1. Arduino IDE and ESP32 Core Setup
It's best to use a recent version of the Arduino IDE (2.x is recommended, but 1.8.x also works).

ESP32 Core: You'll need the esp32 board package installed.

Boards Manager URL: https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

Installation: In the Arduino IDE, go to File > Preferences, paste the URL into "Additional Board Manager URLs." Then go to Tools > Board > Boards Manager, search for "esp32," and install the latest version (e.g., 2.0.14 or newer).

Board Selection: Select the correct board under Tools > Board > ESP32 Arduino:

Board: Choose "ESP32C3 Dev Module" or "Generic ESP32C3".

Flash Mode: QIO (or default).

Port: Select the correct COM port after plugging in the USB-C cable.

5. Pairing on Windows 10
Upload the code to your ESP32-C3 Super Mini.

On Windows 10, go to Settings > Devices > Bluetooth & other devices.

Click "Add Bluetooth or other device" > Bluetooth.

Your device should appear as VolumeKnob-C3 (or whatever name you used).

Click to pair. Windows will recognize it as an input device.

