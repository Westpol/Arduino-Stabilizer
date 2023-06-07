#include <Servo.h>

// Servo Setup, change output pinsto your liking
#define servo1pin 3
#define servo2pin 5
#define servo3pin 6
#define servo4pin 9
#define servo5pin 10

int aux1 = 0;
int aux2 = 0;
int aux3 = 0;
int aux4 = 0;
int aux5 = 0;

Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;
Servo servo5;
// End of Servo Setup

void setup() {
  servo1.attach(2);
  servo2.attach(3);
  servo3.attach(4);
  servo4.attach(5);
  servo5.attach(6);
}

void loop() {

}


void get_aux(){
  
}


void error_fallback(){
  while(true){
    servo1.writeMicroseconds(aux1);
    servo2.writeMicroseconds(aux2);
    servo3.writeMicroseconds(aux3);
    servo4.writeMicroseconds(aux4);
    servo5.writeMicroseconds(aux5);
  }
}
