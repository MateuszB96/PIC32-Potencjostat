/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.h

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */

#ifndef _PARAMETRY_H_   /* Guard against multiple inclusion */
#define _PARAMETRY_H

#define Linear 1
#define Cyclic 0

#define ChronoAmperometry  2
#define LinearVoltametry   3
#define CyclicVoltametry   4
#define SqrtVoltametry     5
#define Amperometric       6
#define ElectrochemicalImpedanceSpectroscopy 7
#define Temperature        8

// Zmienne wykorzystania ogolnego //

uint8_t Procedura;  // Zmienna wyboru procedury
static uint8_t Dane[64]; // Bufor odbioru danych
       uint8_t Bufor[64]; // Bufor do wyslania danych

float *Param1_f, *Param2_f, *Param3_f, *Param4_f, *Param5_f;
uint32_t *Param1_u, *Param2_u, *Param3_u, *Param4_u, *Param5_u;
int *Param1_i;

int      NumData;  // Ilosc pkt po ktorej pomiar zostaje zatrzymany

// Parametry chronoamperometrii //
float    SensorBias;     // mV
float    Vzero;          // mV
 float   PulseAmplitude; // mV
uint32_t PulseLength;    // ms

// Parametry metod voltamperometrycznych //
float RampStartVolt;    //(mV)  
float RampPeakVolt;     //(mV)    
float VzeroStart;       //(mV)       
float VzeroPeak;        //(mV)
uint32_t StepNumber;    //(max 4085)
uint32_t RampDuration;  //(s)
float SampleDelay;      //(ms)  
uint32_t NumCycle;      // > 0
_Bool Direction;

// Parametry SqrtVoltametry //
// float RampStartVolt;    //(mV)
// float RampPeakVolt;     //(mV)  
// float VzeroStart;       //(mV)         
// float VzeroPeak;        //(mV)
uint32_t Freq;             //(Hz)
uint32_t Amplitude;        //(mV)
uint32_t Increment;         //(mV)
// float SampleDelay;      //(ms)

// Parametry Amperometrii //
//int NumData;
float VZero;
float SensorBias;
float AmpODR; // Czas pomiedzy probkami w Hz

// Parametry metod impedancyjnych //
float SweepStart;       //(Hz) Mniejsze od SweepStop
float SweepStop;        //(Hz) Wieksze  od SweepStop
uint32_t SweepPoints;
uint32_t SweepLog;          // 0 vs 1
float DacVoltPeakPeak; 
float BiasVOLT; 


/* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _EXAMPLE_FILE_NAME_H */

/* *****************************************************************************
 End of File
 */
