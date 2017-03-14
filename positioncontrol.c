#include "positioncontrol.h"
#include "utility.h"
#include "isense.h"
#include <stdio.h>

static volatile double Pos_P_Control;
static volatile double Pos_I_Control;
static volatile double Pos_D_Control;

static volatile double Angle; //In encoder ticks
static volatile int EPosINT = 0;
static volatile double EPrev = 0;
static volatile double POSControl = 0;

void __ISR(_TIMER_4_VECTOR, IPL6SOFT) PositionControl(void)
{
    if(getMode() == 3)
    {
        char message[10];
        int error = Angle - encoder_ticks(1);
        EPosINT = EPosINT + error;
        double Pcontrol = Pos_P_Control * error + Pos_I_Control * EPosINT + Pos_D_Control * ((error - EPrev)/.005);
        // sprintf(message, "position %f\r\n", Pcontrol);
        // NU32_WriteUART3(message);
        EPrev = error;
        POSControl = Pcontrol;
    }
    else if(getMode() == 0)
    {
        EPrev = 0;
        EPosINT = 0;
        POSControl = 0;
    }
    IFS0bits.T4IF = 0;
}

void init_PControl()
{
    __builtin_disable_interrupts();
    //Setup Timer4 for 200Hz ISR
    PR4 = 6250;
    TMR4 = 0;
    T4CONbits.TCKPS = 6;

    //Setting Priorities for 200 Hz ISR
    IPC4bits.T4IP = 6;
    IPC4bits.T4IS = 0;
    IFS0bits.T4IF = 0;
    IEC0bits.T4IE = 1;

    T4CONbits.ON = 1;
    __builtin_enable_interrupts();

}

void setAngle(double newAngle)
{
    Angle = newAngle;
}

void setPosP(double KP)
{
    Pos_P_Control = KP;
}

double getPosP()
{
    return Pos_P_Control;
}

void setPosI(double KI)
{
    Pos_I_Control = KI;
}

double getPosI()
{
    return Pos_I_Control;
}

void setPosD(double KD)
{
    Pos_D_Control = KD;
}

double getPosD()
{
    return Pos_D_Control;
}

double getPosControl()
{
    return POSControl;
}
