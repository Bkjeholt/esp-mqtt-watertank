/*************************************************************************************************
 *
 *************************************************************************************************/
 
#include <Arduino.h>
#include "WaterTank.h"

float WaterTank::measureDistance() {
	int airTime;
	
	delay(10);
	digitalWrite(pinTrig, LOW);
	delayMicroseconds(2);
	digitalWrite(pinTrig, HIGH);
	delayMicroseconds(10);
	digitalWrite(pinTrig, LOW);
	airTime = pulseIn(pinEcho, HIGH, WATERTANK_TIMEOUT)/2;
	
	return(airTime/291.0);
}

float WaterTank::getTankVolume() {
	float tankVolume = tankArea*(tankHeight-measureDistance());

  if (alarmLowVolume.active && (tankVolume > (alarmLowVolume.trigLevel + WATERTANK_HYSTERESIS_LOW_VOLUME_ALARM))) {
    //
    // Cease alarm
    //
    alarmLowVolume.active = false;
    alarmLowVolume.callback(false, tankVolume);
  } else if (!alarmLowVolume.active && (tankVolume < alarmLowVolume.trigLevel)) {
    //
    // Raise alarm
    //
    alarmLowVolume.active = true;
    alarmLowVolume.callback(true, tankVolume);
  }
	return(tankVolume);
}

// void WaterTank::WaterTank(uint8_t trigPin, uint8_t echoPin, float area, float height) {
//   WaterTank(trigPin, echoPin, area, height, null);
// }

WaterTank::WaterTank(uint8_t trigPin, uint8_t echoPin, float area, float height, float lowVolumeLevel, WATERTANK_ALARM_CALLBACK_SIGNATURE) {
  pinTrig = trigPin;
  pinEcho = echoPin;
    
  pinMode(pinTrig,OUTPUT);
  pinMode(pinEcho,INPUT);
  
  tankArea = area;
  tankHeight = height;

  alarmLowVolume.active = false;
  alarmLowVolume.trigLevel = lowVolumeLevel;
  alarmLowVolume.callback = lowVolumeAlarmCallback;
}
 
