/* Funciones para el modulo ADC1
==================================
Autores: Garantigui Coulibaly, Alejandro Martin y Asier Septien
Fecha: 29-03-2023
*/

#include <p24HJ256GP610A.h>
#include "ADC1.h"
#include "LCD.h"
#include "commons.h"
#include "memoria.h"
#include "utilidades.h"


// Variables globales
long ADC1_muestras[METRICAS][NMUESTREOS];
long ADC1_avg[METRICAS];
int  cont_muestras = 0;

int flag_ADC = 0;
long num_conversiones;



/* /////////////////////////////
   Inicializador del modulo ADC 
   /////////////////////////////
 */
void inic_ADC1 (void) {
    
    // Inicializacion registro control AD1CON1
    AD1CON1 = 0;       // todos los campos a 0

    // Salida de 12 bits o 10 bits
    //AD1CON1bits.AD12B = 0;  

    // Comienzo digitalizacion automatico
    // 111=Auto-convert / 010=TMR3 ADC1 y TMR5 ADC2 / 001=INT0 / 000= SAMP 
        
    ///* Descomentar para ADCv2_10:
        AD1CON1bits.SSRC = 2;    
        AD1CON1bits.ASAM = 1;
    
    
    //AD1CON1bits.SSRC = 7; /*> Comentar para ADCv2_10 */


    // Muestreo simultaneo o secuencial
    //AD1CON1bits.SIMSAM = 0; 
                
    // Comienzo muestreo automatico o por programa (SAMP=1) 		
    //AD1CON1bits.ASAM = 0;

    /* ////////////////////////////////////////////////////// */

    // Inicializacion registro control AD1CON2
    AD1CON2 = 0;  // todos los campos a 0

    /* ////////////////////////////////////////////////////// */

    // Inicializacion registro control AD1CON3
    AD1CON3 = 0;    // todos los campos a 0

    // Reloj con el que funciona el ADC:  0= reloj CPU; 1= RC erlojua 
    //AD1CON3bits.ADRC = 0;  // 
    AD1CON3bits.SAMC = 31;   // Tiempo muestreo = numero de Tad 
    AD1CON3bits.ADCS = 3;   // Relacion entre TAD y Tcy. TAD = Tcy(ADCS+1)

    /* ////////////////////////////////////////////////////// */

    // Inicializacion registro control AD1CON4
    AD1CON4 = 0;

    /* ////////////////////////////////////////////////////// */

    // Inicializacion registro AD1CHS123
    AD1CHS123 = 0;	// seleccion del canal 1,2 eta 3

    /* ////////////////////////////////////////////////////// */

    // Inicializacion registro AD1CHS0
    AD1CHS0 = 0;

    // Entrada analogica conectada al convertidor
    AD1CHS0bits.CH0SA = AN_POT; // potenciometro como primera muestra a medir
    //AD1CHS0bits.CH0SB = 0;

    /* ////////////////////////////////////////////////////// */

    // Inicializacion registros AD1CSS 
    // Si escaneo secuencial 1, si no 0
    AD1CSSH = 0;   // AN16 - AN31 
    AD1CSSL = 0;   // AN0  - AN15 

    /* ////////////////////////////////////////////////////// */

    // Inicializacion registros AD1PCFG. Inicialmente todas AN como digitales
    AD1PCFGH = 0xFFFF;      // 1= digital / 0= Analog    (AN16 - AN31)
    AD1PCFGL = 0xFFFF;      // Puerto B, todos digitales (AN0  - AN15)

    // Inicializar como analogicas solo las que vayamos a usar
    AD1PCFGLbits.PCFG0 = 0;   // Palanca             (AN0)
    AD1PCFGLbits.PCFG5 = 0;   // potenciometro       (AN5)
    AD1PCFGLbits.PCFG4 = 0;   // sensor temperatura  (AN4)
    AD1PCFGLbits.PCFG8 = 0;   // Joystick X-axis     (AN8)
    AD1PCFGLbits.PCFG9 = 0;   // Joystick Y-axis     (AN9)

    /* ////////////////////////////////////////////////////// */

    // Bits y campos relacionados con las interrupciones
    IFS0bits.AD1IF = 0;    
    IEC0bits.AD1IE = 1; // Interrupcion activada    
    //IPC3bits.AD1IP=xx;    

    // Config DMA/Interrupcion
    AD1CON2bits.SMPI = 0; // 0 = NO usar DMA. Activar por interrupcion
    //AD1CON2bits.SMPI3 = 0;
    //AD1CON2bits.SMPI2 = 0;
    //AD1CON2bits.SMPI1 = 0;
    //AD1CON2bits.SMPI0 = 0;

    /* ////////////////////////////////////////////////////// */

    AD1CON1bits.ADON = 1;  // Habilitar el modulo ADC
}


/* ///////////////////////////////////////
   Comienzo del primer muestreo analogico
   ///////////////////////////////////////
 */
void comienzo_muestreo() {
    int i, j;
    
    for (i = 0; i < METRICAS; i++) {
        for (j = 0; j < NMUESTREOS; j++)
            ADC1_muestras[i][j] = 0;

        ADC1_avg[i] = 0;
    }
    num_conversiones = 0;
    //AD1CON1bits.SAMP = 1; /*> Comentar para ADCv2_10 */
}

/* //////////////////////////////
   Reiniciar muestreo analogico
   //////////////////////////////
 */
void reiniciar_muestreo() {
    int i;

    // Reiniciar contadores de muestras y medias
    for (i = 0; i < METRICAS; i++){
        ADC1_avg[i] = 0;
    }

    cont_muestras = 0;
    flag_ADC = 0;        // Desactivar actualizacion LCD
    AD1CON1bits.ADON = 1;       // Habilitar el modulo ADC
    //AD1CON1bits.SAMP = 1;       // Comenzar nuevo muestreo /*> Comentar para ADCv2_10 */
}

/* /////////////////////////////////////////////////////////////////
   Actualizar valores de los sensores analogicos (por interrupcion)
   /////////////////////////////////////////////////////////////////
 */
void update_ADC(){
    int i, j;
    
    // Obtener medias
    for (i = 0; i < METRICAS; i++){
        for (j = 0; j < NMUESTREOS; j++)
            ADC1_avg[i] += ADC1_muestras[i][j];

        ADC1_avg[i] /= NMUESTREOS;
    }
    
    // Conversion metricas ADC a ascii
    for (i = 0; i < METRICAS; i++)
        conversion_sensores(ascii[i], ADC1_avg[i]);
    

    ///MOSTRAR INFO 

    // Actualizar valor de potencia
    
    ctrl_info[3][12]=ascii[iPOT][0];
    ctrl_info[3][13]=ascii[iPOT][1];
    ctrl_info[3][14]=ascii[iPOT][2];
    ctrl_info[3][15]=ascii[iPOT][3];
    // Actualizar valor de potencia
    
    ctrl_info[4][12]=ascii[iTEMP][0];
    ctrl_info[4][13]=ascii[iTEMP][1];
    ctrl_info[4][14]=ascii[iTEMP][2];
    ctrl_info[4][15]=ascii[iTEMP][3];
    // Actualizar valor de potencia
    
    ctrl_info[5][12]=ascii[iJSTK_X][0];
    ctrl_info[5][13]=ascii[iJSTK_X][1];
    ctrl_info[5][14]=ascii[iJSTK_X][2];
    ctrl_info[5][15]=ascii[iJSTK_X][3];
    // Actualizar valor de potencia
    
    ctrl_info[6][12]=ascii[iJSTK_Y][0];
    ctrl_info[6][13]=ascii[iJSTK_Y][1];
    ctrl_info[6][14]=ascii[iJSTK_Y][2];
    ctrl_info[6][15]=ascii[iJSTK_Y][3];
    // Actualizar valor de potencia
    
    ctrl_info[7][12]=ascii[iPAL][0];
    ctrl_info[7][13]=ascii[iPAL][1];
    ctrl_info[7][14]=ascii[iPAL][2];
    ctrl_info[7][15]=ascii[iPAL][3];




    // Actualizar LCD
    // Info[3][12]=
    //ctrl_info 
    /* switch(pagina_LCD){
        case 1:
            // Actualizar valor de potencia
            Ventana_LCD[0][pos_izq3] = ascii[iPOT][0];
            Ventana_LCD[0][pos_izq2] = ascii[iPOT][1];
            Ventana_LCD[0][pos_izq1] = ascii[iPOT][2];
            Ventana_LCD[0][pos_izq0] = ascii[iPOT][3];

            // Actualizar valor de temperatura
            Ventana_LCD[0][pos_der3] = ascii[iTEMP][0];
            Ventana_LCD[0][pos_der2] = ascii[iTEMP][1];
            Ventana_LCD[0][pos_der1] = ascii[iTEMP][2];
            Ventana_LCD[0][pos_der0] = ascii[iTEMP][3];
        break;

        case 2:
            // Actualizar valor de Joystick eje X
            Ventana_LCD[0][pos_izq3] = ascii[iJSTK_X][0];
            Ventana_LCD[0][pos_izq2] = ascii[iJSTK_X][1];
            Ventana_LCD[0][pos_izq1] = ascii[iJSTK_X][2];
            Ventana_LCD[0][pos_izq0] = ascii[iJSTK_X][3];

            // Actualizar valor de Joystick eje Y
            Ventana_LCD[0][pos_der3] = ascii[iJSTK_Y][0];
            Ventana_LCD[0][pos_der2] = ascii[iJSTK_Y][1];
            Ventana_LCD[0][pos_der1] = ascii[iJSTK_Y][2];
            Ventana_LCD[0][pos_der0] = ascii[iJSTK_Y][3];
        break;

        default:
        break;
    } */


}


/* ////////////////////////////////////////////
   Actualizar valores de LCD (por interrupcion)
   ////////////////////////////////////////////
 */
void _ISR_NO_PSV _ADC1Interrupt(){
    int j;
    
    num_conversiones++;
    j = cont_muestras;

    switch(AD1CHS0bits.CH0SA){
        case AN_POT: 
            ADC1_muestras[iPOT][j] = ADC1BUF0;  // Obtener valor del buffer
            AD1CHS0bits.CH0SA = AN_TEMP;        // Cambio de lectura de sensor
        break;

        case AN_TEMP: 
            ADC1_muestras[iTEMP][j] = ADC1BUF0;  // Obtener valor del buffer
            AD1CHS0bits.CH0SA = AN_JSTK_X;        // Cambio de lectura de sensor
        break;

        case AN_JSTK_X: 
            ADC1_muestras[iJSTK_X][j] = ADC1BUF0;  // Obtener valor del buffer
            AD1CHS0bits.CH0SA = AN_JSTK_Y;        // Cambio de lectura de sensor
        break;

        case AN_JSTK_Y: 
            ADC1_muestras[iJSTK_Y][j] = ADC1BUF0;  // Obtener valor del buffer
            AD1CHS0bits.CH0SA = AN_PALANCA;        // Cambio de lectura de sensor
        case AN_PALANCA:
            ADC1_muestras[AN_PALANCA][J]= ADC1BUF0; // Obtener valor del buffer
            AD1CHS0bits.CH0SA = AN_POT;  // Cambio de lectura de sensor
        
        
            cont_muestras++;
        break;
    }

    
    // Si hemos recogido suficientes muestras...
    if (cont_muestras == NMUESTREOS){
        AD1CON1bits.ADON = 0;  // Deshabilitar el modulo ADC
        flag_ADC = 1;   // Comenzar actualizacion LCD
    }
      
    // Si no, realizar nuevo muestreo
    /*} else {
        AD1CON1bits.SAMP = 1; //> Comentar para ADCv2_10 
    }*/
    
    // Desactiva el flag IF
    IFS0bits.AD1IF = 0;
}


