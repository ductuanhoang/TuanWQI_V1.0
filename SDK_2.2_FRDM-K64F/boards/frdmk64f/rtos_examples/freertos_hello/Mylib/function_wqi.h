/*#**************************************************************************#*/
/*#     System name :   Water                               		     #*/
/*#     File name   :   uart_task.h                              	     #*/
/*#     Summary     :                                                        #*/
/*#     Author      :   Tuanhd                                         	     #*/
/*#     Date        :   12/1/2017                                            #*/
/*#                                                                          #*/
/*#     Copyright (c) 2016 VNPT-Technology                                   #*/
/*#                        All rights reserved.                              #*/
/*#**************************************************************************#*/
/*#  Version                   : Date        : Infomation                    #*/
/*#  Major Ver.Minor Ver.Rev   : YYYY/MM/DD  :                               #*/
/*#--------------------------------------------------------------------------#*/
/*#     1.0.0                  : 12/1/2017  : New making                     #*/
/*#**************************************************************************#*/


#ifndef __FUNCTION_WQI_H__
#define __FUNCTION_WQI_H__
#ifdef __cplusplus
extern "C"
{
#endif
/*********************************************************************
* INCLUDES
*/
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
  
#include "main.h"
/*********************************************************************
* CONSTANTS
*/




/*********************************************************************
* Function
*/
void wqi_DO_calculate_hours(float data_value);
void wqi_PH_calculate_hours(float data_value);
void wqi_NH4_calculate_hours(float data_value);
void wqi_Turbi_calculate_hours(float data_value);

#ifdef __cplusplus
}
#endif
#endif