#include "isense.h"
#include "currentcontrol.h"
#include "NU32.h"

void init_ADC()
{
    __builtin_disable_interrupts();
    AD1PCFGbits.PCFG0 = 0;
    AD1CON1bits.SSRC = 7;
    AD1CON1bits.ASAM = 0;
    AD1CHSbits.CH0SA = 0;
    AD1CON3bits.ADCS = 2;
    __builtin_enable_interrupts();
    AD1CON1bits.ON = 1;
}

int get_ADC()
{
    unsigned int elapsed, finish_time;
    unsigned int sum;
    int i;
    for(i = 0; i < 3; i++)
    {
        elapsed = 0;
        finish_time = 0;
        AD1CON1bits.SAMP = 1;
        elapsed = _CP0_GET_COUNT();
        finish_time = elapsed + 6;
        while(_CP0_GET_COUNT() < finish_time)
        {
            ;
        }
        while(!AD1CON1bits.DONE)
        {
            ;
        }
        AD1CON1bits.SAMP = 0;
        sum += ADC1BUF0;
    }
    return sum/3;

}

double get_Current()
{
    double ADC = get_ADC();
    if(ADC > 503)
    {
        return (1.942*ADC) - 974.081;
    }
    else
    {
        return -((-1.998*ADC) + 1001.68);
    }
}




