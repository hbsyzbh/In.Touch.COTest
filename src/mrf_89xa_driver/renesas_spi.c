#include "iodefine.h"
#include "../r_cg_macrodriver.h"
#include "../r_cg_serial.h"
#include "../r_cg_userdefine.h"
#include "drv_89xa_export.h"
#include "drv_mrf_89xa.h"

//need length is 1 , but for debug make it 8
static unsigned char SPI01_CMD_BUF[8];
static unsigned char SPI01_ACK_BUF[8];

unsigned char g_csi01_done = 0;

void on_csi01_done()
{
	g_csi01_done = 1;
}

/*********************************************************************
* Function:         void SPIPut(uint8_t v)
*
* PreCondition:     SPI has been configured
*
* Input:		    v - is the byte that needs to be transfered
*
* Output:		    none
*
* Side Effects:	    SPI transmits the byte
*
* Overview:		    This function will send a byte over the SPI
*
* Note:			    None
********************************************************************/
 void SPIPut(uint8_t v)
{
	 R_CSI01_Send_Receive(SPI01_CMD_BUF, 1, SPI01_ACK_BUF);
	 while(g_csi01_done == 0);
}


/*********************************************************************
* Function:         uint8_t SPIGet(void)
*
* PreCondition:     SPI has been configured
*
* Input:		    none
*
* Output:		    uint8_t - the byte that was last received by the SPI
*
* Side Effects:	    none
*
* Overview:		    This function will read a byte over the SPI
*
* Note:			    None
********************************************************************/

BYTE SPIGet(void)
{
    SPIPut(0x00);
		//write a dummy value to read the data from the other SPI module
		//read the SSPBUF value for data transmitted by other SSP module
    return SPI01_ACK_BUF[0];
}
