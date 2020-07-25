/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
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

/*
 * TEXT BELOW IS USED AS SETTING FOR THE PINS TOOL *****************************
PinsProfile:
- !!product 'Pins v2.0'
- !!processor 'MK64FN1M0xxx12'
- !!package 'MK64FN1M0VLL12'
- !!mcu_data 'ksdk2_0'
- !!processor_version '1.0.9'
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR THE PINS TOOL ***
 */

#include "fsl_common.h"
#include "fsl_port.h"
#include "pin_mux.h"

/*FUNCTION**********************************************************************
 *
 * Function Name : BOARD_InitPins
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 *END**************************************************************************/
void BOARD_InitPins(void) {
  
    /* Declare and initialise for pull up configuration */
    port_pin_config_t pinConfig = {0};
    pinConfig.pullSelect = kPORT_PullUp;
#if defined(FSL_FEATURE_PORT_HAS_OPEN_DRAIN) && FSL_FEATURE_PORT_HAS_OPEN_DRAIN
    pinConfig.openDrainEnable = kPORT_OpenDrainEnable;
#endif /* FSL_FEATURE_PORT_HAS_OPEN_DRAIN */
    
    /* Port A, B, C, D, E Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortA);
    CLOCK_EnableClock(kCLOCK_PortB);
    CLOCK_EnableClock(kCLOCK_PortC);
    CLOCK_EnableClock(kCLOCK_PortD);
    CLOCK_EnableClock(kCLOCK_PortE);

    /* Set uart for mc60 */
    
    /* Affects PORTD_PCR6 register */
    PORT_SetPinMux(PORTD, 6U, kPORT_MuxAlt3);
    /* Affects PORTD_PCR7 register */
    PORT_SetPinMux(PORTD, 7U, kPORT_MuxAlt3);
    
    /* Set uart for ke02 */
    
    /* Affects PORTE_PCR24 register */
    PORT_SetPinMux(PORTE, 24U, kPORT_MuxAlt3);
    /* Affects PORTE_PCR25 register */
    PORT_SetPinMux(PORTE, 25U, kPORT_MuxAlt3);

    /* Set uart pin for debug */
    
    /* Affects PORTB_PCR10 register */
    PORT_SetPinMux(PORTB, 10U, kPORT_MuxAlt3);
    /* Affects PORTB_PCR11 register */
    PORT_SetPinMux(PORTB, 11U, kPORT_MuxAlt3);     
    
    
}

/*******************************************************************************
 * EOF
 ******************************************************************************/
