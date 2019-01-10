/**
 * @file BleSensor.cpp
 * 
 * @brief implementation of the BleSensor
 * 
 * @author Peter
 */

#include "BleSensor.h"
#include <Arduino.h>
#include <BLEUtils.h>
#include <BLE2902.h>


void ConnectionHandler::onConnect(BLEServer* pServer) {
    deviceConnected = true;
    connectionChanged = false;
}

void ConnectionHandler::onDisconnect(BLEServer* pServer) {
    connectionChanged = true;
    deviceConnected = false;
}

int ConnectionHandler::getConnectionStatus() {
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

BleSensor::BleSensor() {
    // ble server creation
    BLEDevice::init("ARTrate");
    bleServer = BLEDevice::createServer();
    connectionHandler = new ConnectionHandler();
    bleServer->setCallbacks(connectionHandler);
    // hr ble service creation
    hrService = bleServer->createService(HEART_RATE_SERVICE_UUID);
    // hr ble characteristics
    hrCharacteristic = hrService->createCharacteristic(
                            HEART_RATE_CHARACTERISTIC_UUID,
                            BLECharacteristic::PROPERTY_READ | 
                            BLECharacteristic::PROPERTY_NOTIFY
    );
    // hr ble service creation
    respirationService = bleServer->createService(RESPIRATION_SERVICE_UUID);
    // hr ble characteristics
    respirationCharacteristic = respirationService->createCharacteristic(
                            RESPIRATION_CHARACTERISTIC_UUID,
                            BLECharacteristic::PROPERTY_READ | 
                            BLECharacteristic::PROPERTY_NOTIFY
    );
    // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
    // Create a BLE Descriptor
    hrCharacteristic->addDescriptor(new BLE2902());
    respirationCharacteristic->addDescriptor(new BLE2902());
    // start the services
    hrService->start();
    respirationService->start();
    bleServer->getAdvertising()->start();
}

int BleSensor::setHRValue(uint8_t val) {
    // covert the value to according to the specifications here: https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.characteristic.heart_rate_measurement.xm
    uint8_t flags = 0x01;
    uint16_t valToSend = val<<8 | flags;
    int connectionStatus = connectionHandler->getConnectionStatus();
    if (connectionStatus == DEVICE_STILL_CONNECTED || connectionStatus == NEW_DEVICE_CONNECTED) {
        //there is a connection we can set values
        hrCharacteristic->setValue(valToSend);
        hrCharacteristic->notify();
        delay(10);
        return 0;
    } else if (connectionStatus == DEVICE_HAS_DISCONNECTED) {
        // we have to get ready for a new connection
        delay(500);
        bleServer->startAdvertising();
        return ERROR_DEVICE_NOT_CONNECTED;
    } else {
        // there is no deviceConnected -> nothing to do
        return ERROR_DEVICE_NOT_CONNECTED;
    }
}

int BleSensor::setRespirationValue(int16_t* xAccel, int16_t* yAccel, int16_t* zAccel ) {
    // built an byte array from the acceleration values

    /// they bytearray for the data to send. 
    uint8_t bytearray[18];
    bytearray[0] = xAccel[0] & 0xFF;
    bytearray[1] = (xAccel[0] >> 8) & 0xFF;
    bytearray[2] = xAccel[1] & 0xFF;
    bytearray[3] = (xAccel[1] >> 8) & 0xFF;
    bytearray[4] = xAccel[2] & 0xFF;
    bytearray[5] = (xAccel[2] >> 8) & 0xFF;
    bytearray[6] = yAccel[0] & 0xFF;
    bytearray[7] = (yAccel[0] >> 8) & 0xFF;
    bytearray[8] = yAccel[1] & 0xFF;
    bytearray[9] = (yAccel[1] >> 8) & 0xFF;
    bytearray[10] = yAccel[2] & 0xFF;
    bytearray[11] = (yAccel[2] >> 8) & 0xFF;
    bytearray[12] = zAccel[0] & 0xFF;
    bytearray[13] = (zAccel[0] >> 8) & 0xFF;
    bytearray[14] = zAccel[1] & 0xFF;
    bytearray[15] = (zAccel[1] >> 8) & 0xFF;
    bytearray[16] = zAccel[2] & 0xFF;
    bytearray[17] = (zAccel[2] >> 8) & 0xFF;

    int connectionStatus = connectionHandler->getConnectionStatus();
    if (connectionStatus == DEVICE_STILL_CONNECTED || connectionStatus == NEW_DEVICE_CONNECTED) {
        //there is a connection we can set values
        respirationCharacteristic->setValue(bytearray, 18);
        respirationCharacteristic->notify();
        delay(10);
        return 0;
    } else if (connectionStatus == DEVICE_HAS_DISCONNECTED) {
        // we have to get ready for a new connection
        delay(500);
        bleServer->startAdvertising();
        return ERROR_DEVICE_NOT_CONNECTED;
    } else {
        // there is no deviceConnected -> nothing to do
        return ERROR_DEVICE_NOT_CONNECTED;
    }
}