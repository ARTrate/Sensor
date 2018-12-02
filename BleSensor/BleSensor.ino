/*!
   Arduino test program for the ble sensor
*/

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define HEART_RATE_SERVICE_UUID        "0000180d-0000-1000-8000-00805f9b34fb"
#define HEART_RATE_CHARACTERISTIC_UUID "00002a37-0000-1000-8000-00805f9b34fb"
#define ERROR_DEVICE_NOT_CONNECTED -20

#define NEW_DEVICE_CONNECTED 22
#define DEVICE_STILL_CONNECTED 23
#define DEVICE_HAS_DISCONNECTED 24
#define NO_CONNECTED_DEVICE 25

class BleSensor {
    private:
        class ConnectionHandler: public BLEServerCallbacks {
            private: 
                bool deviceConnected;
                bool connectionChanged;

            public: 
                void onConnect(BLEServer* pServer) {
                    deviceConnected = true;
                    connectionChanged = false;
                 };

                void onDisconnect(BLEServer* pServer) {
                    connectionChanged = true;
                    deviceConnected = false;
                }
                
                int getConnectionStatus() {
                    if(deviceConnected && connectionChanged){
                        connectionChanged = false;
                        return NEW_DEVICE_CONNECTED;
                    } else if (deviceConnected) {
                        connectionChanged = false;
                        return DEVICE_STILL_CONNECTED;
                    } else if(!deviceConnected && connectionChanged) {
                        connectionChanged = false;
                        return DEVICE_HAS_DISCONNECTED;
                    } else if (!deviceConnected) {
                        connectionChanged = false;
                        return NO_CONNECTED_DEVICE;
                    }
                }
        };

        BLEServer* bleServer;
        BLECharacteristic* bleCharacteristic;
        BLEService* bleService;
        ConnectionHandler* connectionHandler;

    public:
        BleSensor() {
            // ble server creation
            BLEDevice::init("ARTrate");
            bleServer = BLEDevice::createServer();
            connectionHandler = new ConnectionHandler();
            bleServer->setCallbacks(connectionHandler);

            // ble service creation
            bleService = bleServer->createService(HEART_RATE_SERVICE_UUID);

            // ble characteristics
            bleCharacteristic = bleService->createCharacteristic(
                                    HEART_RATE_CHARACTERISTIC_UUID,
                                    BLECharacteristic::PROPERTY_READ | 
                                    BLECharacteristic::PROPERTY_NOTIFY
            );

            // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
            // Create a BLE Descriptor
            bleCharacteristic->addDescriptor(new BLE2902());

            // start the service
            bleService->start();
            bleServer->getAdvertising()->start();
        };

        int setValue(uint32_t val) {
            int connectionStatus = connectionHandler->getConnectionStatus();
            if (connectionStatus == DEVICE_STILL_CONNECTED || connectionStatus == NEW_DEVICE_CONNECTED) {
                //there is a connection we can set values
                bleCharacteristic->setValue(val);
                bleCharacteristic->notify();
                delay(10);
                return 0;
            } else if (connectionStatus == DEVICE_HAS_DISCONNECTED) {
                // we have to get ready for a new connection
                delay(500);
                bleServer->startAdvertising();
                return -1;
            } else {
                // there is no deviceConnected -> nothing to do
                return -1;
            }
        }
};

//----------------------------------------------------------------------------
static BleSensor* bleSensor;
static uint32_t heartRate = 80;

void setup() {
    bleSensor = new BleSensor();
}

void loop() {
    if (heartRate == 50) {
        heartRate = 80;
    }
    bleSensor->setValue(heartRate);
    heartRate--;
    delay(500);
}
