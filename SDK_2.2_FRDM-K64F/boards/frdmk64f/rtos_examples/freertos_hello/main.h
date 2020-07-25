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
#ifndef __MAIN_H_
#define __MAIN_H_
#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
* INCLUDES
*/
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdarg.h>

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

/* Freescale includes. */
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_gpio.h"  
#include "fsl_common.h"
#include "fsl_port.h"
#include "fsl_debug_console.h"
#include "fsl_i2c.h"
#include "fsl_i2c_freertos.h"
#include "fsl_device_registers.h"
#include "fsl_dspi.h"
#include "fsl_dspi_freertos.h"
#include "fsl_rtc.h"
#include "clock_config.h"
#include "fsl_uart.h"
#include "fsl_sim.h"
  
/* ma hoa */

#include "mymmcau.h"
/* json */
#include "jsmn.h"

/* Flash */
#include <slld.h>
#include <slld_hal.h>
#include <slld_targetspecific.h>

/* User include */
#include "uart_task.h"
#include "function_rtc.h"
#include "function_spi.h"
#include "function_wqi.h"

//================================ ID Sensor ===================================
                                   // DEFINE //
/* Type device is SensorWater */																	 
#define TYPE_DEVICE_SENSORWATER                  101

/* Type device is Temp */
#define TYPE_DEVICE_WQI_ID_Temp                  51

/* Type device is Ph */
#define TYPE_DEVICE_WQI_ID_PH                    52

/* Type device is Do */
#define TYPE_DEVICE_WQI_ID_DO                    53

/* Type device is N-NH4 */
#define TYPE_DEVICE_WQI_ID_NH4                   54

/* Type device is Turbidity */
#define TYPE_DEVICE_WQI_ID_Turbidity             55
//================================ ID WQI OF Sensor ============================
                                   // DEFINE //
/* Type device is Temp */
#define TYPE_DEVICE_WQI_ID_Temp                  51

/* Type device is Ph */
#define TYPE_DEVICE_WQI_ID_PH                    52

/* Type device is Do */
#define TYPE_DEVICE_WQI_ID_DO                    53

/* Type device is N-NH4 */
#define TYPE_DEVICE_WQI_ID_NH4                   54

/* Type device is Turbidity */
#define TYPE_DEVICE_WQI_ID_Turbidity             55

//==============================================================================
// define information server
#define MY_ID_DEVICE_ADDRESS                    (00000003)
#define DST_ID_DEVICE_ADDRESS                   (00000002)
	// TIME SENDS PACKET TO SERVER  
#define TIME_SENDS_SERVER_1_HOUR                60      // 60  // Default
#define TIME_SENDS_SERVER_2_HOUR                120     // 120
#define TIME_SENDS_SERVER_4_HOUR                240     // 240
#define TIME_SENDS_SERVER_8_HOUR                480     // 480s
#define TIME_SENDS_SERVER_24_HOUR               1440    // 1440
#define TIME_SENDS_SERVER_MAX                   1440
      
	// TIME READ SENSOR CYCLE       
#define TIME_READ_SENSOR_30_MINUTE              30      // 30 // Default
#define TIME_READ_SENSOR_1_HOUR                 60      // 60
#define TIME_READ_SENSOR_2_HOUR                 120     // 120
#define TIME_READ_SENSOR_4_HOUR                 240     // 240
#define TIME_READ_SENSOR_8_HOUR                 480     // 480
#define TIME_READ_SENSOR_24_HOUR                1440    // 1440
#define TIME_READ_SENSOR_MAX                    1440
      
	// TIME SAVE FLASH      
#define TIME_SAVE_FLASHS_30_MINUTE              30      // 30 //Default
#define TIME_SAVE_FLASHS_1_HOUR                 60      // 60
#define TIME_SAVE_FLASHS_2_HOUR                 120     // 120
#define TIME_SAVE_FLASHS_4_HOUR                 240     // 240
#define TIME_SAVE_FLASHS_8_HOUR                 480     // 480
#define TIME_SAVE_FLASHS_24_HOUR                1440    // 1440
#define TIME_SAVE_FLASHS_MAX                    1440
#define TIME_CHECK_TASK		                    30
  
#define STR_SYNC_TIME                           "3:0"
  
#define DEBUG_ENABLE		                1
#define DEBUG_DISABLE		                0

//================================ Struct for board ===========================
#define ACTIVE_SEND 1
#define NON_SEND 0
                                   // DEFINE FLASH// 
#define BSP_SPI_MEMORY_S25FL032P		1		
#define FLASH_TRANSFER_SIZE (256)

//-----------------------------------------------------------------------------
typedef enum _status_response
{
    /* status response msg SUCCESS */
    RESPONSE_SUCCESS = 0U,
    /* status response msg ERROR */
    RESPONSE_ERROR = 1U
    
}status_response;
//------------------------------------------------------------------------------

typedef struct {                  
    uint16_t time_send_server;
    uint16_t time_read_sensor;
    uint16_t time_save_flashs;
    uint64_t myaddress;       
    uint64_t dstaddress;      
    uint16_t enable_debug;
}board_config;
//-----------------------------------------------------------------------------


typedef struct {
    double TEMP;
    double PH;
    double DO;
    double NH4;
    double TURBIDITY;
}wqi_data;
//-----------------------------------------------------------------------------


typedef struct {
    double WQI_PH;
    double WQI_DO;
    double WQI_NH4;
    double WQI_TURBIDITY;
}wqi_calculate;

//-----------------------------------------------------------------------------
typedef struct{
    uint8_t Sendto_MC60; // gui du lieu len MC60 neu TRUE
    uint8_t Sendto_KE02; // gui du lieu xuong KE02 neu TRUE
    uint8_t Sendto_flash; // Save flash neu TRUE
    uint8_t update_time; // Update time neu TRUE
}my_taskHandle_t;
//-----------------------------------------------------------------------------
typedef struct
{
    uint8_t ID;
    uint8_t length;
    uint16_t number;
    char data[FLASH_TRANSFER_SIZE];
}pxMessageFlash_t;

void MY_UART_DEBUG(const char *format, ...);
int test_json();

#ifdef __cplusplus
}
#endif
#endif