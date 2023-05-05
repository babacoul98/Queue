/*
Impresion en LCD de datos analogicos convertidos digitalmente.
================================================================
Autores: Garantigui Coulibaly, Alejandro Martin y Asier Septien
Fecha: 29-03-2023
*/

#include "p24HJ256GP610A.h"

#include "commons.h"
#include "GPIO.h"
#include "CN.h"
#include "oscilator.h"
#include "timers.h"
#include "memoria.h"
#include "LCD.h"
#include "UART2_RS232.h"
#include "ADC1.h"
#include "PWM.h"
//#include "OCPWM.h"


int main()
{
    ///////////////////
    // Inicializadores 
    ///////////////////
	inic_oscilator();	    // Oscilador a 80MHz. Reloj a 40MHz
    inic_leds();            // Luces LED
    inic_pulsadores();      // Botones
    TRISAbits.TRISA7 = 1;   // Pulsador S5 (I) / LED D10 (O). Config como I (pulsador)
    inic_LCD();             // Pantalla LCD
    inic_UART2();           // UART (transmisor y receptor)
    inic_PWM();             // Pulso PWM para servomotor
    
    inic_Timer2_PWM();      // Timer pulso PWM
    inic_Timer5();          // Timer actualizador de LCD
	inic_Timer9();	        // Timer contador de delays
    


    /////////////
    // Programa
    /////////////
    
    // Primera pantalla
    copiar_FLASH_RAM(Mens_LCD_3, 0); /* "---- ADC_v2 ----" */
    copiar_FLASH_RAM(Mens_LCD_2, 1); /* "--- Pulsa S3  --" */
    while(PORTDbits.RD6); // wait for S3

    // Copiar mensajes iniciales a pantalla LCD
    copiar_FLASH_RAM(Mens_LCD_7, 0); /* "P: 0000  T: 0000" */
    copiar_FLASH_RAM(Mens_LCD_6, 1); /* "Crono: 00:00,0  " */

    // Inicializaciones de comienzo de programa
    inic_CN();           // Interrupciones de CN (pulsadores)
    inic_crono();        // Variables de cronometro
    inic_Timer7();       // Timer cronometro

    // Comienzo de muestreo
    inic_ADC1();            // Convertidor Analogico/Digital
    comienzo_muestreo(); // Inicializacion de muestreo (por interrupcion)
    inic_Timer3();       // Timer siguiente muestreo /*> Descomentar para ADCv2_10 */


    while(1) {
                
        // Evaluar tiempo transcurrido + Actualizar crono en LCD
        cronometro();           

        /* ---- FLAGS ----- */

        // UART transmite comando de reinicio
        if (flag_resetcrono)
            reset_crono();
        
        // Se pulsa S6 para cambiar de datos a mostrar en LCD
        if (flag_CN)
            cambiar_pagina();

        // Actualizar muestras nuevas de los sensores analogicos (LCD paginas 1-2)
        if (flag_ADC){
            update_ADC();
            update_duty(); // por potenciometro
            reiniciar_muestreo();
        }
        
        // Actualizar los duty cycles del servomotor (LCD pagina 3)
        if (flag_PWM > 0){ 
            update_duty(); // por UART 
        }

        
    }

	return (0);
}
	





