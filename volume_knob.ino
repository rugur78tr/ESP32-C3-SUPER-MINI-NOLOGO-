#include "NimBLEDevice.h"
#include "NimBLEHIDDevice.h"
#include "AiEsp32RotaryEncoder.h"

// --- Pin Definitions ---
#define ROTARY_ENCODER_CLK_PIN 4  // CLK pin from encoder (A) -> GPIO 4
#define ROTARY_ENCODER_DT_PIN  3  // DT pin from encoder (B) -> GPIO 3
#define ROTARY_ENCODER_SW_PIN  2  // SW pin from encoder (Button) -> GPIO 2

// Rotary Encoder Configuration
#define ROTARY_ENCODER_STEPS 1
AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(ROTARY_ENCODER_DT_PIN, ROTARY_ENCODER_CLK_PIN, ROTARY_ENCODER_SW_PIN, -1, ROTARY_ENCODER_STEPS);

// --- BLE HID Setup ---
NimBLEHIDDevice* hid;
NimBLECharacteristic* inputReport;
bool bleConnected = false;
static const char* DEVICE_NAME = "VolumeKnob-C3";

// Media Key HID Codes
#define MEDIA_KEY_VOLUME_UP     0x80
#define MEDIA_KEY_VOLUME_DOWN   0x81
#define MEDIA_KEY_MUTE          0xE2

// --- Interrupt and Helper Functions ---
void IRAM_ATTR readEncoderISR() {
  rotaryEncoder.readEncoder_ISR();
}

void reportMediaKey(uint8_t mediaKey) {
  if (bleConnected) {
    uint8_t report[] = {0x00, 0x00, mediaKey, 0x00}; // Consumer Report: Usage Page 0xC, Usage 0x1
    inputReport->setValue(report, 4);
    inputReport->notify();
    // Send a 'key up' report (all zeroes) to signal the press is over
    uint8_t releaseReport[] = {0x00, 0x00, 0x00, 0x00};
    inputReport->setValue(releaseReport, 4);
    inputReport->notify();
  }
}

// --- Bluetooth Callbacks ---
class MyServerCallbacks : public NimBLEServerCallbacks {
  void onConnect(NimBLEServer* pServer, ble_gap_conn_desc* desc) {
    Serial.println("BLE Client Connected");
    bleConnected = true;
  }
  void onDisconnect(NimBLEServer* pServer) {
    Serial.println("BLE Client Disconnected, starting advertising...");
    bleConnected = false;
    NimBLEDevice::startAdvertising(); // Restart advertising to allow re-connection
  }
};

void initBLE() {
  NimBLEDevice::init(DEVICE_NAME);
  NimBLEDevice::setSecurityAllowed((esp_ble_auth_req_t)(BLE_SM_PAIR_IO_CAP_NO_IO));

  NimBLEServer* pServer = NimBLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  hid = new NimBLEHIDDevice(pServer);
  
  // Custom HID Report Map for Consumer Control (Media Keys)
  std::string hidReportMap =
      "0x05, 0x0C,       "  // Usage Page (Consumer Devices)
      "0x09, 0x01,       "  // Usage (Consumer Control)
      "0xA1, 0x01,       "  // Collection (Application)
      "0x85, 0x01,       "  // Report ID (1)
      "0x09, 0xE0,       "  // Usage (Volume Increment)
      "0x09, 0xE1,       "  // Usage (Volume Decrement)
      "0x09, 0xE2,       "  // Usage (Mute)
      "0x15, 0x00,       "  // Logical Minimum (0)
      "0x25, 0x01,       "  // Logical Maximum (1)
      "0x75, 0x01,       "  // Report Size (1)
      "0x95, 0x03,       "  // Report Count (3)
      "0x81, 0x02,       "  // Input (Data, Variable, Absolute)
      "0x95, 0x05,       "  // Report Count (5)
      "0x81, 0x03,       "  // Input (Constant, Variable, Absolute) - Padding
      "0xC0              "; // End Collection

  hid->setReportMap((uint8_t*)hidReportMap.c_str(), hidReportMap.length());
  hid->startServices();

  inputReport = hid->getCharacteristic(hid->inputReportUUID);

  NimBLEAdvertising* pAdvertising = pServer->getAdvertising();
  pAdvertising->setAppearance(ESP_BLE_APPEARANCE_GENERIC_HID);
  pAdvertising->addServiceUUID(hid->hidService()->getUUID());
  pAdvertising->start();
  
  Serial.println("BLE Advertising started. Ready to pair.");
  Serial.print("BLE Device Address: ");
  Serial.println(NimBLEDevice::getAddress().toString().c_str());
}

// --- Setup and Loop ---

void setup() {
  Serial.begin(115200);
  Serial.println("Starting ESP32-C3 Volume Knob...");

  // Setup Rotary Encoder
  rotaryEncoder.begin();
  rotaryEncoder.setup(readEncoderISR);
  rotaryEncoder.setBoundaries(-100000, 100000, true); // Arbitrary large range
  rotaryEncoder.setAcceleration(0);

  // Setup Push Button for Mute
  pinMode(ROTARY_ENCODER_SW_PIN, INPUT_PULLUP);

  // Setup Bluetooth HID
  initBLE();
}

void loop() {
  // 1. Handle Rotary Encoder Rotation (Volume Up/Down)
  static int32_t lastEncoderPos = 0;
  int32_t newEncoderPos = rotaryEncoder.readEncoder();

  if (newEncoderPos != lastEncoderPos) {
    if (newEncoderPos > lastEncoderPos) {
      Serial.println("Volume Up ⬆️");
      reportMediaKey(MEDIA_KEY_VOLUME_UP);
    } else {
      Serial.println("Volume Down ⬇️");
      reportMediaKey(MEDIA_KEY_VOLUME_DOWN);
    }
    lastEncoderPos = newEncoderPos;
  }

  // 2. Handle Rotary Encoder Switch Press (Mute/Unmute)
  static bool lastButtonState = HIGH;
  bool currentButtonState = digitalRead(ROTARY_ENCODER_SW_PIN);
  
  // Check for a falling edge (button press)
  if (currentButtonState == LOW && lastButtonState == HIGH) {
    Serial.println("Mute Toggle 🔇");
    reportMediaKey(MEDIA_KEY_MUTE);
    delay(200); // Simple debounce
  }
  
  lastButtonState = currentButtonState;

  delay(10);
}
