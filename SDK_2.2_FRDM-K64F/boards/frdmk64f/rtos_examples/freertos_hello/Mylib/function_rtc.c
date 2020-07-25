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


#include "function_rtc.h"


extern rtc_datetime_t       date_time;
extern packet_t 		    wqi_packet_sends; // gui du lieu len MC60
extern void send_config_message(packet_t *packet_send);
//---------------------------   STRUCT CREATE   -------------------------------
/*
 * RTC initialize time: Fri Sep 27 9:47:15 2013
 */
#define RTC_TIME_INIT_TM_YEAR   (2013)
#define RTC_TIME_INIT_TM_MON    (9)
#define RTC_TIME_INIT_TM_MDAY   (26)
#define RTC_TIME_INIT_TM_HOUR   (11)
#define RTC_TIME_INIT_TM_MIN    (15)
#define RTC_TIME_INIT_TM_SEC    (10)

#define ALARM_NEXT_TIME         (30)
#define ALARM_PERIOD            (4)
#define ALARM_MAX_TIME          (0xFFFFFFFF)

	//---------------------------------------------------------------
const char *_days_abbrev[] = {
  "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

const char *_months_abbrev[] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
//-----------------------------------------------------------------------------

extern board_config 		wqi_boards_config; // config default board
extern packet_t 			wqi_packet_sends; // gui du lieu len MC60

extern void uart_mc60_send(char data_buffer[UART_MAX_LENGTH_MESSAGE]);
/*FUNCTION**********************************************************************
 *
 * Function Name    : rtc_callback
 * Returned Value   :
 * Comments         :
 *    
 *
 *END**************************************************************************/

void rtc_task( void *pvParameters )
{
    rtc_config_t rtcConfig;
    
    /* Init RTC */
    /*
     * rtcConfig.wakeupSelect = false;
     * rtcConfig.updateMode = false;
     * rtcConfig.supervisorAccess = false;
     * rtcConfig.compensationInterval = 0;
     * rtcConfig.compensationTime = 0;
     */
    RTC_GetDefaultConfig(&rtcConfig);
    RTC_Init(RTC, &rtcConfig);
    
    /* Select RTC clock source */
    Board_SetRtcClockSource();
    
    /* Set a start date time and start RT default */
    /* Read flash and init RTC here */
    date_time.year   = RTC_TIME_INIT_TM_YEAR;
    date_time.month  = RTC_TIME_INIT_TM_MON;
    date_time.day    = RTC_TIME_INIT_TM_MDAY;
    date_time.hour   = RTC_TIME_INIT_TM_HOUR;
    date_time.minute = RTC_TIME_INIT_TM_MIN;
    date_time.second = RTC_TIME_INIT_TM_SEC;
    
    RTC_InitTimer( &date_time );
    
    MY_UART_DEBUG(">>>> RTC task call \r\n");
   
    for(;;)
    {
       /* Yeu cau dong bo thoi gian RTC voi he thong sau xxx*/
       wqi_packet_sends.addr_destination    = DST_ID_DEVICE_ADDRESS;
       wqi_packet_sends.addr_source         = MY_ID_DEVICE_ADDRESS;
       wqi_packet_sends.type_device         = TYPE_DEVICE_SENSORWATER;
       wqi_packet_sends.type_packet         = TYPE_PACKET_REQUEST;
       wqi_packet_sends.data_length         = strlen(STR_SYNC_TIME);
       wqi_packet_sends.data                = STR_SYNC_TIME;
       
       RTC_GetDatetime(RTC, &date_time);
       
       MY_UART_DEBUG("Current datetime: %04d-%02d-%02d %02d:%02d:%02d\r\n",\
               date_time.year, date_time.month, date_time.day, date_time.hour,\
               date_time.minute, date_time.second);
       
       // Send data 
       MY_UART_DEBUG("rtc_task request sys time \r\n");
       
       send_config_message( &wqi_packet_sends );
       
       /* dong bo thoi gian sau 24h */
       vTaskDelay( TIME_DELAY_SYNC_MC60_CYC_TEST );
    }
}
void Board_SetRtcClockSource(void)
{
    /* Enable the RTC 32KHz oscillator */
    RTC->CR |= RTC_CR_OSCE_MASK;
}

void RTC_InitTimer( const rtc_datetime_t *datetime )
{
    /* RTC time counter has to be stopped before setting the date & time in the TSR register */
    RTC_StopTimer(RTC);
    
    /* Set RTC time to default */
    RTC_SetDatetime(RTC, datetime);
    
    /* Enable RTC alarm interrupt */
    RTC_EnableInterrupts(RTC, kRTC_AlarmInterruptEnable);
    
    /* Enable at the NVIC */
    EnableIRQ(RTC_IRQn);

    /* Start the RTC time counter */
    RTC_StartTimer(RTC);
}