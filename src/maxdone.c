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

#define RESP_HEAD (0x4F)

#define RESP_CMD_POS	(1)
#define RESP_LEN_POS	(2)
#define RESP_DATA_POS	(3)

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

const unsigned char notsupport[] = {0x40, 0, 0};
const unsigned char hardwareType[] = {RESP_HEAD, 0, 1, 'C'};
unsigned char respbuff[16] = {RESP_HEAD, 0};
rtc_counter_value_t value;

static void fillRTCRespBuf()
{
	respbuff[RESP_DATA_POS+0] = value.year;
	respbuff[RESP_DATA_POS+1] = value.month;
	respbuff[RESP_DATA_POS+2] = value.day;
	respbuff[RESP_DATA_POS+3] = value.hour;
	respbuff[RESP_DATA_POS+4] = value.min;
	respbuff[RESP_DATA_POS+5] = value.sec;
}

static void fillRTCRespValue()
{
	value.year = revBuff[RESP_DATA_POS+0];
	value.month = revBuff[RESP_DATA_POS+1];
	value.day = revBuff[RESP_DATA_POS+2];
	value.hour = revBuff[RESP_DATA_POS+3];
	value.min = revBuff[RESP_DATA_POS+4];
	value.sec = revBuff[RESP_DATA_POS+5];
}

static void analysisCmd()
{

	respbuff[RESP_CMD_POS] = cmd;

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
			R_UART2_Send(respbuff, 	RESP_DATA_POS+6);
		} else {
			R_UART2_Send(notsupport, 3);
		}
		break;

	case Cmd_getRTC:
		respbuff[RESP_LEN_POS] = 6;
		if(MD_OK ==	R_RTC_Get_CounterValue(&value))
		{
			fillRTCRespBuf();
		} else {
			memset(&respbuff[RESP_DATA_POS], 0, 6);
		}
		R_UART2_Send(respbuff, 	RESP_DATA_POS+6);
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