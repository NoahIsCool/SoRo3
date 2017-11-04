#include "mbed.h"


PwmOut pwm(p22);
AnalogIn actIn(p15);
Serial pc(USBTX, USBRX);

float averageVoltage(void)
{
    float sum;
    sum = 0;
    for(int i = 0; i <100; i++) {
        sum = sum + actIn;
    }
    return (sum/100)*3.3;
}


int main()
{
    pwm.period(1./1000);

    pc.baud(9600);
    pc.format(8,Serial::None,1);
    int i = actIn;

    while(1) {

        float v = averageVoltage();
        //pc.printf("v = %d\n\r", v);
        pc.printf("v = %f\n\r", v);
        wait(0.1);

        if(v >= 2.8) {
            pwm = 0.9;
        }
        if(v <= 0.5) {
            pwm = 0.1;
        }

    }

}

