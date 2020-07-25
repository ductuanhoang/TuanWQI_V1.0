/* slld_hal.c - SLLD Hardware Abstraction Layer example Code */
 
/**************************************************************************
* Copyright 2011 Spansion LLC. All Rights Reserved. 
*
* This software is owned and published by: 
* Spansion LLC, 915 DeGuigne Drive, Sunnyvale, CA 94088 ("Spansion").
*
* BY DOWNLOADING, INSTALLING OR USING THIS SOFTWARE, YOU AGREE TO BE BOUND 
* BY ALL THE TERMS AND CONDITIONS OF THIS AGREEMENT.
*
* This software constitutes driver source code for use in programming Spansion's 
* Flash memory components. This software is licensed by Spansion to be adapted only 
* for use in systems utilizing Spansion's Flash memories. Spansion is not be 
* responsible for misuse or illegal use of this software for devices not 
* supported herein.  Spansion is providing this source code "AS IS" and will 
* not be responsible for issues arising from incorrect user implementation 
* of the source code herein.  
*
* Spansion MAKES NO WARRANTY, EXPRESS OR IMPLIED, ARISING BY LAW OR OTHERWISE, 
* REGARDING THE SOFTWARE, ITS PERFORMANCE OR SUITABILITY FOR YOUR INTENDED 
* USE, INCLUDING, WITHOUT LIMITATION, NO IMPLIED WARRANTY OF MERCHANTABILITY, 
* FITNESS FOR A  PARTICULAR PURPOSE OR USE, OR NONINFRINGEMENT.  Spansion WILL 
* HAVE NO LIABILITY (WHETHER IN CONTRACT, WARRANTY, TORT, NEGLIGENCE OR 
* OTHERWISE) FOR ANY DAMAGES ARISING FROM USE OR INABILITY TO USE THE SOFTWARE, 
* INCLUDING, WITHOUT LIMITATION, ANY DIRECT, INDIRECT, INCIDENTAL, 
* SPECIAL, OR CONSEQUENTIAL DAMAGES OR LOSS OF DATA, SAVINGS OR PROFITS, 
* EVEN IF Spansion HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.  
*
* This software may be replicated in part or whole for the licensed use, 
* with the restriction that this Copyright notice must be included with 
* this software, whether used in part or whole, at all times.  
*/

#include <stdio.h>
#include "slld_targetspecific.h"
#include "slld.h"
#include "slld_hal.h"
#include "main.h"

extern dspi_rtos_handle_t dspi_master_rtos_handle;
extern uint8_t masterSendBuffer[FLASH_TRANSFER_SIZE];
extern uint8_t masterReceiveBuffer[FLASH_TRANSFER_SIZE];
// ***************************************************************************
//  FLASH_READ - HAL read function
//
//  input : device_num            device number to which operation will be done
//          command               write a single command byte to flash
//          sys_addr              system address to be used
//          data_buffer           Pointer to the data buffer where to store the read data
//          Number_Of_Read_Bytes  number of bytes to be read
//
//  return value : status of the operation - FAIL or SUCCESS
// ***************************************************************************
SLLD_STATUS FLASH_READ
(
BYTE     command,                        /* write a single command byte to flash */
ADDRESS  sys_addr,                       /* system address to be used */
BYTE    *data_buffer,                    /* Pointer to the data buffer containing data to be written */
int      Number_Of_Read_Bytes            /* number of bytes to be read */
)
{
    SLLD_STATUS status = SLLD_OK;
    
    dspi_transfer_t masterXfer;
    masterXfer.dataSize = 0;
    
    
    /* Max length read/write is page 256 byte */
    uint8_t *ptr;
    uint16_t Number_Of_Dummy_Bytes = 0;
    ENABLE_CHIPSELECT;
    
    /* Erase buffer */
    memset(masterSendBuffer, 0x00, sizeof( masterSendBuffer ));
    memset(masterReceiveBuffer, 0x00, sizeof( masterReceiveBuffer ));
    
    ptr = masterSendBuffer;
    
    /* Write the command */
    *ptr = command;
    ptr++;
    masterXfer.dataSize++;
    
    /* Write the address */
    if( sys_addr != ADDRESS_NOT_USED )
    {
        *ptr = ((sys_addr >> 16) & 0x000000FF);
        ptr++;
        *ptr = ((sys_addr >>  8) & 0x000000FF);
        ptr++;
        *ptr =  (sys_addr        & 0x000000FF);
        ptr++;
        masterXfer.dataSize+=3;
    }
    
    /* Write the dummy byte */
    switch( command )
    {
    case SPI_FAST_READ_CMD:
    case SPI_FAST_READ_4B_CMD:
    case SPI_DUALIO_RD_CMD:
    case SPI_DUALIO_RD_4B_CMD:
    case SPI_QUADIO_RD_CMD:
    case SPI_QUADIO_RD_4B_CMD:
    case SPI_OTPR_CMD:
        {
            Number_Of_Dummy_Bytes = 1;
            // Write a dummy byte to the data bus 
            for(uint8_t i = 0; i < Number_Of_Dummy_Bytes; i++)
            {
                *ptr = 0;
                ptr++;
            }
            masterXfer.dataSize += Number_Of_Dummy_Bytes;
            break;
        }
    case SPI_DUALIO_HPRD_CMD:
    case SPI_DUALIO_HPRD_4B_CMD:
        {
            Number_Of_Dummy_Bytes = 1;
            // Write a dummy byte to the data bus
            for(uint16_t i = 0; i < Number_Of_Dummy_Bytes; i++)
            {
                *ptr = 0;
                ptr++;
            }
            masterXfer.dataSize += Number_Of_Dummy_Bytes;
            break;
        }
    case SPI_QUADIO_HPRD_CMD:
    case SPI_QUADIO_HPRD_4B_CMD:
        {
            Number_Of_Dummy_Bytes = 3;
            // Write the dummy bytes to the data bus - The first byte is actually the mode bit
            for(uint8_t i = 0; i < Number_Of_Dummy_Bytes; i++)
            {
                *ptr = 0;
                ptr++;
            }
            masterXfer.dataSize += Number_Of_Dummy_Bytes;
            break;
        }
    case SPI_RES_CMD:
    default:
        Number_Of_Dummy_Bytes = 0;
        break;
    }
    
    /* Start master transfer */
    masterXfer.txData = masterSendBuffer;
    masterXfer.rxData = masterReceiveBuffer;
    masterXfer.dataSize += Number_Of_Read_Bytes;
    masterXfer.configFlags = kDSPI_MasterCtar0 | kDSPI_MasterPcs0 | kDSPI_MasterPcsContinuous;
    
    DSPI_RTOS_Transfer(&dspi_master_rtos_handle, &masterXfer);
    
    //MYTRACE("->> DSPI master transfer completed successfully. \r\n");
    
    
    ptr = masterReceiveBuffer;
    ptr++;
    if( sys_addr != ADDRESS_NOT_USED )
        ptr += 3;
    memcpy( data_buffer, ptr, Number_Of_Read_Bytes );
    
    DISABLE_CHIPSELECT;
    
    return(status);
}


// ***************************************************************************
//  FLASH_WRITE - HAL write function
//
//  input : device_num               device number to which operation will be done
//          command                  write a single command byte to flash
//          sys_addr                 system address to be used
//          data_buffer              Pointer to the data buffer where to store the written data
//          Number_Of_Written_Bytes  number of bytes to be written
//
//  return value : status of the operation - FAIL or SUCCESS
// ***************************************************************************
SLLD_STATUS FLASH_WRITE
(
BYTE     command,                        /* write a single command byte to flash */
ADDRESS  sys_addr,                       /* system address to be used */
BYTE    *data_buffer,                    /* Pointer to the data buffer containing data to be written */
int      Number_Of_Written_Bytes         /* number of bytes to be written */
)
{
    uint8_t* ptr;
    SLLD_STATUS status = SLLD_OK;
    dspi_transfer_t masterXfer;
    masterXfer.dataSize = 0;
    
    ENABLE_CHIPSELECT;
    
    /* Erase buffer */
    memset( masterSendBuffer, 0x00, sizeof(masterSendBuffer) );
    
    ptr = masterSendBuffer;
    
    /* Write the command */
    *ptr =  command;
    ptr++;
    masterXfer.dataSize++;
    /* Write the address */
    if( sys_addr != ADDRESS_NOT_USED )
    {
        *ptr =  ((sys_addr >> 16) & 0x000000FF);
        ptr++;
        *ptr =  ((sys_addr >>  8) & 0x000000FF);
        ptr++;
        *ptr =   (sys_addr        & 0x000000FF);
        ptr++;
        masterXfer.dataSize += 3;
    }
    
    /* Write the data */
    for( uint16_t i = 0; i < Number_Of_Written_Bytes; i++ )
    {
        *ptr =  data_buffer[i];
        ptr++;
    }
    
    /* Start master transfer */
    masterXfer.txData = masterSendBuffer;
    masterXfer.rxData = NULL;
    masterXfer.dataSize += Number_Of_Written_Bytes;
    masterXfer.configFlags = kDSPI_MasterCtar0 | kDSPI_MasterPcs0 | kDSPI_MasterPcsContinuous;
    
    DSPI_RTOS_Transfer(&dspi_master_rtos_handle, &masterXfer) ;
    
    DISABLE_CHIPSELECT;
    return(status);
}


/*****************************************************************************/
