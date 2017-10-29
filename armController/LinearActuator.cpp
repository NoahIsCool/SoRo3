#include "LinearActuator.h"

LinearActuator::LinearActuator(int motorPin,int encoderHighPin,int encoderLowPin){
	motor = PWMOut(motorPin);
	encoder = analogIn(encoderPin);
	currentPos = getPos();
	motor.period(1/2000000);	//standard frequency for motors is 2MHZ
}

/*
takes the raw value from the controller and maps it to a percentage of how far it should move.
so pushing the joystick all the way forward will cause it to move the most and pushing it a
quarter of the way will cause it to move only a quarter of the max distance.
*/
float LinearActuator::move(int controllerValue){
	motor = getPos() + map(controllerValue);

	return getPos();
}

/*
unless I'm misunderstanding how the controller board works, we should recieve a percentage between [0,1] that describes how far out it is extended.
*/
float LinearActuator::getPos(){
	return encoder.read();
}

float LinearActuator::map(long x){
	return (x - controllerMin) * (actuatorMax - actuatorMin) / (controllerMax - controllerMin) + actuatorMin;
}
