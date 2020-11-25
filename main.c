/**
  Section: Included Files
*/
#include "ad5940.h"
#include "Parametry.h"
#include "Amperometric.h"
#include "Impedance.h"
#include "RampTest.h"
#include "mcc_generated_files/system.h"
#include "mcc_generated_files/usb/usb.h"
#include "mcc_generated_files/usb/app_device_custom_hid.h"
#include "mcc_generated_files/delay.h"

/**
  Section: Variables
*/

/**
  Section: Functions
*/
uint8_t *Zerowanie(uint8_t *wsk, int dlugosc); // Funkcja do zerowania buforów

int main(void)
{
    SYSTEM_Initialize();     // initialize the device
    USBDeviceTasks();        // enumeracja
    
    APP_DeviceCustomHIDInitialize(); // Inicjalizuje endpointy
    AD5940_HWReset();     // Resetuje AD5940 przy wlaczeniu
    AD5940_Initialize();  // Uruchamia AD5940

    while(1)
    {
        AD5940_EnterSleepS(); // Wprowadz AD5940 w stan uspienia
                              // Dla zaoszczedzenia energii gdy 
                              // zaden pomiar nie jest wykonywany
        
        _wait();              // Wprowadza w tryb IDLE
        
        Procedura = Recive_USB_HID(Dane);
        //Send_USB_HID(Dane); // Uncomenent to check recived data 
        //Procedura = 99; // Set to stop any measurment
        
        switch(Procedura)
        {
            // Procedury pomiarowe //
            case ChronoAmperometry:
                DELAY_milliseconds(10);
                
                NumData         = *(int*)(Dane+1);
                SensorBias      = *(float*)(Dane+5);
                Vzero           = *(float*)(Dane+9);
                PulseAmplitude  = *(float*)(Dane+13);
                PulseLength     = *(uint32_t*)(Dane+14);
                
                Param1_i  = &NumData;
                Param2_f  = &SensorBias;
                Param3_f  = &Vzero;
                Param4_f  = &PulseAmplitude;
                Param5_u  = &PulseLength;
                
                AD5940_WakeUp(10); // Budzimy AD5940 przed pomiarem
                AD5940_ChronoAmperometric_Main(Param1_i,Param2_f,Param3_f,Param4_f, \
                                               Param5_u); 
                Zerowanie(&Dane[0],64);
                break;
            case LinearVoltametry:
                DELAY_milliseconds(10);

                RampStartVolt = *(float*)(Dane+1);
                RampPeakVolt  = *(float*)(Dane+5);
                VzeroStart    = *(float*)(Dane+9);
                VzeroPeak     = *(float*)(Dane+13);
                StepNumber    = *(uint32_t*)(Dane+17);
                RampDuration  = *(uint32_t*)(Dane+21);
                SampleDelay   = *(float*)(Dane+25);
                NumCycle      = *(uint32_t*)(Dane+29);
                
                Param1_f  = &RampStartVolt;
                Param2_f  = &RampPeakVolt;
                Param3_f  = &VzeroStart;
                Param4_f  = &VzeroPeak;
                Param1_u  = &StepNumber;
                Param2_u  = &RampDuration;
                Param5_f  = &SampleDelay;
                Param3_u  = &NumCycle;

                Direction = Linear;

                AD5940_WakeUp(10); // Budzimy AD5940 przed pomiarem
                AD5940_CV_Main(Direction,Param1_f,Param2_f,Param3_f,Param4_f, \
                               Param1_u,Param2_u,Param5_f,Param3_u);
                Zerowanie(&Dane[0],64);
                break;
            case CyclicVoltametry:
                DELAY_milliseconds(10);

                RampStartVolt = *(float*)(Dane+1);
                RampPeakVolt  = *(float*)(Dane+5);
                VzeroStart    = *(float*)(Dane+9);
                VzeroPeak     = *(float*)(Dane+13);
                StepNumber    = *(uint32_t*)(Dane+17);
                RampDuration  = *(uint32_t*)(Dane+21);
                SampleDelay   = *(float*)(Dane+25);
                NumCycle      = *(uint32_t*)(Dane+29);
                
                Param1_f  = &RampStartVolt;
                Param2_f  = &RampPeakVolt;
                Param3_f  = &VzeroStart;
                Param4_f  = &VzeroPeak;
                Param1_u  = &StepNumber;
                Param2_u  = &RampDuration;
                Param5_f  = &SampleDelay;
                Param3_u  = &NumCycle;

                Direction = Cyclic;
                
                AD5940_WakeUp(10); // Budzimy AD5940 przed pomiarem
                AD5940_CV_Main(Direction,Param1_f,Param2_f,Param3_f,Param4_f, \
                               Param1_u,Param2_u,Param5_f,Param3_u);
                Zerowanie(&Dane[0],64);
                break;
            case SqrtVoltametry:
                DELAY_milliseconds(10);

                RampStartVolt = *(float*)(Dane+1);
                RampPeakVolt  = *(float*)(Dane+5);
                VzeroStart    = *(float*)(Dane+9);
                VzeroPeak     = *(float*)(Dane+13);
                Freq          = *(uint32_t*)(Dane+17);
                Amplitude     = *(uint32_t*)(Dane+21);
                SampleDelay   = *(float*)(Dane+25);
                Increment     = *(uint32_t*)(Dane+29);

                Param1_f  = &RampStartVolt;
                Param2_f  = &RampPeakVolt;
                Param3_f  = &VzeroStart;
                Param4_f  = &VzeroPeak;
                Param1_u  = &Freq;
                Param2_u  = &Amplitude;
                Param5_f  = &SampleDelay;
                Param3_u  = &Increment;
                
                AD5940_WakeUp(10); // Budzimy AD5940 przed pomiarem
                AD5940_SqrtVoltametry_Main(Param1_f, Param2_f, Param3_f, Param4_f, \
                                           Param1_u, Param2_u, Param3_u, Param5_f);

                Zerowanie(&Dane[0],64);
                break;
            case Amperometric:  // numer 6
                NumData    = *(int*)(Dane+1);
                AmpODR     = *(float*)(Dane+5);
                SensorBias = *(float*)(Dane+9);
                VZero      = *(float*)(Dane+13);
                
                Param1_f  = &AmpODR;
                Param2_f  = &VZero;
                Param3_f  = &SensorBias;
                Param1_i  = &NumData;

                AD5940_Amperometric_Main(Param1_f,Param2_f,Param3_f,Param1_i);  
                Zerowanie(&Dane[0],64);
                break;
            case ElectrochemicalImpedanceSpectroscopy:
                DELAY_milliseconds(10);
                 
                SweepStart      = *(float*)(Dane+1);
                SweepStop       = *(float*)(Dane+5);
                SweepPoints     = *(uint32_t*)(Dane+9);
                SweepLog        = *(uint32_t*)(Dane+13);
                NumData         = *(int*)(Dane+17);
                DacVoltPeakPeak = *(float*)(Dane+21);
                BiasVOLT        = *(float*)(Dane+25);
                
                Param1_f = &SweepStart;
                Param2_f = &SweepStop;
                Param1_u = &SweepPoints;
                Param2_u = &SweepLog;
                Param1_i = &NumData;
                Param3_f = &DacVoltPeakPeak;
                Param4_f = &BiasVOLT;
                
                AD5940_WakeUp(10); // Budzimy AD5940 przed pomiarem

                AD5940_EIS_Main(Param1_f,Param2_f,Param1_u,Param2_u,Param1_i, Param3_f, Param4_f);
                
                Zerowanie(&Dane[0],64);
                break;
            case Temperature:
                AD5940_WakeUp(10); // Budzimy AD5940 przed pomiarem
                AD5940_Temperature_Main(); // Cos nie dziala
                Zerowanie(&Dane[0],64);
                break;
            // Funkcje do kalibracji ///    
//            case Calibration_HS_DAC:
//                AD5940_WakeUp(10);        // Budzimy AD5940 przed kalibracja
//                //AD5940_HSDACCal_Main();   // Kalibracja HSDAC
//                Zerowanie(&Dane[0],64);
//                break;
      }
    }
    
    return 1; 
}

uint8_t *Zerowanie(uint8_t *wsk, int dlugosc)
{
    int i  = 0;
    for(i=0; i<dlugosc+1; i++)
    {
        *(wsk+i) = 0;
    }
}

/**
 End of File
*/

