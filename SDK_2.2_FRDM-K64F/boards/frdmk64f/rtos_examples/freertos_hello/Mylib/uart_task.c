//#**************************************************************************#*/
//#     System name :   uart_task.c                                          #*/
//#     File name   :   uart_task.c                              	         #*/
//#     Summary     :                                                        #*/
//#     Author      :   Tuanhd                                               #*/
//#     Date        :   12/1/2017                                            #*/
//#                                                                          #*/
//#     Copyright (c) 2016 VNPT-Technology                                   #*/
//#                        All rights reserved.                              #*/
//#**************************************************************************#*/
//#  Version                   : Date        : Infomation                    #*/
//#  Major Ver.Minor Ver.Rev   : YYYY/MM/DD  :                               #*/
//#--------------------------------------------------------------------------#*/
//#     1.0.0                  : 12/1/2017  : New making                     #*/
//#**************************************************************************#*/

/******************************************************************************
 * Includes
 ******************************************************************************/
#include "main.h"
#include "uart_task.h"

//---------------------------   STRUCT extern    -------------------------------
extern char Wqi_data_sendtoMC60[UART_MAX_LENGTH_MESSAGE];

extern packet_t                 wqi_packet_receive;
extern my_taskHandle_t 		    xTask;
extern pxMessageFlash_t 	    tmpMessage;

extern packet_t 	            wqi_packet_sends;

extern board_config 		    wqi_boards_config;
extern rtc_datetime_t           date_time;

extern wqi_data                 wqi_data_read;
extern wqi_calculate            wqi_calculates;

extern void MY_UART_DEBUG(const char *format, ...);


//---------------------------   Buffer CREATE   -------------------------------
char data_MC60_receive[UART_MAX_LENGTH_MESSAGE];
char data_KE02_receive[UART_MAX_LENGTH_MESSAGE];

int countMC60Rx = 0;      // data count read from uart interrupt 
int countKE02Rx = 0;      // data count read from uart interrupt 

static     uint32_t     timeout, msec_base, diff,buff;
static     uint32_t 	msec_count = 1000;
static     char buff2[1];
/*Init*-------------------------------------------------------------------------
* Task Name : user_uartMC60_init
* Comments  : Config uart for communication mc60 
*             UART_MK60_CONF define is uart 2
*
*END*-------------------------------------------------------------------------*/
void request_data_sesor_task(void *pvParameters)
{

    for (;;)
    {
        uart_ke02_send("1,2,3,100,3,1:0!");
        /* test */
        wqi_DO_calculate_hours(2);
        wqi_PH_calculate_hours(3);        
        wqi_NH4_calculate_hours(4);        
        wqi_Turbi_calculate_hours(1);     
        
        MY_UART_DEBUG("----->> request_data_sesor_task\r\n");
        if(wqi_boards_config.enable_debug == DEBUG_DISABLE)
        {
          printf("Disable debug begin\r\n");
        }
        vTaskDelay(5000);
        //vTaskDelay(wqi_boards_config.time_send_server);
    }
}
/*Init*-------------------------------------------------------------------------
* Task Name : user_uartMC60_init
* Comments  : Config uart for communication mc60 
*             UART_MK60_CONF define is uart 2
*
*END*-------------------------------------------------------------------------*/
void user_uartmc60_init(void)
{
    uart_config_t  uart_config_mc60;
    /*
    * uart_config_mc60.baudRate_Bps = 115200U;
    * uart_config_mc60.parityMode = kUART_ParityDisabled;
    * uart_config_mc60.stopBitCount = kUART_OneStopBit;
    * uart_config_mc60.txFifoWatermark = 0;
    * uart_config_mc60.rxFifoWatermark = 1;
    * uart_config_mc60.enableTx = false;
    * uart_config_mc60.enableRx = false;
    */
    UART_GetDefaultConfig(&uart_config_mc60);
    uart_config_mc60.baudRate_Bps = UART_MC60_BAUDRATE;  
    uart_config_mc60.enableTx = true;
    uart_config_mc60.enableRx = true;
    
    UART_Init(UART_MC60_CONF, &uart_config_mc60, CLOCK_GetFreq(UART_MC60_CLKSRC));
    
    /* Enable RX interrupt. */
    NVIC_SetPriority(UART_MC60_IRQn, 6);
    UART_EnableInterrupts(UART_MC60_CONF, kUART_RxDataRegFullInterruptEnable \
                            | kUART_RxOverrunInterruptEnable);
    EnableIRQ(UART_MC60_IRQn);
    
    MY_UART_DEBUG(">> user_uartMC60_init OK>> \r\n");
}
/*Init*-------------------------------------------------------------------------
* Task Name : user_uartMC60_init
* Comments  :
*            
*            
*END*-------------------------------------------------------------------------*/
void user_uartke02_init(void)
{
    uart_config_t  uart_config_ke02;
    /*
    * uart_config_ke02.baudRate_Bps = 115200U;
    * uart_config_ke02.parityMode = kUART_ParityDisabled;
    * uart_config_ke02.stopBitCount = kUART_OneStopBit;
    * uart_config_ke02.txFifoWatermark = 0;
    * uart_config_ke02.rxFifoWatermark = 1;
    * uart_config_ke02.enableTx = false;
    * uart_config_ke02.enableRx = false;
    */
    UART_GetDefaultConfig(&uart_config_ke02);
    uart_config_ke02.baudRate_Bps = UART_KE02_BAUDRATE;  
    uart_config_ke02.enableTx = true;
    uart_config_ke02.enableRx = true;
    
    UART_Init(UART_KE02_CONF, &uart_config_ke02, CLOCK_GetFreq(UART_KE02_CLKSRC));
    
    NVIC_SetPriority(UART_KE02_IRQn,5);
    UART_EnableInterrupts(UART_KE02_CONF, kUART_RxDataRegFullInterruptEnable \
                            | kUART_RxOverrunInterruptEnable);
    EnableIRQ(UART_KE02_IRQn);
    
    MY_UART_DEBUG(">> user_uartKE02_init OK>> \r\n");
}
/*Init*-------------------------------------------------------------------------
* Task Name : RTCS_timer_get_interval
* Comments  : 
*            
*                         
*END*-------------------------------------------------------------------------*/
uint32_t RTCS_timer_get_interval( uint32_t start_time, uint32_t end_time)
{ 
    if(start_time <= end_time)
        return (end_time - start_time);
    else
        return (0xffffffff - start_time + end_time + 1);
}
/*Init*-------------------------------------------------------------------------
* Task Name : UART_MC60_IRQHandler
* Comments  : reciever data from uart MC60
*            
*                         
*END*-------------------------------------------------------------------------*/
void UART_MC60_IRQHandler( void )
{
     /* If new data arrived. */
    if ( UART_GetStatusFlags( UART_MC60_CONF ) )
    {
        /* check time out */
        msec_base = xTaskGetTickCount();
        
        if (msec_count == 0)  {
            timeout = (uint32_t)-1;
        } else  {
            timeout = msec_count;
        } /* Endif */
        
        data_MC60_receive[countMC60Rx] = UART_ReadByte(UART_MC60_CONF);
        buff2[0] = data_MC60_receive[countMC60Rx];
        countMC60Rx++;
        
        buff = xTaskGetTickCount();
        diff = RTCS_timer_get_interval(msec_base, buff);
        
        if (diff > msec_count)  {
             msec_count = 1000;
            //printf("time out yolo\r\n");
            memset(data_MC60_receive, 0x00, (unsigned char) sizeof(data_MC60_receive));
            countMC60Rx = 0;
            data_MC60_receive[countMC60Rx] = buff2[0];
            countMC60Rx++;
        } else  {
             msec_count -= diff;
        } /* Endif */
        
        if(countMC60Rx >= 150 - 1)
        {
            countMC60Rx = 0;
            memset(data_MC60_receive, 0x00,(unsigned char) sizeof(data_MC60_receive));
        }
        
        /* check data end '!' */
        if (data_MC60_receive[countMC60Rx - 1] == END_PACKET)
        {
            MY_UART_DEBUG("--- data read %s ---\r\n", data_MC60_receive);
            /* send message config true to save flash */
            MY_UART_DEBUG("Debug go to if \r\n");
                
            if (parse_message(&wqi_packet_receive,(char*)data_MC60_receive) == SUCCESS)
            {
                MY_UART_DEBUG(">>> Go to switch case >>> \r\n");
                switch (wqi_packet_receive.type_packet)
                {
                    case TYPE_PACKET_DATA:
                          MY_UART_DEBUG(">>> Go to TYPE_PACKET_DATA >>> \r\n");
                          break;
                    
                    case TYPE_PACKET_CONFIG: // thiet lap thong so cho mach
                            /* 
                            * 1:60 thoi gian dinh ki gui la 60p
                            * 2:60 docdu lieu dinh ki la 60p
                            * 5:So luong thong tin luu vao flash
                            * 6: key ma hoa ban tin
                            */ 
                            MY_UART_DEBUG(">>> Go to send_response_message(RESPONSE_SUCCESS) >>> \r\n");
                            if (active_message_config(&wqi_packet_receive) == SUCCESS) 
                            {
                                send_response_message(RESPONSE_SUCCESS);										
                            }
                            else 
                            {
                                MY_UART_DEBUG(">>> Go to send_response_message(RESPONSE_ERROR) >>> \r\n");
                                send_response_message(RESPONSE_ERROR);												
                            }
                            break;
                                                    
                    case TYPE_PACKET_REQUEST: // send back data setup
                           MY_UART_DEBUG(">>> Go to TYPE_PACKET_REQUEST >>> \r\n");
                           active_message_request(&wqi_packet_receive);											
                           break;
                    
                    case TYPE_PACKET_CONTROL: // 
                           MY_UART_DEBUG(">>> Go to TYPE_PACKET_CONTROL >>> \r\n");
                           break;
                    
                    case TYPE_PACKET_UPDATE: // 
                           MY_UART_DEBUG(">>> TYPE_PACKET_UPDATE >>> \r\n");
                           break;
                           
                    case TYPE_PACKET_DEBUG_ENABLE:
                            wqi_boards_config.enable_debug = DEBUG_ENABLE;
                           break;
                           
                    case TYPE_PACKET_DEBUG_DISABLE:
                            wqi_boards_config.enable_debug = DEBUG_DISABLE;
                           break;
                           
                    default:
                            MY_UART_DEBUG(">>> NONE >>> \r\n");
                            break;          
                }
            }
            else
            {
                send_response_message(RESPONSE_ERROR);
                memset(data_MC60_receive, 0x00, UART_MAX_LENGTH_MESSAGE);
                countMC60Rx = 0;
                return;
            }
            
            memset(data_MC60_receive, 0x00, UART_MAX_LENGTH_MESSAGE);
            countMC60Rx = 0;
        }
    }
}
/*Init*-------------------------------------------------------------------------
* Task Name : UART_MC60_IRQHandler
* Comments  : reciever data from uart KE02
*            
*                         
*END*-------------------------------------------------------------------------*/
void UART_KE02_IRQHandler( void )
{
     /* If new data arrived. */
    if ( UART_GetStatusFlags( UART_KE02_CONF ) )
    {
      	data_KE02_receive[countKE02Rx++] = UART_ReadByte(UART_KE02_CONF);
		
		if(countKE02Rx >= 150 - 1)
		{
			countKE02Rx = 0;
			memset(data_KE02_receive, 0x00,(unsigned char) sizeof(data_KE02_receive));
		}
		
		// data read
		// 3,1,3,101,59,51:0.000000,52:0.000000,53:0.000000,54:0.000000,55:0.000000!
		if(data_KE02_receive[countKE02Rx - 1] == '!')
		{
			MY_UART_DEBUG("data read %s\r\n",data_KE02_receive);
            
			parse_message_ke02((char *)data_KE02_receive);
            
			if(check_packet(data_KE02_receive) == 0)
			{
				MY_UART_DEBUG("packet read error \r\n");
			}
			else
			{
				MY_UART_DEBUG("************ check 2.1 ********\r\n");
				
				if(xTask.Sendto_MC60 == ACTIVE_SEND)
				{
					uart_mc60_send(data_KE02_receive);
					xTask.Sendto_MC60 = NON_SEND;
				}
				else
				{
					memset(Wqi_data_sendtoMC60,0x00,(unsigned char) sizeof(Wqi_data_sendtoMC60));
					memcpy(Wqi_data_sendtoMC60,data_KE02_receive,sizeof(data_KE02_receive));
				}
			}
			
			memset(data_KE02_receive,0x00,(unsigned char) sizeof(data_KE02_receive));
			countKE02Rx = 0;
		}
    }
}
/*Init*-------------------------------------------------------------------------
* Task Name : parse_message_ke02
* Comments  : parse message read from ke02
*            example 51:3.141592,52:00.000,53:00.000,54:00.000,55:00.000,!
*                                                                       ','
*
*END*-------------------------------------------------------------------------*/
void parse_message_ke02(char* data)
{
	char* newpch;
	char* newpch1;
	char* oldpch;
	char temp[20];
	char temp2[20];
	newpch = strchr(data, PADDING);
	oldpch = data;

    MY_UART_DEBUG("---->parse_message_ke02 call: %s \r\n", data);

	while (newpch != NULL)
	{
		memset(temp, 0x00, sizeof(temp));
		memset(temp2, 0x00, sizeof(temp2));

		memcpy(temp, oldpch, newpch - oldpch);
        
		newpch1 = strchr(temp, DIV_PAD);

		memcpy(temp2, temp, newpch1 - temp);
        
		/* save position and next PADDING */
		switch (atoi(temp2))
		{
			case 51:
				MY_UART_DEBUG("Cam bien nhiet ");
				wqi_data_read.TEMP = (float)atof(temp + 3 + 1);
				MY_UART_DEBUG("gia tri...%2.2f\r\n", wqi_data_read.TEMP);
				break;
			case 52:
				MY_UART_DEBUG("Cam bien PH ");
				wqi_data_read.PH = (float)atof(temp + 3);
				MY_UART_DEBUG("gia tri...%2.2f\r\n", wqi_data_read.PH);
				break;
			case 53:
				MY_UART_DEBUG("Cam bien DO ");
				wqi_data_read.DO = (float)atof(temp + 3);
				MY_UART_DEBUG("gia tri...%2.2f\r\n", wqi_data_read.DO);
				break;
			case 54:
				MY_UART_DEBUG("Cam bien NH4 ");
				wqi_data_read.NH4 = (float)atof(temp + 3);
				MY_UART_DEBUG("gia tri...%2.2f\r\n", wqi_data_read.NH4);
				break;
			case 55:
				MY_UART_DEBUG("Cam bien Tur ");
				wqi_data_read.TURBIDITY = (float)atof(temp + 3);
				MY_UART_DEBUG("gia tri...%2.2f\r\n", wqi_data_read.TURBIDITY);
				break;
			default:
				break;
		}
        
		oldpch = newpch + 1;
		newpch = strchr(oldpch + 1, PADDING);
	}
}
/*Init*-------------------------------------------------------------------------
* Task Name : KE02 SEND
* Comments  : uart of ke02 send data
*            
*                         
*END*-------------------------------------------------------------------------*/
void uart_ke02_send(char *data_buffer)
{
    char *ptr;
    
    ptr = malloc(UART_MAX_LENGTH_MESSAGE);
    
    memset(ptr, 0x00, UART_MAX_LENGTH_MESSAGE + 1);

    memcpy(ptr, data_buffer, UART_MAX_LENGTH_MESSAGE);
    /* Packing msg to packet array */
    sprintf(ptr, "1,2,3,100,3,1:0!");
    /* Send data to UART */
    UART_WriteBlocking( UART_KE02_CONF, (uint8_t const*)ptr, strlen(ptr));
    
    free(ptr);
    
    MY_UART_DEBUG("---->UART_KE02_SEND  \r\n");
}
/*Init*-------------------------------------------------------------------------
* Task Name : MC60 SEND
* Comments  : uart of mc60 send data
*            
*                         
*END*-------------------------------------------------------------------------*/
void uart_mc60_send(char *data_buffer)
{
    char *ptr;
    
    ptr = malloc(UART_MAX_LENGTH_MESSAGE);
    
    memcpy(ptr, data_buffer, UART_MAX_LENGTH_MESSAGE);
    /* Send data to UART */
    UART_WriteBlocking( UART_MC60_CONF, (uint8_t const*)ptr, strlen(ptr));
    
    free(ptr);
    
    MY_UART_DEBUG("---->UART_MC60_SEND  \r\n");
}
/*---------------------------------------------------------------------
  Function Name: isnumber
  Description:   
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------*/
bool isnumber(char *input) {
    for (int i = 0; input[i] != '\0'; i++)
            if (isalpha(input[i]))
                      return FAILURE;
    return SUCCESS;
}
/*---------------------------------------------------------------------
  Function Name: parse_message
  Description:   Phan tich chuoi ban tin nhan duoc theo cac truong 
                    da quy dinh
  Inputs:        cau truc ban tin quy dinh va ban tin nhan duoc
  Returns:       True/ Fail
-----------------------------------------------------------------------*/
uint8_t parse_message(packet_t *packet, char* data)
{
    char* ptr;
    char* newpch;
    char* oldpch;
    char temp[20];
    uint8_t count = 0;
    newpch = strchr(data, PADDING);
    oldpch = data;
    
    while (newpch != NULL)
    {
        if ((newpch - oldpch) > sizeof(temp))
        {
            memset(data_MC60_receive, 0x00, sizeof(data_MC60_receive));
            countMC60Rx = 0;
            MY_UART_DEBUG("->> Parse packet: check format data error\r\n");
            return FAILURE;
        }
        memset(temp, 0x00, sizeof(temp));
        memcpy(temp, oldpch, newpch - oldpch);

        /* check data from string parse */
        if (isnumber(temp) == FAILURE)
        {
            memset(data_MC60_receive, 0x00, sizeof(data_MC60_receive));
            countMC60Rx = 0;
            MY_UART_DEBUG("->> Parse packet: check format data error\r\n");
            return FAILURE;
        }

        switch (count)
        {
                      /* parse addr source */
            case 0:
                  packet->addr_source = atoi(temp);
                  break;

                  /* parse addr destination */
            case 1:
                  packet->addr_destination = atoi(temp);
                  break;

                  /* parse type packet */
            case 2:
                  packet->type_packet = atoi(temp);
                  break;

                  /* parse type device */
            case 3:
                  packet->type_device = atoi(temp);
                  break;

                  /* parse length data */
            case 4:
                  packet->data_length = atoi(temp);
                  ptr = strchr(data, END_PACKET);
                  /* Check data length */
                  if (packet->data_length < (ptr - newpch - 1))
                  {
                      memset(data_MC60_receive, 0x00, sizeof(data_MC60_receive));
                      countMC60Rx = 0;
                      MY_UART_DEBUG("->> Parse packet: check data length error: %d\r\n", (ptr - newpch - 1));
                      return FAILURE;
                  }
                  /* save data poiter */
            case 5:
                  packet->data = newpch + 1;
                  MY_UART_DEBUG("->> Data parse addr_source:%d---addr_dst:%d---type_packet:%d---type_device:%d---length_data:%d\r\n",
                            packet->addr_source, packet->addr_destination,
                            packet->type_packet, packet->type_device,
                            packet->data_length);
                  MY_UART_DEBUG("->> data:%s\r\n", packet->data);

                  return SUCCESS;

            default:
                   break;
        }

        /* save position and next PADDING */
        count++;
        oldpch = newpch + 1;
        newpch = strchr(oldpch + 1, PADDING);
    }

    /* don't success because not find data */
    return FAILURE;
}
/*----------------------------------------------------------------------
//----------------------- ACTIVE MESSAGE CONGFIG -----------------------
  Function Name: active_message_config
  Description:   
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------*/
uint8_t active_message_config(packet_t *packet)
{
	char* ptr;
	char* endptr;
	char temp[20];
	uint8_t status = FAILURE;
	uint16_t temp_value = 0;
	MY_UART_DEBUG("----------active_message_config call\r\n");
	// Check 
	if ((endptr = strchr(packet->data, END_PACKET)) == NULL)
	{
		MY_UART_DEBUG("->> Active message config error\r\n");
		return FAILURE;
	}
	/* Case config time send data */
	if((ptr = strstr(packet->data, STR_CONFIG_SEND_DATA)) != NULL)
	{
        if(strchr(ptr, PADDING) != NULL)
            endptr = strchr(ptr, PADDING);
        else endptr = strchr(ptr, END_PACKET);
        
        MY_UART_DEBUG("STR_CONFIG_SEND_DATA\r\n");
        
        memset(temp, 0x00, sizeof(temp));
        memcpy(temp, ptr + strlen(STR_CONFIG_SEND_DATA), endptr - ptr - strlen(STR_CONFIG_SEND_DATA));
        temp_value = atoi(temp);
        
        // Check
        if( isnumber(temp) == FAILURE )
        {
            MY_UART_DEBUG("->> Active message config time send data error\r\n");
            return FAILURE;
        }
        if( temp_value >= VALUE_TIME_MIN  && temp_value < VALUE_TIME_MAX )
            wqi_boards_config.time_send_server = temp_value;
        else
            return FAILURE;
        // Active
        status = SUCCESS;
        
        MY_UART_DEBUG("->> Config time send data :%d\r\n", wqi_boards_config.time_send_server);
	}
		
	/* Case config time read data */
	if ((ptr = strstr(packet->data, STR_CONFIG_READ_DATA)) != NULL)
	{
        MY_UART_DEBUG("STR_CONFIG_READ_DATA\r\n");
        if (strchr(ptr, PADDING) != NULL)
                endptr = strchr(ptr, PADDING);
        else endptr = strchr(ptr, END_PACKET);

        memset(temp, 0x00, sizeof(temp));
        memcpy(temp, ptr + strlen(STR_CONFIG_SEND_DATA), endptr - ptr - strlen(STR_CONFIG_SEND_DATA));

        // Check
        if (isnumber(temp) == FAILURE)
        {
            MY_UART_DEBUG("->> Active message config time read data error\r\n");
            return FAILURE;
        }

        // Active
        status = SUCCESS;
        wqi_boards_config.time_read_sensor = atoi(temp);
        MY_UART_DEBUG("->> Config time time_read_sensor :%d\r\n", wqi_boards_config.time_read_sensor);

	}

	/* Case config time save flash */
	if ((ptr = strstr(packet->data, STR_CONFIG_SAVE_FLASH)) != NULL)
	{
        MY_UART_DEBUG("STR_CONFIG_SAVE_FLASH\r\n");
        if (strchr(ptr, PADDING) != NULL)
                endptr = strchr(ptr, PADDING);
        else endptr = strchr(ptr, END_PACKET);
    
        memset(temp, 0x00, sizeof(temp));
        memcpy(temp, ptr + strlen(STR_CONFIG_SEND_DATA), endptr - ptr - strlen(STR_CONFIG_SEND_DATA));
    
        // Check
        if (isnumber(temp) == FAILURE)
        {
                MY_UART_DEBUG("->> Active message config time save flash error\r\n");
                return FAILURE;
        }
    
        // Active
        status = SUCCESS;
        wqi_boards_config.time_save_flashs = atoi(temp);
        MY_UART_DEBUG("->> Config time save flashs :%d\r\n", wqi_boards_config.time_save_flashs);
	}

	/* case config info save log */
	if ((ptr = strstr(packet->data, STR_CONFIG_LOG_INFO)) != NULL)
	{
		status = SUCCESS;
	}

	/* Case config key security */
	if ((ptr = strstr(packet->data, STR_CONFIG_KEY_SECR)) != NULL)
	{
		status = SUCCESS;
	}

	/* Case not find */
	if (status == SUCCESS)
		return SUCCESS;
	else
	{
		MY_UART_DEBUG("->> Don't find field config\r\n");
		return FAILURE;
	}
}
/*------------------------------------------------------------------------------
//----------------------- ACTIVE MESSAGE REQUEST -------------------------------
  Function Name: active_message_request
  Description:   yeu cau du lieu cua cam bien
  Inputs:        None
  Returns:       None
//----------------------------------------------------------------------------*/
uint8_t active_message_request(packet_t *packet)
{
    char *ptr = 0;
    char* endptr;
    char temp[20];
    
	MY_UART_DEBUG("active_message_request call\r\n");
    
    /* gui lenh update so lieu moi nhat tat ca cam bien voi ke02 */
    uart_ke02_send("1,2,3,100,3,1:0!");
    /* check xem  mau request da dung hay chua */
    if((endptr = strchr(packet->data, END_PACKET)) == NULL)
    {
        MY_UART_DEBUG("->> Active message request error\r\n");
        return FAILURE;
    }    
    /* Case request send all data cua cam bien */
    if(strstr(packet->data, STR_REQUEST_ALL_DATA) != NULL)
    {
        wqi_packet_sends.addr_source = MY_ID_DEVICE_ADDRESS;
        wqi_packet_sends.type_device = DST_ID_DEVICE_ADDRESS;
        wqi_packet_sends.type_packet = packet->type_packet;
        wqi_packet_sends.addr_destination = packet->addr_source;
        
        send_data_message(&wqi_packet_sends, NULL);
        //MY_UART_DEBUG("%s\r\n",data_MC60_receive);
        //uart_ke02_send(data_MC60_receive); // dang loi
        //xTask.Sendto_MC60 = ACTIVE_SEND;
    }   
    /* Case request send 1 data */
    else if((ptr = strstr(packet->data, STR_REQUEST_SENSOR)) != NULL)
    {
        memset(temp, 0x00, sizeof(temp));
        memcpy(temp, ptr + strlen(STR_REQUEST_SENSOR), endptr - ptr - strlen(STR_REQUEST_SENSOR));
        
        // Check
        if( isnumber(temp) == FAILURE )
        {
            MY_UART_DEBUG("->> Active message request error\r\n");
            return FAILURE;
        }
        
        wqi_packet_sends.addr_source = MY_ID_DEVICE_ADDRESS;
        wqi_packet_sends.type_device = DST_ID_DEVICE_ADDRESS;
        wqi_packet_sends.type_packet = packet->type_packet;
        wqi_packet_sends.addr_destination = packet->addr_source;
        
        send_data_message(&wqi_packet_sends, atoi( temp ));
        //MY_UART_DEBUG("%s\r\n",data_MC60_receive);
        //uart_ke02_send(data_MC60_receive); // dang loi
        //xTask.Sendto_MC60 = ACTIVE_SEND;
    }
    //-------------------------------- Yeu cau chi so WQI ----------------------
     /* Case request send all chi so wqi cua cam bien */
    else if(strstr(packet->data, STR_REQUEST_ALL_DATA_WQI) != NULL)
    {
        wqi_packet_sends.addr_source = MY_ID_DEVICE_ADDRESS;
        wqi_packet_sends.type_device = DST_ID_DEVICE_ADDRESS;
        wqi_packet_sends.type_packet = packet->type_packet;
        wqi_packet_sends.addr_destination = packet->addr_source;
        
        send_data_wqi_message(&wqi_packet_sends, NULL);
        //MY_UART_DEBUG("%s\r\n",data_MC60_receive);
        //uart_ke02_send(data_MC60_receive); // dang loi
        //xTask.Sendto_MC60 = ACTIVE_SEND;
    }   
    /* Case request send 1 data wqi */
    else if((ptr = strstr(packet->data, STR_REQUEST_SENSOR_WQI)) != NULL)
    {
        memset(temp, 0x00, sizeof(temp));
        memcpy(temp, ptr + strlen(STR_REQUEST_SENSOR_WQI), endptr - ptr - strlen(STR_REQUEST_SENSOR_WQI));
        
        // Check
        if( isnumber(temp) == FAILURE )
        {
            MY_UART_DEBUG("->> Active message request WQI error\r\n");
            return FAILURE;
        }
        
        wqi_packet_sends.addr_source = MY_ID_DEVICE_ADDRESS;
        wqi_packet_sends.type_device = DST_ID_DEVICE_ADDRESS;
        wqi_packet_sends.type_packet = packet->type_packet;
        wqi_packet_sends.addr_destination = packet->addr_source;
        
        send_data_wqi_message(&wqi_packet_sends, atoi( temp ));
        //MY_UART_DEBUG("%s\r\n",data_MC60_receive);
        //uart_ke02_send(data_MC60_receive); // dang loi
        //xTask.Sendto_MC60 = ACTIVE_SEND;
    }
    //--------------------------------------------------------------------------
    /* Case server setup time */
    else if((ptr = strstr(packet->data, STR_REQUSET_TIME)) != NULL)
    {
		MY_UART_DEBUG("---->Debug setup time call \r\n");
        uint8_t count = 0;
        char *newptr;
        char *oldptr;
        char time[5];
        
        // Copy both char '!' end_packet -> + 1 in memcpy
        memset(temp, 0x00, sizeof(temp));
        memcpy(temp, ptr + strlen(STR_REQUEST_SENSOR), endptr - ptr - strlen(STR_REQUEST_SENSOR) + 1);
        newptr = strchr(temp, PADDING_TIME); 
        oldptr = temp;
        
        while(newptr != NULL)
        {
            /* check size of data */
            if((newptr - oldptr) > sizeof(temp))
            {
                memset(data_MC60_receive, 0x00, sizeof(data_MC60_receive));
                MY_UART_DEBUG("->> Parse time: format error\r\n");
                return FAILURE;
            }
            
            memset(time, 0x00, sizeof(time));
            memcpy(time, oldptr, newptr - oldptr);
            
            /* check format of data */
            if( isnumber(temp) == FAILURE )
            {
                memset(data_MC60_receive, 0x00, sizeof(data_MC60_receive));
                MY_UART_DEBUG("->> Parse time: format error\r\n");
                return FAILURE;
            }
            /* parse time */
            switch(count)
            {
                case 0:
                    date_time.hour = atoi(time);
                    if( date_time.hour > 23 )
                        return FAILURE;
                    break;
                case 1:
                    date_time.minute = atoi(time);
                    if( date_time.minute > 59 )
                        return FAILURE;
                    break;
                case 2:
                    date_time.second = atoi(time);
                    if( date_time.second > 59 )
                        return FAILURE;
                    break;
                case 3:
                    date_time.day = atoi(time);
                    if( date_time.day > 33 )
                        return FAILURE;
                    break;
                case 4:
                    date_time.month = atoi(time);
                    if( date_time.month > 12 )
                        return FAILURE;
                    count++;
                    oldptr = newptr + 1;
                    newptr = strchr( oldptr + 1, END_PACKET );
                    goto labeeeee;
                case 5:
                    date_time.year = atoi(time);
                    break;
                default:
                    break;
            }
            count++;
            oldptr = newptr + 1;
            newptr = strchr( oldptr + 1, PADDING_TIME );
            labeeeee:;
        }
        
        RTC_InitTimer( &date_time );
        
        MY_UART_DEBUG("Current datetime: %04d-%02d-%02d %02d:%02d:%02d\r\n",\
               date_time.year, date_time.month, date_time.day, date_time.hour,\
               date_time.minute, date_time.second);
        
        send_response_message(RESPONSE_SUCCESS);
    }	
    return SUCCESS;	
}

void send_response_message(uint8_t status)
{
    char *ptr;
    ptr = (char*)malloc(UART_MAX_LENGTH_MESSAGE);
    
	memset(ptr, 0x00, UART_MAX_LENGTH_MESSAGE + 1);
	
	wqi_packet_sends.data_length = 1;
	wqi_packet_sends.type_device = wqi_packet_receive.type_device;
	wqi_packet_sends.type_packet = wqi_packet_receive.type_packet;
	wqi_packet_sends.addr_source = wqi_boards_config.myaddress;
	wqi_packet_sends.addr_destination = wqi_packet_receive.addr_source;
	
	/* Packing msg to packet array */
	sprintf(ptr,"%d,%d,%d,%d,%d,%d!",
					wqi_packet_sends.addr_source,
					wqi_packet_sends.addr_destination,
					wqi_packet_sends.type_packet,
					wqi_packet_sends.type_device,
					wqi_packet_sends.data_length,
					status);

	/* Send data to UART DEGUB */
    
	MY_UART_DEBUG(">>> DEBUG RESPON>> \r\n");
	MY_UART_DEBUG("%s \r\n",ptr);
	MY_UART_DEBUG("%d \r\n",strlen(ptr));
	MY_UART_DEBUG("send_response_message: %s and lengh : %d : LINE : %d\r\n", ptr, strlen(ptr), __LINE__);
	
    /* Send data to UART MC60 */
    uart_mc60_send(ptr);
    
	free(ptr);
}
/***************************************************************************************
**check number ',', check pos ':'
**in: packet
**return: failed or length_data_field
*****************************************************************************************/
int check_packet(char* packet){
    int i=0;
    volatile uint8_t pos_start_data =0;
    volatile uint8_t cnt_comma = 0;/* number ',' */
    int length_data_field = 0;
    uint8_t pos = 0;//pos ':' first

    if(packet[strlen(packet)-1] != '!') return 0;
    for(i=0; i<strlen(packet); i++){
        if(packet[i] == ':'){
            pos = i;
                MY_UART_DEBUG("pos = %d \r\n",pos);
            break;
        }
    }
    for(i = 0; i< pos; i++){//count ',' and find pos_start_data
        if(packet[i] == ','){
            cnt_comma++;
            if(cnt_comma == START_DATA){
                pos_start_data = i;//see packet format, pos_start_data = ',' fifth

            }else{};
        }else{};
    }
    
    MY_UART_DEBUG("\ncnt_comma = %d\r\n",cnt_comma);
    
    if(cnt_comma != START_DATA)
        return 0;
    else length_data_field = strlen(packet) - pos_start_data - 1 -1;// -1 is '!'

    return length_data_field;
}
/*------------------------------------------------------------------------------
  Function Name: send_data_message
  Description:    								 
  Inputs:        cau truc ban tin quy dinh va ban tin nhan duoc
  Returns:       True/ Fail
------------------------------------------------------------------------------*/
void send_data_message(packet_t *packet_send, uint8_t sensor_type)
{
    char *ptr;
    float tmp_answer            = 0;
    uint8_t tmp_sensor_type     = 0;

    ptr = (char*)malloc(UART_MAX_LENGTH_MESSAGE*sizeof(char));
    memset(ptr, 0x00, UART_MAX_LENGTH_MESSAGE + 1);
    if(sensor_type == NULL)
    {
        /* Send all data value */
        sprintf(ptr,"%d:%2.2f,%d:%2.2f,%d:%2.2f,%d:%2.2f,%d:%2.2f\r\n",
                TYPE_DEVICE_WQI_ID_Temp, wqi_data_read.TEMP,
                TYPE_DEVICE_WQI_ID_PH, wqi_data_read.PH,
                TYPE_DEVICE_WQI_ID_DO, wqi_data_read.DO,
                TYPE_DEVICE_WQI_ID_NH4, wqi_data_read.NH4,
                TYPE_DEVICE_WQI_ID_Turbidity, wqi_data_read.TURBIDITY);

        packet_send->data_length = strlen(ptr);

        /* Packing msg to packet array */
        sprintf(ptr, "%d,%d,%d,%d,%d,%d:%2.2f,%d:%2.2f,%d:%2.2f,%d:%2.2f,%d:%2.2f!\r\n",\
                wqi_packet_sends.addr_source,\
                wqi_packet_sends.addr_destination,\
                wqi_packet_sends.type_packet,\
                wqi_packet_sends.type_device,\
                wqi_packet_sends.data_length,\
                TYPE_DEVICE_WQI_ID_Temp, wqi_data_read.TEMP,\
                TYPE_DEVICE_WQI_ID_PH, wqi_data_read.PH,\
                TYPE_DEVICE_WQI_ID_DO, wqi_data_read.DO,\
                TYPE_DEVICE_WQI_ID_NH4, wqi_data_read.NH4,\
                TYPE_DEVICE_WQI_ID_Turbidity, wqi_data_read.TURBIDITY);
    }
    
    /* Request data from sensor I */
    else
    {
        MY_UART_DEBUG("->> Request wqi message: Sensor %d \r\n", sensor_type);
        
        if( sensor_type ==  TYPE_DEVICE_WQI_ID_PH)
        {
            tmp_sensor_type     = sensor_type;
            tmp_answer          = wqi_data_read.PH;
        }
        else if( sensor_type == TYPE_DEVICE_WQI_ID_DO )
        {
            tmp_sensor_type     = sensor_type;
            tmp_answer          = wqi_data_read.DO;
        }
        else if( sensor_type == TYPE_DEVICE_WQI_ID_Temp )
        {
            tmp_sensor_type     = sensor_type;
            tmp_answer          = wqi_data_read.TEMP;
        }
        else if( sensor_type == TYPE_DEVICE_WQI_ID_NH4 )
        {
            tmp_sensor_type     = sensor_type;
            tmp_answer          = wqi_data_read.NH4;
        }
        else if( sensor_type == TYPE_DEVICE_WQI_ID_Turbidity )
        {
            tmp_sensor_type     = sensor_type;
            tmp_answer          = wqi_data_read.TURBIDITY;
        }
        else
        {
            MY_UART_DEBUG("->> Don't find information sensor in sensorboard\r\n");
            return;
        }
				
        sprintf(ptr, "%d:%2.2f", tmp_sensor_type, tmp_answer);
        packet_send->data_length = strlen(ptr);
        
        /* Packing msg to packet array */
        sprintf(ptr, "%d,%d,%d,%d,%d,%d:%2.2f!\r\n",
                wqi_packet_sends.addr_source,
                wqi_packet_sends.addr_destination,
                wqi_packet_sends.type_packet,
                wqi_packet_sends.type_device,
                wqi_packet_sends.data_length,
                tmp_sensor_type,
				tmp_answer );
    }
    
    /* Send data to UART */
    UART_WriteBlocking( UART_MC60_CONF, (uint8_t const*)ptr, strlen(ptr) );
    
    free(ptr);
}
/*---------------------------------------------------------------------
  Function Name: send_config_message
  Description:    								 
  Inputs:        cau truc ban tin quy dinh va ban tin nhan duoc
  Returns:       
-----------------------------------------------------------------------*/
void send_config_message(packet_t *packet_send)
{
    char *ptr;
    char temp[100];
    
    ptr = (char*)malloc(UART_MAX_LENGTH_MESSAGE*sizeof(char));
    memset(ptr, 0x00, UART_MAX_LENGTH_MESSAGE + 1);
    memset(temp, 0x00, sizeof(temp));
    memcpy(temp, packet_send->data, packet_send->data_length);
    
    sprintf(ptr, "%d,%d,%d,%d,%d,%s!",
            packet_send->addr_source,
            packet_send->addr_destination,
            packet_send->type_packet,
            packet_send->type_device,
            packet_send->data_length,
            temp);
    
    /* Send data to UART */
    
    //UART_WriteBlocking( UART_MC60_CONF, (uint8_t const*)ptr, strlen(ptr) ); 
    
    MY_UART_DEBUG("test request sys time %s\r\n",ptr);
    
    uart_mc60_send(ptr);
    
    free( ptr );
    /* Send data to UART */
}
/*------------------------------------------------------------------------------
  Function Name: send_data_wqi_message
  Description:    								 
  Inputs:        gui gia tri WQI theo yeu vau tung chat hoac tat ca cac chat
  Returns:       
------------------------------------------------------------------------------*/
void send_data_wqi_message(packet_t *packet_send, uint8_t sensor_type)
{
    char *ptr;
    float tmp_answer            = 0;
    uint8_t tmp_sensor_type     = 0;

    ptr = (char*)malloc(UART_MAX_LENGTH_MESSAGE*sizeof(char));
    memset(ptr, 0x00, UART_MAX_LENGTH_MESSAGE + 1);
    
    if(sensor_type == NULL)
    {
        /* Send all data value */
        sprintf(ptr,"%d:%2.2f,%d:%2.2f,%d:%2.2f,%d:%2.2f\r\n",
                TYPE_DEVICE_WQI_ID_PH, wqi_calculates.WQI_PH,
                TYPE_DEVICE_WQI_ID_DO, wqi_calculates.WQI_DO,
                TYPE_DEVICE_WQI_ID_NH4, wqi_calculates.WQI_NH4,
                TYPE_DEVICE_WQI_ID_Turbidity, wqi_calculates.WQI_TURBIDITY);

        packet_send->data_length = strlen(ptr);

        /* Packing msg to packet array */
        sprintf(ptr, "%d,%d,%d,%d,%d,%d:%2.2f,%d:%2.2f,%d:%2.2f,%d:%2.2f!\r\n",\
                wqi_packet_sends.addr_source,\
                wqi_packet_sends.addr_destination,\
                wqi_packet_sends.type_packet,\
                wqi_packet_sends.type_device,\
                wqi_packet_sends.data_length,\
                TYPE_DEVICE_WQI_ID_PH, wqi_calculates.WQI_PH,\
                TYPE_DEVICE_WQI_ID_DO, wqi_calculates.WQI_DO,\
                TYPE_DEVICE_WQI_ID_NH4, wqi_calculates.WQI_NH4,\
                TYPE_DEVICE_WQI_ID_Turbidity, wqi_calculates.WQI_TURBIDITY);
    }
    
    /* Request data from sensor I */
    else
    {
        MY_UART_DEBUG("->> Request message: Sensor %d \r\n", sensor_type);
        
        if( sensor_type ==  TYPE_DEVICE_WQI_ID_PH)
        {
            tmp_sensor_type     = sensor_type;
            tmp_answer          = wqi_calculates.WQI_PH;
        }
        else if( sensor_type == TYPE_DEVICE_WQI_ID_DO )
        {
            tmp_sensor_type     = sensor_type;
            tmp_answer          = wqi_calculates.WQI_DO;
        }
        else if( sensor_type == TYPE_DEVICE_WQI_ID_NH4 )
        {
            tmp_sensor_type     = sensor_type;
            tmp_answer          = wqi_calculates.WQI_NH4;
        }
        else if( sensor_type == TYPE_DEVICE_WQI_ID_Turbidity )
        {
            tmp_sensor_type     = sensor_type;
            tmp_answer          = wqi_calculates.WQI_TURBIDITY;
        }
        else
        {
            MY_UART_DEBUG("->> Don't find information sensor in sensorboard\r\n");
            return;
        }
				
        sprintf(ptr, "%d:%2.2f", tmp_sensor_type, tmp_answer);
        packet_send->data_length = strlen(ptr);
        
        /* Packing msg to packet array */
        sprintf(ptr, "%d,%d,%d,%d,%d,%d:%2.2f!\r\n",
                wqi_packet_sends.addr_source,
                wqi_packet_sends.addr_destination,
                wqi_packet_sends.type_packet,
                wqi_packet_sends.type_device,
                wqi_packet_sends.data_length,
                tmp_sensor_type,
				tmp_answer );
    }
    
    /* Send data to UART */
    UART_WriteBlocking( UART_MC60_CONF, (uint8_t const*)ptr, strlen(ptr) );
    
    free(ptr);
}
//-------------------------------------- END ----------------------------------