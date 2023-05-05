/* Funciones para los temporizadores
======================================
Autores: Garantigui Coulibaly, Alejandro Martin y Asier Septien
Fecha: 29-03-2023
*/

#include "p24HJ256GP610A.h"

#include "commons.h"
#include "LCD.h"
#include "memoria.h"
#include "utilidades.h"
#include "ADC1.h"
#include "PWM.h"

unsigned char estado_lcd = 0;
unsigned int mili, deci, seg, min;

///////////////////
// Inicializadores 
///////////////////

/* void inic_Timer2_PWM() {  // Inicializa T2

    T2CON = 0; // TON=0,TCS=TGATE=0,TCKPS=0 (prescaler 1:1)
    TMR2 = 0; // Inicializa contador T2
    PR2 = PR20ms - 1; // Carga valor del periodo a 800k ciclos (12500 * 64)
    T2CONbits.TCKPS = 2; // prescaler 1:64

    IFS0bits.T2IF = 0; // Borra flag interrupción
    IEC0bits.T2IE = 1; // Habilita Interrupción Timer 2
    // Gestionaremos los pulsos PWM por interrupcion, en vez de por un modulo OC.

    T2CONbits.TON = 1; // Activa Timer
}
 */

void inic_Timer3(){
    TMR3 = 0;
    PR3 = 40000 - 1; // 40.000 ciclos (1,000,000 / 25)
    T3CONbits.TCKPS = 0;
    T3CONbits.TCS = 0;	// reloj interno
    T3CONbits.TGATE = 0;	// Deshabilitar el modo Gate
    T3CONbits.TON = 1;	// puesta en marcha del temporizador
    IEC0bits.T3IE = 0; // deshabilitacion de la interrupcion general de T3
    IFS0bits.T3IF = 0; // Puesta a 0 del flag IF de T3
}


/* void inic_Timer5(){
    TMR5 = 0;
    PR5 =  40000 - 1;	// Periodo del temporizador, 40000 ciclos / prescales, menos 1 porque empieza en 0
		// Queremos que cuente 1 ms.
		// Fosc= 80 MHz (vease Inic_oscilator()) de modo que
		// Fcy = 40 MHz (cada instruccion dos ciclos de reloj)
		// Por tanto, Tcy= 25 ns para ejecutar una instruccion
		// Para contar 1 ms se necesitan 40000 ciclos.
    
        /* 3 + 16 + 2 + 16 = 37 caracteres
         quiero hacer un refresco cada decima de segundo (un caracter por cada decima)
         10 bits por caracter -> 370 bits
         370 bits / 0.1 s = 3700 bits/s minimo */
    T5CONbits.TCKPS = 0;	// escala del prescaler 1
    T5CONbits.TCS = 0;	// reloj interno
    T5CONbits.TGATE = 0;	// Deshabilitar el modo Gate
    T5CONbits.TON = 1;	// puesta en marcha del temporizador
    IEC1bits.T5IE = 1;      // habilitacion de la interrupcion de T5
	IFS1bits.T5IF = 0;      // Puesta a 0 del flag IF de T5
}


void inic_Timer7 ()
{ // contar 10 ms
    TMR7 = 0;
    PR7 = 50000 - 1; // 400k ciclos / 8 (prescaler 1:8)
    T7CONbits.TCKPS = 1;
    T7CONbits.TCS = 0;	// reloj interno
    T7CONbits.TGATE = 0;	// Deshabilitar el modo Gate
    T7CONbits.TON = 1;	// puesta en marcha del temporizador
    IEC3bits.T7IE = 1; // habilitacion de la interrupcion general de T7
    IFS3bits.T7IF = 0; // Puesta a 0 del flag IF de T7
}


void inic_Timer9 ()
{
    T9CONbits.TCS = 0;	// reloj interno
    T9CONbits.TGATE = 0;	// Deshabilitar el modo Gate
}	


void inic_crono()	
// inicializacion de las variables del cronometro
{
    //Iniciar contadores a 0
    mili = 0;
    deci = 0;
    seg = 0;
    min = 0;
    
    // Reiniciar contador de timer 7
    TMR7 = 0;

}





/////////////////////////////////
// Funciones crono y auxiliares 
/////////////////////////////////

unsigned int calcular_prescaler(unsigned int ciclos){
    /* Esta funcion determina el prescaler a usar, 
    a partir de los ciclos del parametro. */
    unsigned int prescaler;

    if(ciclos > 65535 * 8){
        prescaler = 64;
        T9CONbits.TCKPS = 2;
    } else if(ciclos > 65535){
        prescaler = 8;
        T9CONbits.TCKPS = 1;
    } else {
        prescaler = 1;
        T9CONbits.TCKPS = 0;
    }

    return prescaler;
}


void delay_us(unsigned int time)
{
    // Limite de periodo
    if(time > 100000){
        while(1);
    }
    
    // Inicializar Timer 9
    TMR9 = 0;
    unsigned int ciclos = time * 40; // 1 us = 40 ciclos
    unsigned int prescaler = calcular_prescaler(ciclos); 
    PR9 = ciclos / prescaler - 1;

    // Puesta en marcha del temporizador
    T9CONbits.TON = 1;	
    while (IFS3bits.T9IF == 0); // esperar a que pasen time us
    T9CONbits.TON = 0;          // parar el temporizador

    IFS3bits.T9IF = 0;
}


void delay_ms(unsigned int time)
{
    // Limite de periodo
    if(time > 100) {
         while(1);
     }
        
    // Inicializar Timer 9
    TMR9 = 0;
    unsigned int ciclos = time * 40000; // 1 ms = 40k ciclos
    unsigned int prescaler = calcular_prescaler(ciclos); 
    PR9 = ciclos / prescaler - 1;

    // Puesta en marcha del temporizador
    T9CONbits.TON = 1;	
    while (IFS3bits.T9IF == 0); // esperar a que pasen time ms
    T9CONbits.TON = 0;          // parar el temporizador

    IFS3bits.T9IF = 0;
}


void cronometro()
{
    unsigned char mins[2];
    unsigned char segs[2];
    unsigned char decs[2];

    if(mili == 100){    // cuando el contador de milisegundos llega a 1 decima
        deci++;         // incrementar el contador de decimas en 1
        mili = 0;       // reiniciar contador de milisegundos a 0
        LATAbits.LATA0 = !LATAbits.LATA0; // conmutar led D3

        conversion_tiempo(decs, deci);
        Ventana_LCD[1][13] = decs[1];

        if(deci==10){       // cuando el contador de decimales llega a 10 (1 segundo)
            seg++;          // aumentamos la variable segundo
            deci = 0;       // reiniciar contador de decimales a 0
            LATAbits.LATA2 = !LATAbits.LATA2; //conmutamos el led D5
            
            num_conversiones = 0; ////////////////////////////////////////////////////////

            if(seg==60){        // cuando el contador de segundos llega a 60 (1 minuto)
                min++;          // aumentamos la variable minuto
                seg=0;          // reiniciar contador de segundos a 0
                LATAbits.LATA4 = !LATAbits.LATA4; //Conmutamos el led D7

                conversion_tiempo(mins,min);
                Ventana_LCD[1][7] = mins[0];
                Ventana_LCD[1][8] = mins[1];
            }

            conversion_tiempo(segs,seg);
            Ventana_LCD[1][10] = segs[0];
            Ventana_LCD[1][11] = segs[1]; 
            
        }
            
    }
}





///////////////////////////////////////////////////////////////
// Rutinas de atencion a la interrupcion de los temporizadores 
///////////////////////////////////////////////////////////////

void _ISR_NO_PSV _T2Interrupt(){

    // Reset del timer -> Cambio de periodos (segun DUTY)
    if(PWM_pulse_high) {
        LATBbits.LATB12 = 1; // Subir pulso
        PR2 = duty - 1;      // Durar pulso "duty" ciclos
    } else {
        LATBbits.LATB12 = 0; // Bajar pulso
        PR2 = PR20ms - duty - 1;      // Durar pulso el resto de ciclos hasta 20ms
    }

    // Alternar estado del pulso
    PWM_pulse_high ^= 1;

    // Terminar interrupcion T2
    IFS0bits.T2IF = 0;

}


void _ISR_NO_PSV _T5Interrupt(){
    static unsigned int indice = 0;
    switch (estado_lcd){
    case 0:
        lcd_cmd(0x80);  	// Set DDRAM address (@0)
        indice = 0;
        estado_lcd = 1;
        break;
    case 1:
        
        lcd_data(Ventana_LCD[0][indice]);
        indice ++;
        if (indice == 16){
            estado_lcd = 2;
        }
        
        break;
    case 2:
        lcd_cmd(0xC0);  	// Set DDRAM address (@40)
        indice = 0;
        estado_lcd = 3;
        break;
    case 3:
        
        lcd_data(Ventana_LCD[1][indice]);
        indice ++;
        if (indice == 16){
            estado_lcd = 0;
        }
        break;      
    }
    IFS1bits.T5IF = 0;
}


void _ISR_NO_PSV _T7Interrupt(){
    mili += 10;   //incrementar el contador de milisegundos en 10
    IFS3bits.T7IF = 0;
}
 */




