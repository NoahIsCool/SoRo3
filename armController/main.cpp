/*
Eventually will add more to this file but for now Im trying to keep it pretty bare-bones to 
make sure it is simple and it works. One day, these methods should be imported into the main 
file of last years code so we dont have to rewrite everything.
*/

#include "mbed.h"

PwmOut actuator(p22);
PwmOut stepper(p23);
DigitalOut cw(p24);
DigitalOut ccw(p25);
AnalogIn posIn(p15);

DigitalIn cwButton(p26);
DigitalIn ccwButton(p27);

Serial pc(USBTX, USBRX);

//max and min values the actuator or stepper motor should go to.
//Please note, these are pwm duty cycles. So anything goes from 0 to 1.
const int MAX = 0.9;
const int MIN = 0.1;

//for the actuator and stepper, I thought it would be best to tell it to go to its current
//position when stopping so this will help that.
float map(float x){
	return (x - 3.3) * (MAX - MIN) / (3.3 - 0) + MIN;
}

float averageVoltage(void)
{
    float sum;
    sum = 0;
    for(int i = 0; i <100; i++) {
        sum = sum + posIn;
    }
    return (sum/100)*3.3;
}

/*until someone comes up with a better idea:
1 = clockwise
2 = counter-clockwise
0 = stop
*/
void actMove(int dir){
	if(dir == 1){
		actuator = MAX;
	}else if(dir == 2){
		actuator = MIN;
	}else{
		actuator = averageVoltate(); //need to cast this into a pwm signal
	}
}

/*until someone comes up with a better idea:
1 = clockwise
2 = counter-clockwise
0 = stop
*/
//assuming this is a 2 pin moter
void contMove(int dir){
	if(dir == 1){
		ccw = false;
		cw = true;
	}else if(dir == 2){
		cw = false;
		ccw = true;
	}else{
		cw = false;
		ccw = false;
	}
}

/*until someone comes up with a better idea:
1 = clockwise
2 = counter-clockwise
0 = stop
*/
void stepperMove(int dir){
	if(dir == 1){
		stepper = MAX;
	}else if(dir == 2){
		stepper = MIN;
	}else{
		stepper = averageVoltate(); //need to cast this into a pwm signal
	}
}

void main(int argc, char* argv[]){
	actuator.period(1./1000);
	stepper.period(1./2000000);

	pc.baud(9600);
	pc.format(8,Serial::None,1);

	while(true){
		if(cwButton){
			actMove(1);
		}else if(ccwButton){
			actMove(2);
		}else{
			actMove(0);
		}
	}
}
