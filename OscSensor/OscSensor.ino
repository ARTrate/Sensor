/**
 * @brief Arduino test/sample program
 * 
 */

#include <WiFi.h>
#include "OscSensor.hpp"

/*---------------------------------------------------------------------------*/
// config --> you should do that
const char* ssid = "Artrate";
const char* pwd = "artratewifi";
const int port = 5005;
const char* serverIp = "192.168.43.39";
/*---------------------------------------------------------------------------*/

static OscSensor* oscSensor;

void setup(){
    // setup wifi!
    WiFi.begin(ssid, pwd);
    while(WiFi.status() != WL_CONNECTED){
        delay(100);
        // without wifi nothing todo... 
    }
    oscSensor = new OscSensor(port, serverIp);
}

void loop(){
    delay(500);
    oscSensor->sendHr(22);
    oscSensor->sendRr(-2321,3422,35254);
}