/**
 * @file BleSensor.hpp
 * 
 * @brief Class definitions for the BleSensor
 * 
 * @author Peter
 */

#include <BLEDevice.h>
#include <BLEServer.h>

/// Service UUID of the heart rate service
#define HEART_RATE_SERVICE_UUID        "0000180d-0000-1000-8000-00805f9b34fb"

/// Characteristic UUID of the heart rate service
#define HEART_RATE_CHARACTERISTIC_UUID "00002a37-0000-1000-8000-00805f9b34fb"

/// Service UUID of the heart rate service
#define RESPIRATION_SERVICE_UUID        "00001700-0000-1000-8000-00805f9b34fb"

/// Characteristic UUID of the heart rate service
#define RESPIRATION_CHARACTERISTIC_UUID "00002222-0000-1000-8000-00805f9b34fb"

/// Error Code when no device is connected
#define ERROR_DEVICE_NOT_CONNECTED -20

/// internal callback code new device connected
#define NEW_DEVICE_CONNECTED 22

/// internal callback code device still connected
#define DEVICE_STILL_CONNECTED 23

/// internal callback code device has disconnected
#define DEVICE_HAS_DISCONNECTED 24

/// internal callback code no device connected
#define NO_CONNECTED_DEVICE 25

/**
 * @brief Class which hadles the connection collbacks and therefore the connection state
 * 
 */
class ConnectionHandler: public BLEServerCallbacks {
private:
    /// is there a device connected
    bool deviceConnected;

    /// has some connection changed
    bool connectionChanged;
public:
    /**
     * @brief on connect callback
     * 
     * @param pServer ble server
     */
    void onConnect(BLEServer* pServer);

    /**
     * @brief on disconnect callback
     * 
     * @param pServer ble server
     */
    void onDisconnect(BLEServer* pServer);
    
    /**
     * @brief returns the current connection status
     * 
     * @return int status code see defines
     */
    int getConnectionStatus();
};

/**
 * @brief Class which handles all the ble stuff 
 * 
 */
class BleSensor {
private:
    /// The ble server
    BLEServer* bleServer;

    /// the ble hr characteristic
    BLECharacteristic* hrCharacteristic;
    
    /// the ble hr service
    BLEService* hrService;

    /// the respiration ble characteristic
    BLECharacteristic* respirationCharacteristic;
    
    /// the respiration ble service
    BLEService* respirationService;

    /// the connection handler
    ConnectionHandler* connectionHandler;
public:
    /**
     * @brief Construct a new Ble Sensor object
     * 
     */
    BleSensor();

    /**
     * @brief The new hr value is set and the ble client gets notified
     * 
     * @param val value to set
     * @return int 0 if there is a device connected else ERROR_DEVICE_NOT_CONNECTED
     */
    int setHRValue(uint8_t val);

    /**
     * @brief The new respiration values are set and the ble client gets notified
     * 
     * @param xAccel acceleration x 
     * @param yAccel acceleration y
     * @param zAccel acceleration z
     * @return int 0 if there is a device connected else ERROR_DEVICE_NOT_CONNECTED
     */
    int setRespirationValue(int16_t xAccel, int16_t yAccel, int16_t zAccel );

};