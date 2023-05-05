#include "commons.h"

// Funciones que se llaman desde otros modulos

void inic_ADC1();
void comienzo_muestreo();
void reiniciar_muestreo();

extern int flag_ADC;
extern long num_conversiones;

extern long ADC1_muestras[METRICAS][NMUESTREOS];
extern long ADC1_avg[METRICAS];
extern int  cont_muestras;

void update_ADC();