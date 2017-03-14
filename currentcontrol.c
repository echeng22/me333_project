#include "currentcontrol.h"
#include "utility.h"
#include "positioncontrol.h"
#include "isense.h"
#include <stdio.h>

#define PLOTPTS 100

static int DC_Value;

static volatile double KP_Control;
static volatile double KI_Control;

static int current_counter;
static volatile double REFarray[100];
static volatile double PIarray[100];
static double REFCurrent = 200;

static volatile double EINT = 0;

void __ISR(_TIMER_2_VECTOR, IPL5SOFT) Control(void)
{
    static int plotind = 0;
    char message[10];
    switch(getMode())
    {
        case 0: //IDLE
        {
            inputPWM(0);
            setDutyCycle();
            break;
        }
        case 1: //PWM
        {
            setDutyCycle();
            break;
        }
        case 2: //ITEST
        {
            if(current_counter > 25)
            {
                current_counter = 0;
                REFCurrent = -1*REFCurrent;
            }
            double sense_current = get_Current();
            double error = REFCurrent - sense_current;
            EINT = EINT + error;

            double control = KP_Control * error + KI_Control * EINT;

            REFarray[plotind] = REFCurrent;
            PIarray[plotind] = sense_current;

            inputPWM(control);
            setDutyCycle();
            current_counter++;
            plotind++;
            if(plotind >= PLOTPTS)
            {
                setMode(IDLE);
                plotind = 0;
                EINT = 0;
                current_counter = 0;
                REFCurrent = 200;
            }
            break;
        }
        case 3: //HOLD
        {
            double sense_current = get_Current();
            double error = getPosControl() - sense_current;
            EINT = EINT + error;

            double control = KP_Control * error + KI_Control * EINT;

            inputPWM(control);
            setDutyCycle();
            break;
        }
    }
    IFS0bits.T2IF = 0;
}

void init_CControl()
{
    __builtin_disable_interrupts();
    //Make Pin RD3 a digital output to control direction
    //Make it initially send out a low signal.
    TRISDbits.TRISD3 = 0;
    LATDbits.LATD3 = 0;

    //Setup Timer2 for 5KHz ISR
    PR2 = 16000;
    TMR2 = 0;
    T2CONbits.TCKPS = 0;

    //Setup Timer3 for 20KHz PWM
    PR3 = 4000;
    TMR3 = 0;
    T3CONbits.TCKPS = 0;

    //Setup PWM
    OC1CONbits.OCTSEL = 1; //Use Timer 3
    OC1CONbits.OCM = 6;
    OC1RS = 2000;
    OC1R = 0;

    //Setting Priorities for 5 kHz ISR
    IPC2bits.T2IP = 5;
    IPC2bits.T2IS = 0;
    IFS0bits.T2IF = 0;
    IEC0bits.T2IE = 1;

    //Turning on Timers, OC, and Digital Output
    T2CONbits.ON = 1;
    T3CONbits.ON = 1;
    OC1CONbits.ON = 1;
    DC_Value = 0;
    KP_Control = 0;
    KI_Control = 0;
    __builtin_enable_interrupts();
}



void setDutyCycle()
{
    int tempDC = DC_Value;
    if(DC_Value < 0)
    {
        setDirection(0);
        tempDC = tempDC * - 1;
    }
    else
    {
        setDirection(1);
    }

    // if(DC_Value > 100)
    // {
    //     DC_Value = 100;
    // }
    OC1RS = (unsigned int)(4000 * tempDC/100);
}

void setDirection(int direction)
{
    if(direction)
    {
        LATDbits.LATD3 = 0;
    }
    else
    {
        LATDbits.LATD3 = 1;
    }

}

int getDirection()
{
    return LATDbits.LATD3;
}

void inputPWM(int newDC)
{
    DC_Value = newDC;
}

void setKP(double KP)
{
    KP_Control = KP;
}

double getKP()
{
    return KP_Control;
}

void setKI(double KI)
{
    KI_Control = KI;
}

double getKI()
{
    return KI_Control;
}



void sendData()
{
    int i;
    char message[100];
    sprintf(message,"%d\r\n",PLOTPTS);
    NU32_WriteUART3(message);
    for(i=0;i < PLOTPTS; i++)
    {
            sprintf(message, "%f %f\r\n", REFarray[i], PIarray[i]);
            NU32_WriteUART3(message);
    }
}















