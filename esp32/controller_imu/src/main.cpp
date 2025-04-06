#include <cmath>
#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <cmath>
#include <MPU6050_6Axis_MotionApps20.h>
#include "quat_helper.hpp"

constexpr float MOVEMENT_GAIN = 0.4f;
constexpr unsigned long CLIENT_UPDATE_MS = 50;
constexpr std::size_t NCLIENTS = 5;

const char* const WIFI_SSID = "koala";
const char* const WIFI_PWRD = "esp32esp32";
const IPAddress IP_LOCAL(192, 168, 1, 100);
const IPAddress IP_GATEWAY(192, 168, 12, 1);
const IPAddress SUBNET_MASK(255, 255, 255, 0);
constexpr int PUBLISH_PORT = 1000;

MPU6050 mpu;
WiFiServer server(PUBLISH_PORT);
uint8_t fifoBuffer[64];
WiFiClient client[NCLIENTS];
unsigned long client_updated_last = 0;

Quaternion qGet, qSend;

void waitTillProceedSignal(){
    while(Serial.available() && Serial.read());
    int proceed = 0;
    do{
        while(!Serial.available());
        proceed = Serial.read();
    }while(!proceed);
}

int setup_mpu6050(){

    mpu.initialize();
    if(!mpu.testConnection()) return 1;
    if(mpu.dmpInitialize()) return 2;

    mpu.setXGyroOffset(-300);
    mpu.setYGyroOffset(-40);
    mpu.setZGyroOffset(-230);
    mpu.setZAccelOffset(1688);

    mpu.CalibrateAccel(6);
    mpu.CalibrateGyro(6);

    mpu.setDMPEnabled(true);

    return 0;
}

void setup(){


    Serial.begin(115200, SERIAL_8E1);

    WiFi.config(IP_LOCAL, IP_GATEWAY, SUBNET_MASK);
    WiFi.begin(WIFI_SSID, WIFI_PWRD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
    Serial.print("local IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("gateway: ");
    Serial.println(WiFi.gatewayIP());
    Serial.print("subnet: ");
    Serial.println(WiFi.subnetMask());
    server.begin();
    Serial.println("Server started");

    Wire.begin();
    Wire.setClock(400000);

    int err = setup_mpu6050();
    if(err) Serial.println("error starting mpu6050");

    Serial.write(0xFF);
    Serial.write(err);

}

void loop(){

    for(WiFiClient& c : client){
        if(!c.connected()){
            c = server.available();
        }
    }

    if (!mpu.dmpPacketAvailable()) return;

    if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) {
        if(!mpu.dmpGetQuaternion(&qGet, fifoBuffer)){
            qSend = pow(qGet, MOVEMENT_GAIN);
            // qSend = qGet;
        }
    }

    if(millis() - client_updated_last >= CLIENT_UPDATE_MS){
        client_updated_last = millis();
        for(WiFiClient& c : client){
            if(c.connected()){
                c.write((char*)&qSend, sizeof(qSend));
            }
        }
    }

    if(Serial.available()){
        do Serial.read();
        while(Serial.available());
        Serial.write((char*)&qSend, sizeof(qSend));
    }
    
}
