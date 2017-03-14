#ifndef CURRENTCONTROL_H_
#define CURRENTCONTROL_H_
#include "NU32.h"

void init_CControl();
void setDutyCycle();
void setDirection(int direction);
int getDirection();
void inputPWM(int newDC);
void setKP(double KP);
double getKP();
void setKI(double KI);
double getKI();
void setKD(double KD);
double getKD();
void sendData();
#endif
