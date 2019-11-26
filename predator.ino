#include "gbuffer.h"
#include "gbufferconnector.h"
#include "uart.h"
#include <Zumo32U4.h>

Zumo32U4Motors motors;
Zumo32U4Buzzer buzzer;
Zumo32U4ProximitySensors proxSensors;

extern GBufferConnector conn;

uint16_t brightnessLevels[] = {1,2,4,8,16,32,54};

void setup() {
  Uart_begin (57600);
  Serial.begin(115200);
  while (!Serial) {;}
  // put your setup code here, to run once:
  Serial.println("started v0");  
  buzzer.playFrequency(440,200,15);
  proxSensors.initFrontSensor();
  proxSensors.setBrightnessLevels(brightnessLevels,7);
  delay(1000);
  buzzer.playFrequency(440,200,15);
}

char b[256];

struct data {
    int32_t axis[2];
};

void loop() {
  data new_data;
  if (conn.read_buffer(&new_data,sizeof(data))){
    int l = -new_data.axis[0]/200 +new_data.axis[1]/400;
    int r = -new_data.axis[0]/200 -new_data.axis[1]/400;
    motors.setSpeeds(l,r);
    Serial.print(l);
    Serial.print(",");
    Serial.println(r);
    
  }
  // put your main code here, to run repeatedly:

}
