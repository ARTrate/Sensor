/**
 * @file BleSensor.cpp
 * 
 * @brief implementation of the BleSensor
 * 
 * @author Peter
 */

#include "BleSensor.hpp"
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
}

int BleSensor::setValue(uint8_t val) {
    // covert the value to according to the specifications here: https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.characteristic.heart_rate_measurement.xm
    uint8_t flags = 0x01;
    uint16_t valToSend = val<<8 | flags;
    int connectionStatus = connectionHandler->getConnectionStatus();
    if (connectionStatus == DEVICE_STILL_CONNECTED || connectionStatus == NEW_DEVICE_CONNECTED) {
        //there is a connection we can set values
        bleCharacteristic->setValue(valToSend);
        bleCharacteristic->notify();
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