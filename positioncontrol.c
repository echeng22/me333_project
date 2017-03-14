#include "positioncontrol.h"
#include "utility.h"
#include "isense.h"
#include "NU32.h"
#include <stdio.h>

static volatile double Pos_P_Control;
static volatile double Pos_I_Control;
static volatile double Pos_D_Control;

static volatile double Angle; //In encoder ticks
static volatile int EPosINT = 0;
static volatile double EPrev = 0;
static volatile double POSControl = 0;

static int *Traj;
static double *TrajDegrees;
static volatile int numSamples = 0;

void __ISR(_TIMER_4_VECTOR, IPL6SOFT) PositionControl(void)
{
    static int counter = 0;
    switch(getMode())
    {
        case 0:
        {
            EPrev = 0;
            EPosINT = 0;
            POSControl = 0;
            counter = 0;
        }
        case 3:
        {
            int error = Angle - encoder_ticks(1);
            EPosINT = EPosINT + error;
            double Pcontrol = Pos_P_Control * error + Pos_I_Control * EPosINT + Pos_D_Control * ((error - EPrev)/.005);
            EPrev = error;
            POSControl = Pcontrol;
        }
        case 4:
        {
            int trajPoint = Traj[counter];
            int error = trajPoint - encoder_ticks(1);
            TrajDegrees[counter] = getDegree();
            EPosINT = EPosINT + error;
            double Pcontrol = Pos_P_Control * error + Pos_I_Control * EPosINT + Pos_D_Control * ((error - EPrev)/.005);
            EPrev = error;
            POSControl = Pcontrol;
            counter++;
            if(counter > numSamples)
            {
                Angle = Traj[counter - 1];
                counter = 0;
                setMode(HOLD);
            }
        }
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

void loadTraj()
{
    int i, point = 0;
    char buffer[10];
    NU32_ReadUART3(buffer, 10);
    sscanf(buffer,"%d",&numSamples);
    Traj = (int *) malloc(numSamples * sizeof(int));
    TrajDegrees = (double *) malloc(numSamples * sizeof(double));
    for(i=0; i < numSamples; i++)
    {
        NU32_ReadUART3(buffer, 10);
        sscanf(buffer,"%d",&point);
        Traj[i] = point;
    }

}

void sendTrajDegree()
{
    int i;
    char message[100];
    sprintf(message,"%d\r\n",numSamples);
    NU32_WriteUART3(message);
    for(i=0;i < numSamples; i++)
    {
        sprintf(message, "%f\r\n", TrajDegrees[i]);
        NU32_WriteUART3(message);
    }
}
