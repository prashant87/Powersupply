#include "micro.h"
#include "signal_proc.h"
#include "uart.h"

T_UWORD Convert_to_Voltage (T_UWORD uw_raw_value);

/*-----------------------------------------------------------------------------------*/
/*Description:  Function to convert raw values to voltage in Q5.11                   */
/*                                                                                   */
/*Parameters:   uw_voltage (input) volatage input in Q5.11 format                    */
/*Return value: volatage in Q5.11 format                                             */
/*-----------------------------------------------------------------------------------*/
T_UWORD Convert_to_Voltage (T_UWORD uw_raw_value)
{
 T_UWORD luw_result = 0;
 T_ULONG lul_result = 0;
 lul_result = uw_raw_value * AdcVoltageRef;
 lul_result = lul_result >> (AdcBitResolution);
 luw_result = (T_UWORD)lul_result;
 return luw_result;
}

void init_signal_procesing(void)
{

}
