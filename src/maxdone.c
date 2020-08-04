#include "iodefine.h"
#include "r_cg_macrodriver.h"
#include "r_cg_serial.h"
#include "r_cg_userdefine.h"
#include "r_cg_rtc.h"
#include "string.h"

unsigned char g_u2_sendend = 0;

void UART2_Sendstr(char *str)
{
	if(str == 0) return;

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
	Cmd_testLED


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
	Uart_waittingData
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

static unsigned char SPI10_CMD_BUF[16];
static unsigned char SPI10_ACK_BUF[16];
static void spi10_sendcmd(unsigned char cmd, uint16_t datalen)
{
	SPI10_CMD_BUF[0] = cmd;
	P0_bit.no1 = 0;
	R_CSI10_Send_Receive(SPI10_CMD_BUF, datalen + 1,SPI10_ACK_BUF);
	P0_bit.no1 = 1;
}

unsigned char TestSpiFlash()
{
	spi10_sendcmd(0x9F, 3);
	return ACK_OK;
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


	default:
		R_UART2_Send(notsupport, 3);
		break;
	}
	startWaitForNewCmd();
}

void doUartTask()
{
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
					analysisCmd();
				}
			} else {
				UartState  = Uart_StartwaitHead;
			}
			break;
		case Uart_waittingData:
			analysisCmd();
			break;
	}
}

void on_uart2_error(uint8_t err_type)
{
	startWaitForNewCmd();
}
