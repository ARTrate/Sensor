/**
 * @file OscSensor.cpp
 * @author Peter
 * @brief 
 * @date 2019-01-12
 * 
 */

#include "OscSensor.hpp"
#include "Arduino.h"

OscSensor::OscSensor(const int port, const char* serverIp) : serverIp(serverIp), port(port){
    udp.begin(port);
}

int OscSensor::sendRr(int x, int y, int z){
    OSCMessage msg("/artrate/rr"); 
    udp.beginPacket(serverIp, port);
    msg.add(x);
    msg.add(",");
    msg.add(y);
    msg.add(",");
    msg.add(z);
    msg.add(",");
    msg.send(udp);
    udp.endPacket();
}

int OscSensor::sendHr(uint8_t hr){
    OSCMessage msg("/artrate/bpm"); 
    udp.beginPacket(serverIp, port);
    msg.add((int)hr);
    msg.send(udp);
    udp.endPacket();
}
