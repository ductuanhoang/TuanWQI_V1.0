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

#include <mqx.h>
#include <bsp.h>
#include <spi_memory.h>
#include <CommonParameter.h>

/* Test strings */
#define TEST_BYTE_1       0xAA
#define TEST_BYTE_2       0xA5
#define TEST_STRING_SHORT "Hello,World!"
#define TEST_STRING_LONG  "222222222222222222222"

const char *clock_mode[] =
{
    "SPI_CLK_POL_PHA_MODE0",
    "SPI_CLK_POL_PHA_MODE1",
    "SPI_CLK_POL_PHA_MODE2",
    "SPI_CLK_POL_PHA_MODE3"
};
const char *device_mode[] =
{
    "SPI_DEVICE_MASTER_MODE",
    "SPI_DEVICE_SLAVE_MODE",
};


/* The SPI serial memory test addresses */
#define SPI_MEMORY_ADDR1               0x000000 /* test address 1 */
#define SPI_MEMORY_ADDR2               0x100000 /* test address 2 */
#define SPI_MEMORY_ADDR3               0x300000 /* test address 3 */

unsigned char send_buffer[SPI_MEMORY_PAGE_SIZE];
unsigned char recv_buffer[sizeof(TEST_STRING_LONG)];
extern pxMessageFlash_t 		tmpMessage;
extern _mqx_uint_ptr	Wqi_rece_message[sizeof(tmpMessage)];

extern uint32_t server_queue[];
extern uint32_t client_queue[];

LWGPIO_STRUCT          lwgpio_hold;
LWGPIO_STRUCT          lwgpio_wp;
LWGPIO_STRUCT          spigpio;
SPI_CS_CALLBACK_STRUCT callback;
MQX_FILE_PTR           spifd;
uint32_t                param, result;//, i = 0;
SPI_STATISTICS_STRUCT  stats;
SPI_READ_WRITE_STRUCT  rw;

/*FUNCTION*---------------------------------------------------------------
*
* Function Name : set_CS
* Comments  : This function sets chip select signal to enable/disable memory.
*             It's used only on platforms with manual CS handling.
*END*----------------------------------------------------------------------*/


_mqx_int set_CS (uint32_t cs_mask, void *user_data)
{
    LWGPIO_STRUCT_PTR spigpio = (LWGPIO_STRUCT_PTR)user_data;

    if (cs_mask & 1)
    {
        lwgpio_set_value(spigpio, LWGPIO_VALUE_LOW);

    }
    else
    {
        lwgpio_set_value(spigpio, LWGPIO_VALUE_HIGH);

    }
    return MQX_OK;
}
// Function task
void test_spi(void)
{
/*
**  The WQI board has PIN E4,5 for Wp and Hold
**  controling ~WP and ~HOLD signals on MRAM memory.
**  For the purpose of the demo this pins are set to LOG1.
*/
	_mqx_uint_ptr msg[sizeof(tmpMessage)];
//	_mqx_uint result;
//	uint32_t i;
	char * pch;
	printf("\n-------------- SPI driver example --------------\n\n");
//	spifd = fopen ("spi1:", NULL);
//  spi_init_WP_HOLD_pin();
//	spi_setup_param();
//	memset (send_buffer, 0, sizeof (send_buffer));
//	memset (recv_buffer, 0, sizeof (recv_buffer));
//	printf("---  CHECK FLASH ----\r\n");
//	    /* Read status */
//    memory_read_status (spifd);

//    /* Disable protection */
//    memory_set_protection (spifd, FALSE);

//    /* Erase memory before tests */
//    memory_chip_erase (spifd);

    printf ("\n");
//    /* Write a data byte to memory */
//		_time_delay(5);
//    memory_write_byte (spifd, SPI_MEMORY_ADDR1, TEST_BYTE_1);
//		_time_delay(5);
//    /* Read status */
//    memory_read_status (spifd);
//		_time_delay(5);
//    /* Read a data byte from memory */
//    if (TEST_BYTE_1 == memory_read_byte (spifd, SPI_MEMORY_ADDR1))
//    {
//        printf ("Byte test ... OK\n");
//    }
//    else
//    {
//        printf ("Byte test ... ERROR\n");
//    }

//    /* Get statistics */
//    printf ("Getting statistics: ");
//    result = ioctl (spifd, IO_IOCTL_SPI_GET_STATS, &stats);
//    if (SPI_OK == result)
//    {
//        printf ("\n");
//        printf ("Rx packets:   %d\n", stats.RX_PACKETS);
//        printf ("Tx packets:   %d\n", stats.TX_PACKETS);
//    }
//    else if (MQX_IO_OPERATION_NOT_AVAILABLE == result)
//    {
//        printf ("not available, define BSPCFG_ENABLE_SPI_STATS\n");
//    }
//    else
//    {
//        printf ("ERROR\n");
//    }
//    printf ("\n");

		while(1)
		{
			_lwmsgq_receive((void *)server_queue, (uint32_t *)msg, LWMSGQ_RECEIVE_BLOCK_ON_EMPTY, 0, 0);
			pch=strchr((char*)msg,'!');
			printf(" Event receive mesaage:%s legth: %d\n", msg,pch-(char*)msg+2);
//    /* Page write to memory */
//			_time_delay(10);
//			memory_write_data (spifd, SPI_MEMORY_ADDR3,pch-(char*)msg+2 - 1, (unsigned char *)msg);
//			_time_delay(10);
//			/* Read status */
//			memory_read_status (spifd);
//			_time_delay(10);
//			/* Read data */
//			result = memory_read_data (spifd, SPI_MEMORY_ADDR3, pch-(char*)msg+2 - 1, recv_buffer);
//			_time_delay(10);
//			/* Test result */
//			if ((result != pch-(char*)msg+2 - 1) || (0 != memcmp (msg, recv_buffer, result)))
//			{
//					printf ("Write long data test ... ERROR\n");
//			}
//			else
//			{
//					printf ("Write long data test ... OK\n");
//			}
//			printf ("\n");
				/* Write a data byte to memory */
//				_time_delay(5);
//				memory_write_byte (spifd, SPI_MEMORY_ADDR1, i++);
//				_time_delay(5);
//				/* Read status */
//				memory_read_status (spifd);
//				_time_delay(5);
//				/* Read a data byte from memory */
//				if (i == memory_read_byte (spifd, SPI_MEMORY_ADDR1))
//				{
//						printf ("Byte test ... OK\n");
//				}
//				else
//				{
//						printf ("Byte test ... ERROR\n");
//				}
		}

}
//============================ Define GPIO CS, WP, HOLD  ======================
void spi_init_WP_HOLD_pin(void)
{
	 /* Open GPIO file containing SPI pin SS == chip select for memory */
    if (!lwgpio_init(&lwgpio_hold, (GPIO_PORT_E | GPIO_PIN4), LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE))
    {
        printf("Initializing GPIO with associated pins failed.\n");
        _time_delay (200L);
        _task_block();
    }
		lwgpio_set_value(&lwgpio_hold, LWGPIO_VALUE_HIGH);
    lwgpio_set_functionality(&lwgpio_hold, LWGPIO_MUX_E4_GPIO);/*BSP_SPI_MUX_GPIO need define in BSP for function mux as GPIO*/
    

    if (!lwgpio_init(&lwgpio_wp, (GPIO_PORT_E | GPIO_PIN5), LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE))
    {
        printf("Initializing GPIO with associated pins failed.\n");
        _time_delay (200L);
        _task_block();
    }
		lwgpio_set_value(&lwgpio_wp, LWGPIO_VALUE_HIGH);
    lwgpio_set_functionality(&lwgpio_wp, LWGPIO_MUX_E5_GPIO);/*BSP_SPI_MUX_GPIO need define in BSP for function mux as GPIO*/
   
//#endif
	
	/* Open GPIO file containing SPI pin SS == chip select for memory */

	if (!lwgpio_init(&spigpio, (GPIO_PORT_E | GPIO_PIN0), LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE))
	{
		printf("Initializing GPIO with associated pins failed.\n");
		_time_delay (200L);
		_task_block();
	}
	lwgpio_set_functionality(&spigpio,LWGPIO_MUX_E0_GPIO);/*BSP_SPI_MUX_GPIO need define in BSP for function mux as GPIO*/

	/* Set CS callback */

	callback.CALLBACK = set_CS;
	callback.USERDATA = &spigpio;
	printf ("Setting CS callback ... ");
	if (SPI_OK == ioctl (spifd, IO_IOCTL_SPI_SET_CS_CALLBACK, &callback))
	{
		printf ("OK\n");
	}
	else
	{
		printf ("ERROR\n");
	}
}

void spi_setup_param(void)
{
	    /* Display baud rate */
    printf ("Current baud rate ... ");
    if (SPI_OK == ioctl (spifd, IO_IOCTL_SPI_GET_BAUD, &param))
    {
        printf ("%d Hz\n", param);
    }
    else
    {
        printf ("ERROR\n");
    }

    /* Set a different rate */
    param = 400000;
    printf ("Changing the baud rate to %d Hz ... ", param);
    if (SPI_OK == ioctl (spifd, IO_IOCTL_SPI_SET_BAUD, &param))
    {
        printf ("OK\n");
    }
    else
    {
        printf ("ERROR\n");
    }

    /* Display baud rate */
    printf ("Current baud rate ... ");
    if (SPI_OK == ioctl (spifd, IO_IOCTL_SPI_GET_BAUD, &param))
    {
        printf ("%d Hz\n", param);
    }
    else
    {
        printf ("ERROR\n");
    }

    /* Set clock mode */
    param = SPI_CLK_POL_PHA_MODE0;
    printf ("Setting clock mode to %s ... ", clock_mode[param]);
    if (SPI_OK == ioctl (spifd, IO_IOCTL_SPI_SET_MODE, &param))
    {
        printf ("OK\n");
    }
    else
    {
        printf ("ERROR\n");
    }

    /* Get clock mode */
    printf ("Getting clock mode ... ");
    if (SPI_OK == ioctl (spifd, IO_IOCTL_SPI_GET_MODE, &param))
    {
        printf ("%s\n", clock_mode[param]);
    }
    else
    {
        printf ("ERROR\n");
    }

    /* Set big endian */
    param = SPI_DEVICE_BIG_ENDIAN;
    printf ("Setting endian to %s ... ", param == SPI_DEVICE_BIG_ENDIAN ? "SPI_DEVICE_BIG_ENDIAN" : "SPI_DEVICE_LITTLE_ENDIAN");
    if (SPI_OK == ioctl (spifd, IO_IOCTL_SPI_SET_ENDIAN, &param))
    {
        printf ("OK\n");
    }
    else
    {
        printf ("ERROR\n");
    }

    /* Get endian */
    printf ("Getting endian ... ");
    if (SPI_OK == ioctl (spifd, IO_IOCTL_SPI_GET_ENDIAN, &param))
    {
        printf ("%s\n", param == SPI_DEVICE_BIG_ENDIAN ? "SPI_DEVICE_BIG_ENDIAN" : "SPI_DEVICE_LITTLE_ENDIAN");
    }
    else
    {
        printf ("ERROR\n");
    }

    /* Set transfer mode */
    param = SPI_DEVICE_MASTER_MODE;
    printf ("Setting transfer mode to %s ... ", device_mode[param]);
    if (SPI_OK == ioctl (spifd, IO_IOCTL_SPI_SET_TRANSFER_MODE, &param))
    {
        printf ("OK\n");
    }
    else
    {
        printf ("ERROR\n");
    }

    /* Get transfer mode */
    printf ("Getting transfer mode ... ");
    if (SPI_OK == ioctl (spifd, IO_IOCTL_SPI_GET_TRANSFER_MODE, &param))
    {
        printf ("%s\n", device_mode[param]);
    }
    else
    {
        printf ("ERROR\n");
    }

    /* Clear statistics */
    printf ("Clearing statistics ... ");
    result = ioctl (spifd, IO_IOCTL_SPI_CLEAR_STATS, NULL);
    if (SPI_OK == result)
    {
        printf ("OK\n");
    }
    else if (MQX_IO_OPERATION_NOT_AVAILABLE == result)
    {
        printf ("not available, define BSPCFG_ENABLE_SPI_STATS\n");
    }
    else
    {
        printf ("ERROR\n");
    }

    /* Get statistics */
    printf ("Getting statistics: ");
    result = ioctl (spifd, IO_IOCTL_SPI_GET_STATS, &stats);
    if (SPI_OK == result)
    {
        printf ("\n");
        printf ("Rx packets:   %d\n", stats.RX_PACKETS);
        printf ("Tx packets:   %d\n", stats.TX_PACKETS);
    }
    else if (MQX_IO_OPERATION_NOT_AVAILABLE == result)
    {
        printf ("not available, define BSPCFG_ENABLE_SPI_STATS\n");
    }
    else
    {
        printf ("ERROR\n");
    }
    printf ("\n");
}
//-----------------------------------------------------------------------------
/*FUNCTION*---------------------------------------------------------------
*
* Function Name : memory_addr_to_buffer
* Comments  : Fills in given address into buffer in correct byte order
*
*END*----------------------------------------------------------------------*/
static int memory_addr_to_buffer(uint32_t addr, uint8_t *buffer)
{
    int i;

    for (i = SPI_MEMORY_ADDRESS_BYTES; i; i--)
    {
        buffer[i-1] = (uint8_t)(addr & 0xFF);
        addr >>= 8;
    }

    return SPI_MEMORY_ADDRESS_BYTES;
}


/*FUNCTION*---------------------------------------------------------------
*
* Function Name : memory_chip_erase
* Comments  : This function erases the whole memory SPI chip
*
*END*----------------------------------------------------------------------*/
void memory_chip_erase (MQX_FILE_PTR spifd)
{
    _mqx_int result;
    uint8_t buffer[1];

    /* This operation must be write-enabled */
    memory_set_write_latch (spifd, TRUE);

    memory_read_status (spifd);

    printf ("Erase whole memory chip:\n");
    buffer[0] = SPI_MEMORY_CHIP_ERASE;

    /* Write instruction */
    result = fwrite (buffer, 1, 1, spifd);

    /* Wait till transfer end (and deactivate CS) */
    fflush (spifd);

    while (memory_read_status (spifd) & 1)
    {
        _time_delay (1000);
    }

    printf ("Erase chip ... ");
    if (result != 1)
    {
        printf ("ERROR\n");
    }
    else
    {
        printf ("OK\n");
    }
}

/*FUNCTION*---------------------------------------------------------------
*
* Function Name : memory_set_write_latch
* Comments  : This function sets latch to enable/disable memory write
*             operation
*
*END*----------------------------------------------------------------------*/
void memory_set_write_latch (MQX_FILE_PTR spifd, bool enable)
{
    _mqx_int result;
    uint8_t buffer[1];

    if (enable)
    {
        printf ("Enable write latch in memory ... ");
        buffer[0] = SPI_MEMORY_WRITE_LATCH_ENABLE;
    } else {
        printf ("Disable write latch in memory ... ");
        buffer[0] = SPI_MEMORY_WRITE_LATCH_DISABLE;
    }

    /* Write instruction */
    result = fwrite (buffer, 1, 1, spifd);

    /* Wait till transfer end (and deactivate CS) */
    fflush (spifd);

    if (result != 1)
    {
        printf ("ERROR\n");
    }
    else
    {
        printf ("OK\n");
    }
}

/*FUNCTION*---------------------------------------------------------------
*
* Function Name : memory_set_protection
* Comments  : This function sets write protection in memory status register
*
*END*----------------------------------------------------------------------*/
void memory_set_protection (MQX_FILE_PTR spifd, bool protect)
{
    _mqx_int result, i;
    uint8_t protection;
    uint8_t buffer[2];

    /* Must do it twice to ensure right transitions in protection status register */
    for (i = 0; i < 2; i++)
    {
        /* Each write operation must be enabled in memory */
        memory_set_write_latch (spifd, TRUE);

        memory_read_status (spifd);

        if (protect)
        {
            printf ("Write protect memory ... ");
            protection = 0xFF;
        } else {
            printf ("Write unprotect memory ... ");
            protection = 0x00;
        }

        buffer[0] = SPI_MEMORY_WRITE_STATUS;
        buffer[1] = protection;

        /* Write instruction */
        result = fwrite (buffer, 1, 2, spifd);

        /* Wait till transfer end (and deactivate CS) */
        fflush (spifd);

        if (result != 2)
        {
            printf ("ERROR\n");
        }
        else
        {
            printf ("OK\n");
        }
    }
}

/*FUNCTION*---------------------------------------------------------------
*
* Function Name : memory_read_status
* Comments  : This function reads memory status register
* Return:
*         Status read.
*
*END*----------------------------------------------------------------------*/
uint8_t memory_read_status (MQX_FILE_PTR spifd)
{
    _mqx_int result;
    uint8_t buffer[1];
    uint8_t state = 0xFF;

    printf ("Read memory status ... ");

    buffer[0] = SPI_MEMORY_READ_STATUS;

    /* Write instruction */
    result = fwrite (buffer, 1, 1, spifd);

    if (result != 1)
    {
      /* Stop transfer */
        printf ("ERROR (tx)\n");
        return state;
    }

    /* Read memory status */
    result = fread (&state, 1, 1, spifd);

    /* Deactivate CS */
    fflush (spifd);

    if (result != 1)
    {
        printf ("ERROR (rx)\n");
    }
    else
    {
        printf ("0x%02x\n", state);
    }

    return state;
}

/*FUNCTION*---------------------------------------------------------------
*
* Function Name : memory_write_byte
* Comments  : This function writes a data byte to memory
*
*
*END*----------------------------------------------------------------------*/
void memory_write_byte (MQX_FILE_PTR spifd, uint32_t addr, unsigned char data)
{
    _mqx_int result;
    uint8_t buffer[1 + SPI_MEMORY_ADDRESS_BYTES + 1];

    /* Each write operation must be enabled in memory */
    memory_set_write_latch (spifd, TRUE);

    memory_read_status (spifd);

    printf ("Write byte 0x%02x to location 0x%08x in memory ... ", data, addr);

    /* Write instruction, address and data to buffer */
    buffer[0] = SPI_MEMORY_WRITE_DATA;
    memory_addr_to_buffer(addr, &(buffer[1]));
    buffer[1 + SPI_MEMORY_ADDRESS_BYTES] = data;

    result = fwrite (buffer, 1, 1 + SPI_MEMORY_ADDRESS_BYTES + 1, spifd);

    /* Deactivate CS */
    fflush (spifd);

    if (result != 1 + SPI_MEMORY_ADDRESS_BYTES + 1)
    {
        printf ("ERROR\n");
    }
    else
    {
        printf ("done\n", data);
    }

    /* There is 5 ms internal write cycle needed for memory */
    _time_delay (5);
}

/*FUNCTION*---------------------------------------------------------------
*
* Function Name : memory_read_byte
* Comments  : This function reads a data byte from memory
* Return:
*         Byte read.
*
*END*----------------------------------------------------------------------*/
uint8_t memory_read_byte (MQX_FILE_PTR spifd, uint32_t addr)
{
    _mqx_int result;
    uint8_t buffer[1 + SPI_MEMORY_ADDRESS_BYTES];
    uint8_t data = 0;

    printf ("Read byte from location 0x%08x in memory ... ", addr);

    /* Read instruction, address */
    buffer[0] = SPI_MEMORY_READ_DATA;
    memory_addr_to_buffer(addr, &(buffer[1]));

    /* Write instruction and address */
    result = fwrite (buffer, 1, 1 + SPI_MEMORY_ADDRESS_BYTES, spifd);
		
    if (result != 1 + SPI_MEMORY_ADDRESS_BYTES)
    {
        /* Stop transfer */
        fflush (spifd);
        printf ("ERROR (tx)\n");
        return data;
    }
    /* Read data from memory */
    result = fread (&data, 1, 1, spifd);
                           
    /* Deactivate CS */
    fflush (spifd);

    if (result != 1)
    {
        printf ("ERROR (rx)\n");
    }
    else
    {
        printf ("0x%02x\n", data);              
    }

    return data;
}

/*FUNCTION*---------------------------------------------------------------
*
* Function Name : memory_write_data
* Comments  : This function writes data buffer to memory using page write
* Return:
*         Number of bytes written.
*
*END*----------------------------------------------------------------------*/
uint32_t memory_write_data (MQX_FILE_PTR spifd, uint32_t addr, uint32_t size, unsigned char *data)
{
    _mqx_int result;
    uint32_t i, len;
    uint32_t count = size;
    uint8_t buffer[1 + SPI_MEMORY_ADDRESS_BYTES];

    while (count > 0)
    {
        /* Each write operation must be enabled in memory */
        memory_set_write_latch (spifd, TRUE);

        memory_read_status (spifd);

        len = count;
        if (len > SPI_MEMORY_PAGE_SIZE - (addr & (SPI_MEMORY_PAGE_SIZE - 1))) len = SPI_MEMORY_PAGE_SIZE - (addr & (SPI_MEMORY_PAGE_SIZE - 1));
        count -= len;

        printf ("Page write %d bytes to location 0x%08x in memory:\n", len, addr);

        /* Write instruction, address */
        buffer[0] = SPI_MEMORY_WRITE_DATA;
        memory_addr_to_buffer(addr, &(buffer[1]));
				_time_delay(5);
				result = fwrite (buffer, 1, 1 + SPI_MEMORY_ADDRESS_BYTES, spifd);
				_time_delay(5);
        if (result != 1 + SPI_MEMORY_ADDRESS_BYTES)
        {
            /* Stop transfer */
            fflush (spifd);
            printf ("ERROR (tx cmd)\n");
            return 0;
        }

        /* Write data */
        result = fwrite (data, 1, (long)len, spifd);
				_time_delay(5);
        /* Deactivate CS */
        fflush (spifd);

        if (result != len)
        {
            printf ("ERROR (tx data)\n");
            return size - count;
        }

        for (i = 0; i < len; i++)
            printf ("%c", data[i]);
        printf ("\n");

        /* Move to next block */
        addr += len;
        data += len;

        /* There is 5 ms internal write cycle needed for memory */
        _time_delay (10);
    }

    return size;
}

/*FUNCTION*---------------------------------------------------------------
*
* Function Name : memory_read_data
* Comments  : This function reads data from memory into given buffer
* Return:
*         Number of bytes read.
*
*END*----------------------------------------------------------------------*/
uint32_t memory_read_data (MQX_FILE_PTR spifd, uint32_t addr, uint32_t size, unsigned char *data)
{
    uint32_t i;
    _mqx_int result;
    uint8_t buffer[5];

    printf ("Reading %d bytes from location 0x%08x in memory: ", size, addr);

    /* Read instruction, address */
    buffer[0] = SPI_MEMORY_READ_DATA;
    memory_addr_to_buffer(addr, &(buffer[1]));

    result = fwrite (buffer, 1, 1 + SPI_MEMORY_ADDRESS_BYTES, spifd);

    if (result != 1 + SPI_MEMORY_ADDRESS_BYTES)
    {
        /* Stop transfer */
        fflush (spifd);
        printf ("ERROR (tx)\n");
        return 0;
    }

    /* Read size bytes of data */
    result = fread (data, 1, (_mqx_int)size, spifd);

    /* De-assert CS */
    fflush (spifd);

    if (result != size)
    {
        printf ("ERROR (rx)\n");
        return 0;
    }
    else
    {
        printf ("\n");
        for (i = 0; i < result; i++)
            printf ("%c", data[i]);
        printf ("\n");
    }

    return size;
}
