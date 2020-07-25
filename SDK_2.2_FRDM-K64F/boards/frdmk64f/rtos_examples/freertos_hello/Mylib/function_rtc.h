/*#**************************************************************************#*/
/*#     System name :   Water  measurement                             	     #*/
/*#     File name   :   function_rtc.h                              	     #*/
/*#     Summary     :   Parameter config for all sensor Water                #*/
/*#     Author      :   Tuanhd                                         		 #*/
/*#     Date        :   20/02/2017                                           #*/
/*#                                                                          #*/
/*#     Copyright (c) 2017 VNPT-Technology                                   #*/
/*#                        All rights reserved.                              #*/
/*#**************************************************************************#*/
/*#  Version                   : Date        : Infomation                    #*/
/*#  Major Ver.Minor Ver.Rev   : YYYY/MM/DD  :                               #*/
/*#--------------------------------------------------------------------------#*/
/*#     1.0.0                  : 20/02/2017  : New making                    #*/
/*#**************************************************************************#*/


#ifndef __FUNCTION_RTC_H__
#define __FUNCTION_RTC_H__
//-----------------------------------------------------------------------------
//                                    Includes

#ifdef __cplusplus
extern "C" {
#endif
    
#include "main.h"
    
#define TIME_DELAY_SYNC_MC60_TEST       (10000)
#define TIME_DELAY_SYNC_MC60_CYC_TEST   (60000)
#include "fsl_rtc.h"
//------------------------------------------------------------------------------
void RTC_InitTimer( const rtc_datetime_t *datetime );
void rtc_task( void *pvParameters );
void Board_SetRtcClockSource(void);
//------------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif 
