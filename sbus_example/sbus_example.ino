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

bfs::SbusRx sbus_rx(&Serial);    //setting up Sbus Rx and Sbus Object (&Serial1 for MEGA, &Serial for UNO)
bfs::SbusData data;


Servo elevator;    //Setting up all servos
Servo aileron_left;
Servo aileron_right;
Servo rudder;
Servo throttle;

int r_p = 0;
int r_d = 0;
int r_i = 0;

int a_p = 0;
int a_d = 0;
int a_i = 0;

int e_p = 0;
int e_d = 0;
int e_i = 0;

float pi = 3.14159265358793;

float xDelta = 0;
float yDelta = 0;
float zDelta = 0;

void setup() {
  delay(5000);

  sbus_rx.Begin();

  throttle.attach(2);
  elevator.attach(3);
  rudder.attach(4);
  aileron_left.attach(5);
  aileron_right.attach(6);

  if (!mpu.begin()) {
    while (1) {
      delay(10);
    }
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_1000_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_260_HZ);

  gyro_calibration();
}

void loop() {
  pid_loop();
  if (sbus_rx.Read()) {

    data = sbus_rx.data();

  }
  check_failsafe();

  throttle.writeMicroseconds(map(data.ch[0],173,1810,1000,2000));
  elevator.writeMicroseconds(map(data.ch[1], 173, 1810, 1000, 2000));
  rudder.writeMicroseconds(map(data.ch[2], 173, 1810, 1000, 2000));
  aileron_left.writeMicroseconds(map(data.ch[3], 173, 1810, 1000, 2000));
  aileron_right.writeMicroseconds(map(data.ch[4], 173, 1810, 1000, 2000));
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

void update_gyro(float* x, float* y, float* z){
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float zGyro = g.gyro.z - zDelta;
  float yGyro = g.gyro.y - yDelta;
  float xGyro = g.gyro.x - xDelta;

  *x += xGyro;
  *y += yGyro;
  *z += zGyro;
}

void gyro_calibration(){
  elevator.writeMicroseconds(1900);
  delay(1000);
  unsigned long mill = millis() + 2000;
  unsigned long iterations = 0;
  float x, y, z;
  while(mill > millis()){
    update_gyro(&x, &y, &z);
    iterations++;
  }

  xDelta = x / float(iterations);
  yDelta = y / float(iterations);
  zDelta = z / float(iterations);
  elevator.writeMicroseconds(1500);
  delay(500);

  if(xDelta > 0.1 || yDelta > 0.1 || zDelta > 0.1){
    while(1){}
  }
}

void pid_loop(){
  
}

int maxmin(int lval, int lmin, int lmax){
  if(lval < lmin){
    return lmin;
  }
  if(lval > lmax){
    return lmax;
  }
  return lval;
}