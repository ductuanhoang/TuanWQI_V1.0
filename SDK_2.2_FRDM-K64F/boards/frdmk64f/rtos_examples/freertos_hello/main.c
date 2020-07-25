/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "main.h"
#include "board.h"

/* Task priorities. */
#define hello_task_PRIORITY                      (configMAX_PRIORITIES - 1)
#define led_task_PRIORITY                        (configMAX_PRIORITIES - 2)
#define rtc_task_PRIORITY                        (configMAX_PRIORITIES - 3)
#define flash_task_PRIORITY                      (configMAX_PRIORITIES - 4)
#define request_data_sesor_task_PRIORITY         (configMAX_PRIORITIES - 6)

/* Check single condition */
#define check(cond) do { if (!(cond)) fail(); } while (0)

static const char *JSON_STRING =
	"{\"user\": \"johndoe\", \"admin\": false, \"uid\": 1000,\n  "
	"\"groups\": {\"users\", \"wheel\", \"audio\", \"video\"}}";


static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
	if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
			strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
		return 0;
	}
	return -1;
}

/*******************************************************************************
 * struct creat
 ******************************************************************************/
packet_t                    wqi_packet_receive;
my_taskHandle_t 		    xTask;
pxMessageFlash_t 	        tmpMessage;

packet_t 	                wqi_packet_sends;
board_config 		        wqi_boards_config;  /* config thong so ban dau */
rtc_datetime_t              date_time; /* config thoi gian */

wqi_data                    wqi_data_read; /* gia tri cua cam bien*/
wqi_calculate               wqi_calculates; /* gia tri sau khi tinh wqi theo cam bien */

char Wqi_data_sendtoMC60[UART_MAX_LENGTH_MESSAGE];

uint8_t g_aesKey128[AES128_KEY_SIZE] = {0x75, 0x6c, 0x74, 0x72, 0x61, 0x70, 0x61, 0x73,\
                                        0x73, 0x77, 0x6f, 0x72, 0x64, 0x31, 0x32, 0x33};
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void led_task(void *pvParameters);
void Power_Ke02(void);
void board_InitDefaultBoard( void );
void reset_device(void);
/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Application entry point.
 */
int main(void)
{
    /* Init board hardware. */
    BOARD_InitPins();
    //BOARD_BootClockRUN(); // config clock for 50MHZ
    BOARD_InitDebugConsole();
    
    /* set gia tri default cho board*/
    board_InitDefaultBoard();
    
    /* start up of uart config */
    user_uartmc60_init();
    user_uartke02_init();
    Power_Ke02();
    mmcau_example();
    reset_device();
    test_json();
    /* Task create all */
    xTaskCreate(led_task, "led_task", configMINIMAL_STACK_SIZE + 10,\
                NULL, led_task_PRIORITY, NULL);
    
    xTaskCreate(rtc_task, "rtc_task", configMINIMAL_STACK_SIZE + 256,\
                NULL, rtc_task_PRIORITY, NULL);
    
    xTaskCreate(request_data_sesor_task, "request_data_sesor_task", \
                configMINIMAL_STACK_SIZE + 256,\
                NULL, request_data_sesor_task_PRIORITY, NULL);
    
    //xTaskCreate(flash_task, "flash_task", configMINIMAL_STACK_SIZE + 10,\
                NULL, flash_task_PRIORITY, NULL);

    vTaskStartScheduler();
    for (;;)
        ;
}
/*Init*-------------------------------------------------------------------------
* Task Name : Board config
* Comments  : set gia tri ban dau cho board
*           
*END*-------------------------------------------------------------------------*/
void board_InitDefaultBoard( void )
{
    // Init variables of board config
    wqi_boards_config.time_read_sensor 	= 5000;//TIME_READ_SENSOR_30_MINUTE;
    wqi_boards_config.time_send_server	= TIME_SENDS_SERVER_1_HOUR;
    wqi_boards_config.time_save_flashs 	= TIME_SAVE_FLASHS_30_MINUTE;
    wqi_boards_config.myaddress 		= MY_ID_DEVICE_ADDRESS;
    wqi_boards_config.enable_debug		= DEBUG_ENABLE;
    
}
/*Init*-------------------------------------------------------------------------
* Task Name : led_task
* Comments  : blinking led
*           
*END*-------------------------------------------------------------------------*/
static void led_task(void *pvParameters)
{
    gpio_pin_config_t led_config = {
        kGPIO_DigitalOutput, 0,
    };
    
    PORT_SetPinMux(PORTC, 4U, kPORT_MuxAsGpio);   
    GPIO_PinInit(GPIOC, 4U, &led_config);
    GPIO_WritePinOutput(GPIOC, 4U, 0);
    
    for (;;)
    {
        GPIO_TogglePinsOutput(GPIOC, 1U << 4U);
        vTaskDelay(1000);
    }
}

/*Init*-------------------------------------------------------------------------
* Task Name : Power_Ke02
* Comments  : 
*           
*END*-------------------------------------------------------------------------*/
void Power_Ke02(void)
{
    MY_UART_DEBUG("Power_Ke02 call\r\n");
    
    gpio_pin_config_t power_config = {
        kGPIO_DigitalOutput, 0,
    };
    
    PORT_SetPinMux(PORTC, 0U, kPORT_MuxAsGpio);   
    GPIO_PinInit(GPIOC, 0U, &power_config);
    GPIO_WritePinOutput(GPIOC, 0U, 1);
    
    
}
/*Init*-------------------------------------------------------------------------
* Task Name : UART_DEBUG
* Comments  : as modifine function printf
*           
*END*-------------------------------------------------------------------------*/
void MY_UART_DEBUG(const char *format, ...)
{
    if(wqi_boards_config.enable_debug == DEBUG_ENABLE)
    {
        va_list ap;
        va_start(ap, format);
        vprintf(format, ap);
        va_end(ap);
    }
    else
    {
        return;
    }
}

//*********************************** Check json ******************************/
/*
 * A small example of jsmn parsing when JSON structure is known and number of
 * tokens is predictable.
 */


int test_json() {
	int i;
	int r;
	jsmn_parser p;
	jsmntok_t t[128]; /* We expect no more than 128 tokens */

	jsmn_init(&p);
	r = jsmn_parse(&p, JSON_STRING, strlen(JSON_STRING), t, sizeof(t)/sizeof(t[0]));
	if (r < 0) {
		printf("Failed to parse JSON: %d\n", r);
		return 1;
	}

	/* Assume the top-level element is an object */
	if (r < 1 || t[0].type != JSMN_OBJECT) {
		printf("Object expected\n");
		return 1;
	}

	/* Loop over all keys of the root object */
	for (i = 1; i < r; i++) {
		if (jsoneq(JSON_STRING, &t[i], "user") == 0) {
			/* We may use strndup() to fetch string value */
			printf("- User: %.*s\n", t[i+1].end-t[i+1].start,
					JSON_STRING + t[i+1].start);
			i++;
		} else if (jsoneq(JSON_STRING, &t[i], "admin") == 0) {
			/* We may additionally check if the value is either "true" or "false" */
			printf("- Admin: %.*s\n", t[i+1].end-t[i+1].start,
					JSON_STRING + t[i+1].start);
			i++;
		} else if (jsoneq(JSON_STRING, &t[i], "uid") == 0) {
			/* We may want to do strtol() here to get numeric value */
			printf("- UID: %.*s\n", t[i+1].end-t[i+1].start,
					JSON_STRING + t[i+1].start);
			i++;
		} else if (jsoneq(JSON_STRING, &t[i], "groups") == 0) {
			int j;
			printf("- Groups:\n");
			if (t[i+1].type != JSMN_ARRAY) {
				continue; /* We expect groups to be an array of strings */
			}
			for (j = 0; j < t[i+1].size; j++) {
				jsmntok_t *g = &t[i+j+2];
				printf("  * %.*s\n", g->end - g->start, JSON_STRING + g->start);
			}
			i += t[i+1].size + 1;
		} else {
			printf("Unexpected key: %.*s\n", t[i].end-t[i].start,
					JSON_STRING + t[i].start);
		}
	}
	return EXIT_SUCCESS;
}

void reset_device(void)
{
    sim_uid_t My_UID;
    char buff[100];
    memset(buff, 0x00, 100);
    
    SIM_GetUniqueId(&My_UID);
    sprintf( buff, "%x%x%x", My_UID.MH >> 16, My_UID.ML, My_UID.L);

    
    printf("my uid ->>> %s\r\n", buff);
  
}