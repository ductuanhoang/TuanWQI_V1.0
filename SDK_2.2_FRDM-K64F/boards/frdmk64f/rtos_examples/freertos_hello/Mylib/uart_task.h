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


#ifndef __UART_TASK_H__
#define __UART_TASK_H__
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
/*********************************************************************
* CONSTANTS
*/
//-----------------------------------------------------------------------------
typedef enum _type_packet
{
    /* Send data */             
    TYPE_PACKET_DATA = 1U,      

    /* Send config infor */     
    TYPE_PACKET_CONFIG = 2U,    

    /* Send request */          
    TYPE_PACKET_REQUEST = 3U,   

    /* Send Control */          
    TYPE_PACKET_CONTROL = 4U,   

    /* Send update firmware */  
    TYPE_PACKET_UPDATE = 255U,   
      
     /* enable disable debug */ 
    TYPE_PACKET_DEBUG_ENABLE = 128U,
           /* enable disable debug */ 
    TYPE_PACKET_DEBUG_DISABLE = 127U
	
}type_packet;

//-----------------------------Cau truc ban tin phan tich----------------------
#define PADDING                         ','
#define END_PACKET                      '!'
#define DIV_PAD                         ':'

#define PADDING_TIME                    '/'    
#define ENABLE_DEBUG_PACKET		        '#'
	
#define PADDING_RESPONSE_SUCCESS        '0'
#define PADDING_RESPONSE_ERROR          '1'

//------------------------------- Lenh Config ----------------------------------
#define STR_CONFIG_SEND_DATA            "1:"
#define STR_CONFIG_READ_DATA            "2:"

#define STR_CONFIG_SAVE_FLASH           "3:"

//#define STR_CONFIG_SOCKET               "4:"
#define STR_CONFIG_LOG_INFO             "5:"
#define STR_CONFIG_KEY_SECR             "6:"

//------------------------------ Lenh request ----------------------------------
#define STR_REQUEST_ALL_DATA            "1:"
#define STR_REQUEST_SENSOR              "2:"
#define STR_REQUSET_TIME                "3:"

#define STR_REQUEST_ALL_DATA_WQI        "4:"
#define STR_REQUEST_SENSOR_WQI          "5:"
//#define STR_REQUEST_PRINT_LOG           "4:"
//#define STR_REQUEST_INFO_CONFIG         "5:"

//------------------------------------------------------------------------------
#define VALUE_TIME_MIN                  5
#define VALUE_TIME_MAX                  1439
//-----------------------------------------------------------------------------
#define UART_MAX_LENGTH_MESSAGE         150

#define FAILURE                         1
#define SUCCESS                         0 
#define START_DATA              	    5 //see packet format, pos_start_data = ',' fifth
//-----------------------------------------------------------------------------
/* UART MK60 Config and define */
#define UART_MC60_CONF                  UART0
#define UART_MC60_BAUDRATE              115200
#define UART_MC60_CLKSRC                UART0_CLK_SRC
#define UART_MC60_IRQn                  UART0_RX_TX_IRQn
#define UART_MC60_IRQHandler            UART0_RX_TX_IRQHandler

/* UART KE02 Config and define */  
#define UART_KE02_CONF                  UART4
#define UART_KE02_BAUDRATE              115200
#define UART_KE02_CLKSRC                UART4_CLK_SRC
#define UART_KE02_IRQn                  UART4_RX_TX_IRQn
#define UART_KE02_IRQHandler            UART4_RX_TX_IRQHandler

/* UART for DEBUG Config and define */
#define UART_DEBUG_CONF                 UART3
#define UART_DEBUG_BAUDRATE             115200
#define UART_DEBUG_CLKSRC               UART3_CLK_SRC
#define UART_DEBUG_IRQn                 UART3_RX_TX_IRQn
#define UART_DEBUG_IRQHandler           UART3_RX_TX_IRQHandler  

//------------------------------- Function for uart use ------------------------
void request_data_sesor_task(void *pvParameters);

void user_uartmc60_init(void);

void uart_mc60_send(char *data_buffer);

void parse_message_ke02(char* data);

void user_uartke02_init(void);

void uart_ke02_send(char *data_buffer);
//-----------------------------------------------------------------------------
typedef struct
{
    uint32_t    addr_source;
    uint32_t    addr_destination;
    uint8_t     type_packet;
    uint8_t     type_device;
    uint16_t    data_length;
    char * data;
}packet_t;


//------------------------------- Function for parser---------------------------
uint8_t active_message_config(packet_t *packet);
uint8_t active_message_request(packet_t *packet);
void send_response_message(uint8_t status);
uint32_t RTCS_timer_get_interval( uint32_t start_time, uint32_t end_time);
void acction(char *data_receive);
uint8_t parse_message(packet_t *packet, char* data);
bool isnumber(char *input);
int check_packet(char* packet);
void send_data_message(packet_t *packet_send, uint8_t sensor_type);
void send_data_wqi_message(packet_t *packet_send, uint8_t sensor_type);
//------------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif
#endif