/*
Class for how we are going to control the Linear Actuators. It reads a raw value from the controller and maps it to a percentage of how extended the linear actuator is.
I used pwmout instead of the servo class because I thought it would be easier to use if we are writing a percentage instead of a position.

I am assuming that the actuator controller board has a pin where it writes the current position as a percentage. So we can just read from that I think...
*/

#ifndef LinearActuator_h
#define LinearActuator_h

class LinearActuator{
public:
	LinearActuator(int motorPin,int encoderPin);

	float move(int velocity);
	float getPos();
private:
	PWMOut motor;
	AnalogIn encoder;

	const long controllerMin = -255;
	const long controllerMax = 255;
	//for now, I think we need to send it a percentage in the range of [0,1]. so the max we should be able to move it is -.50 or move in 50% or .50 or move out 50%.
	const float actuatorMin = 0.50;
	const float actuatorMax = -0.50;

	float map(long x);
};

#endif
