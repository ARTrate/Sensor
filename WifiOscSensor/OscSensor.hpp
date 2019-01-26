/**
 * @file OscSensor.hpp
 * @author Peter
 * @brief 
 * @date 2019-01-12
 * 
 */

#include <WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>


class OscSensor
{
private:
    WiFiUDP udp;
    const char* serverIp;
    const int port; 
    IPAddress sensorIp; 
public:
    OscSensor(const int port, const char* serverIp);
    int sendRr(int x, int y, int z);
    int sendHr(uint16_t hr);
    int sendEda(uint16_t eda);
};

