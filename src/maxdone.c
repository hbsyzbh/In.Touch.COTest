#include "iodefine.h"
#include "r_cg_macrodriver.h"
#include "r_cg_serial.h"
#include "r_cg_userdefine.h"
#include "r_cg_rtc.h"
#include "string.h"

unsigned char g_i2c_done = 0;
unsigned char g_u2_sendend = 0;
unsigned char debug = 0;
void UART2_Sendstr(char *str)
{
	if(str == 0) return;
	if(! debug) return;

	g_u2_sendend = 0;
	R_UART2_Send(str, strlen(str));
	for(; g_u2_sendend == 0;)
	{
		;//R_WDT_Restart();
	}
}

typedef enum {
	Cmd_getType,
	Cmd_syncRTC,
	Cmd_getRTC,
	Cmd_testSPIFlash,
	Cmd_testI2CPort,
	Cmd_testRF 			= 5,
	Cmd_testI2CE2prom,
	Cmd_testEthernet,
	Cmd_testLED,

	Cmd_initI2CPort = 0xF4
}	CMD_TYPE;

#define ACK_HEAD (0x4F)

#define ACK_CMD_POS		(1)
#define ACK_LEN_POS		(2)
#define ACK_DATA_POS	(3)

#define ACK_OK	(0xF0)
#define ACK_NG	(0xFF)

typedef enum {
	Uart_StartwaitHead,
	Uart_waittigHead,
	Uart_revHeadStartwaitlen,
	Uart_Waittinglen,
	Uart_revlenStartwaitData,
	Uart_waittingData,
	Uart_analysisData
}	UART_STATE;

UART_STATE UartState;

unsigned char cmd = 0;
unsigned char datalen = 0;

unsigned char revBuff[256] = {0};

static void startWaitForNewCmd()
{
	UartState = 0;
}

static char isCmdDatalenValied()
{
	switch(cmd) {
		case Cmd_syncRTC:
			return datalen == 6;
			break;
		case Cmd_getRTC:
		case Cmd_getType:
		case Cmd_testSPIFlash:
		case Cmd_testI2CPort:
		case Cmd_testRF:
		case Cmd_testI2CE2prom:
		case Cmd_testEthernet:
			return datalen == 0;
			break;
		case Cmd_testLED:
			return datalen == 1;
			break;

		case Cmd_initI2CPort:
			return datalen == 0;
			break;
	}

	return 0;
}

void SetLed(unsigned char leds)
{
	unsigned char b = leds & 0x1;
	unsigned char g = leds & 0x2;
	unsigned char r = leds & 0x4;

	P15_bit.no0 = ! b;
	P15_bit.no1 = ! g;
	P15_bit.no2 = ! r;
}



#define S25FL_JEDECID		(0x9F)
#define S25FL_ERASE_SA		(0x20)
#define S25FL_WRITE			(0x02)
#define S25FL_READ			(0x03)

#define S25FL_WRITEEN		(0x06)

static unsigned char SPI10_CMD_BUF[32];
static unsigned char SPI10_ACK_BUF[32];

unsigned char g_csi10_done = 0;

static void spi10_checkFlashID(void)
{
	char str[16] = "";

	SPI10_CMD_BUF[0] = S25FL_JEDECID;
	P0_bit.no1 = 0;
	g_csi10_done = 0;
	R_CSI10_Send_Receive(SPI10_CMD_BUF, 4, SPI10_ACK_BUF);
	while(g_csi10_done == 0);

	//01 40 15  S25FL116K
	if( (SPI10_ACK_BUF[1] == 0x01) &&
			(SPI10_ACK_BUF[2] == 0x40) &&
			(SPI10_ACK_BUF[3] == 0x15)
		) {
		UART2_Sendstr("is S25FL116K");
	} else if ( (SPI10_ACK_BUF[1] == 0x1F) &&
				(SPI10_ACK_BUF[2] == 0x86) &&
				(SPI10_ACK_BUF[3] == 0x01)
	){
		UART2_Sendstr("is AT25SF161");
	} else {
		UART2_Sendstr("Unknown");
	}

	str[0] = SPI10_ACK_BUF[1] / 16 + '0';
	str[1] = SPI10_ACK_BUF[1] % 16 + '0';
	str[2] = SPI10_ACK_BUF[2] / 16 + '0';
	str[3] = SPI10_ACK_BUF[2] % 16 + '0';
	str[4] = SPI10_ACK_BUF[3] / 16 + '0';
	str[5] = SPI10_ACK_BUF[3] % 16 + '0';

	if(1) {
		UART2_Sendstr("JEDECID:0x");
		UART2_Sendstr(str);
	}
}

static void spi10_read(unsigned char *addr, uint16_t datalen)
{
	SPI10_CMD_BUF[0] = S25FL_READ;
	memcpy(&SPI10_CMD_BUF[1], addr, 3);
	P0_bit.no1 = 0;
	g_csi10_done = 0;
	R_CSI10_Send_Receive(SPI10_CMD_BUF, datalen + 4, SPI10_ACK_BUF);
	while(g_csi10_done == 0);
}

static void spi10_writeEnable()
{
	SPI10_CMD_BUF[0] = S25FL_WRITEEN;

	P0_bit.no1 = 0;
	g_csi10_done = 0;
	R_CSI10_Send_Receive(SPI10_CMD_BUF, 1, NULL);
	while(g_csi10_done == 0);
}

static void spi10_writeDone()
{
	unsigned char timeout, dly;
	SPI10_CMD_BUF[0] = 0x05;

	for(timeout = 0; timeout < 200; timeout++)
	{
		P0_bit.no1 = 0;
		g_csi10_done = 0;
		R_CSI10_Send_Receive(SPI10_CMD_BUF, 2, SPI10_ACK_BUF);
		while(g_csi10_done == 0);

		if((SPI10_ACK_BUF[1] & 0x01) == 0x00){
			return;
		}


		for(dly = 0; dly < 200; dly++)
			;
	}
}

static void spi10_erase(unsigned char *addr)
{
	spi10_writeEnable();

	SPI10_CMD_BUF[0] = S25FL_ERASE_SA;
	memcpy(&SPI10_CMD_BUF[1], addr, 3);

	P0_bit.no1 = 0;
	g_csi10_done = 0;
	R_CSI10_Send_Receive(SPI10_CMD_BUF, 4,SPI10_ACK_BUF);
	while(g_csi10_done == 0);

	spi10_writeDone();
}

static void spi10_write(unsigned char *addr, unsigned char *data, uint16_t datalen)
{
	spi10_writeEnable();

	SPI10_CMD_BUF[0] = S25FL_WRITE;
	memcpy(&SPI10_CMD_BUF[1], addr, 3);
	memcpy(&SPI10_CMD_BUF[4], data, datalen);

	P0_bit.no1 = 0;
	g_csi10_done = 0;
	R_CSI10_Send_Receive(SPI10_CMD_BUF, datalen + 4,SPI10_ACK_BUF);
	while(g_csi10_done == 0);

	spi10_writeDone();
}

void on_csi10_done()
{
	P0_bit.no1 = 1;
	g_csi10_done = 1;
}

/**
	S25FL116K Main Memory Address Map
	Sector Size (kbyte)	4
	Sector Count  512
	Sector Range   SA0					SA511
	Address Range  000000h-000FFFh    1FF000h-1FFFFFh
 */
#define TEST_ITEM		(2)
unsigned char TestAddr[TEST_ITEM][3] = {
   {0x00, 0x00, 0x00},
   {0x1F, 0xF0, 0x00}
};

unsigned char TestSpiFlash()
{
	int i, j;

	spi10_checkFlashID();

	for(i = 0; i < TEST_ITEM; i++)
	{
		//erase
		spi10_erase(TestAddr[i]);
		spi10_read(TestAddr[i], 8);
		for(j = 0; j < 8; j++)
		if(0xFF != SPI10_ACK_BUF[4+j]) {
			UART2_Sendstr("Check read as 0xFF NG");
			return ACK_NG;
		}
		//write 1234567
		spi10_write(TestAddr[i], "1234567", 8);
		//read as 1234567
		spi10_read(TestAddr[i], 8);
		if(0 != strncmp("1234567", &SPI10_ACK_BUF[4], 8)) {
			UART2_Sendstr("Check read as 1234567 NG");
			return ACK_NG;
		}
		//erase
		spi10_erase(TestAddr[i]);
		//read as all 0xFF
		spi10_read(TestAddr[i], 8);
		for(j = 0; j < 8; j++)
		if(0xFF != SPI10_ACK_BUF[4+j]) {
			UART2_Sendstr("Check read as 0xFF NG");
			return ACK_NG;
		}
	}


	return ACK_OK;
}

unsigned char sendbuf[8] = " Maxdone";
unsigned char revbuf[8] = {0};
const unsigned char deviceaddr = 0xA0;
unsigned char I2cAddr = 0x0;

static void waitI2cDone()
{
	unsigned int i = 0;

	while((0 == IICAIF0) && (i < 0xFFFF))
	{
		i++;
	}
}

unsigned char TestI2cPort()
{
	unsigned int i;
	sendbuf[0] = I2cAddr;

	STT0 = 1;	//start
	IICAIF0 = 0U; /* clear INTIICA0 interrupt flag */
	IICA0 = 0xA0; // 写
	waitI2cDone();

	IICAIF0 = 0U; /* clear INTIICA0 interrupt flag */
	IICA0 = 0x00; // 地址
	waitI2cDone();

	STT0 = 1;	//start
	IICAIF0 = 0U; /* clear INTIICA0 interrupt flag */
	IICA0 = 0xA1; // 读
	waitI2cDone();


	ACKE0 = 1U;
	for(i = 0; i < 7; i++)
	{
		IICAIF0 = 0U; /* clear INTIICA0 interrupt flag */
		WREL0 = 1;
		waitI2cDone();
		revbuf[i] = IICA0;
	}

	ACKE0 = 0U;

	IICAIF0 = 0U; /* clear INTIICA0 interrupt flag */
	WREL0 = 1;
	waitI2cDone();
	revbuf[7] = IICA0;

	SPT0 = 1;	//stop

	if(0 == strncmp("Maxdone", revbuf, 7)){
		return ACK_OK;
	}

	return ACK_NG;
}

unsigned char InitI2cPort()
{
	unsigned int i;

	sendbuf[0] = I2cAddr;

	//R_IICA0_Master_Send(deviceaddr, sendbuf, 8, 10);
	STT0 = 1;	//start
	IICAIF0 = 0U; /* clear INTIICA0 interrupt flag */
	IICA0 = 0xA0; // 写
	waitI2cDone();

	for(i = 0; i < 8; i++)
	{
		IICAIF0 = 0U; /* clear INTIICA0 interrupt flag */
		IICA0 = sendbuf[i]; // 地址
		waitI2cDone();
	}
	SPT0 = 1;	//stop

	for(i = 0; i < 5000; i++)
		;

	return TestI2cPort();
}


const unsigned char notsupport[] = {0x40, 0, 0};
const unsigned char hardwareType[] = {ACK_HEAD, 0, 1, 'C'};
unsigned char ackbuff[16] = {ACK_HEAD, 0};
rtc_counter_value_t value;

static void fillRTCRespBuf()
{
	ackbuff[ACK_DATA_POS+0] = value.year;
	ackbuff[ACK_DATA_POS+1] = value.month;
	ackbuff[ACK_DATA_POS+2] = value.day;
	ackbuff[ACK_DATA_POS+3] = value.hour;
	ackbuff[ACK_DATA_POS+4] = value.min;
	ackbuff[ACK_DATA_POS+5] = value.sec;
}

static void fillRTCRespValue()
{
	value.year = revBuff[ACK_DATA_POS+0];
	value.month = revBuff[ACK_DATA_POS+1];
	value.day = revBuff[ACK_DATA_POS+2];
	value.hour = revBuff[ACK_DATA_POS+3];
	value.min = revBuff[ACK_DATA_POS+4];
	value.sec = revBuff[ACK_DATA_POS+5];
}

static unsigned char TestMRF89XA()
{
	drv_testMRF89XA();
	return ACK_NG;
}

static void analysisCmd()
{

	ackbuff[ACK_CMD_POS] = cmd;

	switch(cmd) {
	case Cmd_getType:
		R_UART2_Send(hardwareType, 4);
		break;
	case Cmd_syncRTC:
		//R_UART2_Send(hardwareType, 4);
		fillRTCRespValue();
		if(MD_OK == R_RTC_Set_CounterValue(value))
		{
			fillRTCRespBuf();
			R_UART2_Send(ackbuff, 	ACK_DATA_POS+6);
		} else {
			R_UART2_Send(notsupport, 3);
		}
		break;

	case Cmd_getRTC:
		ackbuff[ACK_LEN_POS] = 6;
		if(MD_OK ==	R_RTC_Get_CounterValue(&value))
		{
			fillRTCRespBuf();
		} else {
			memset(&ackbuff[ACK_DATA_POS], 0, 6);
		}
		R_UART2_Send(ackbuff, 	ACK_DATA_POS+6);
		break;

	case Cmd_testSPIFlash:
		ackbuff[ACK_DATA_POS] = TestSpiFlash();
		R_UART2_Send(ackbuff, 	ACK_DATA_POS + 1);
		break;

	case Cmd_testLED:
		SetLed(revBuff[ACK_DATA_POS]);
		ackbuff[ACK_DATA_POS] = revBuff[ACK_DATA_POS];
		R_UART2_Send(ackbuff, 	ACK_DATA_POS + 1);
		break;

	case Cmd_testI2CPort:
		ackbuff[ACK_DATA_POS] = TestI2cPort();
		R_UART2_Send(ackbuff, 	ACK_DATA_POS + 1);
		break;

	case Cmd_initI2CPort:
		ackbuff[ACK_DATA_POS] = InitI2cPort();
		R_UART2_Send(ackbuff, 	ACK_DATA_POS + 1);
		break;

	case Cmd_testRF:
		ackbuff[ACK_DATA_POS] = TestMRF89XA();
		R_UART2_Send(ackbuff, 	ACK_DATA_POS + 1);
		break;

	default:
		R_UART2_Send(notsupport, 3);
		break;
	}
	startWaitForNewCmd();
}

void doUartTask()
{
	//TestMRF89XA();

	switch(UartState)
	{
		case Uart_StartwaitHead:
			R_UART2_Receive(revBuff, 1);
			UartState++;
			break;
		case Uart_revHeadStartwaitlen:
			R_UART2_Receive(&revBuff[1], 2);
			UartState++;
			break;
		case Uart_revlenStartwaitData:
			if(datalen > 0 ) {
				R_UART2_Receive(&revBuff[3], datalen);
				UartState++;
			}
			break;

		case Uart_analysisData:
			analysisCmd();
			break;

		default:
			break;
	}
}

void on_uart2_receiveend(void)
{
	switch(UartState)
	{
		case Uart_waittigHead:
			if((*revBuff) == 0xF4){
				UartState = Uart_revHeadStartwaitlen;
			} else {
				UartState = Uart_StartwaitHead;
			}
			break;

			break;
		case Uart_Waittinglen:
			cmd = revBuff[1];
			datalen = revBuff[2];

			if(isCmdDatalenValied())
			{
				if(datalen > 0) {
					UartState = Uart_revlenStartwaitData;
				} else {
					UartState = Uart_analysisData;
				}
			} else {
				UartState  = Uart_StartwaitHead;
			}
			break;
		case Uart_waittingData:
			UartState = Uart_analysisData;
			break;
	}
}

void on_uart2_error(uint8_t err_type)
{
	startWaitForNewCmd();
}
