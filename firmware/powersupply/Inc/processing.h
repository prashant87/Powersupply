#ifndef PROCESSING_H
#define PROCESSING_H
#include "micro.h"
extern double Convert_to_Voltage(uint16_t ADC_LiniarVoltage);
extern double ADC_to_Physical_VoltageDivider(double d_VoltageADC,uint32_t ui32RezistorUper,uint32_t ui32RezistorLower);
extern double ADC_to_PhysicalINA169(double d_VoltageADC,double dRezitorSarcina,uint32_t ui32RezitorLeader);
extern double ADC_to_PhysicalIntrumentationAmplifier(double d_VoltageADC,double dRezitorSarcina,double dfactorInstrumentation);
#endif
