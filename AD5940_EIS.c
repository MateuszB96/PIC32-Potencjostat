/*!
 *****************************************************************************
 @file:    AD5940Main.c
 @author:  Neo Xu
 @brief:   Electrochemical impedance spectroscopy based on example AD5940_Impedance
							This project is optomized for 3-lead electrochemical sensors that typically have 
							an impedance <200ohm. For optimum performance RCAL should be close to 
							impedance of the sensor.
 -----------------------------------------------------------------------------

Copyright (c) 2017-2019 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.
By using this software you agree to the terms of the associated
Analog Devices Software License Agreement.
*****************************************************************************/

#include "Impedance.h"
#include "Parametry.h"
#include <math.h>

#define APPBUFF_SIZE 512
uint32_t AppBuff[APPBUFF_SIZE];

void AD5940_HSDACCal_Main(void){
    
  HSDACCal_Type hsdac_cal;
  ADCPGACal_Type adcpga_cal;
  CLKCfg_Type clk_cfg;
  
  AD5940_AFEPwrBW(AFEPWR_HP, AFEBW_AUTOSET);
  
  clk_cfg.ADCClkDiv = ADCCLKDIV_1;
  clk_cfg.ADCCLkSrc = ADCCLKSRC_HFOSC;
  clk_cfg.SysClkDiv = SYSCLKDIV_1;
  clk_cfg.SysClkSrc = SYSCLKSRC_HFOSC;
  clk_cfg.HfOSC32MHzMode = bFALSE;
  clk_cfg.HFOSCEn = bTRUE;
  clk_cfg.HFXTALEn = bFALSE;
  clk_cfg.LFOSCEn = bTRUE;
  AD5940_CLKCfg(&clk_cfg);
 
  adcpga_cal.SysClkFreq = 16000000.0f;
  adcpga_cal.AdcClkFreq = 16000000.0f;
  adcpga_cal.ADCPga = ADCPGA_9;
  adcpga_cal.ADCSinc2Osr = ADCSINC2OSR_1333;
  adcpga_cal.ADCSinc3Osr = ADCSINC3OSR_4;
  adcpga_cal.PGACalType = PGACALTYPE_OFFSETGAIN;
  adcpga_cal.TimeOut10us = 1000;
  adcpga_cal.VRef1p11 = 1.11;
  adcpga_cal.VRef1p82 = 1.82;
  AD5940_ADCPGACal(&adcpga_cal);
    
  hsdac_cal.fRcal = 200;  /**/
  hsdac_cal.SysClkFreq = 16000000.0f; /**/
  hsdac_cal.AdcClkFreq = 16000000.0f; /**/
  hsdac_cal.ExcitBufGain = EXCITBUFGAIN_0P25;	/**< Select from  EXCITBUFGAIN_2, EXCITBUFGAIN_0P25 */ 
  hsdac_cal.HsDacGain = HSDACGAIN_0P2; 		    /**< Select from  HSDACGAIN_1, HSDACGAIN_0P2 */ 
  hsdac_cal.AfePwrMode = AFEPWR_HP;
  hsdac_cal.ADCSinc2Osr = ADCSINC2OSR_1333;
  hsdac_cal.ADCSinc3Osr = ADCSINC3OSR_4;
  
  AD5940_HSDACCal(&hsdac_cal);
}

int32_t ImpedanceShowResult(uint32_t *pData, uint32_t DataCount)
{
    float freq;
    float Real, Image; 
    
    AppIMPCtrl(IMPCTRL_GETFREQ, &freq);
    *(float*)(Bufor) = freq;

    fImpPol_Type *pImp = (fImpPol_Type*)pData;

    /*Process data*/
    int i = 0;
    for(i=0;i<DataCount;i++)
        {
        Real  = (pImp[i].Magnitude)*cos(pImp[i].Phase);
        Image = -(pImp[i].Magnitude)*sin(pImp[i].Phase);
        
        *(float*)(Bufor+4) = Real;
        *(float*)(Bufor+8) = Image;
        *(float*)(Bufor+12) = pImp[i].Magnitude;
        *(float*)(Bufor+16) = pImp[i].Phase*180/MATH_PI;
        
        Send_USB_HID(Bufor);
        }
    
    return 0;
}

static int32_t AD5940PlatformCfg(void)
{
  CLKCfg_Type clk_cfg;
  FIFOCfg_Type fifo_cfg;
  AGPIOCfg_Type gpio_cfg;

  /* Use hardware reset */
  AD5940_HWReset();
  AD5940_Initialize();
  /* Platform configuration */
  /* Step1. Configure clock */
  clk_cfg.ADCClkDiv = ADCCLKDIV_1;
  clk_cfg.ADCCLkSrc = ADCCLKSRC_HFOSC;
  clk_cfg.SysClkDiv = SYSCLKDIV_1;
  clk_cfg.SysClkSrc = SYSCLKSRC_HFOSC;
  clk_cfg.HfOSC32MHzMode = bFALSE;
  clk_cfg.HFOSCEn = bTRUE;
  clk_cfg.HFXTALEn = bFALSE;
  clk_cfg.LFOSCEn = bTRUE;
  AD5940_CLKCfg(&clk_cfg);
  /* Step2. Configure FIFO and Sequencer*/
  fifo_cfg.FIFOEn = bFALSE;
  fifo_cfg.FIFOMode = FIFOMODE_FIFO;
  fifo_cfg.FIFOSize = FIFOSIZE_4KB;                       /* 4kB for FIFO, The reset 2kB for sequencer */
  fifo_cfg.FIFOSrc = FIFOSRC_DFT;
  fifo_cfg.FIFOThresh = 6;		
  AD5940_FIFOCfg(&fifo_cfg);
  fifo_cfg.FIFOEn = bTRUE;
  AD5940_FIFOCfg(&fifo_cfg);
  
  /* Step3. Interrupt controller */
  AD5940_INTCCfg(AFEINTC_1, AFEINTSRC_ALLINT, bTRUE);   /* Enable all interrupt in INTC1, so we can check INTC flags */
  AD5940_INTCClrFlag(AFEINTSRC_ALLINT);
  AD5940_INTCCfg(AFEINTC_0, AFEINTSRC_DATAFIFOTHRESH, bTRUE); 
  AD5940_INTCClrFlag(AFEINTSRC_ALLINT);
  /* Step4: Reconfigure GPIO */
  gpio_cfg.FuncSet = GP0_INT|GP1_SLEEP|GP2_SYNC;
  gpio_cfg.InputEnSet = 0;
  gpio_cfg.OutputEnSet = AGPIO_Pin0|AGPIO_Pin1|AGPIO_Pin2;
  gpio_cfg.OutVal = 0;
  gpio_cfg.PullEnSet = 0;
  AD5940_AGPIOCfg(&gpio_cfg);
  AD5940_SleepKeyCtrlS(SLPKEY_UNLOCK);  /* Allow AFE to enter sleep mode. */
  return 0;
}

void AD5940ImpedanceStructInit(float *Start, float *Stop, uint32_t *Points, uint32_t *Log, int *NumData, float *DacVPP, float *DC)
{
  AppIMPCfg_Type *pImpedanceCfg;
  
  AppIMPGetCfg(&pImpedanceCfg);
  /* Step1: configure initialization sequence Info */
  pImpedanceCfg->SeqStartAddr = 0;
  pImpedanceCfg->MaxSeqLen = 512; /* @todo add checker in function */

  pImpedanceCfg->RcalVal = 200.0;
  pImpedanceCfg->FifoThresh = 6;
  pImpedanceCfg->SinFreq = 1000.0;
	
	/* Configure Excitation Waveform 
	*
    * Output waveform = DacVoltPP * ExcitBufGain * HsDacGain 
	* 	
	* = 300 * 0.25 * 0.2 = 15mV pk-pk
	*
	*/
  
	pImpedanceCfg->DacVoltPP = *DacVPP; //300;	/* Maximum value is 600mV*/
	pImpedanceCfg->ExcitBufGain = EXCITBUFGAIN_0P25;
	pImpedanceCfg->HsDacGain = HSDACGAIN_0P2;
	
	/* Set switch matrix to onboard(EVAL-AD5940ELECZ) gas sensor. */
	pImpedanceCfg->DswitchSel = SWD_CE0;
	pImpedanceCfg->PswitchSel = SWP_RE0;
	pImpedanceCfg->NswitchSel = SWN_SE0LOAD;
	pImpedanceCfg->TswitchSel = SWT_SE0LOAD;
	/* The dummy sensor is as low as 5kOhm. We need to make sure RTIA is small enough that HSTIA won't be saturated. */
	pImpedanceCfg->HstiaRtiaSel = HSTIARTIA_200;	
    
	pImpedanceCfg->BiasVolt = *DC; // 0.0
    /* Configure the sweep function. */
	pImpedanceCfg->SweepCfg.SweepEn = bTRUE;
	pImpedanceCfg->SweepCfg.SweepStart = *Start;           /* Start from 200 Hz */
	pImpedanceCfg->SweepCfg.SweepStop = *Stop;             /* Stop at 100 kHz */
	pImpedanceCfg->SweepCfg.SweepPoints = *Points;         /* Points is 55 */
	pImpedanceCfg->SweepCfg.SweepLog = *Log;
    pImpedanceCfg->NumOfData = *NumData;
    

	/* Configure Power Mode. Use HP mode if frequency is higher than 80kHz. */
    if(*Start > 80e3)
    {pImpedanceCfg->PwrMod = AFEPWR_HP;}
    else
    {pImpedanceCfg->PwrMod = AFEPWR_LP;}
	/* Configure filters if necessary */
	pImpedanceCfg->ADCSinc3Osr = ADCSINC3OSR_4;		/* Sample rate is 800kSPS/2 = 400kSPS */
    pImpedanceCfg->DftNum = DFTNUM_16384;
    pImpedanceCfg->DftSrc = DFTSRC_SINC3;
    pImpedanceCfg->Zatrzymanie = bFALSE;
}

void AD5940_EIS_Main(float *Start, float *Stop, uint32_t *Points, uint32_t *Log, int *NumData, float *DacVPP, float *DC)
{
  uint32_t temp;
  AppIMPCfg_Type *pImpedanceCfg;
  
  AD5940PlatformCfg();
  
  AD5940_HSDACCal_Main(); // Dokonuje kalibracji 
  
  AD5940ImpedanceStructInit(Start,Stop,Points,Log,NumData, DacVPP, DC);
  
  AppIMPInit(AppBuff, APPBUFF_SIZE);    /* Initialize IMP application. Provide a buffer, which is used to store sequencer commands */
  AppIMPCtrl(IMPCTRL_START, 0);         /* Control IMP measurement to start. Second parameter has no meaning with this command. */
 
  while(1)
  {
    if(AD5940_GetMCUIntFlag())
    {
      AD5940_ClrMCUIntFlag();
      temp = APPBUFF_SIZE;
      AppIMPISR(AppBuff, &temp);
      ImpedanceShowResult(AppBuff, temp);
      AppIMPGetCfg(&pImpedanceCfg);
      if(pImpedanceCfg->Zatrzymanie)
      {break;} // Wyjscie do main po zakonczonym pomiarze
    }
  }
}

