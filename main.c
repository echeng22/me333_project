#include "NU32.h"          // config bits, constants, funcs for startup and UART
#include "encoder.h"
#include "utility.h"
#include "isense.h"
#include "currentcontrol.h"
#include "positioncontrol.h"
#include <stdio.h>
// include other header files here

#define BUF_SIZE 200

int main()
{
  char buffer[BUF_SIZE];
  NU32_Startup(); // cache on, min flash wait, interrupts on, LED/button init, UART init
  encoder_init();
  init_ADC();
  init_CControl();
  init_PControl();
  setMode(IDLE);
  NU32_LED1 = 1;  // turn off the LEDs
  NU32_LED2 = 1;
  __builtin_disable_interrupts();
  // in future, initialize modules or peripherals here
  __builtin_enable_interrupts();
  while(1)
  {
    NU32_ReadUART3(buffer,BUF_SIZE); // we expect the next character to be a menu command
    NU32_LED2 = 1;                   // clear the error LED
    switch (buffer[0]) {
      case 'a':
      {
        sprintf(buffer,"%d\n\r", get_ADC());
        NU32_WriteUART3(buffer);
        break;
      }
      case 'b':
      {
        sprintf(buffer,"%.2f\n\r", get_Current());
        NU32_WriteUART3(buffer);
        break;
      }
      case 'c':
      {
        sprintf(buffer,"%d\n\r", encoder_ticks(1));
        NU32_WriteUART3(buffer); //send encoder count to client
        break;
      }
      case 'd':
      {
        int counter = encoder_ticks(1);
        double degree = ((counter - 32768)/(448.0*4)) * 360;
        sprintf(buffer,"%.2f\n\r", degree);
        NU32_WriteUART3(buffer);
        break;
      }
      case 'e': //
      {
        encoder_ticks(0);
        break;
      }
      case 'f': //Set DC
      {
        int DC = 0;
        NU32_ReadUART3(buffer,BUF_SIZE);
        sscanf(buffer, "%d", &DC);
        inputPWM(DC);
        setMode(PWM);
        break;
      }
      case 'g':
      {
        double Kp, Ki = 0;
        NU32_ReadUART3(buffer,BUF_SIZE);
        sscanf(buffer, "%f", &Kp);

        NU32_ReadUART3(buffer,BUF_SIZE);
        sscanf(buffer, "%f", &Ki);

        setKP(Kp);
        setKI(Ki);
        break;
      }
      case 'h':
      {
        double Kp = getKP();
        double Ki = getKI();
        sprintf(buffer,"%.2f\r\n", Kp);
        NU32_WriteUART3(buffer);
        sprintf(buffer,"%.2f\r\n", Ki);
        NU32_WriteUART3(buffer);
        break;
      }
      case 'i':
      {
        double Kp, Ki, Kd = 0;
        NU32_ReadUART3(buffer,BUF_SIZE);
        sscanf(buffer, "%f", &Kp);

        NU32_ReadUART3(buffer,BUF_SIZE);
        sscanf(buffer, "%f", &Ki);

        NU32_ReadUART3(buffer,BUF_SIZE);
        sscanf(buffer, "%f", &Kd);

        setPosP(Kp);
        setPosI(Ki);
        setPosD(Kd);
        break;
      }
      case 'j':
      {
        double Kp = getPosP();
        double Ki = getPosI();
        double Kd = getPosD();
        sprintf(buffer,"%.2f\r\n", Kp);
        NU32_WriteUART3(buffer);
        sprintf(buffer,"%.2f\r\n", Ki);
        NU32_WriteUART3(buffer);
        sprintf(buffer,"%.2f\r\n", Kd);
        NU32_WriteUART3(buffer);
        break;
      }
      case 'k':
      {
        setMode(ITEST);
        while(getMode() == ITEST)
        {
          ;
        }
        sendData();
        break;
      }
      case 'l':
      {
        double ang = 0;
        int encoder_ang;
        NU32_ReadUART3(buffer,BUF_SIZE);
        sscanf(buffer, "%f", &ang);
        encoder_ang = (int)(((ang/360) * (448*4)) + 32768);
        setAngle(encoder_ang);
        setMode(HOLD);
        break;
      }
      case 'p': //Power Off Motor, Set PWM to 0
      {
        setMode(IDLE);
        // handle q for quit. Later you may want to return to IDLE mode here.
        break;
      }
      case 'q': //Quit
      {
        setMode(IDLE);
        // handle q for quit. Later you may want to return to IDLE mode here.
        break;
      }
      case 'r': // Prints current mode
      {
        printMode();
        break;
      }
      default:
      {
        NU32_LED2 = 0;  // turn on LED2 to indicate an error
        break;
      }
    }
  }
  return 0;
}
