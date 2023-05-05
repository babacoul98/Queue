
#ifndef COMMONS_H
#define	COMMONS_H

#ifdef	__cplusplus
extern "C" {
#endif

#ifdef	__cplusplus
}
#endif

#endif	/* COMMONS_H */

// definiciones generales 

#define Nop()   __builtin_nop()

#define _ISR_NO_PSV __attribute__((interrupt, no_auto_psv))

//////////////////////////////////////////
// P3: Definiciones para la pantalla LCD
//////////////////////////////////////////

/* 
   For Explorer 16 board, here are the data and control signal definitions
   RS -> RB15
   E  -> RD4
   RW -> RD5
   DATA -> RE0 - RE7   
*/

// Control signal data pins 
#define  RW  LATDbits.LATD5       // LCD R/W signal
#define  RS  LATBbits.LATB15      // LCD RS signal
#define  E   LATDbits.LATD4       // LCD E signal 

// Control signal pin direction 
#define  RW_TRIS	TRISDbits.TRISD5 
#define  RS_TRIS	TRISBbits.TRISB15
#define  E_TRIS	TRISDbits.TRISD4

// Data signals and pin direction
#define  DATA      LATE           // Port for LCD data
#define  DATAPORT  PORTE
#define  TRISDATA  TRISE          // I/O setup for data Port

///////////////////////////////////////////
// P3: Definiciones posiciones cronometro
///////////////////////////////////////////

#define posds 13
#define posseg 10
#define posmin 7

////////////////////////////////////////////////////////////////////
// P4: Definiciones ligadas a la velocidad de transmision de UART2 
////////////////////////////////////////////////////////////////////

#define Fosc 80000000                   // Frecuencia de reloj de la CPU (oscillator)
#define Fcy	 Fosc/2                     // Velocidad de ejecucion de las instrucciones
#define BAUDRATE2 9600                  // Velocidad de transmision de UART2 (baudios)
#define T_1BIT_US (1000000/BAUDRATE2)+1 // Duracion de 1 bit 

#define BAUD_RATEREG_2_BRGH1 ((Fcy/BAUDRATE2)/4)-1	// valor U2BRG si BRGH=1
#define BAUD_RATEREG_2_BRGH0 ((Fcy/BAUDRATE2)/16)-1	// valor U2BRG si BRGH=0

// Definiciones relacionadas con la pantalla del terminal del PC

#define clrscr "\x1b[2J" //4 character: \x1b, [, 2, J
#define home "\x1b[H"    //3 character: \x1b, [, H

#define CR 0x0D		// carriage return
#define LF 0x0A		// line feed

// Estados UART Receiver
#define STAT_HOME 0
#define STAT_LINE 1
#define STAT_NL   2
#define STAT_CLEAR 3


//////////////////////////
// P5: Definiciones ADC1: 
//////////////////////////

#define pos_izq0 6
#define pos_izq1 5
#define pos_izq2 4
#define pos_izq3 3

#define pos_der0 15
#define pos_der1 14
#define pos_der2 13
#define pos_der3 12

#define AN_PALANCA 0 // RB0
#define AN_POT    5 // RB4
#define AN_TEMP   4 // RB5
#define AN_JSTK_X 8 // RB8
#define AN_JSTK_Y 9 // RB9



#define METRICAS     4
#define NMUESTREOS   8
#define NPAGINAS_LCD 3

// Indices ascii
#define iPOT    0
#define iTEMP   1
#define iJSTK_X 2
#define iJSTK_Y 3
#define iPAL    4
//#define iDUTY   4

///////////////////////////////////////////////////////
// P6: Definiciones relacionadas con los servos y PWM
///////////////////////////////////////////////////////

#define PR20ms 12500    	// Periodo adecuado para conseguir 20 ms = 800.000 ciclos
                           // teniendo en cuenta un prescaler de 1:64.
                           // Fosc= 80MHz; Fcy= 40MHz; Tcy= 25ns
#define MINPWM 0.5         // Duracion minima pulso PWM (en milisegundos)
#define MAXPWM 2.1         // Duracion maxima pulso PWM (en milisegundos)

#define FLAGOC_SUMA  1
#define FLAGOC_RESTA 2
