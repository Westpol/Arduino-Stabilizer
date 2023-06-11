/*
* Brian R Taylor
* brian.taylor@bolderflight.com
* 
* Copyright (c) 2022 Bolder Flight Systems Inc
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the “Software”), to
* deal in the Software without restriction, including without limitation the
* rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
* sell copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
* IN THE SOFTWARE.
*/

#include "sbus.h"   //Serial Rx

#include <Servo.h>    //Servo Outputs

#include <Adafruit_MPU6050.h>   //Gyro Inputs
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;   //setting up Gyro thing

bfs::SbusRx sbus_rx(&Serial1);    //setting up Sbus Rx and Sbus Object
bfs::SbusData data;


Servo elevator;    //Setting up all servos
Servo aileron_left;
Servo aileron_right;
Servo rudder;
Servo throttle;

void setup() {
  Serial.begin(115200);

  sbus_rx.Begin();

  throttle.attach(2);
  elevator.attach(3);
  rudder.attach(4);
  aileron_left.attach(5);
  aileron_right.attach(6);

  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_1000_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_260_HZ);
}

void loop () {

  update_gyro();

  if (sbus_rx.Read()) {

    data = sbus_rx.data();

    throttle.writeMicroseconds(map(data.ch[0],173,1810,1000,2000));
    elevator.writeMicroseconds(map(data.ch[1], 173, 1810, 1000, 2000));
    rudder.writeMicroseconds(map(data.ch[2], 173, 1810, 1000, 2000));
    aileron_left.writeMicroseconds(map(data.ch[3], 173, 1810, 1000, 2000));
    aileron_right.writeMicroseconds(map(data.ch[4], 173, 1810, 1000, 2000));

  }
  check_failsafe();
}


void check_failsafe(){
  if(data.failsafe == true){
    throttle.writeMicroseconds(map(data.ch[0],173,1810,1000,2000));
    elevator.writeMicroseconds(map(data.ch[1], 173, 1810, 1000, 2000));
    rudder.writeMicroseconds(map(data.ch[2], 173, 1810, 1000, 2000));
    aileron_left.writeMicroseconds(map(data.ch[3], 173, 1810, 1000, 2000));
    aileron_right.writeMicroseconds(map(data.ch[4], 173, 1810, 1000, 2000));
  }
}

void update_gyro(){
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
}