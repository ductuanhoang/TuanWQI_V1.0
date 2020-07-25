/*#**************************************************************************#*/
/*#     System name :   Water  measurement                             	     #*/
/*#     File name   :   function_spi.c                              	     #*/
/*#     Summary     :   Parameter config for all sensor Water                #*/
/*#     Author      :   Tuanhd                                         		 #*/
/*#     Date        :   30/03/2017                                           #*/
/*#                                                                          #*/
/*#     Copyright (c) 2017 VNPT-Technology                                   #*/
/*#                        All rights reserved.                              #*/
/*#**************************************************************************#*/
/*#  Version                   : Date        : Infomation                    #*/
/*#  Major Ver.Minor Ver.Rev   : YYYY/MM/DD  :                               #*/
/*#--------------------------------------------------------------------------#*/
/*#     1.0.0                  : 30/03/2017  : New making                    #*/
/*#**************************************************************************#*/


#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <timers.h>
#include <main.h>
#include "function_spi.h"
#include <slld_hal.h>

//------------------------------------------------------------------------------
extern rtc_datetime_t date_time;
extern board_config wqi_boards_config;
extern void MY_UART_DEBUG(const char *format, ...);

dspi_rtos_handle_t dspi_master_rtos_handle;
uint8_t masterSaveBuffer[FLASH_TRANSFER_SIZE] = {0};
uint8_t masterSendBuffer[FLASH_TRANSFER_SIZE] = {0};
uint8_t masterReceiveBuffer[FLASH_TRANSFER_SIZE] = {0};
uint32_t addressFlash;
/*FUNCTION*---------------------------------------------------------------------
*
* Function Name : flash_task
* Comments  : 
*
*END*-------------------------------------------------------------------------*/
void flash_task( void *pvParameters )
{
    uint32_t sourceClock;
    uint32_t temp_flash;
    
    pxMessageFlash_t pxMessageFlash;
    dspi_transfer_t masterXfer;
    dspi_master_config_t masterConfig;
    status_t status;
    
    gpio_pin_config_t pincs_config = {
        kGPIO_DigitalOutput, 0,
    };
    
    /* Chip Select && pin mosi config*/
    CLOCK_EnableClock(BOARD_CHIPSELECT_CLOCK);
    PORT_SetPinMux( BOARD_CHIPSELECT_PORT, BOARD_CHIPSELECT_GPIO_PIN, kPORT_MuxAsGpio);
    GPIO_PinInit( BOARD_CHIPSELECT_GPIO, BOARD_CHIPSELECT_GPIO_PIN, &pincs_config );
    
    ENABLE_CHIPSELECT;
    DISABLE_CHIPSELECT;
     /* Master config */
    masterConfig.whichCtar = kDSPI_Ctar0;
    masterConfig.ctarConfig.baudRate = FLASH_TRANSFER_BAUDRATE;
    masterConfig.ctarConfig.bitsPerFrame = 8;
    masterConfig.ctarConfig.cpol = kDSPI_ClockPolarityActiveHigh;
    masterConfig.ctarConfig.cpha = kDSPI_ClockPhaseFirstEdge;
    masterConfig.ctarConfig.direction = kDSPI_MsbFirst;
    masterConfig.ctarConfig.pcsToSckDelayInNanoSec = 0;
    masterConfig.ctarConfig.lastSckToPcsDelayInNanoSec = 0;
    masterConfig.ctarConfig.betweenTransferDelayInNanoSec = 0;
    
    masterConfig.whichPcs = kDSPI_Pcs0;
    masterConfig.pcsActiveHighOrLow = kDSPI_PcsActiveLow;
    
    masterConfig.samplePoint = kDSPI_SckToSin0Clock;
    masterConfig.enableContinuousSCK = false;
    masterConfig.enableRxFifoOverWrite = false;
    masterConfig.enableModifiedTimingFormat = false;
    
#if (FLASH_DSPI_MASTER_BASE == SPI0_BASE)
    NVIC_SetPriority(SPI0_IRQn, 6);
#elif(FLASH_DSPI_MASTER_BASE == SPI1_BASE)
    NVIC_SetPriority(SPI1_IRQn, 6);
#elif(FLASH_DSPI_MASTER_BASE == SPI2_BASE)
    NVIC_SetPriority(SPI2_IRQn, 6);
#elif(FLASH_DSPI_MASTER_BASE == SPI3_BASE)
    NVIC_SetPriority(SPI3_IRQn, 6);
#elif(FLASH_DSPI_MASTER_BASE == SPI4_BASE)
    NVIC_SetPriority(SPI4_IRQn, 6);
#endif
    
    sourceClock = CLOCK_GetFreq(DSPI_MASTER_CLK_SRC);
    status = DSPI_RTOS_Init(&dspi_master_rtos_handle, FLASH_DSPI_MASTER_BASEADDR, &masterConfig, sourceClock);
    
    if (status != kStatus_Success)
    {
        MY_UART_DEBUG("->> DSPI master: error during initialization. \r\n");
        vTaskSuspend( NULL );
    }
    /* Read info config from flash */
    //setAddressPoiterFlash(ADDRESS_ORGIN_LOG_SAVE);
    addressFlash = getAddressPoiterFlash();
    if(temp_flash = getTimeSendDataFlash())
        wqi_boards_config.time_send_server = temp_flash;
    if(temp_flash = getTimeReadSensorFlash())
        wqi_boards_config.time_read_sensor = temp_flash;
    
}
/* Function: 	This function first erase sector in sys_addr, after write data to it
 * Parameters:	NONE. 
 * Returns: 	SLLD_OK if OK
 *              SLLD_E_DEVICE_SOFTWARE_PROTECTED
 * 		SLLD_E_HAL_ERROR
 *              SLLD_ERROR
 */
SLLD_STATUS slld_WriteOp1(ADDRESS sys_addr, BYTE *data_buf, BYTECOUNT len_in_bytes)
{
    DEVSTATUS  status;
    SLLD_STATUS status_erase;
    
    status_erase = slld_SEOp1( sys_addr );
    
    if( status_erase == SLLD_ERROR )
        return SLLD_ERROR;
    if( len_in_bytes > 256 )
        return SLLD_ERROR;
    
    return slld_BufferedProgramOp(sys_addr, data_buf, len_in_bytes, &status);
}

/* Function: 	This function first erase sector in sys_addr, after write data to it
 * Parameters:	NONE. 
 * Returns: 	SLLD_OK if OK
 *              SLLD_E_DEVICE_SOFTWARE_PROTECTED
 * 		SLLD_E_HAL_ERROR
 *              SLLD_ERROR
 */
SLLD_STATUS slld_WriteOp2(ADDRESS sys_addr, BYTE *data_buf, BYTECOUNT len_in_bytes)
{
    DEVSTATUS  status;
//    SLLD_STATUS status_erase;
//    
//    status_erase = slld_SEOp1( sys_addr );
//    
//    if( status_erase == SLLD_ERROR )
//        return SLLD_ERROR;
    if( len_in_bytes > 256 )
        return SLLD_ERROR;
    
    return slld_BufferedProgramOp(sys_addr, data_buf, len_in_bytes, &status);
}

/* Function: 	This function erase in a address special in sector to end that sector.
 * Parameters:	NONE. 
 * Returns: 	SLLD_OK if OK
 *              SLLD_E_DEVICE_SOFTWARE_PROTECTED
 * 		SLLD_E_HAL_ERROR
 *              SLLD_ERROR
 */
SLLD_STATUS slld_SEOp1( ADDRESS sys_addr )
{
    DEVSTATUS  status;
    return slld_P4EOp( sys_addr, &status );
}

/* Function: 	This function erase all data from flash. <><WARNING><> 
 * Parameters:	NONE.
 * Returns: 	SLLD_OK if OK
 *              SLLD_E_DEVICE_SOFTWARE_PROTECTED
 * 		SLLD_E_HAL_ERROR
 *              SLLD_ERROR
 */
SLLD_STATUS slld_BEOp1( void )
{
    DEVSTATUS  status;
    return slld_BEOp(&status);
}

/* Function: 	This function write value time send data cycle in address define 
 * Parameters:	NONE. address save in define ADDRESS_TIME_READ_SENSOR
 * Returns: 	SLLD_OK if OK
 *              SLLD_E_DEVICE_SOFTWARE_PROTECTED
 * 		SLLD_E_HAL_ERROR
 *              SLLD_ERROR
 */
SLLD_STATUS setTimeSendDataFlash( uint32_t time )
{
    char strTime[20];
    
    memset( strTime, 0x00, sizeof( strTime ) );
    sprintf( strTime, "%d", time );
    
    /* Check */
    if( time > VALUE_TIME_MAX )
        return SLLD_ERROR;

    if( isnumber( strTime ) == FAILURE )
        return SLLD_ERROR;
    
    return slld_WriteOp1( ADDRESS_TIME_SEND_DATA, (BYTE*)strTime, strlen( strTime ) );
}

/* Function: 	This function get time send data cycle in flash 
 * Parameters:	NONE. address save poiter in define ADDRESS_TIME_SEND_DATA
 * Returns: 	value address. ex 8MB max 7FFFFFh
 * 		
 */
uint32_t getTimeSendDataFlash( void )
{
    uint32_t valueTime = 0;
    char strTime[20];
    
    memset( strTime, 0x00, sizeof( strTime ) );
    
    slld_ReadOp( ADDRESS_TIME_SEND_DATA, (BYTE*)strTime, LENGTH_DISTANCE_TIME );
    
    //  for( int i = 0; i < sizeof(strTime); i++ )
    //    if(strTime[i] == 0xFF)
    //      strTime[i] = 0x00;
    
    if(isnumber(strTime) == FAILURE)
    {
        MY_UART_DEBUG("->> Time read not typenumber\r\n");
        return -1;
    }
    else
        valueTime = atoi( strTime );
    
    if( valueTime > VALUE_TIME_MAX )
        return -1;
    return valueTime;
}

/* Function: 	This function write value time read sensor in address define 
 * Parameters:	NONE. address save in define ADDRESS_TIME_READ_SENSOR
 * Returns: 	SLLD_OK if OK
 *              SLLD_E_DEVICE_SOFTWARE_PROTECTED
 * 		SLLD_E_HAL_ERROR
 *              SLLD_ERROR
 */
SLLD_STATUS setTimeReadSensorFlash( uint32_t time )
{
    char strTime[20];
    
    memset( strTime, 0x00, sizeof( strTime ) );
    sprintf( strTime, "%d", time );
    
    /* Check */
    if( time > VALUE_TIME_MAX )
        return SLLD_ERROR;

    if( isnumber( strTime ) == FAILURE )
        return SLLD_ERROR;
    
    return slld_WriteOp1( ADDRESS_TIME_READ_SENSOR, (BYTE*)strTime, strlen( strTime ) );
}

/* Function: 	This function get time read sensor cycle in flash 
 * Parameters:	NONE. address save poiter in define ADDRESS_TIME_READ_SENSOR
 * Returns: 	value address. ex 8MB max 7FFFFFh
 * 		
 */
uint32_t getTimeReadSensorFlash( void )
{
    uint32_t valueTime = 0;
    char strTime[20];
    
    memset( strTime, 0x00, sizeof( strTime ) );
    
    slld_ReadOp( ADDRESS_TIME_READ_SENSOR, (BYTE*)strTime, LENGTH_DISTANCE_TIME );
    
    //  for( int i = 0; i < sizeof(strTime); i++ )
    //    if(strTime[i] == 0xFF)
    //      strTime[i] = 0x00;
    
    if(isnumber(strTime) == FAILURE)
    {
        MY_UART_DEBUG("->> Time read not typenumber\r\n");
        return -1;
    }
    else
        valueTime = atoi( strTime );
    
    if( valueTime > VALUE_TIME_MAX )
        return -1;
    return valueTime;
}

/* Function: 	This function write value poiter address flash in address define 
 * Parameters:	NONE. address save poiter in define ADDRESS_POITER_ADDRESS_FLASH
 * Returns: 	SLLD_OK if OK
 *              SLLD_E_DEVICE_SOFTWARE_PROTECTED
 * 		SLLD_E_HAL_ERROR
 *              SLLD_ERROR
 */
SLLD_STATUS setAddressPoiterFlash( uint32_t address )
{
    char strAddress[20];
    
    memset( strAddress, 0x00, sizeof( strAddress ) );
    sprintf( strAddress, "%d", address );
    
    /* Check */
    if( address > VALUE_ADDRESS_FLASH_MAX )
    {
        address = ADDRESS_ORGIN_LOG_SAVE;
        addressFlash = ADDRESS_ORGIN_LOG_SAVE;
        MY_UART_DEBUG("->> Write flash interrupt [MAX ADDRESS]\r\n");
        return SLLD_ERROR;
    }

    if( isnumber( strAddress ) == FAILURE )
        return SLLD_ERROR;
    
    return slld_WriteOp1( ADDRESS_POITER_ADDRESS_FLASH, (BYTE*)strAddress, strlen( strAddress ) );
}

/* Function: 	This function get value of poiter address of flash. 
 * Parameters:	NONE. address save poiter in define ADDRESS_POITER_ADDRESS_FLASH
 * Returns: 	value address. ex 8MB max 7FFFFFh
 * 		
 */
uint32_t getAddressPoiterFlash( void )
{
    uint32_t valueAddress = 0;
    char strAddress[20];
    
    memset( strAddress, 0x00, sizeof( strAddress ) );
    
    slld_ReadOp( ADDRESS_POITER_ADDRESS_FLASH, (BYTE*)strAddress, LENGTH_DISTANCE_POITER );
    
    //  for( int i = 0; i < sizeof(strTime); i++ )
    //    if(strTime[i] == 0xFF)
    //      strTime[i] = 0x00;
    
    if(isnumber(strAddress) == FAILURE)
    {
        MY_UART_DEBUG("->> address read not typenumber\r\n");
        return -1;
    }
    else
        valueAddress = atoi( strAddress );
    
    return valueAddress;
}

void print_log(uint32_t beginAddress, uint32_t endAddress)
{
    uint8_t length = VALUE_LENGTH_PRINT_LOG;
    uint32_t tempAddress = beginAddress;
    
    while(tempAddress < endAddress)
    {
        memset(masterSaveBuffer, 0x00, sizeof(masterSaveBuffer));
        slld_ReadOp(tempAddress, masterSaveBuffer, length);
        
        MY_UART_DEBUG("->> Log flash: %s\r\n", masterSaveBuffer);
        tempAddress += length;
    }
}

void eraseAreaLogFlash()
{
    DEVSTATUS   status;
    uint32_t    address = ADDRESS_ORGIN_LOG_SAVE;
    
    /* check address */
    if(addressFlash < ADDRESS_ORGIN_LOG_SAVE || addressFlash > VALUE_ADDRESS_FLASH_MAX)
        return;
    
    /* erase */
    while(address < addressFlash)
    {
        slld_P4EOp( address, &status );
        address += SECTOR_SIZE_;
    }
    
    addressFlash = ADDRESS_ORGIN_LOG_SAVE;
    setAddressPoiterFlash(addressFlash);
    
    return;
}
