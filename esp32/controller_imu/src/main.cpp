#include <cmath>
#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <cmath>
#include <MPU6050_6Axis_MotionApps20.h>
#include "quat_helper.hpp"

constexpr float BODY_X =  2.f;
constexpr float BODY_Y =  3.f;
constexpr float BODY_Z =  0.5f;
constexpr float LEG_L1 =  1.f;
constexpr float LEG_L2 =  1.f;

constexpr float MOVEMENT_GAIN = 0.1f;

const char* const WIFI_SSID = "koala";
const char* const WIFI_PWRD = "esp32esp32";
const IPAddress IP_LOCAL(192, 168, 1, 100);
const IPAddress IP_GATEWAY(192, 168, 12, 1);
const IPAddress SUBNET_MASK(255, 255, 255, 0);
constexpr int PUBLISH_PORT = 1000;

MPU6050 mpu;
WiFiServer server(PUBLISH_PORT);
uint8_t fifoBuffer[64];

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

WiFiClient client[5];

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
        }
    }

    for(WiFiClient& c : client){
        if(c.connected()){
            float z = (LEG_L1+LEG_L2)/2.f;
            c.write((char*)&qSend, sizeof(qSend));
            c.write((char*)&z, sizeof(z));
        }
    }

    if(Serial.available()){
        do Serial.read();
        while(Serial.available());
        float z = (LEG_L1+LEG_L2)/2.f;
        Serial.write((char*)&qSend, sizeof(qSend));
        Serial.write((char*)&z, sizeof(z));
    }
    
}
