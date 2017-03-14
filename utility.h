#ifndef UTILITY_H_
#define UTIILTY_H_

typedef enum {IDLE, PWM, ITEST, HOLD, TRACK} mode;


void setMode(mode newMode);
void printMode();
int getMode();

#endif
