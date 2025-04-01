#include <cmath>
#include <Arduino.h>
#include <Wire.h>
#include <cmath>
#include <MPU6050_6Axis_MotionApps20.h>
#include "quat_helper.hpp"

// Thanks to Jeff Rowberg's example code for the MPU6050 DMP6

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2012 Jeff Rowberg

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
===============================================
*/

/**
 * Pin Connections:
 * Arduino -> MPU6050
 * 3.3V    -> VCC
 * GND     -> GND
 * SDA     -> SDA
 * SCL     -> SCL
 * 2       -> INT
*/

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

    uint8_t devStatus = mpu.dmpInitialize();

    // supply your own gyro offsets here, scaled for min sensitivity
    mpu.setXGyroOffset(-300);
    mpu.setYGyroOffset(-40);
    mpu.setZGyroOffset(-230);
    mpu.setZAccelOffset(1788); // 1688 factory default for my test chip

    if(devStatus != 0) return 2;

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
