#include "power.h"
#include "tasks.h"
#define POWER_RET_LIMIT 100
T_UBYTE ub_counter_retry;
T_UWORD Divizor_U_Sense(T_UWORD luw_ADCV0)
{
	T_UWORD luw_UoutSense = 0;
	T_ULONG lul_aux = 0;
	/*UOutsense = Uadc*5.7*/
	lul_aux = (((T_ULONG)luw_ADCV0) * 91);
 	lul_aux = lul_aux>> 4 ;
	luw_UoutSense = (T_UWORD)lul_aux;
	return luw_UoutSense;
}

T_UWORD Rawvalue_to_Current(T_UWORD luw_ADCV1)
{
	
	T_ULONG lul_aux = 0;
	/*x = raw * 2.30A /5V */
	//lul_aux = luw_ADCV1 >> 1;
	lul_aux = luw_ADCV1>>2 ;
	/* divizior rezistiv 10k/(22k+10k)*/
	lul_aux = lul_aux *32 /10;
	/* I current Q5.10*/
	return (T_UWORD)lul_aux;
}
T_UWORD Divizor_U_Pot(T_UWORD luw_ADCV2)
{
	T_UWORD luw_UoutSense = 0;
	T_ULONG lul_aux = 0;
	/*UOutpot = Uadc*2.2/3.2*/
	lul_aux = (((T_ULONG)luw_ADCV2) * 93);
 	lul_aux = lul_aux>> 6 ;
	luw_UoutSense = (T_UWORD)lul_aux;
	/* U voltage pot Q5.10*/
	return luw_UoutSense;
}

T_UBYTE UpotToPWM(T_UWORD luw_UPot)
{
	T_UBYTE lub_duty_pwm1 = 0;
	T_ULONG lul_temp = 0;
	lul_temp = (((T_ULONG)luw_UPot) *55) >>11;
	lub_duty_pwm1 = (T_UWORD)lul_temp;
	if(lub_duty_pwm1>100)
	{
		lub_duty_pwm1 = 100;
	}
	return lub_duty_pwm1;
}
T_UBYTE GetLimitVoltageTarget(T_UWORD uw_IPot,T_UWORD uw_Isense)
{
	T_UBYTE ub_ret = POWER_LIMIT_DIS;
	if (uw_IPot < uw_Isense)
	{
		ub_ret = POWER_LIMIT_EN;
		ub_counter_retry = POWER_RET_LIMIT;
	}
	else
	{
		if(ub_counter_retry>0)
		{
			ub_counter_retry--;
			ub_ret = POWER_LIMIT_EN;
		}
		else
		{
			ub_ret = POWER_LIMIT_DIS;
		}
	}
	return ub_ret;
}
void PowerInit(void)
{
	ub_islimit_power = POWER_LIMIT_DIS;
	ub_counter_retry = 0;
}
