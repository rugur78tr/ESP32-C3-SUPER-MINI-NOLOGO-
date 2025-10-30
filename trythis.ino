// USE THE TOOLS MENU TO SELECT
// board ESP32 C3 SUPERMINI
// IDE board: ESP32C Dev Module
// Libraries Expressif 
// programmer  AVRmk2
// USB ON CDC ENABLED


#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLEAdvertising.h>  // v10 gemini

//BLE comms
String rcvdstring;
String rcvdcopy;
String report = "STOPPED";   // used to reply to phone
String widgetStatus = "STOPPED" ; // first phase of a flight
boolean datarcvd = false;
bool deviceConnected = false; // Flag to track connection status

// BLE stuff
#define SERVICE_UUID         "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
//                                       4fafc201-1fb5-459e-8fcc-c5c9c331914b  SEEED
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
//                                       beb5483e-36e1-4688-b7f5-ea07361b26a8  SEEED

BLEServer* pServer = nullptr; // Declare pServer globally so we can access it in callbacks
BLEAdvertising* pAdvertising = nullptr; // Declare pAdvertising globally

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
     
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    
       pAdvertising->start(); // Restart advertising on disconnection
      
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue().c_str(); //gemini v10
      pCharacteristic->setValue(report.c_str()); // reply to phone
      if (value.length() > 0) {
        rcvdstring = "";
        for (int i = 0; i < value.length(); i++) {
          // Serial.print(value[i]); //
          rcvdstring = rcvdstring + value[i];
        }
        datarcvd = true;
        report=""; // 
      }
    }
};

//end of gemini code copied over

///////////////////////////////////     SETUP   ///////////////////////////
void setup() {

//  BLE stuff
// gemini code onwards
  BLEDevice::init("MyBLE_DEvice");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks()); // Set the server callbacks

  BLEService *pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                             CHARACTERISTIC_UUID,
                                             BLECharacteristic::PROPERTY_READ |
                                             BLECharacteristic::PROPERTY_WRITE
                                           );

  pCharacteristic->setCallbacks(new MyCallbacks());
  pCharacteristic->setValue("Profiler connected!");
  pService->start();

  pAdvertising = pServer->getAdvertising();
  pAdvertising->start();
  
// end of gemini code


}   // end of setup



//               **************  MAIN LOOP   *******************
void loop() {

  if (datarcvd) {  // had a msg from the phone
   // checkMessage(); // msg rcvd from phone via BLE - this will also set the reply as variable 'report'
  }

 // PUT YOUR CODE HERE TO DEAL WITH THE INCOMING MSG


}                                             // end of main loop

//////////////////////////////////////////////////////////////////

/*
Compilation output
Using board 'esp32c3' from platform in folder: C:\Users\Dave\AppData\Local\Arduino15\packages\esp32\hardware\esp32\3.2.0
Using core 'esp32' from platform in folder: C:\Users\Dave\AppData\Local\Arduino15\packages\esp32\hardware\esp32\3.2.0

Detecting libraries used...
C:\Users\Dave\AppData\Local\arduino\sketches\0E7F874EB7DA0967668CB84A920D6DD8/ESP32C3_BLE.ino.merged.bin, ready to flash to offset 0x0
Multiple libraries were found for "BLEDevice.h"
  Used: C:\Users\Dave\AppData\Local\Arduino15\packages\esp32\hardware\esp32\3.2.0\libraries\BLE
  Not used: C:\Users\Dave\Documents\Personal\Electronics\Arduino\Sketches\libraries\ArduinoBLE
Using library BLE at version 3.2.0 in folder: C:\Users\Dave\AppData\Local\Arduino15\packages\esp32\hardware\esp32\3.2.0\libraries\BLE 


*/
