#ifndef POSITIONCONTROL_H_
#define POSITIONCONTROL_H_
#include "NU32.h"

void init_PControl();
double getPosControl();
void setAngle(double newAngle);
void setPosP(double KP);
double getPosP();
void setPosI(double KI);
double getPosI();
void setPosD(double KD);
double getPosD();

#endif
