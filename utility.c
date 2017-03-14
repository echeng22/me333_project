#include "utility.h"
#include "NU32.h"
#include <stdio.h>

mode myMode;

void setMode(mode newMode)
{
    myMode = newMode;
}

void printMode()
{
    char buffer[10];
    switch(myMode)
    {
        case 0:
        {
            sprintf(buffer,"IDLE\n\r");
            break;
        }
        case 1:
        {
            sprintf(buffer,"PWM\n\r");
            break;
        }
        case 2:
        {
            sprintf(buffer,"ITEST\n\r");
            break;
        }
        case 3:
        {
            sprintf(buffer,"HOLD\n\r");
            break;
        }
        case 4:
        {
            sprintf(buffer,"TRACK\n\r");
            break;
        }
    }
    NU32_WriteUART3(buffer);
}

int getMode()
{
    return myMode;
}
