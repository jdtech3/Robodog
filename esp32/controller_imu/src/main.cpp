#include <cmath>
#include <Arduino.h>
#include <Wire.h>
#include <cmath>
#include <MPU6050_6Axis_MotionApps20.h>
#include "quat_helper.hpp"

constexpr float BODY_X =  2.f;
constexpr float BODY_Y =  3.f;
constexpr float BODY_Z =  0.5f;
constexpr float LEG_L1 =  1.f;
constexpr float LEG_L2 =  1.f;

constexpr float MOVEMENT_GAIN = 0.1f;

MPU6050 mpu;
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

    Wire.begin();
    Wire.setClock(400000);

    Serial.begin(115200, SERIAL_8E1);

    int err = setup_mpu6050();
    if(err) Serial.println("error starting mpu6050");

    Serial.write(0xFF);
    Serial.write(err);

}

void loop(){

    if (!mpu.dmpPacketAvailable()) return;

    if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) {
        if(!mpu.dmpGetQuaternion(&qGet, fifoBuffer)){
            qSend = pow(qGet, MOVEMENT_GAIN);
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
