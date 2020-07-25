/*#**************************************************************************#*/
/*#     System name :   Water  measurement                             	     #*/
/*#     File name   :   mymmcau.c                              	             #*/
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

#include "main.h"
#include "mymmcau.h"
#include "fsl_mmcau.h"

static uint8_t g_output[OUTPUT_ARRAY_LEN];
static uint8_t g_result[OUTPUT_ARRAY_LEN];
static const uint8_t g_testFull[] =
    "Once upon a midnight dreary, while I pondered weak and weary,"
    "Over many a quaint and curious volume of forgotten lore,"
    "While I nodded, nearly napping, suddenly there came a tapping,"
    "As of some one gently rapping, rapping at my chamber door"
    "Its some visitor, I muttered, tapping at my chamber door"
    "Only this, and nothing more.";

/*16 bytes key: "ultrapassword123"*/
extern uint8_t g_aesKey128[AES128_KEY_SIZE];
//const uint8_t g_aesKey128[AES128_KEY_SIZE] = {0x75, 0x6c, 0x74, 0x72, 0x61, 0x70, 0x61, 0x73,\
                                              0x73, 0x77, 0x6f, 0x72, 0x64, 0x31, 0x32, 0x33};
/*initialization vector: 16 bytes: "mysecretpassword"*/
const uint8_t g_aesIV[AES_BLOCK_LENGTH] = {0x6d, 0x79, 0x73, 0x65, 0x63, 0x72, 0x65, 0x74,
                                           0x70, 0x61, 0x73, 0x73, 0x77, 0x6f, 0x72, 0x64};

/*!
 * @brief Get througput in MB/s
 * @param elapsedMs Time interval in milliseconds.
 * @param numBytes Number of bytes processed within the given interval.
 * @return Throughput in MB/s.
 */
float mmcau_get_throughput(float elapsedMs, size_t numBytes)
{
    return ((float)(1000 * numBytes / 1024 / 1024) / elapsedMs);
}
/*Init*-------------------------------------------------------------------------
* Task Name : mmcau_example
* Comments  : as modifine function printf
*           
*END*-------------------------------------------------------------------------*/
void mmcau_example(void)
{
    uint32_t length;
    uint8_t status;
    float timeBefore;
    float timeAfter;
    int cycles;
    
    /* Print welcome string */
    PRINTF("............................. MMCAU  DRIVER  EXAMPLE ............................. \r\n\r\n");
    memset(&g_output[0], 0, OUTPUT_ARRAY_LEN);
    memset(&g_result[0], 0, OUTPUT_ARRAY_LEN);
    length = sizeof(g_testFull) - 1u;

    PRINTF("Testing input string: \r\n");
    mmcau_print_msg(&g_testFull[0], length);
    /* Format console output */
    PRINTF("\r\n");
    
        /***************************************************/
    /******* FIRST PART USING AES-CBC method *********/
    /***************************************************/
    PRINTF("----------------------------------- AES-CBC method --------------------------------------\r\n");

    /*   ENCRYPTION   */
    PRINTF("AES CBC Encryption of %d bytes.\r\n", length);

    /* Call AES_cbc encryption */
    cycles = CYCLES_FOR_THROUGHPUT;
    timeBefore = xTaskGetTickCount();
    while (cycles)
    {
        status = mmcau_encrypt_aes_cbc(g_aesKey128, AES128, g_testFull, g_output, length, g_aesIV);
        if (status != MMCAU_OK)
        {
            PRINTF("AES CBC encryption failed !\r\n");
            return;
        }
        cycles--;
    }
    timeAfter = xTaskGetTickCount();

    /* Result message */
    PRINTF("AES CBC encryption finished. Speed %f MB/s.\r\n\r\n",
           mmcau_get_throughput(timeAfter - timeBefore, CYCLES_FOR_THROUGHPUT * length));

    /*   DECRYPTION   */
    PRINTF("AES CBC Decryption of %d bytes.\r\n", length);

    /* Call AES_cbc decryption */
    cycles = CYCLES_FOR_THROUGHPUT;
    timeBefore = xTaskGetTickCount();
    while (cycles)
    {
        status = mmcau_decrypt_aes_cbc(g_aesKey128, AES128, g_output, g_result, length, g_aesIV);
        if (status != MMCAU_OK)
        {
            PRINTF("AES CBC decryption failed !\r\n");
            return;
        }
        cycles--;
    }
    timeAfter = xTaskGetTickCount();

    /* Result message */
    PRINTF("AES CBC decryption finished. Speed %f MB/s.\r\n",
           mmcau_get_throughput(timeAfter - timeBefore, CYCLES_FOR_THROUGHPUT * length));
    /* Print decrypted string */
    PRINTF("Decrypted string :\r\n");
    mmcau_print_msg(g_result, length);
    PRINTF("\r\n");
}

/*!
 * @brief Function mmcau_print_msg prints data bytes into console.
 */
void mmcau_print_msg(const uint8_t *data, uint32_t length)
{
    uint32_t i;

    PRINTF("          ");
    for (i = 0; i < length; i++)
    {
        PUTCHAR(data[i]);
        if (data[i] == ',')
        {
            PRINTF("\r\n          ");
        }
    }
    PRINTF("\r\n");
}
/*
 * mmcau_decrypt_aes_cbc: AES decryption function
 *
 * Parameters:
 *   [in] key: key of 8 bytes
 *   [in] mode: 128, 192 or 256 AES mode
 *   [in] inputData: pointer to in data
 *   [out] outputData: pointer to out data
 *   [in] dataLength: number of bytes of input data. Must be divisible by 8 (DES block)
 *   [in] initVector: initVector to use during xor
 * Return:
 *   0 if OK, otherwise error
 *
 */
int mmcau_decrypt_aes_cbc(const uint8_t *key,
                                 uint32_t mode,
                                 const uint8_t *inputData,
                                 uint8_t *outputData,
                                 uint16_t dataLength,
                                 const uint8_t *initVector)
{
    
    uint8_t i;
    uint16_t blocks;
    uint16_t rounds;
    uint8_t tempBlock[AES_BLOCK_LENGTH];
    uint8_t tempIV[AES_BLOCK_LENGTH];
    /*
     * AES128 needs 44 longwords for expansion
     * AES192 needs 52 longwords for expansion
     * AES256 needs 60 longwords for expansion
     *    Reserving 60 longwords as the max space
     */
    uint8_t keyExpansion[60 * 4];
    uint32_t keyLen;

    /*validate NULL for key, inputData, outputData or initVector*/
    if ((key == NULL) || (inputData == NULL) || (outputData == NULL) || (initVector == NULL))
    {
        return MMCAU_ERROR; /*wrong pointer*/
    }

    /*validate AES mode*/
    if ((mode != 128u) && (mode != 192u) && (mode != 256u))
    {
        return MMCAU_ERROR; /*wrong AES mode*/
    }

    /*validate data length*/
    if (dataLength % AES_BLOCK_LENGTH)
    {
        return MMCAU_ERROR; /*wrong length*/
    }

    /*calculate number of AES rounds*/
    if (mode == 128u)
    {
        rounds = 10u;
        keyLen = 16u;
    }
    else if (mode == 192u)
    {
        rounds = 12u;
        keyLen = 24u;
    }
    else /*AES256*/
    {
        rounds = 14u;
        keyLen = 32u;
    }

    /*expand AES key*/
    MMCAU_AES_SetKey(key, keyLen, keyExpansion);

    /*execute AES in CBC mode*/
    /*http://en.wikipedia.org/wiki/Block_cipher_modes_of_operation*/

    /*get number of blocks*/
    blocks = dataLength / AES_BLOCK_LENGTH;

    /*copy init vector to temp storage*/
    memcpy(tempIV, initVector, AES_BLOCK_LENGTH);

    do
    {
        /*copy to temp storage*/
        memcpy(tempBlock, inputData, AES_BLOCK_LENGTH);

        /*FSL: single AES round*/
        MMCAU_AES_DecryptEcb(inputData, keyExpansion, rounds, outputData);

        /*xor for CBC*/
        for (i = 0; i < AES_BLOCK_LENGTH; i++)
        {
            outputData[i] ^= tempIV[i];
        }

        /*update initVector for next AES round*/
        memcpy(tempIV, tempBlock, AES_BLOCK_LENGTH);

        /*adjust pointers for next 3DES round*/
        inputData += AES_BLOCK_LENGTH;
        outputData += AES_BLOCK_LENGTH;

    } while (--blocks);

    return MMCAU_OK;
}
 /*
 * mmcau_encrypt_aes_cbc: AES encryption function
 *
 * Parameters:
 *   [in] key: key of 8 bytes
 *   [in] mode: 128, 192 or 256 AES mode
 *   [in] inputData: pointer to in data
 *   [out] outputData: pointer to out data
 *   [in] dataLength: number of bytes of input data. Must be divisible by 8 (DES block)
 *   [in] initVector: initVector to use during xor
 * Return:
 *   0 if OK, otherwise error
 *
 */
int mmcau_encrypt_aes_cbc(const uint8_t *key,
                                 uint32_t mode,
                                 const uint8_t *inputData,
                                 uint8_t *outputData,
                                 uint16_t dataLength,
                                 const uint8_t *initVector)
{
    uint8_t i;
    uint16_t blocks;
    uint16_t rounds;
    uint8_t tempBlock[AES_BLOCK_LENGTH];
    uint8_t tempIV[AES_BLOCK_LENGTH];
    /*
     * AES128 needs 44 longwords for expansion
     * AES192 needs 52 longwords for expansion
     * AES256 needs 60 longwords for expansion
     * Reserving 60 longwords as the max space
     */
    uint32_t keyLen;
    uint8_t keyExpansion[60 * 4];

    /*validate NULL for key, inputData, outputData or initVector*/
    if ((key == NULL) || (inputData == NULL) || (outputData == NULL) || (initVector == NULL))
    {
        return MMCAU_ERROR; /*wrong pointer*/
    }

    /*validate AES mode*/
    if ((mode != 128u) && (mode != 192u) && (mode != 256u))
    {
        return MMCAU_ERROR; /*wrong AES mode*/
    }

    /*validate data length*/
    if (dataLength % AES_BLOCK_LENGTH)
    {
        return MMCAU_ERROR; /*wrong length*/
    }

    /*calculate number of AES rounds*/
    if (mode == 128u)
    {
        rounds = 10u;
        keyLen = 16u;
    }
    else if (mode == 192u)
    {
        rounds = 12u;
        keyLen = 24u;
    }
    else /*AES256*/
    {
        rounds = 14u;
        keyLen = 32u;
    }

    /*expand AES key*/
    MMCAU_AES_SetKey(key, keyLen, keyExpansion);

    /*execute AES in CBC mode*/
    /*http://en.wikipedia.org/wiki/Block_cipher_modes_of_operation*/

    /*get number of blocks*/
    blocks = dataLength / AES_BLOCK_LENGTH;

    /*copy init vector to temp storage*/
    memcpy(tempIV, initVector, AES_BLOCK_LENGTH);

    do
    {
        /*copy to temp storage*/
        memcpy(tempBlock, inputData, AES_BLOCK_LENGTH);

        /*xor for CBC*/
        for (i = 0; i < AES_BLOCK_LENGTH; i++)
        {
            tempBlock[i] ^= tempIV[i];
        }

        /*FSL: single AES round*/
        MMCAU_AES_EncryptEcb(tempBlock, keyExpansion, rounds, outputData);

        /*update initVector for next 3DES round*/
        memcpy((void *)tempIV, (void *)outputData, AES_BLOCK_LENGTH);

        /*adjust pointers for next 3DES round*/
        inputData += AES_BLOCK_LENGTH;
        outputData += AES_BLOCK_LENGTH;

    } while (--blocks);

    return MMCAU_OK;
}
               