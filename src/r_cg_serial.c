/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2011, 2019 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : r_cg_serial.c
* Version      : CodeGenerator for RL78/G14 V2.05.04.02 [20 Nov 2019]
* Device(s)    : R5F104MF
* Tool-Chain   : GCCRL78
* Description  : This file implements device driver for Serial module.
* Creation Date: 
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_serial.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
volatile uint8_t * gp_csi01_rx_address;        /* csi01 receive buffer address */
volatile uint16_t  g_csi01_rx_length;          /* csi01 receive data length */
volatile uint16_t  g_csi01_rx_count;           /* csi01 receive data count */
volatile uint8_t * gp_csi01_tx_address;        /* csi01 send buffer address */
volatile uint16_t  g_csi01_send_length;        /* csi01 send data length */
volatile uint16_t  g_csi01_tx_count;           /* csi01 send data count */
volatile uint8_t * gp_csi10_rx_address;        /* csi10 receive buffer address */
volatile uint16_t  g_csi10_rx_length;          /* csi10 receive data length */
volatile uint16_t  g_csi10_rx_count;           /* csi10 receive data count */
volatile uint8_t * gp_csi10_tx_address;        /* csi10 send buffer address */
volatile uint16_t  g_csi10_send_length;        /* csi10 send data length */
volatile uint16_t  g_csi10_tx_count;           /* csi10 send data count */
volatile uint8_t * gp_uart2_tx_address;        /* uart2 transmit buffer address */
volatile uint16_t  g_uart2_tx_count;           /* uart2 transmit data number */
volatile uint8_t * gp_uart2_rx_address;        /* uart2 receive buffer address */
volatile uint16_t  g_uart2_rx_count;           /* uart2 receive data number */
volatile uint16_t  g_uart2_rx_length;          /* uart2 receive data length */
volatile uint8_t   g_iica0_master_status_flag; /* iica0 master flag */
volatile uint8_t   g_iica0_slave_status_flag;  /* iica0 slave flag */
volatile uint8_t * gp_iica0_rx_address;        /* iica0 receive buffer address */
volatile uint16_t  g_iica0_rx_len;             /* iica0 receive data length */
volatile uint16_t  g_iica0_rx_cnt;             /* iica0 receive data count */
volatile uint8_t * gp_iica0_tx_address;        /* iica0 send buffer address */
volatile uint16_t  g_iica0_tx_cnt;             /* iica0 send data count */
volatile uint8_t   g_iica1_master_status_flag; /* iica1 master flag */
volatile uint8_t   g_iica1_slave_status_flag;  /* iica1 slave flag */
volatile uint8_t * gp_iica1_rx_address;        /* iica1 receive buffer address */
volatile uint16_t  g_iica1_rx_len;             /* iica1 receive data length */
volatile uint16_t  g_iica1_rx_cnt;             /* iica1 receive data count */
volatile uint8_t * gp_iica1_tx_address;        /* iica1 send buffer address */
volatile uint16_t  g_iica1_tx_cnt;             /* iica1 send data count */
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_SAU0_Create
* Description  : This function initializes the SAU0 module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_SAU0_Create(void)
{
    SAU0EN = 1U;    /* supply SAU0 clock */
    NOP();
    NOP();
    NOP();
    NOP();
    SPS0 = _0002_SAU_CK00_FCLK_2 | _0020_SAU_CK01_FCLK_2;
    R_CSI01_Create();
    R_CSI10_Create();
}

/***********************************************************************************************************************
* Function Name: R_CSI01_Create
* Description  : This function initializes the CSI01 module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_CSI01_Create(void)
{
    ST0 |= _0002_SAU_CH1_STOP_TRG_ON;    /* disable CSI01 */
    CSIMK01 = 1U;    /* disable INTCSI01 interrupt */
    CSIIF01 = 0U;    /* clear INTCSI01 interrupt flag */
    /* Set INTCSI01 low priority */
    CSIPR101 = 1U;
    CSIPR001 = 1U;
    SIR01 = _0004_SAU_SIRMN_FECTMN | _0002_SAU_SIRMN_PECTMN | _0001_SAU_SIRMN_OVCTMN;    /* clear error flag */
    SMR01 = _0020_SAU_SMRMN_INITIALVALUE | _0000_SAU_CLOCK_SELECT_CK00 | _0000_SAU_CLOCK_MODE_CKS |
            _0000_SAU_TRIGGER_SOFTWARE | _0000_SAU_MODE_CSI | _0000_SAU_TRANSFER_END;
    SCR01 = _C000_SAU_RECEPTION_TRANSMISSION | _3000_SAU_TIMING_4 | _0000_SAU_MSB | _0007_SAU_LENGTH_8;
    SDR01 = _CE00_CSI01_DIVISOR;
    SO0 &= ~_0200_SAU_CH1_CLOCK_OUTPUT_1;    /* CSI01 clock initial level */
    SO0 &= ~_0002_SAU_CH1_DATA_OUTPUT_1;    /* CSI01 SO initial level */
    SOE0 |= _0002_SAU_CH1_OUTPUT_ENABLE;    /* enable CSI01 output */
    /* Set SI01 pin */
    PM4 |= 0x10U;
    /* Set SO01 pin */
    P4 |= 0x20U;
    PM4 &= 0xDFU;
    /* Set SCK01 pin */
    P4 |= 0x08U;
    PM4 &= 0xF7U;
}

/***********************************************************************************************************************
* Function Name: R_CSI01_Start
* Description  : This function starts the CSI01 module operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_CSI01_Start(void)
{
    SO0 &= ~_0200_SAU_CH1_CLOCK_OUTPUT_1;   /* CSI01 clock initial level */
    SO0 &= ~_0002_SAU_CH1_DATA_OUTPUT_1;           /* CSI01 SO initial level */
    SOE0 |= _0002_SAU_CH1_OUTPUT_ENABLE;           /* enable CSI01 output */
    SS0 |= _0002_SAU_CH1_START_TRG_ON;             /* enable CSI01 */
    CSIIF01 = 0U;    /* clear INTCSI01 interrupt flag */
    CSIMK01 = 0U;    /* enable INTCSI01 */
}

/***********************************************************************************************************************
* Function Name: R_CSI01_Stop
* Description  : This function stops the CSI01 module operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_CSI01_Stop(void)
{
    CSIMK01 = 1U;    /* disable INTCSI01 interrupt */
    ST0 |= _0002_SAU_CH1_STOP_TRG_ON;        /* disable CSI01 */
    SOE0 &= ~_0002_SAU_CH1_OUTPUT_ENABLE;    /* disable CSI01 output */
    CSIIF01 = 0U;    /* clear INTCSI01 interrupt flag */
}

/***********************************************************************************************************************
* Function Name: R_CSI01_Send_Receive
* Description  : This function sends and receives CSI01 data.
* Arguments    : tx_buf -
*                    transfer buffer pointer
*                tx_num -
*                    buffer size
*                rx_buf -
*                    receive buffer pointer
* Return Value : status -
*                    MD_OK or MD_ARGERROR
***********************************************************************************************************************/
MD_STATUS R_CSI01_Send_Receive(uint8_t * const tx_buf, uint16_t tx_num, uint8_t * const rx_buf)
{
    MD_STATUS status = MD_OK;

    if (tx_num < 1U)
    {
        status = MD_ARGERROR;
    }
    else
    {
        g_csi01_tx_count = tx_num;        /* send data count */
        gp_csi01_tx_address = tx_buf;     /* send buffer pointer */
        gp_csi01_rx_address = rx_buf;     /* receive buffer pointer */
        CSIMK01 = 1U;                     /* disable INTCSI01 interrupt */

        if (0U != gp_csi01_tx_address)
        {
            SIO01 = *gp_csi01_tx_address;    /* started by writing data to SDR[7:0] */
            gp_csi01_tx_address++;
        }
        else
        {
            SIO01 = 0xFFU;
        }

        g_csi01_tx_count--;
        CSIMK01 = 0U;                     /* enable INTCSI01 interrupt */
    }

    return (status);
}

/***********************************************************************************************************************
* Function Name: R_CSI10_Create
* Description  : This function initializes the CSI10 module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_CSI10_Create(void)
{
    ST0 |= _0004_SAU_CH2_STOP_TRG_ON;    /* disable CSI10 */
    CSIMK10 = 1U;    /* disable INTCSI10 interrupt */
    CSIIF10 = 0U;    /* clear INTCSI10 interrupt flag */
    /* Set INTCSI10 low priority */
    CSIPR110 = 1U;
    CSIPR010 = 1U;
    SIR02 = _0002_SAU_SIRMN_PECTMN | _0001_SAU_SIRMN_OVCTMN;    /* clear error flag */
    SMR02 = _0020_SAU_SMRMN_INITIALVALUE | _0000_SAU_CLOCK_SELECT_CK00 | _0000_SAU_CLOCK_MODE_CKS |
            _0000_SAU_TRIGGER_SOFTWARE | _0000_SAU_MODE_CSI | _0000_SAU_TRANSFER_END;
    SCR02 = _C000_SAU_RECEPTION_TRANSMISSION | _0000_SAU_TIMING_1 | _0000_SAU_MSB | _0007_SAU_LENGTH_8;
    SDR02 = _CE00_CSI10_DIVISOR;
    SO0 |= _0400_SAU_CH2_CLOCK_OUTPUT_1;    /* CSI10 clock initial level */
    SO0 &= ~_0004_SAU_CH2_DATA_OUTPUT_1;    /* CSI10 SO initial level */
    SOE0 |= _0004_SAU_CH2_OUTPUT_ENABLE;    /* enable CSI10 output */
    /* Set SI10 pin */
    PMC0 &= 0xF7U;
    PM0 |= 0x08U;
    /* Set SO10 pin */
    PMC0 &= 0xFBU;
    P0 |= 0x04U;
    PM0 &= 0xFBU;
    /* Set SCK10 pin */
    P0 |= 0x10U;
    PM0 &= 0xEFU;
}

/***********************************************************************************************************************
* Function Name: R_CSI10_Start
* Description  : This function starts the CSI10 module operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_CSI10_Start(void)
{
    SO0 |= _0400_SAU_CH2_CLOCK_OUTPUT_1;    /* CSI10 clock initial level */
    SO0 &= ~_0004_SAU_CH2_DATA_OUTPUT_1;           /* CSI10 SO initial level */
    SOE0 |= _0004_SAU_CH2_OUTPUT_ENABLE;           /* enable CSI10 output */
    SS0 |= _0004_SAU_CH2_START_TRG_ON;             /* enable CSI10 */
    CSIIF10 = 0U;    /* clear INTCSI10 interrupt flag */
    CSIMK10 = 0U;    /* enable INTCSI10 */
}

/***********************************************************************************************************************
* Function Name: R_CSI10_Stop
* Description  : This function stops the CSI10 module operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_CSI10_Stop(void)
{
    CSIMK10 = 1U;    /* disable INTCSI10 interrupt */
    ST0 |= _0004_SAU_CH2_STOP_TRG_ON;        /* disable CSI10 */
    SOE0 &= ~_0004_SAU_CH2_OUTPUT_ENABLE;    /* disable CSI10 output */
    CSIIF10 = 0U;    /* clear INTCSI10 interrupt flag */
}

/***********************************************************************************************************************
* Function Name: R_CSI10_Send_Receive
* Description  : This function sends and receives CSI10 data.
* Arguments    : tx_buf -
*                    transfer buffer pointer
*                tx_num -
*                    buffer size
*                rx_buf -
*                    receive buffer pointer
* Return Value : status -
*                    MD_OK or MD_ARGERROR
***********************************************************************************************************************/
MD_STATUS R_CSI10_Send_Receive(uint8_t * const tx_buf, uint16_t tx_num, uint8_t * const rx_buf)
{
    MD_STATUS status = MD_OK;

    if (tx_num < 1U)
    {
        status = MD_ARGERROR;
    }
    else
    {
        g_csi10_tx_count = tx_num;        /* send data count */
        gp_csi10_tx_address = tx_buf;     /* send buffer pointer */
        gp_csi10_rx_address = rx_buf;     /* receive buffer pointer */
        CSIMK10 = 1U;                     /* disable INTCSI10 interrupt */

        if (0U != gp_csi10_tx_address)
        {
            SIO10 = *gp_csi10_tx_address;    /* started by writing data to SDR[7:0] */
            gp_csi10_tx_address++;
        }
        else
        {
            SIO10 = 0xFFU;
        }

        g_csi10_tx_count--;
        CSIMK10 = 0U;                     /* enable INTCSI10 interrupt */
    }

    return (status);
}

/***********************************************************************************************************************
* Function Name: R_SAU1_Create
* Description  : This function initializes the SAU1 module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_SAU1_Create(void)
{
    SAU1EN = 1U;    /* supply SAU1 clock */
    NOP();
    NOP();
    NOP();
    NOP();
    SPS1 = _0002_SAU_CK00_FCLK_2 | _0020_SAU_CK01_FCLK_2;
    R_UART2_Create();
}

/***********************************************************************************************************************
* Function Name: R_UART2_Create
* Description  : This function initializes the UART2 module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_UART2_Create(void)
{
    ST1 |= _0002_SAU_CH1_STOP_TRG_ON | _0001_SAU_CH0_STOP_TRG_ON;    /* disable UART2 receive and transmit */
    STMK2 = 1U;    /* disable INTST2 interrupt */
    STIF2 = 0U;    /* clear INTST2 interrupt flag */
    SRMK2 = 1U;    /* disable INTSR2 interrupt */
    SRIF2 = 0U;    /* clear INTSR2 interrupt flag */
    SREMK2 = 1U;   /* disable INTSRE2 interrupt */
    SREIF2 = 0U;   /* clear INTSRE2 interrupt flag */
    /* Set INTST2 low priority */
    STPR12 = 1U;
    STPR02 = 1U;
    /* Set INTSR2 low priority */
    SRPR12 = 1U;
    SRPR02 = 1U;
    SMR10 = _0020_SAU_SMRMN_INITIALVALUE | _0000_SAU_CLOCK_SELECT_CK00 | _0000_SAU_TRIGGER_SOFTWARE |
            _0002_SAU_MODE_UART | _0000_SAU_TRANSFER_END;
    SCR10 = _8000_SAU_TRANSMISSION | _0000_SAU_INTSRE_MASK | _0000_SAU_PARITY_NONE | _0080_SAU_LSB | _0010_SAU_STOP_1 |
            _0007_SAU_LENGTH_8;
    SDR10 = _CE00_UART2_TRANSMIT_DIVISOR;
    NFEN0 |= _10_SAU_RXD2_FILTER_ON;
    SIR11 = _0004_SAU_SIRMN_FECTMN | _0002_SAU_SIRMN_PECTMN | _0001_SAU_SIRMN_OVCTMN;    /* clear error flag */
    SMR11 = _0020_SAU_SMRMN_INITIALVALUE | _0000_SAU_CLOCK_SELECT_CK00 | _0100_SAU_TRIGGER_RXD | _0000_SAU_EDGE_FALL |
            _0002_SAU_MODE_UART | _0000_SAU_TRANSFER_END;
    SCR11 = _4000_SAU_RECEPTION | _0000_SAU_INTSRE_MASK | _0000_SAU_PARITY_NONE | _0080_SAU_LSB | _0010_SAU_STOP_1 |
            _0007_SAU_LENGTH_8;
    SDR11 = _CE00_UART2_RECEIVE_DIVISOR;
    SO1 |= _0001_SAU_CH0_DATA_OUTPUT_1;
    SOL1 |= _0000_SAU_CHANNEL0_NORMAL;    /* output level normal */
    SOE1 |= _0001_SAU_CH0_OUTPUT_ENABLE;    /* enable UART2 output */
    /* Set RxD2 pin */
    PM1 |= 0x10U;
    /* Set TxD2 pin */
    PMC1 &= 0xF7U;
    P1 |= 0x08U;
    PM1 &= 0xF7U;
}

/***********************************************************************************************************************
* Function Name: R_UART2_Start
* Description  : This function starts the UART2 module operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_UART2_Start(void)
{
    SO1 |= _0001_SAU_CH0_DATA_OUTPUT_1;    /* output level normal */
    SOE1 |= _0001_SAU_CH0_OUTPUT_ENABLE;    /* enable UART2 output */
    SS1 |= _0002_SAU_CH1_START_TRG_ON | _0001_SAU_CH0_START_TRG_ON;    /* enable UART2 receive and transmit */
    STIF2 = 0U;    /* clear INTST2 interrupt flag */
    SRIF2 = 0U;    /* clear INTSR2 interrupt flag */
    STMK2 = 0U;    /* enable INTST2 interrupt */
    SRMK2 = 0U;    /* enable INTSR2 interrupt */
}

/***********************************************************************************************************************
* Function Name: R_UART2_Stop
* Description  : This function stops the UART2 module operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_UART2_Stop(void)
{
    STMK2 = 1U;    /* disable INTST2 interrupt */
    SRMK2 = 1U;    /* disable INTSR2 interrupt */
    ST1 |= _0002_SAU_CH1_STOP_TRG_ON | _0001_SAU_CH0_STOP_TRG_ON;    /* disable UART2 receive and transmit */
    SOE1 &= ~_0001_SAU_CH0_OUTPUT_ENABLE;    /* disable UART2 output */
    STIF2 = 0U;    /* clear INTST2 interrupt flag */
    SRIF2 = 0U;    /* clear INTSR2 interrupt flag */
   
}

/***********************************************************************************************************************
* Function Name: R_UART2_Receive
* Description  : This function receives UART2 data.
* Arguments    : rx_buf -
*                    receive buffer pointer
*                rx_num -
*                    buffer size
* Return Value : status -
*                    MD_OK or MD_ARGERROR
***********************************************************************************************************************/
MD_STATUS R_UART2_Receive(uint8_t * const rx_buf, uint16_t rx_num)
{
    MD_STATUS status = MD_OK;

    if (rx_num < 1U)
    {
        status = MD_ARGERROR;
    }
    else
    {
        g_uart2_rx_count = 0U;
        g_uart2_rx_length = rx_num;
        gp_uart2_rx_address = rx_buf;
    }

    return (status);
}

/***********************************************************************************************************************
* Function Name: R_UART2_Send
* Description  : This function sends UART2 data.
* Arguments    : tx_buf -
*                    transfer buffer pointer
*                tx_num -
*                    buffer size
* Return Value : status -
*                    MD_OK or MD_ARGERROR
***********************************************************************************************************************/
MD_STATUS R_UART2_Send(uint8_t * const tx_buf, uint16_t tx_num)
{
    MD_STATUS status = MD_OK;

    if (tx_num < 1U)
    {
        status = MD_ARGERROR;
    }
    else
    {
        gp_uart2_tx_address = tx_buf;
        g_uart2_tx_count = tx_num;
        STMK2 = 1U;    /* disable INTST2 interrupt */
        TXD2 = *gp_uart2_tx_address;
        gp_uart2_tx_address++;
        g_uart2_tx_count--;
        STMK2 = 0U;    /* enable INTST2 interrupt */
    }

    return (status);
}

/***********************************************************************************************************************
* Function Name: R_IICA0_Create
* Description  : This function initializes the IICA0 module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_IICA0_Create(void)
{
    IICA0EN = 1U; /* supply IICA0 clock */
    IICE0 = 0U; /* disable IICA0 operation */
    IICAMK0 = 1U; /* disable INTIICA0 interrupt */
    IICAIF0 = 0U; /* clear INTIICA0 interrupt flag */
    /* Set INTIICA0 low priority */
    IICAPR10 = 1U;
    IICAPR00 = 1U; 
    /* Set SCLA0, SDAA0 pin */
    P6 &= 0xFCU;
    PM6 |= 0x03U;
    SMC0 = 0U;
    IICWL0 = _4C_IICA0_IICWL_VALUE;
    IICWH0 = _55_IICA0_IICWH_VALUE;
    IICCTL01 &= (uint8_t)~_01_IICA_fCLK_HALF;
    SVA0 = _10_IICA0_MASTERADDRESS;
    STCEN0 = 1U;
    IICRSV0 = 1U;
    SPIE0 = 0U;
    WTIM0 = 1U;
    ACKE0 = 1U;
    IICAMK0 = 0U;
    IICE0 = 1U;
    LREL0 = 1U;
    /* Set SCLA0, SDAA0 pin */
    PM6 &= 0xFCU;
}

/***********************************************************************************************************************
* Function Name: R_IICA0_Stop
* Description  : This function stops IICA0 module operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_IICA0_Stop(void)
{
    IICE0 = 0U;    /* disable IICA0 operation */
}

/***********************************************************************************************************************
* Function Name: R_IICA0_StopCondition
* Description  : This function sets IICA0 stop condition flag.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_IICA0_StopCondition(void)
{
    SPT0 = 1U;     /* set stop condition flag */
}

/***********************************************************************************************************************
* Function Name: R_IICA0_Master_Send
* Description  : This function starts to send data as master mode.
* Arguments    : adr -
*                    transfer address
*                tx_buf -
*                    transfer buffer pointer
*                tx_num -
*                    buffer size
*                wait -
*                    wait for start condition
* Return Value : status -
*                    MD_OK or MD_ERROR1 or MD_ERROR2
***********************************************************************************************************************/
MD_STATUS R_IICA0_Master_Send(uint8_t adr, uint8_t * const tx_buf, uint16_t tx_num, uint8_t wait)
{
    MD_STATUS status = MD_OK;

    IICAMK0 = 1U;  /* disable INTIICA0 interrupt */
    
    if (1U == IICBSY0)
    {
        /* Check bus busy */
        IICAMK0 = 0U;  /* enable INTIICA0 interrupt */
        status = MD_ERROR1;
    }
    else
    {
        STT0 = 1U; /* send IICA0 start condition */
        IICAMK0 = 0U;  /* enable INTIICA0 interrupt */
        
        /* Wait */
        while (wait--)
        {
            ;
        }
        
        if (0U == STD0)
        {
            status = MD_ERROR2;
        }
		
        /* Set parameter */
        g_iica0_tx_cnt = tx_num;
        gp_iica0_tx_address = tx_buf;
        g_iica0_master_status_flag = _00_IICA_MASTER_FLAG_CLEAR;
        adr &= (uint8_t)~0x01U; /* set send mode */
        IICA0 = adr; /* send address */
    }

    return (status);
}

/***********************************************************************************************************************
* Function Name: R_IICA0_Master_Receive
* Description  : This function starts to receive IICA0 data as master mode.
* Arguments    : adr -
*                    receive address
*                rx_buf -
*                    receive buffer pointer
*                rx_num -
*                    buffer size
*                wait -
*                    wait for start condition
* Return Value : status -
*                    MD_OK or MD_ERROR1 or MD_ERROR2
***********************************************************************************************************************/
MD_STATUS R_IICA0_Master_Receive(uint8_t adr, uint8_t * const rx_buf, uint16_t rx_num, uint8_t wait)
{
    MD_STATUS status = MD_OK;

    IICAMK0 = 1U;  /* disable INTIIA0 interrupt */
    
    if (1U == IICBSY0)
    {
        /* Check bus busy */
        IICAMK0 = 0U;  /* enable INTIIA0 interrupt */
        status = MD_ERROR1;
    }
    else
    {
        STT0 = 1U; /* set IICA0 start condition */
        IICAMK0 = 0U;  /* enable INTIIA0 interrupt */
        
        /* Wait */
        while (wait--)
        {
            ;
        }
        
        if (0U == STD0)
        {
            status = MD_ERROR2;
        }
		
        /* Set parameter */
        g_iica0_rx_len = rx_num;
        g_iica0_rx_cnt = 0U;
        gp_iica0_rx_address = rx_buf;
        g_iica0_master_status_flag  = _00_IICA_MASTER_FLAG_CLEAR;
        adr |= 0x01U; /* set receive mode */
        IICA0 = adr; /* receive address */
    }

    return (status);
}

/***********************************************************************************************************************
* Function Name: R_IICA1_Create
* Description  : This function initializes the IICA1 module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_IICA1_Create(void)
{
    IICA1EN = 1U; /* supply IICA1 clock */
    IICE1 = 0U; /* disable IICA1 operation */
    IICAMK1 = 1U; /* disable INTIICA1 interrupt */
    IICAIF1 = 0U; /* clear INTIICA1 interrupt flag */
    /* Set INTIICA1 low priority */
    IICAPR11 = 1U;
    IICAPR01 = 1U; 
    /* Set SCLA1, SDAA1 pin */
    P6 &= 0xF3U;
    PM6 |= 0x0CU;
    SMC1 = 0U;
    IICWL1 = _4C_IICA1_IICWL_VALUE;
    IICWH1 = _55_IICA1_IICWH_VALUE;
    IICCTL11 &= (uint8_t)~_01_IICA_fCLK_HALF;
    SVA1 = _10_IICA1_MASTERADDRESS;
    STCEN1 = 1U;
    IICRSV1 = 1U;
    SPIE1 = 0U;
    WTIM1 = 1U;
    ACKE1 = 1U;
    IICAMK1 = 0U;
    IICE1 = 1U;
    LREL1 = 1U;
    /* Set SCLA1, SDAA1 pin */
    PM6 &= 0xF3U;
}

/***********************************************************************************************************************
* Function Name: R_IICA1_Stop
* Description  : This function stops IICA1 module operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_IICA1_Stop(void)
{
    IICE1 = 0U;    /* disable IICA1 operation */
}

/***********************************************************************************************************************
* Function Name: R_IICA1_StopCondition
* Description  : This function sets IICA1 stop condition flag.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_IICA1_StopCondition(void)
{
    SPT1 = 1U;     /* set stop condition flag */
}

/***********************************************************************************************************************
* Function Name: R_IICA1_Master_Send
* Description  : This function starts to send data as master mode.
* Arguments    : adr -
*                    transfer address
*                tx_buf -
*                    transfer buffer pointer
*                tx_num -
*                    buffer size
*                wait -
*                    wait for start condition
* Return Value : status -
*                    MD_OK or MD_ERROR1 or MD_ERROR2
***********************************************************************************************************************/
MD_STATUS R_IICA1_Master_Send(uint8_t adr, uint8_t * const tx_buf, uint16_t tx_num, uint8_t wait)
{
    MD_STATUS status = MD_OK;

    IICAMK1 = 1U;  /* disable INTIICA1 interrupt */
    
    if (1U == IICBSY1)
    {
        /* Check bus busy */
        IICAMK1 = 0U;  /* enable INTIICA1 interrupt */
        status = MD_ERROR1;
    }
    else
    {
        STT1 = 1U; /* send IICA1 start condition */
        IICAMK1 = 0U;  /* enable INTIICA1 interrupt */
        
        /* Wait */
        while (wait--)
        {
            ;
        }
        
        if (0U == STD1)
        {
            status = MD_ERROR2;
        }
		
        /* Set parameter */
        g_iica1_tx_cnt = tx_num;
        gp_iica1_tx_address = tx_buf;
        g_iica1_master_status_flag = _00_IICA_MASTER_FLAG_CLEAR;
        adr &= (uint8_t)~0x01U; /* set send mode */
        IICA1 = adr; /* send address */
    }

    return (status);
}

/***********************************************************************************************************************
* Function Name: R_IICA1_Master_Receive
* Description  : This function starts to receive IICA1 data as master mode.
* Arguments    : adr -
*                    receive address
*                rx_buf -
*                    receive buffer pointer
*                rx_num -
*                    buffer size
*                wait -
*                    wait for start condition
* Return Value : status -
*                    MD_OK or MD_ERROR1 or MD_ERROR2
***********************************************************************************************************************/
MD_STATUS R_IICA1_Master_Receive(uint8_t adr, uint8_t * const rx_buf, uint16_t rx_num, uint8_t wait)
{
    MD_STATUS status = MD_OK;

    IICAMK1 = 1U;  /* disable INTIIA1 interrupt */
    
    if (1U == IICBSY1)
    {
        /* Check bus busy */
        IICAMK1 = 0U;  /* enable INTIIA1 interrupt */
        status = MD_ERROR1;
    }
    else
    {
        STT1 = 1U; /* set IICA1 start condition */
        IICAMK1 = 0U;  /* enable INTIIA1 interrupt */
        
        /* Wait */
        while (wait--)
        {
            ;
        }
        
        if (0U == STD1)
        {
            status = MD_ERROR2;
        }
		
        /* Set parameter */
        g_iica1_rx_len = rx_num;
        g_iica1_rx_cnt = 0U;
        gp_iica1_rx_address = rx_buf;
        g_iica1_master_status_flag  = _00_IICA_MASTER_FLAG_CLEAR;
        adr |= 0x01U; /* set receive mode */
        IICA1 = adr; /* receive address */
    }

    return (status);
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
