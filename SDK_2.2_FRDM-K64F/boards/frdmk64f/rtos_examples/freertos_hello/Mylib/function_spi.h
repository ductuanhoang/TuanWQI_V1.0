/*#**************************************************************************#*/
/*#     System name :   Water  measurement                             	     #*/
/*#     File name   :   function_spi.h                              	     #*/
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

#ifndef __FUNCTION_SPI_H__
#define __FUNCTION_SPI_H__
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

    
// Chip select 
#define BOARD_CHIPSELECT_PORT           PORTE
#define BOARD_CHIPSELECT_GPIO           GPIOE
#define BOARD_CHIPSELECT_CLOCK          kCLOCK_PortE
#define BOARD_CHIPSELECT_GPIO_PIN       (0U)

// MOSI
#define BOARD_MOSI_PORT                 PORTE
#define BOARD_MOSI_GPIO                 GPIOE
#define BOARD_MOSI_CLOCK                kCLOCK_PortE
#define BOARD_MOSI_GPIO_PIN             (3U)
    
#define FLASH_TRANSFER_SIZE (256)

#define FLASH_TRANSFER_BAUDRATE         (500000U)
#define FLASH_DSPI_MASTER_BASE          SPI1_BASE

#define FLASH_DSPI_MASTER_BASEADDR ((SPI_Type *)FLASH_DSPI_MASTER_BASE)
#define FLASH_DSPI_SLAVE_BASEADDR ((SPI_Type *)FLASH_DSPI_SLAVE_BASE)

#if (FLASH_DSPI_MASTER_BASE == SPI0_BASE)
#define DSPI_MASTER_CLK_SRC (DSPI0_CLK_SRC)
#elif(FLASH_DSPI_MASTER_BASE == SPI1_BASE)
#define DSPI_MASTER_CLK_SRC (DSPI1_CLK_SRC)
#elif(FLASH_DSPI_MASTER_BASE == SPI2_BASE)
#define DSPI_MASTER_CLK_SRC (DSPI2_CLK_SRC)
#elif(FLASH_DSPI_MASTER_BASE == SPI3_BASE)
#define DSPI_MASTER_CLK_SRC (DSPI3_CLK_SRC)
#elif(FLASH_DSPI_MASTER_BASE == SPI4_BASE)
#define DSPI_MASTER_CLK_SRC (DSPI4_CLK_SRC)
#else
#error Should define the DSPI_MASTER_CLK_SRC!
#endif

#define ENABLE_CHIPSELECT       GPIO_WritePinOutput(BOARD_CHIPSELECT_GPIO, BOARD_CHIPSELECT_GPIO_PIN, 0)
#define DISABLE_CHIPSELECT      GPIO_WritePinOutput(BOARD_CHIPSELECT_GPIO, BOARD_CHIPSELECT_GPIO_PIN, 1)

#define SIZEOF_QUEUE_SAVE_FLASH         10
#define TIME_DELAY_CHECK_QUEUE          1000
    
/* Define area save data for flash */
    /* Data config : time send data, time read sensor, poiter address flash */
#define PADDING_DISTANCE                0xFF
#define PAGE_SIZE_                      256
#define SECTOR_SIZE_                    4000

    
// define Id send flash to queue
#define ID_MESSAGE_FROM_SERVER                  1
#define ID_MESSAGE_DATA_DEVICE                  2
#define ID_MESSAGE_PRINT_FLASH                  3
#define ID_MESSAGE_CONFIG_TIME_SEND_SERVER      4
#define ID_MESSAGE_CONFIG_TIME_READ_SENSOR      5

#define VALUE_LENGTH_PRINT_LOG          128
#define VALUE_ADDRESS_FLASH_MAX         (0x7FFFFF)

/* define area flash */
#define ADDRESS_ORGIN_INFO_DEFINE       (0x000000)
#define ADDRESS_ORGIN_FILE_UPDATE       (0x100000)
#define ADDRESS_ORGIN_LOG_SAVE          (0x300000)


// abort value hex,  cal after
#define ADDRESS_TIME_SEND_DATA          (ADDRESS_ORGIN_INFO_DEFINE+(8000* 0))       
#define ADDRESS_TIME_READ_SENSOR        (ADDRESS_ORGIN_INFO_DEFINE+(8000* 1))
#define ADDRESS_ENCRYP_KEY              (ADDRESS_ORGIN_INFO_DEFINE+(8000* 2))
#define ADDRESS_POITER_ADDRESS_FLASH    (ADDRESS_ORGIN_INFO_DEFINE+(8000* 3))
  
#define LENGTH_DISTANCE_TIME            10
#define LENGTH_DISTANCE_KEY             32
#define LENGTH_DISTANCE_POITER          10
    

enum EraseMode{
    PARAM_4K_SECTOR_ERASE = 0,
    PARAM_4K_SECTOR_4B_ERASE,
    PARAM_8K_SECTOR_ERASE,
    PARAM_8K_SECTOR_4B_ERASE,
    SECTOR_ERASE,
    SECTOR_4B_ERASE,
    BULK_ERASE, /*Chip erase for FL1K*/
    BULK_ERASE1,/*Chip erase for FL1K*/
    BLOCK_ERASE,
};   
    
void flash_task( void *pvParameters );

SLLD_STATUS slld_BEOp1( void );

SLLD_STATUS slld_SEOp1( ADDRESS sys_addr );

SLLD_STATUS slld_WriteOp1(ADDRESS sys_addr, BYTE *databuf, BYTECOUNT len_in_bytes);
SLLD_STATUS slld_WriteOp2(ADDRESS sys_addr, BYTE *data_buf, BYTECOUNT len_in_bytes);

SLLD_STATUS setTimeReadSensorFlash( uint32_t time );

SLLD_STATUS setTimeSendDataFlash( uint32_t time );

SLLD_STATUS setAddressPoiterFlash( uint32_t address );

uint32_t getTimeSendDataFlash( void );

uint32_t getTimeReadSensorFlash( void );

uint32_t getAddressPoiterFlash( void );

void print_log(uint32_t beginAddress, uint32_t endAddress);

void eraseAreaLogFlash();

SLLD_STATUS slld_EraseFlash(ADDRESS addr,BYTE mode);

#ifdef __cplusplus
}
#endif
#endif