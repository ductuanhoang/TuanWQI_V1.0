/*#**************************************************************************#*/
/*#     System name :   Water  measurement                             	     #*/
/*#     File name   :   mymmcau.h                              	             #*/
/*#     Summary     :   Parameter config for all sensor Water                #*/
/*#     Author      :   Tuanhd                                         		 #*/
/*#     Date        :   10/06/2017                                           #*/
/*#                                                                          #*/
/*#     Copyright (c) 2017 VNPT-Technology                                   #*/
/*#                        All rights reserved.                              #*/
/*#**************************************************************************#*/
/*#  Version                   : Date        : Infomation                    #*/
/*#  Major Ver.Minor Ver.Rev   : YYYY/MM/DD  :                               #*/
/*#--------------------------------------------------------------------------#*/
/*#     1.0.0                  : 10/06/2017  : New making                    #*/
/*#**************************************************************************#*/


#ifndef __MYMMCAU_H__
#define __MYMMCAU_H__
//-----------------------------------------------------------------------------
//                                    Includes

#ifdef __cplusplus
extern "C" {
#endif
    
/*********************************************************************
* INCLUDES
*/
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

//--------------------------------------------------
/*length of AES & DES encrypted data array*/
/*length of AES & DES encrypted data array*/
#define OUTPUT_ARRAY_LEN 512
/*length of result hash in bytes*/
#define SHA1_RESULT_LENGTH 20
#define SHA256_RESULT_LENGTH 32
#define MD5_RESULT_LENGTH 16
/* Number of cycles for througput measurement. One data buffer of certain size if processed this times. */
#define CYCLES_FOR_THROUGHPUT 128
/*AES specific*/
#define AES128 128
#define AES128_KEY_SIZE 16
#define AES_BLOCK_LENGTH 16

/*MMCAU result codes*/
#define MMCAU_OK 0
#define MMCAU_ERROR -1




void mmcau_example(void);
void mmcau_print_msg(const uint8_t *data, uint32_t length);
int mmcau_decrypt_aes_cbc(const uint8_t *key,
                                 uint32_t mode,
                                 const uint8_t *inputData,
                                 uint8_t *outputData,
                                 uint16_t dataLength,
                                 const uint8_t *initVector);
int mmcau_encrypt_aes_cbc(const uint8_t *key,
                                 uint32_t mode,
                                 const uint8_t *inputData,
                                 uint8_t *outputData,
                                 uint16_t dataLength,
                                 const uint8_t *initVector);
float mmcau_get_throughput(float elapsedMs, size_t numBytes);

#ifdef __cplusplus
}
#endif

#endif 
