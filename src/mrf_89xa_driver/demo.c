#if 0
void demo()
{
BYTE input;
						//call all the initialization routines
BoardInit();			//initialize the board
CONSOLE_Initialize();			//initialize the console
MRF89XAInit();			//initialize MRF89XA
Setup();				//Configure the basic settings
						//Print the Main menu on the screen
MainMenu:
	PingPongSend = FALSE;
	PingPongReceive = FALSE;
	PingPongTest = FALSE;
	Synchron = FALSE;
	PERSend = FALSE;
	PERReceive = FALSE;
	LED_1_SET = FALSE;
	LED_2_SET = FALSE;
	PrintHeader((char*) "Main Menu");
	c("	a.Configure MRF89XA\r\n");
	c("	b.Transmit\r\n");
	c("	c.Receive\r\n");
	c("	d.Read MRF89XA Registers\r\n");
	c("	e.Program MRF89XA Registers \r\n");
	c("	f.Program Radio to Continuous Mode - Transmit\r\n");
	c("	g.Program Radio to Continuous Mode - Receive\r\n");
	c("	h.Ping Pong Test\r\n");
	c("	i.PER Test between two Devices\r\n");
	c("	j.Program MRF89XA sleep mode\r\n");
	c(">>");

						//Idle loop
while(!CONSOLE_IsGetReady())
	{

	}
						//Read the input from the console

	input = CONSOLE_Get( );
	ConsolePut(input);
	nl();

	switch(input)		//Main menu cases
	{

		case 'a':
		case 'A':

ConfigureMenu:
				 	PrintSubHeader((char*)"Configure MRF89XA");
					c(" a. Program modulation type\r\n");
					c(" b. Set operating frequency\r\n");
					c(" c. Select the Receiver bandwidth\r\n");
					c(" d. Select the frequency deviation\r\n");
					c(" e. Program TX data rate\r\n");
					c(" f. Select IF gain\r\n");
					c(" g. Set output power\r\n");
					c(" h. Program packet delay\r\n");
					c(" i. Program ping pong package size\r\n");
					c(" j. Program PER test packet size\r\n");
					c(" k. Enable/Disable data whitening\r\n");
					c(" l. Enable/Disable Frequency Hopping Spread Spectrum\r\n");
					c(" m. Select the TX bandwidth\r\n");

					c(">>");
					while(!CONSOLE_IsGetReady());
					input = CONSOLE_Get( );
					ConsolePut(input);
					nl();
					switch(input)
					{

						case 'b':
						case 'B':
FrequencyMenu:						PrintSubHeader((char*) "Set Frequency Band");
									c(" a. 902-915 MHz\r\n");
									c(" b. 915-928 MHz\r\n");
									c(" c. 950-960 or 863-870 MHz\r\n");
									c(">>");
									while(!CONSOLE_IsGetReady());
									input = CONSOLE_Get( );
									ConsolePut(input);
									nl();
									switch(input)
									{
										case 'a':
										case 'A':	FREQ_BAND = FREQBAND_902;
													break;
										case 'b':
										case 'B':	FREQ_BAND = FREQBAND_915;
													break;
										case 'c':
										case 'C':	FREQ_BAND = FREQBAND_950;
													break;
										HOTKEYS
										default:	c("\r\nRe-Program\r\n");
													goto FrequencyMenu;
									}

									c("Enter the values for R, P and S for programming center frequency\r\n");
ProgramR:							c("Enter the value of R\r\n");
									c(">>");
									{
									WORD read = 0;
ReadR:								while(!CONSOLE_IsGetReady());
									input = CONSOLE_Get( );
									ConsolePut(input);
									if(input != 13)
									{
										if (input==26) goto MainMenu;
										if((input<0x30) & (input>0x39))
											{
												c("\r\nError - Input Format\r\n");
												goto ProgramR;
											}
										input = input - 0x30;
										read = (read*10)+input;
										goto ReadR;
									}
									nl();
									if((read < 64) | (read > 169))
									{
										c("Program correct value -> 64 <= R <= 169 for the correct operation of PLL\r\n");
										goto ProgramR;
									}


									RVALUE = read;
									}
ProgramP:							c("Enter the value of P\r\n");
									c(">>");
									{
									WORD read = 0;
ReadP:								while(!CONSOLE_IsGetReady());
									input = CONSOLE_Get( );
									ConsolePut(input);
									if(input != 13)
									{
										if(input == 26) goto MainMenu;
										if((input<0x30) & (input>0x39))
											{
												c("\r\nError - Input format\r\n");
												goto ProgramP;
											}
										input = input - 0x30;
										read = (read*10)+input;
										goto ReadP;
									}
									nl();
									PVALUE = read;
									}
ProgramS:							c("Enter the value of S\r\n");
									c(">>");
									{
									WORD read = 0;
ReadS:								while(!CONSOLE_IsGetReady());
									input = CONSOLE_Get( );
									ConsolePut(input);
									if(input != 13)
									{
										if(input == 26) goto MainMenu;
										if((input<0x30) & (input>0x39))
											{
												c("\r\nError - Input Format\r\n");
												goto ProgramS;
											}
										input = input - 0x30;
										read = (read*10)+input;
										goto ReadS;
									}
									nl();
									if(read >= (PVALUE+1))
										{
											c("Program correct value -> S < (P+1) for the correct operation of PLL\r\n");
											goto ProgramS;
										}
									SVALUE = read;
									RegisterSet(REG_R1, RVALUE);
									RegisterSet(REG_P1, PVALUE);
									RegisterSet(REG_S1, SVALUE);
									input = RegisterRead(REG_MCPARAM0);
									RegisterSet(REG_MCPARAM0, (input & 0xE7) | FREQ_BAND);
									//Set to Frequency Synthesizer mode
									RegisterSet(REG_MCPARAM0, (input & 0x1F) | RF_SYNTHESIZER);
									RF_Mode = RF_SYNTHESIZER;
									// clear PLL_LOCK flag so we can see it restore on the new frequency
									input = RegisterRead(REG_IRQPARAM1);
									RegisterSet(REG_IRQPARAM1, (input | 0x02));
									SetRFMode(RF_STANDBY);

									}

									break;
						case 'c':
						case 'C':
BandwidthMenu:						PrintSubHeader((char*)"Set Bandwidth");
									c(" a. 400 KHz\r\n");
									c(" b. 250 KHz\r\n");
									c(" c. 175 KHz\r\n");
									c(" d. 150 KHz\r\n");
									c(" e. 125 KHz\r\n");
									c(" f. 100 KHz\r\n");
									c(" g. 75 KHz\r\n");
									c(" h. 50 KHz\r\n");
									c(" i. More advanced options\r\n");
									c(">>");
									while(!CONSOLE_IsGetReady());
									input = CONSOLE_Get( );
									ConsolePut(input);
									nl();
									switch(input)
									{
										case 'a':
										case 'A': 	FREQ_BW = RXFC_FOPLUS400;			//BW = 400 KHz
													break;
										case 'b':
										case 'B': 	FREQ_BW = RXFC_FOPLUS250;			//BW = 250 KHz
													break;
										case 'c':
										case 'C': 	FREQ_BW = RXFC_FOPLUS175;			//BW = 175 KHz
													break;
										case 'd':
										case 'D': 	FREQ_BW = RXFC_FOPLUS150;			//BW = 150 KHz
													break;
										case 'e':
										case 'E': 	FREQ_BW = RXFC_FOPLUS125;			//BW = 125 KHz
													break;
										case 'f':
										case 'F': 	FREQ_BW = RXFC_FOPLUS100;			//BW = 100 KHz
													break;
										case 'g':
										case 'G':	FREQ_BW = RXFC_FOPLUS75;			//BW = 75 KHz
													break;
										case 'h':
										case 'H':	FREQ_BW = RXFC_FOPLUS50;			//BW = 50 KHz
													break;
										case 'i':
										case 'I':
ReadBWValue:										c("\r\nChoose a value between 0 - 15 for 'BWValue' in the Equation below\r\n");
													c("  Receiver Bandwidth = (25 * (1 + BWValue))\r\n");
													{
														WORD BWValue = 0;

ReadNextBW:												while(!CONSOLE_IsGetReady());
														input = CONSOLE_Get( );
														ConsolePut(input);
														if(input != 13)
														{
															if((input < 0x30) && (input > 0x39))
															{
																c("Incorrect input\r\n");
																goto ReadBWValue;
															}
															else
															{
																BWValue = (BWValue*10 + (input - '0'));
															}
															goto ReadNextBW;
														}
														else
														{
														c("\r\nReceiver Bandwidth is set to ");
														FREQ_BW = BWValue;
														BWValue = (25 * (BWValue+1));
														PrintDigitW(BWValue);
														}

													}
													break;

										HOTKEYS

										default:
													c("Re-Program\r\n");
													goto BandwidthMenu;
									}
									input = RegisterRead(REG_RXPARAM0);
									RegisterSet(REG_RXPARAM0, (input & 0xF0) | FREQ_BW);
									break;
						case 'd':
						case 'D':
FSKMenu:							PrintSubHeader((char*)"Select Frequency Deviation");
									c("Choose from one of the following options\r\n");
									c(" a. 200 KHz\r\n");
									c(" b. 133 KHz\r\n");
									c(" c. 100 KHz\r\n");
									c(" d. 80 KHz\r\n");
									c(" e. 67 KHz\r\n");
									c(" f. 50 KHz\r\n");
									c(" g. 40 KHz\r\n");
									c(" h. 33 KHz\r\n");
									c(" i. More advanced options\r\n");
									c(">>");
									while(!CONSOLE_IsGetReady());
									input = CONSOLE_Get( );
									ConsolePut(input);
									switch(input)
									{
									case 'a':
									case 'A':	TX_FSK = FREGDEV_200;
												break;
									case 'b':
									case 'B':	TX_FSK = FREGDEV_133;
												break;
									case 'c':
									case 'C':	TX_FSK = FREGDEV_100;
												break;
									case 'd':
									case 'D':	TX_FSK = FREGDEV_80;
												break;
									case 'e':
									case 'E':	TX_FSK = FREGDEV_67;
												break;
									case 'f':
									case 'F':	TX_FSK = FREGDEV_50;
												break;
									case 'g':
									case 'G':	TX_FSK = FREGDEV_40;
												break;
									case 'h':
									case 'H':	TX_FSK = FREGDEV_33;
												break;

									case 'i':
									case 'I':
AdvancedFSKMenu:								c("\r\nProgram the frequency deviation according to the formula mentioned below-\r\n");
												c("Frequency Deviation =   Fxtal\r\n");
												c("                      -----------\r\n");
												c("                      32.(FDVAL+1)\r\n");
												c("where 0 <= FDVAL <= 255\r\n>>");
												TX_FSK = 0;
ReadInput_FSK:									while(!CONSOLE_IsGetReady());
												input = CONSOLE_Get( );
												ConsolePut(input);
												if((input >= 0x30) & (input <= 0x39))
													{
													input = input - 0x30;
													TX_FSK = (TX_FSK*10)+ input;
													goto ReadInput_FSK;
													}
												else
													{
													switch(input)
														{
														HOTKEYS
														case 13:
																break;
														default:
																c("Error - Incorrect Input");
																goto AdvancedFSKMenu;
														}
													}

												break;
									HOTKEYS
									default:	c("Re-Program\r\n");
												goto FSKMenu;
								}
								RegisterSet(REG_FREGDEV, TX_FSK);
								c("\r\nHint: Program TX and RX Bandwidth accordingly\r\n");
								break;

						case 'e':
						case 'E':
DataRateMenu:						PrintSubHeader((char*)"Select Data Rate");
									c("Choose the Data Rate from following options (Range: 600 bps - 115.2 Kbps)\r\n");
									c(" a. 1.56 kbps\r\n");
									c(" b. 2 kbps\r\n");
									c(" c. 2.41 kbps\r\n");
									c(" d. 4.76 kbps\r\n");
									c(" e. 5 kbps\r\n");
									c(" f. 8 kbps\r\n");
									c(" g. 9.52 kbps\r\n");
									c(" h. 10 kbps\r\n");
									c(" i. 12.5 kbps\r\n");
									c(" j. 16.67 kbps\r\n");
									c(" k. 20 kbps\r\n");
									c(" l. 40 kbps\r\n");
									c(" m. 50 kbps\r\n");
									c(" n. 100 kbps\r\n");
									c(" o. 200 kbps\r\n");
									c(" p. More advanced options\r\n");
									c(">>");
									while(!CONSOLE_IsGetReady());
									input = CONSOLE_Get( );
									ConsolePut(input);
									switch(input)
									{
										case 'a':
										case 'A': 	Rate_C = BITRATE_1_56; Rate_R = 0x8000;
													break;
										case 'b':
										case 'B':	Rate_C = BITRATE_2; Rate_R = 2;		//Rate 2 Kbps
													break;
										case 'c':
										case 'C':	Rate_C = BITRATE_2_41; Rate_R = 0x8000;
													break;

										case 'd':
										case 'D':	Rate_C = BITRATE_4_76; Rate_R = 0x8000;
													break;

										case 'e':
										case 'E':	Rate_C = BITRATE_5; Rate_R = 5;		//Rate 5 Kbps
													break;
										case 'f':
										case 'F':	Rate_C = BITRATE_8; Rate_R = 8;
													break;

										case 'g':
										case 'G':	Rate_C = BITRATE_9_52; Rate_R = 0x8000;
													break;

										case 'h':
										case 'H':	Rate_C = BITRATE_10; Rate_R = 10;	//Rate 10 Kbps
													break;
										case 'i':
										case 'I':	Rate_C = BITRATE_12_5; Rate_R = 0x8000;
													break;

										case 'j':
										case 'J':	Rate_C = BITRATE_16_67; Rate_R = 0x8000;
													break;

										case 'k':
										case 'K':	Rate_C = BITRATE_20; Rate_R = 20;	//Rate 20 Kbps
													break;
										case 'l':
										case 'L':	Rate_C = BITRATE_40; Rate_R = 40;	//Rate 40 Kbps
													break;
										case 'm':
										case 'M':	Rate_C = BITRATE_50; Rate_R = 50;	//Rate 50 Kbps
													break;
										case 'n':
										case 'N':	Rate_C = BITRATE_100; Rate_R =100;	//Rate 100 Kbps
													break;
										case 'o':
										case 'O':	Rate_C = BITRATE_200; Rate_R = 200;	//Rate 200 Kbps
													break;
										case 'p':
										case 'P':	c("\r\nFor setting custom and standard bit rates between 2 kbps and 200 kbps use the expression below \r\n");
													c("Program the values for C based on the expression below -  \r\n");
ReadC:												c("\r\nBit Rate = (Fxtal / (64 * (C+1)), 0<=C<=127\r\n");
													c("Enter the value for C\r\n");
													Rate_C = 0;
ReadCNext:											while(!CONSOLE_IsGetReady());
													input = CONSOLE_Get( );
													ConsolePut(input);

													if((input >= 0x30) && (input <= 0x39))
													{
														Rate_C = (Rate_C * 10 + (input - 0x30));
														goto ReadCNext;
													}
													else
													{
														switch(input)
														{
															HOTKEYS
															case 13:	goto ProgramRate;
																		break;
															default:	c("\r\nError - Input Format\r\n");
																		goto ReadC;
														}
													}
ProgramRate:										Rate_R = (Fxtal /(64 * (Rate_C+1)));
													break;
										HOTKEYS
										default:	c("Re-Program\r\n");
												 	goto DataRateMenu;
									}
									RegisterSet(REG_BITRATE, Rate_C);
									c("\r\nProgramming Success!\r\n");
									c("Hint: Program the Frequency Deviation and Bandwidth settings accordingly\r\n");
									break;
						case 'f':
						case 'F':
IFGAINMenu:							PrintSubHeader((char*)"Program IF GAIN");
									c("Choose from one of the following options\r\n");
									c(" a.0 dB - Maximal Gain\r\n");
									c(" b.-4.5 dB\r\n");
									c(" c.-9 dB\r\n");
									c(" d.-13.5 dB\r\n");
									c(">>");
									while(!CONSOLE_IsGetReady());
									input = CONSOLE_Get( );
									ConsolePut(input);
									switch(input)
									{
										case 'a':
										case 'A':
													IF_GAIN = IFGAIN_0;
													break;
										case 'b':
										case 'B':	IF_GAIN = IFGAIN_45;
													break;
										case 'c':
										case 'C':	IF_GAIN = IFGAIN_9;
													break;
										case 'd':
										case 'D':	IF_GAIN = IFGAIN_135;
													break;
										HOTKEYS
										default:	c("\r\nRe-Program\r\n");
													goto IFGAINMenu;
									}
									input = RegisterRead(REG_MCPARAM1);
									RegisterSet(REG_MCPARAM1, ((input & 0xFC)|IF_GAIN));
									break;
						case 'g':
						case 'G':
TXPWRMenu:							PrintSubHeader((char*)"Set TX Output Power");
									c("Choose from one of the following options\r\n");
									c(" a.13 dBm\r\n");
									c(" b.10 dBm\r\n");
									c(" c.7 dBm\r\n");
									c(" d.4 dBm\r\n");
									c(" e.1 dBm\r\n");
									c(" f.-2 dBm\r\n");
									c(" g.-5 dBm\r\n");
									c(" h.-8 dBm\r\n");
									c(">>");
									while(!CONSOLE_IsGetReady());
									input = CONSOLE_Get( );
									ConsolePut(input);
									switch(input)
										{
											case 'a':
											case 'A':	TX_PWR = TXPOWER_13;
														break;
											case 'b':
											case 'B':	TX_PWR = TXPOWER_10;
														break;
											case 'c':
											case 'C':	TX_PWR = TXPOWER_7;
														break;
											case 'd':
											case 'D':	TX_PWR = TXPOWER_4;
														break;
											case 'e':
											case 'E':	TX_PWR = TXPOWER_1;
														break;
											case 'f':
											case 'F':	TX_PWR = TXPOWER_2;
														break;
											case 'g':
											case 'G':	TX_PWR = TXPOWER_5;
														break;
											case 'h':
											case 'H':	TX_PWR = TXPOWER_8;
														break;
											HOTKEYS
											default:	c("\r\nRe-Program\r\n");
														goto TXPWRMenu;
										}
										input = RegisterRead(REG_TXPARAM);
										RegisterSet(REG_TXPARAM, ((input & 0xF1) | TX_PWR));
										break;


						case 'h':
						case 'H':
									PrintSubHeader((char*)"Packet Delay");
									c("Choose from one of the following options\r\n1 being the lowest and 100 being the largest\r\n");
									c("On Explorer 16, 1 unit = 5ms; On PIC18 Explorer, 1unit = ms\r\n");
									c(" a.1 unit\r\n");
									c(" b.10 units\r\n");
									c(" c.100 units \r\n");
									c(" d.0.1% Duty Cycle\r\n");
									c(" e.1% Duty Cycle\r\n");
									c(" f.10% Duty Cycle\r\n");
									c(">>");
									while(!CONSOLE_IsGetReady());
									input = CONSOLE_Get( );
									ConsolePut(input);
									Enable_DutyCycle = FALSE;
									switch(input)
									{
									case 'a':
									case 'A':
												PacketDelay = 1;
												break;
									case 'b':
									case 'B':
												PacketDelay = 10;
												break;
									case 'c':
									case 'C':	PacketDelay = 100;
												break;
									case 'd':
									case 'D':
												Enable_DutyCycle = TRUE;
												Duty_Cycle = 1000;
												break;
									case 'e':
									case 'E':
												Enable_DutyCycle = TRUE;
												Duty_Cycle = 100;
												break;
									case 'f':
									case 'F':
												Enable_DutyCycle = TRUE;
												Duty_Cycle = 10;
												break;
									HOTKEYS
									default: c("\r\nRe-Program");
												goto MainMenu;
									}
									break;



						case 'i':
						case 'I':
PingPongPackageMenu:				PrintSubHeader((char*)"PingPong Test Package size");
									c("Choose from one of the following options\r\n10 being the lowest and 250 being the largest\r\n");
									c(" a.10 packets\r\n");
									c(" b.100 packets\r\n");
									c(" c.250 packets \r\n");
									c(">>");
									while(!CONSOLE_IsGetReady());
									input = CONSOLE_Get( );
									ConsolePut(input);
									switch(input)
									{
									case 'a':
									case 'A':
												PingPong_Package = 10;
												break;
									case 'b':
									case 'B':
												PingPong_Package = 100;
												break;
									case 'c':
									case 'C':	PingPong_Package = 250;
												break;
									HOTKEYS
									default: c("\r\nRe-Program");
												goto PingPongPackageMenu;
									}
									break;
						case 'j':
						case 'J':
PERPacketMenu:							PrintSubHeader((char*)"PER Test Packet size");
									c("Choose from one of the following options\r\n16 being the lowest and 128 being the largest\r\n");
									c(" a. 16 bytes\r\n");
									c(" b. 32 bytes\r\n");
									c(" c. 64 bytes\r\n");
									c(">>");
									while(!CONSOLE_IsGetReady());
									input = CONSOLE_Get( );
									ConsolePut(input);
									switch(input)
									{
									case 'a':
									case 'A':
												PER_Packet_Size = 16;
												break;
									case 'b':
									case 'B':
												PER_Packet_Size = 32;
												break;
									case 'c':
									case 'C':
												PER_Packet_Size = 64;
												break;
									HOTKEYS
									default:	c("\r\nRe-Program");
												goto PERPacketMenu;
									}
									break;
									c("\r\n Programming Success!\r\n");
						case 'k':
						case 'K':
									PrintSubHeader((char*)"Data Whitening");
DataWhiteningMenu:					c(" a. Enable\r\n");
									c(" b. Disable\r\n");
									c(">>");
									while(!CONSOLE_IsGetReady());
									input = CONSOLE_Get( );
									ConsolePut(input);
									switch(input)
									{
									case 'a':
									case 'A':
												DATA_Whitening = TRUE;
												break;
									case 'b':
									case 'B':
												DATA_Whitening = FALSE;
												break;
									HOTKEYS
									default:	c("\r\nRe-Program\r\n");
												goto DataWhiteningMenu;
									}
									input = RegisterRead(REG_PKTPARAM2);
									if(DATA_Whitening)
										input = (input | 0x10);
									else
										 input = (input & 0xEF);
									RegisterSet(REG_PKTPARAM2, input);
									c("Programming Success!\r\n");
									break;
						case 'a':
						case 'A':
									PrintSubHeader((char*)"Select Modulation type");
ModulationMenu:						c(" a. FSK Modulation\r\n");
									c(" b. OOK Modulation\r\n");
									c(">>");
									while(!CONSOLE_IsGetReady());
									input = CONSOLE_Get( );
									ConsolePut(input);
									nl();
									switch(input)
									{
									case 'a':
									case 'A':	Modulation_Type = TRUE;
												break;
									case 'b':
									case 'B':	Modulation_Type = FALSE;
												break;
									HOTKEYS
									default:	c("\r\nRe-Program\r\n");
												goto ModulationMenu;
									}
									if(Modulation_Type)
										{

											//Modulation type = FSK
											//Program FSK settings

											//Program DMODREG to FSK Modulation
											input = RegisterRead(REG_MCPARAM1);
											input = ((input & 0x3F) | MODSEL_FSK);
											RegisterSet(REG_MCPARAM1, input);

											//Program the FSK Data Rate, BW and Fdev
												Rate_C = BITRATE_10; Rate_R = 10;
												FREQ_BW = RXFC_FOPLUS50;
												TX_FSK = FREGDEV_33;
												PFILTER_SETTING = PASSIVEFILT_157;

												RegisterSet(REG_BITRATE, Rate_C);
												RegisterSet(REG_RXPARAM0, (FREQ_BW | PFILTER_SETTING));
												RegisterSet(REG_FREGDEV, TX_FSK);


											c("Programmed MRF89XA to FSK Modulation\r\n");
											c("Hint: Modify Data Rate Setting as per FSK Mode - Up to 200 kbps\r\n");

										}
									else
										{
											//Modulation type = OOK
											//Program OOK settings

											//program OOK threshold = Peak Mode and Set Modulation type = OOK
											input = RegisterRead(REG_MCPARAM1);
											input = ((input & 0x27) | MODSEL_OOK | OOKTHRESHTYPE_PEAK);
											RegisterSet(REG_MCPARAM1, input);

											RegisterSet(REG_FREGDEV, FREGDEV_100);
											RegisterSet(REG_BITRATE, BITRATE_16);	//BitRate = 16.67kbps
											RegisterSet(REG_RXPARAM0, (RXFC_FOPLUS50 | PASSIVEFILT_458));

											//OOK Floor Threshold  - 0.5dB --> 6 dB
											RegisterSet(REG_OOKFLOORTHRESH, OOKFLOORTHRESH_VALUE);

											//Polyphase Filter setting - Should be 100 KHz
											RegisterSet(REG_RXPARAM1, (DEF_RXPARAM1 | FO_VALUE));

											//Program OOK register settings
											RegisterSet(REG_OOKSETTINGS, (OOK_THRESH_DECSTEP_000 | OOK_THRESH_DECPERIOD_000 | OOK_THRESH_AVERAGING_00));

											c("Programmed MRF89XA to OOK Modulation\r\n");
											c("Hint: Modify Data Rate Setting as per OOK Mode - Up to 32 Kbps\r\n ");
										}

									break;

						case 'l':
						case 'L':
FHSSMenu:							PrintSubHeader((char*)"Enable/Disable Frequency Hopping Spread Spectrum");
									c("Note: This menu option does not configure the Master - Slave FHSS Mode.\r\n");
									c("Note: This mode is designed to work with 'Transmit' option in the main menu.\r\n\r\n");
									c(" a. Enable FHSS in 902-928 MHz Band\r\n");
									c(" b. Enable FHSS in 863-870 MHz Band\r\n");
									c(" c. Disable FHSS\r\n");
									c(">>");
									while(!CONSOLE_IsGetReady());
									input = CONSOLE_Get( );
									ConsolePut(input);
									switch(input)
									{
										case 'a':
										case 'A':
													c("Enabling FHSS in 902-928 MHz\r\n");
													c("Channel spacing approximately 500KHz\r\n");
													FHSS_Enable_902 = TRUE;
													FHSS_Init();
													c("Frequency Hopping Enabled\r\n");
													break;

										case 'b':
										case 'B':
													c("Enabling FHSS in 863-870 MHz\r\n");
													c("Select the channel spacing\r\n")
													c("a. 100 KHz\r\n");
													c("b. 50 KHz\r\n");
													c("c. 25 KHz\r\n");									//Choosing higher data rates / lower data rates at this frequency will produce overlapped channels
													c(">>");
													while(!CONSOLE_IsGetReady());
													input = CONSOLE_Get( );
													ConsolePut(input);
													switch(input)
													{
														case 'a':
														case 'A':
																	Channel_spacing_100 = TRUE;
																	break;
														case 'b':
														case 'B':	Channel_spacing_50 = TRUE;
																	break;
														case 'c':
														case 'C':	Channel_spacing_25 = TRUE;
																	break;
														default:	Channel_spacing_100 = TRUE;

													}
													FHSS_Enable_863 = TRUE;
													FHSS_Init();
													break;				//Case 'b' break statement
										case 'c':
										case 'C':
													c("Disabling FHSS\r\n");
													FHSS_Enable_902 = FALSE;
													FHSS_Enable_863 = FALSE;
													FHSS_Exit();
													break;
										HOTKEYS
										default:	c("\r\nRe-Program\r\n")
													goto FHSSMenu;
									}
									break;

						HOTKEYS

						default:
									c("\r\nRe-Program\r\n");
									goto ConfigureMenu;
						}
						if(input!=19)
						{
						c("\r\n");
						PrintStatus();
						}
						break;


		case 'b':
		case 'B':
TransmitMenu:		PrintSubHeader((char*)"Transmit");
					c("	a.Transmit predefined packet continuously\r\n");
					c("	b.Transmit User defined packet\r\n");
					c("	c.On-and-Off Transmit \r\n");
					c("	d.On-and-Off Transmit in Continuous Mode\r\n");
					c(">>");
					while(!CONSOLE_IsGetReady());
					input = CONSOLE_Get( );
					ConsolePut(input);
					nl();
					switch(input)
					{
						case 'a':
						case 'A': 	c("Transmitting predefined packet at");
									PrintMiniStatus(PR_TX_MODE);
								  	c("Press (Ctrl+z) to exit\r\n");
									{
										BYTE TxPacket_Len;
										BYTE i;
										WORD j,k;
										for(i=0;i<sizeof(PredefinedPacket);i++)
										TxPacket[i] = PredefinedPacket[i];
										TxPacket_Len = sizeof(PredefinedPacket);
								  	while(1)
									{
										if(CONSOLE_IsGetReady())
										{
											input = CONSOLE_Get( );
											ConsolePut(input);
											if(input == 26)
											goto MainMenu;
										}
										if(FHSS_Hop)
												{
													FHSS_FreqHop();
													FHSS_Hop = FALSE;
												}
										Send_Packet(TxPacket_Len);
										if(!Enable_DutyCycle)
										{
											k = PacketDelay;
											do
	                        				{
	                 			               for(j=0;j<2000;j++)
	               	                          {
	                				          }
	                        				}while(k--);
										}
									}
									}
									break;
						case 'b':
						case 'B':	c("Please Enter the packet you want to transmit\r\n");
									c("Use hex values and press '=' when you are done\r\n");
									c(">>");
									TxPacLen = 0;
									{
									BYTE value,byte2=0;
									BOOL transmitpacket=0;
									while(!transmitpacket)
									{
									while(!CONSOLE_IsGetReady());
									input = CONSOLE_Get( );
									ConsolePut(input);
									switch(input)
									{
										case '0':
										case '1':
										case '2':
										case '3':
										case '4':
										case '5':
										case '6':
										case '7':
										case '8':
										case '9':	input = input - '0';
													byte2++;
													if(byte2 == 2)
													{
														value = (value<<4) + input;
														TxPacket[TxPacLen]= value;
														TxPacLen++;
														byte2 = 0;
													}
													else
													{
														value = input;
													}
													break;
										case 'a':
										case 'b':
										case 'c':
										case 'd':
										case 'e':
										case 'f':	input = (input - 'a' + 10);
													byte2++;
													if(byte2 == 2)
													{
														value = (value<<4) + input;
														TxPacket[TxPacLen]= value;
														TxPacLen++;
														byte2 = 0;
													}
													else
													{
														value = input;
													}
													break;
										case 'A':
										case 'B':
										case 'C':
										case 'D':
										case 'E':
										case 'F':	input = (input - 'A' + 10);
													byte2++;
													if(byte2 == 2)
													{
														value = (value<<4) + input;
														TxPacket[TxPacLen]= value;
														TxPacLen++;
														byte2 = 0;
													}
													else
													{
														value = input;
													}
													break;
										case '=':	transmitpacket = 1;
													break;
										HOTKEYS
										default:	c("Re-Program\r\n");
													break;
									}
									}

									c("\r\nTransmitting User-defined packet at");
									PrintMiniStatus(PR_TX_MODE);
								  	c("Press (Ctrl+z) to exit\r\n");
									{
										WORD j,k;
								  	while(1)
									{
										if(CONSOLE_IsGetReady())
										{
											input = CONSOLE_Get( );
											ConsolePut(input);
											if(input == 26)
												goto MainMenu;
										}
										if(FHSS_Hop)
											{
												FHSS_FreqHop();
												FHSS_Hop = FALSE;
											}
										Send_Packet(TxPacLen);
										if(!Enable_DutyCycle)
										{
											k = PacketDelay;
											do
	                        				{
	                 			               for(j=0;j<2000;j++)
	               	                          {
	                				          }
	                        				}while(k--);
										}
									  }
									}
									}
									break;
						case 'c':
						case 'C':	c("On-and-Off Transmit for every Second at");
									if(FHSS_Enable_902 | FHSS_Enable_863)
										{
											FHSS_Exit();
											c("FHSS Mode is Disabled\r\n");
										}
									PrintMiniStatus(PR_TX_MODE);
									c("Press (Ctrl+z) to exit\r\n");
									{
										//Uses buffer mode
										WORD jcounter, j;
										BYTE last_icounter;			//Indicating 4 * 64 = 256 bytes
										BYTE i;
										BYTE data;
										//Initialize MRF89XA in Buffered Mode
										SetRFMode(RF_STANDBY);
										input = RegisterRead(REG_MCPARAM1);
										input = ((input & 0xDB) | DATAMODE_BUFFERED);
										Data_Mode = DATAMODE_BUFFERED;
										RegisterSet(REG_MCPARAM1, input);			//Set the data mode to continuous mode
										RegisterSet(REG_IRQPARAM0, (IRQ0_RX_STDBY_FIFOEMPTY | IRQ1_RX_STDBY_FIFOTHRESH | IRQ1_TX_TXDONE));
										RegisterSet(REG_IRQPARAM1, (DEF_IRQPARAM1 | IRQ0_TX_START_FIFONOTEMPTY | IRQ1_PLL_LOCK_PIN_ON ));

										if(Rate_C == BITRATE_2)
											{jcounter = 3; last_icounter = 58;}
										else if(Rate_C == BITRATE_5)
											{jcounter = 9; last_icounter = 49;}
										else if(Rate_C == BITRATE_10)
											{jcounter = 19; last_icounter = 34;}

										else if(Rate_C == BITRATE_20)
											{jcounter = 39; last_icounter = 4;}

										else if(Rate_C == BITRATE_40)
											{jcounter = 78; last_icounter = 8;}

										else if(Rate_C == BITRATE_50)
											{jcounter = 97; last_icounter = 42;}

										else if(Rate_C == BITRATE_66)
											{jcounter = 130; last_icounter = 18;}

										else if(Rate_C == BITRATE_100)
											{jcounter = 195; last_icounter = 20;}

										else if(Rate_C == BITRATE_200)
											{jcounter = 390; last_icounter = 40;}

										else { c("\r\nThis Data rate is not supported\r\n");
											jcounter = 3; last_icounter = 58;
								  			 }
								  		while(1)
										{
											if(CONSOLE_IsGetReady())
											{
												input = CONSOLE_Get( );
												ConsolePut(input);
												if(input == 26)
												goto Exit_Buffered_Mode;
											}
												//Buffered Mode - Transmit
											{
													RegisterSet(REG_IRQPARAM0, (IRQ0_RX_STDBY_FIFOEMPTY | IRQ1_RX_STDBY_FIFOTHRESH | IRQ1_TX_TXDONE | IRQ1_FIFO_OVERRUN_CLEAR));
													RegisterSet(REG_IRQPARAM1, (DEF_IRQPARAM1 | IRQ0_TX_START_FIFONOTEMPTY | IRQ1_PLL_LOCK_PIN_ON | 0x02));

													SetRFMode(RF_TRANSMITTER);
													for(i=0;i<0xFF;i++);
													for(j = 0; j < jcounter; j++)
													{
														if(j != (jcounter-1))
														{
															for(i = 0; i < 64; i++)
															{
																data = (BYTE)rand();
																if(i == 0)
																	WriteFIFO(0x0F);
																else if (i == 1)
																	WriteFIFO(0x01);
																else
																	WriteFIFO(data);
															}
														}
														else
														{
															for(i = 0; i < last_icounter; i++)
															{
																data = (BYTE)rand();
																if(i == 0)
																	WriteFIFO(0x0F);
																else if (i == 1)
																	WriteFIFO(0x01);
																else
																	WriteFIFO(data);
															}
														}
														input = RegisterRead(REG_IRQPARAM0);
														while(input & 0x02)
															{
																input = RegisterRead(REG_IRQPARAM0);
															};
													}
													while(!IRQ1_Received);
													for(i = 0;i <0x02 ; i++)
													{
													}
													IRQ1_Received = FALSE;
													SetRFMode(RF_STANDBY);
													if(LED_1_SET)
													{
													LED_1 = 0;
													LED_1_SET = FALSE;
													}
												else
													{
													LED_1 = 1;
													LED_1_SET = TRUE;
													}
											}
										#if defined(__18F87J11)
											T0CON = 0x87;
											TMR0H = 0xD9;
											TMR0L = 0xDA;
											INTCONbits.TMR0IE = 0;
											INTCONbits.TMR0IF = 0;
											while(!INTCONbits.TMR0IF);
											INTCONbits.TMR0IF = 0;
											T0CON = 0x07;
										#else									//Explorer 16
											T1CON = 0x8030;
											PR1 = 0x3D09;
											IEC0bits.T1IE = 0;
											IFS0bits.T1IF = 0;
											while(!IFS0bits.T1IF);
											IFS0bits.T1IF = 0;
											T1CON = 0x0030;
										#endif
										}
									}
Exit_Buffered_Mode:						input = RegisterRead(REG_MCPARAM1);
										input = ((input & 0xDB) | DATAMODE_PACKET);
								  		Data_Mode = DATAMODE_PACKET;
										RegisterSet(REG_MCPARAM1, input);			//Set the data mode to packet mode
										RegisterSet(REG_IRQPARAM0, (IRQ0_RX_STDBY_SYNCADRS | IRQ1_RX_STDBY_CRCOK | IRQ1_TX_TXDONE | IRQ1_FIFO_OVERRUN_CLEAR));
										RegisterSet(REG_IRQPARAM1, (DEF_IRQPARAM1 | IRQ0_TX_START_FIFOTHRESH | IRQ1_PLL_LOCK_PIN_ON ));

									break;
						case 'd':
						case 'D':	c("On-and-Off Transmit - Continuous Mode at");
									if(FHSS_Enable_902 | FHSS_Enable_863)
										{
											FHSS_Exit();
											c("\r\nFHSS Mode is Disabled\r\n");
										}
									PrintMiniStatus(PR_TX_MODE);
									c("Press (Ctrl+z) to exit\r\n");
									//Generate Random Buffer
									{
										BYTE i;
										for(i = 0;i <100; i++)
											{
											RandomBuffer[i] = (rand() & 0x01);
											}
									Data_out = 0;
									}

									//Set Radio Mode to CW mode and when returning from the mode reset to data mode
									input = RegisterRead(REG_MCPARAM1);
									input = ((input & 0xDB) | DATAMODE_CONTINUOUS);
									RegisterSet(REG_MCPARAM1, input);			//Set the data mode to continuous mode
									RegisterSet(REG_IRQPARAM0, (IRQ0_RX_STDBY_SYNCADRS | IRQ1_RX_STDBY_CRCOK | IRQ1_TX_DCLK));	//Enable DCLK interrupt
									Data_Mode = DATAMODE_CONTINUOUS;
									while(1)
										{
											if(CONSOLE_IsGetReady())
												{
													input = CONSOLE_Get( );
													ConsolePut(input);
													goto Exit_OnandOff_d;
												};
											SetRFMode(RF_TRANSMITTER);
											if(LED_1_SET)
													{
													LED_1 = 0;
													LED_1_SET = FALSE;
													}
											else
													{
													LED_1 = 1;
													LED_1_SET = TRUE;
													}
											#if defined(__18F87J11)
												T0CON = 0x87;
												TMR0H = 0xD9;
												TMR0L = 0xDA;
												INTCONbits.TMR0IE = 0;
												INTCONbits.TMR0IF = 0;
												while(!INTCONbits.TMR0IF);
												INTCONbits.TMR0IF = 0;
												T0CON = 0x07;
											#else									//Explorer 16
												T1CON = 0x8030;
												PR1 = 0x3D09;
												IEC0bits.T1IE = 0;
												IFS0bits.T1IF = 0;
												while(!IFS0bits.T1IF);
												IFS0bits.T1IF = 0;
												T1CON = 0x0030;
											#endif
											SetRFMode(RF_STANDBY);
											if(CONSOLE_IsGetReady())
												{
													input = CONSOLE_Get( );
													ConsolePut(input);
													goto Exit_OnandOff_d;
												};
											#if defined(__18F87J11)
												T0CON = 0x87;
												TMR0H = 0xD9;
												TMR0L = 0xDA;
												INTCONbits.TMR0IE = 0;
												INTCONbits.TMR0IF = 0;
												while(!INTCONbits.TMR0IF);
												INTCONbits.TMR0IF = 0;
												T0CON = 0x07;
											#else									//Explorer 16
												T1CON = 0x8030;
												PR1 = 0x3D09;
												IEC0bits.T1IE = 0;
												IFS0bits.T1IF = 0;
												while(!IFS0bits.T1IF);
												IFS0bits.T1IF = 0;
												T1CON = 0x0030;
											#endif

										}
Exit_OnandOff_d:					SetRFMode(RF_STANDBY);
									input = ((input & 0xDB) | DATAMODE_PACKET);
									RegisterSet(REG_MCPARAM1, input);			//Reset the Data mode to packet mode
									RegisterSet(REG_IRQPARAM0, (IRQ0_RX_STDBY_SYNCADRS | IRQ1_RX_STDBY_CRCOK | IRQ1_TX_TXDONE));
									Data_Mode = DATAMODE_PACKET;
									break;

						HOTKEYS
						default:	c("Re-Program\r\n");
									goto TransmitMenu;
					}
					break;
		case 'c':
		case 'C':
					PrintSubHeader((char*)"Receive");
					c("	a.Verbose mode\r\n");
					c("	b.Summary mode\r\n");
					c("	c.Packet Count mode\r\n");
					c(">>");
					while(!CONSOLE_IsGetReady());
					input = CONSOLE_Get( );
					ConsolePut(input);
					nl();
					switch(input)
					{
						case 'a':
						case 'A':	if(FHSS_Enable_902 | FHSS_Enable_863)
										{
											FHSS_Exit();
											c("\r\nFHSS Mode is Disabled\r\n");
										}
									c("\r\nMRF89XA in Verbose Receive Mode at");
									PrintMiniStatus(PR_RX_MODE);
									c("Press Ctrl+z to exit\r\n");
									RxPrintCount = 20;
									hasPacket = FALSE;	//reset buffer
									RxCount = 0;
									SetRFMode(RF_RECEIVER);
									#if(__18F87J11)
										PHY_IRQ0 = 0;
										PHY_IRQ0_TRIS = 1;
									#endif
									while(1)
									{
										#if(__18F87J11)
											if(IRQ0_Received)
												{
													PHY_IRQ0 = 0;
													RSSIRegVal = (RegisterRead(REG_RSSIVALUE)>>1);
												}
										#endif

										if(CONSOLE_IsGetReady())
											{
											input = CONSOLE_Get( );
											if(input == 26)
												goto MainMenu;
											}
										if(RxPrintCount == 20 )
										{
										c("Number | Length |        Packet Data         | RSSI (in dB)\r\n");
										nl();
										RxPrintCount = 0;
										}

										if(IRQ1_Received)
										{
											ReceiveFrame();
										}
										if( hasPacket )
							        	{
										BYTE i;
										RxCount++;
										PrintDigitW(RxCount);
										c("   |     ");
										PrintChar(RxPacketLen);
										c(" | ");
							        	for(i = 0; i < RxPacketLen; i++)
							        	{
							        		PrintChar(RxPacket[i]);
											c(" ");
							            }
										c(" | ");

										PrintDigit(RSSIRegVal);
							            hasPacket = FALSE;
										RxPrintCount++;
										nl();
							            RxPacketLen = 0;
							        	}
									}
									SetRFMode(RF_STANDBY);
									break;
						case 'b':
						case 'B':	if(FHSS_Enable_902 | FHSS_Enable_863)
										{
											FHSS_Exit();
											c("\r\nFHSS Mode is Disabled\r\n");
										}
									c("\r\n MRF89XA in Summary Receive Mode at");
									PrintMiniStatus(PR_RX_MODE);
									c("Press Ctrl+z to exit\r\n");
									{
									BYTE HeaderCount = 0;
										//initialize timers
									#if defined(__18F87J11)
										T0CON = 0x87;
										TMR0H = 0xD9;
										TMR0L = 0xDA;
										INTCONbits.TMR0IE = 0;
									#else
										T1CON = 0x8030;
										PR1 = 0x3D09;
										IEC0bits.T1IE = 0;
									#endif
										hasPacket = FALSE;
										RxCount = 0;
										SetRFMode(RF_RECEIVER);
										while(1)
											{
												if(CONSOLE_IsGetReady())
													{
														input = CONSOLE_Get( );
														ConsolePut(input);
														if(input == 26)
															{
															#if defined(__18F87J11)
																T0CON = 0x07;
															#else
																T1CON = 0x0030;
															#endif
															goto MainMenu;
															}
														if(input == 24)
															{
																ResetMRF89XA();
																goto MainMenu;
															}
													}
												if(IRQ1_Received)
													ReceiveFrame();
												if(hasPacket)
													{
														RxCount++;
														hasPacket = FALSE;
													}
													#if defined(__18F87J11)
														if(INTCONbits.TMR0IF)
													#else
														if(IFS0bits.T1IF)
													#endif
															{
																#if defined(__18F87J11)
																	INTCONbits.TMR0IF = 0;
																	TMR0H = 0xD9;
																	TMR0L = 0xDA;
																	T0CON = 0x87;
																#else
																	IFS0bits.T1IF = 0;
																#endif
																if(HeaderCount == 0)
																	c("\r\nRX Packet Count\r\n");
																HeaderCount++;
																if(HeaderCount == 20)
																	HeaderCount = 0;
																PrintDigitW(RxCount);
																c("\r\n");
																RxCount = 0;

															}
											}
										}
									SetRFMode(RF_STANDBY);
									break;
						case 'c':
						case 'C':	if(FHSS_Enable_902 | FHSS_Enable_863)
										{
											FHSS_Exit();
											c("\r\nFHSS Mode is Disabled\r\n");
										}
									c("\r\n MRF89XA in Packet Count Mode\r\n");
									PrintMiniStatus(PR_RX_MODE);
									c("Press <Ctrl+z> to exit\r\n");
									{
										WORD RxPacketCount = 0;
										SetRFMode(RF_RECEIVER);
										while(!CONSOLE_IsGetReady())
										{
											if(IRQ1_Received) ReceiveFrame();
											if(hasPacket)
											{
												hasPacket = FALSE;
												RxPacketCount++;
												c("\r\nRx Packet Count = ");
												PrintDigitW(RxPacketCount);
											}

										}
										c("\r\n Total Packet Count received = ");
										PrintDigitW(RxPacketCount);
										input = CONSOLE_Get( );
										ConsolePut(input);
										if(input == 24) ResetMRF89XA();
									}
									SetRFMode(RF_STANDBY);
									break;

						HOTKEYS
						default: 	c("\r\nRe-Program\r\n");
									break;
					}
					break;

		case 'd':
		case 'D':	c("Current MRF89XA Register Settings\r\n");
					{
					BYTE i, value;
					for(i = 0; i <= 31; i++)
						{
							value = RegisterRead(i);
							c("Register ");
							PrintDigit(i);
							c(" - ");
							PrintChar(value);
							c("\r\n");
						}
					}
					c("Press any key to exit\r\n");
					while(!CONSOLE_IsGetReady());
					input = CONSOLE_Get( );
					ConsolePut(input);
					if(input == 24) ResetMRF89XA();
					nl();
					break;
		case 'f':
		case 'F':
					PrintSubHeader((char*)"MRF89XA Continuous Mode  - Transmit");
					c("Note: Refer to MRF89XA Datasheet for more details about Continuous Mode\r\n\r\n");
					c("Press any key to exit\r\n");
					//Set Radio Mode to CW mode and when returning from the mode reset to data mode
					input = RegisterRead(REG_MCPARAM1);
					input = ((input & 0xDB) | DATAMODE_CONTINUOUS);
					RegisterSet(REG_MCPARAM1, input);			//Set the data mode to continuous mode
					RegisterSet(REG_IRQPARAM0, (IRQ0_RX_STDBY_SYNCADRS | IRQ1_RX_STDBY_CRCOK | IRQ1_TX_DCLK));	//Enable DCLK interrupt
					Data_Mode = DATAMODE_CONTINUOUS;
					SetRFMode(RF_TRANSMITTER);
					while(!CONSOLE_IsGetReady()){if(FHSS_Hop){FHSS_FreqHop(); FHSS_Hop = FALSE;}};
					SetRFMode(RF_STANDBY);
					input = ((input & 0xDB) | DATAMODE_PACKET);
					RegisterSet(REG_MCPARAM1, input);			//Reset the Data mode to packet mode
					RegisterSet(REG_IRQPARAM0, (IRQ0_RX_STDBY_SYNCADRS | IRQ1_RX_STDBY_CRCOK | IRQ1_TX_TXDONE));
					Data_Mode = DATAMODE_PACKET;
					input = CONSOLE_Get( );
					ConsolePut(input);
					if(input == 24) ResetMRF89XA();
					nl();
					break;

		case 'e':
		case 'E':
					c("Enter 4 digit hex value - Register Address and Data in the");
					c("\r\nformat (D7-D0||00||A4-A0||0)\r\n");
					c("Press Ctrl+z to exit\r\n");
ReadRegisterValue:	c(">>");
					{
					BYTE address,data;
					WORD hexvalue = 0x0000 ;
					while(1)
					{
					while(!CONSOLE_IsGetReady());
					input = CONSOLE_Get( );
					ConsolePut(input);
					switch(input)
					{
					case '0':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9':	input = input - '0';
								break;
					case 'A':
					case 'B':
					case 'C':
					case 'D':
					case 'E':
					case 'F':	input = input - 'A'+10;
								break;
					case 'a':
					case 'b':
					case 'c':
					case 'd':
					case 'e':
					case 'f':	input = input -'a'+10;
								break;
					HOTKEYS
					case 13:	address = (BYTE)((hexvalue<<8)>>9);
								data = (BYTE)(hexvalue>>8);
								RegisterSet(address, data);
								c("\r\nProgramming Success!\r\n");
								goto MainMenu;

					default: c("\r\nRe-Program\r\nPlease re-enter the 4 digit hex value\r\n");
								goto ReadRegisterValue;
					}
					hexvalue = ((hexvalue << 4) | input);
					}
					}

		case 'h':
		case 'H':	if(FHSS_Enable_902 | FHSS_Enable_863)
										{
											FHSS_Exit();
											c("FHSS Mode is Disabled\r\n");
										}
PingPongMenu:		PrintSubHeader((char*) "Ping Pong Test");
					c("a. Send\r\n");
					c("b. Receive\r\n");
					//each packet takes around 1ms (at max data rate) + delay of 20ms(Explorer 16) and 16ms(PIC18 Explorer)
					//total round trip is 21ms
					//for max ping pong package the total duration is 21sec
					//hence introducing a delay for 40sec between send and receive.
					c(">>");
					while(!CONSOLE_IsGetReady());
					input = CONSOLE_Get( );
					ConsolePut(input);
					switch(input)
					{
					case 'a':
					case 'A':	PingPongSend = TRUE;
								PingPongTest = TRUE;
								break;
					case 'b':
					case 'B':	PingPongReceive = TRUE;
								PingPongTest = TRUE;
								break;
					HOTKEYS
					default:	goto PingPongMenu;
					}
				c("\r\nPing Pong Test working at");
				PrintMiniStatus(PR_TX_RX_MODE);
				c("Press Ctrl+z to exit\r\n");
				{
				BYTE i, TxPacket_Len;
				BYTE case_value = 0, previous_state = 0 ;
				WORD j,k, PingPong_RxCount = 0;
				if(PingPongReceive) {case_value = 3; SetRFMode(RF_RECEIVER);}
				PingPong_Count = 0;
				while(1)
				{
					switch(case_value)
						{
						case 0:			//Send 0x01 frame to initiate ping pong test
								TxPacket[0] = 0x01;
								for(i=1;i<sizeof(PredefinedPacket);i++)
								TxPacket[i] = rand();
								TxPacket_Len = sizeof(PredefinedPacket);
								Send_Packet(TxPacket_Len);
								SetRFMode(RF_RECEIVER);
								PingPong_Count = 0;
								previous_state = 0;
								case_value = 8;
							//	PrintDigit(case_value);
								break;
						case 1:			//Send 0x04 frames - data
								TxPacket[0] = 0x04;
								for(i=1;i<sizeof(PredefinedPacket);i++)
								TxPacket[i] = rand();
								TxPacket_Len = sizeof(PredefinedPacket);
								if(PingPong_Count != PingPong_Package)
									{
									Send_Packet(TxPacket_Len);
									if(!Enable_DutyCycle)
									{
										k = PacketDelay;
										do
	                        				{
	                 			            	for(j=0;j<2000;j++)
	               	                        		{}
	                        				}while(k--);
									}
									case_value = 1;
									PingPong_Count++;
									}
								else
									{
									c("\r\nSent Packet Count:");
									PrintDigitW(PingPong_Package);
									PingPong_Count = 0;
									case_value = 2;
									}
									previous_state = 1;
							//	PrintDigit(case_value);
								break;

						case 2: 		//Send 0x02 frame to collect status
								TxPacket[0] = 0x02;
								for(i=1;i<sizeof(PredefinedPacket);i++)
								TxPacket[i] = rand();
								TxPacket_Len = sizeof(PredefinedPacket);
								Send_Packet(TxPacket_Len);
								SetRFMode(RF_RECEIVER);
								previous_state = 2;
								case_value = 8;
							//	PrintDigit(case_value);
								break;

						case 3:			//Receive block
										//Receive Start
								if(IRQ1_Received) ReceiveFrame();
								if(hasPacket)
								{
								hasPacket = FALSE;
								if(RxPacket[0] == 0x01)
									{
									TxPacket[0] = 0x03;
									for(i=1;i<sizeof(PredefinedPacket);i++)
									TxPacket[i] = rand();
									TxPacket_Len = sizeof(PredefinedPacket);
									Send_Packet(TxPacket_Len);
									SetRFMode(RF_RECEIVER);
									PingPong_RxCount = 0;
									}
								if(RxPacket[0] == 0x04)
									{
									PingPong_RxCount++;
									previous_state = 3;
									case_value++;
									}
								}
							//	PrintDigit(case_value);
								break;
						case 4:
								if(IRQ1_Received) ReceiveFrame();
								if(hasPacket)
								{
									hasPacket = FALSE;
									if(RxPacket[0] == 0x04)
									{
									PingPong_RxCount++;
									}
									if(RxPacket[0] == 0x02)
									{
									c("\r\nReceived Packet Count:");
									PrintDigitW(PingPong_RxCount);
									case_value++;
									}
								}
							//	PrintDigit(case_value);
								break;
						case 5:		TxPacket[0] = 0x03;
									for(i=1;i<sizeof(PredefinedPacket);i++)
									TxPacket[i] = rand();
									TxPacket_Len = sizeof(PredefinedPacket);
									Send_Packet(TxPacket_Len);
									SetRFMode(RF_RECEIVER);
									previous_state = 0;
									case_value = 8;
							//		PrintDigit(case_value);
									break;

						case 8:			//Wait state
								case_value = previous_state;
								k = 20;
								do
                    			{
                 	 				for(j=0;j<2000;j++)
               	        			{
									if(IRQ1_Received) ReceiveFrame();
									if(hasPacket)
										{
										hasPacket = FALSE;
										if(RxPacket[0] == 0x03)
										case_value = (previous_state+1);
										if(RxPacket[0] == 0x02)
										case_value = 5;
										if(case_value == 3) SetRFMode(RF_RECEIVER);
										break;
										}
									}
								}while(k--);
							//	PrintDigit(case_value);
								break;
						default: break;

						}//end of switch
						if(CONSOLE_IsGetReady())
						{
							input = CONSOLE_Get( );
							ConsolePut(input);
							if(input == 26)
							goto MainMenu;
						}

				}	//end of while(1)
				} //end of definitions
		SetRFMode(RF_STANDBY);
		break;
		case 'i':
		case 'I':
					if(FHSS_Enable_902 | FHSS_Enable_863)
										{
											FHSS_Exit();
											c("\r\nFHSS Mode is Disabled\r\n");
										}
PERMenu:			PrintSubHeader((char*) "PER Test");
					c("Note: Configure one node in 'Receive' Mode and the other in 'Send' Mode\r\n");
					c("a. Send\r\n");
					c("b. Receive\r\n");
					//each packet takes around 15ms (at min data rate) + delay of 20ms(Explorer 16) and 16ms(PIC18 Explorer)
					//total round trip is 35ms
					//for max ping pong package the total duration is 35*250ms
					//hence introducing a delay for 16 sec between send and receive.
					c(">>");
					while(!CONSOLE_IsGetReady());
					input = CONSOLE_Get( );
					ConsolePut(input);
					switch(input)
					{
					case 'a':
					case 'A':	PERSend = TRUE;
								PERTest = TRUE;
								c("\r\n PER Send operating at");
								PrintMiniStatus(PR_TX_MODE);
								c("Press Ctrl+z to exit\r\n");
								{
								BYTE l, packageid=0;
								for(l = 0; l<sizeof(PredefinedPacket);l++)
									{
									TxPacket[l] = PredefinedPacket[l];
									}

								while(!CONSOLE_IsGetReady())
								{
								WORD i,j,PERCount=0;
								BYTE k;
								TxPacket[7] = packageid;
								packageid++;
								for(i=0;i<PERMaxCount;i++)
									{
									TxPacket[8] = i;				//packetid
									PERCount++;
									c("\rSending Packet Count:");
									PrintDigitW(PERCount);
									Send_Packet(sizeof(PredefinedPacket));
									if(CONSOLE_IsGetReady())
										break;
									if(!Enable_DutyCycle)
									{
										k = PacketDelay;
											do
	                        				{
		  			              	 			for(j=0;j<2000;j++)
	               	                    		{}
	                        				}while(k--);
									}
									}
								c("\rSent Packet Count:");
								PrintDigitW(PERCount);
								c("        \r\n");
								}
								}
								break;
					case 'b':
					case 'B':	PERReceive = TRUE;
								PERTest = TRUE;
								c("\r\n PER Receive operating at");
								PrintMiniStatus(PR_RX_MODE);
								//Use timers so that the display screen does not hang
									#if defined(__18F87J11)
										T0CON = 0x87;
										TMR0H = 0xD9;
										TMR0L = 0xDA;
										INTCONbits.TMR0IE = 0;
									#else
										T1CON = 0x8030;
										PR1 = 0x3D09;
										IEC0bits.T1IE = 0;
									#endif

								c("Press Ctrl+z to exit\r\n");
								{
								WORD RxPERCount = 0;
								BYTE Packageid = 0;
								BOOL PacketReceived = FALSE;
								SetRFMode(RF_RECEIVER);
								#if defined(__18F87J11)
									PHY_IRQ0 = 0;
									PHY_IRQ0_TRIS = 1;
								#endif
								while(!CONSOLE_IsGetReady())
								{
								#if(__18F87J11)
									if(IRQ0_Received) 	RSSIRegVal = (RegisterRead(REG_RSSIVALUE)>>1);
									PHY_IRQ0 = 0;
								#endif
								if(IRQ1_Received) ReceiveFrame();
								if(hasPacket)
									{
									hasPacket = FALSE;
									PacketReceived = TRUE;
									//RxRSSI = (RegisterRead(REG_RSSIVALUE)>>1);

									if(RxPacket[7] != Packageid)
										{
										c("Received Packet Count(With Package id:");
										PrintDigit(Packageid);
										c(") = ");
										PrintDigitW(RxPERCount);
										c(" RSSI Value = ");
										PrintDigit(RSSIRegVal);
										c("\r\n");
										RxPERCount = 0;
										Packageid = RxPacket[7];
										}
									RxPERCount++;
									}
								#if defined(__18F87J11)
									if(INTCONbits.TMR0IF)
								#else
									if(IFS0bits.T1IF)
								#endif
									{
										#if defined(__18F87J11)
											INTCONbits.TMR0IF = 0;
											TMR0H = 0xD9;
											TMR0L = 0xDA;
											T0CON = 0x87;
										#else
											IFS0bits.T1IF = 0;
										#endif
										if(!PacketReceived)
											{
												c("Received Packet Count = 0\r\n");
											}
										PacketReceived = FALSE;	//de-assert the flag
									}
								}
								}
								break;
					HOTKEYS
					default:	goto PERMenu;
					}


				break;
		case 'j':
		case 'J':
					PrintSubHeader((char*)"MRF89XA Sleep Mode");
					SetRFMode(RF_SLEEP);
					c("Press any key to exit Sleep mode\r\n");
					c(">>");
					while(!CONSOLE_IsGetReady());
					input = CONSOLE_Get( );
					ConsolePut(input);
					if(input == 24) ResetMRF89XA();
					SetRFMode(RF_STANDBY);
					break;
		case 'g':
		case 'G':	PrintSubHeader((char*) "MRF89XA Continuous Mode - Receive");
					c("Note: Refer to MRF89XA Datasheet for more details about Continuous Mode\r\n\r\n");
					c("Press any key to exit\r\n");
					//Set Radio Mode to CW mode and when returning from the mode reset to data mode
					input = RegisterRead(REG_MCPARAM1);
					input = ((input & 0xDB) | DATAMODE_CONTINUOUS);
					RegisterSet(REG_MCPARAM1, input);			//Set the data mode to continuous mode
					SetRFMode(RF_RECEIVER);
					while(!CONSOLE_IsGetReady()){};
					SetRFMode(RF_STANDBY);
					input = ((input & 0xDB) | DATAMODE_PACKET);
					RegisterSet(REG_MCPARAM1, input);			//Reset the Data mode to packet mode
					input = CONSOLE_Get( );
					ConsolePut(input);
					if(input == 24) ResetMRF89XA();
					break;
		case 'k':
		case 'K':
					PrintSubHeader((char*) "FHSS Demo");
FHSSDemoStart:		c("Note: Configure one node is 'Slave' mode and the other in 'Master' mode\r\n\r\n");
					c("	a.Configure as Master\r\n");
					c("	b.Configure as Slave\r\n");
					c(">>");
					while(!CONSOLE_IsGetReady());
					input = CONSOLE_Get( );
					ConsolePut(input);
					nl();
					#if defined(__PIC24F__)
////						mLCDInit();
////						LCDProcessEvents();
					#endif
					switch(input)
						{
							case 'a':
							case 'A':	FHSS_Master_Enabled = TRUE;
										if( !(FHSS_Enable_902 | FHSS_Enable_863))
											FHSS_Enable_902 = TRUE;
										FHSS_Init();

										c("Configured the Node as FHSS Master\r\n");
										{
											BOOL Sync = FALSE;
											BYTE Count = 0;
											#if defined(__PIC24F__)
////											char LCD_Upper[16] = "Master         ";
////											char LCD_Lower[16] = "               ";
											//Explorer 16 - LCD Update taking more than 300ms
											//sprintf((char*)LCD_Upper, (const char*)"Master Hop: %d", Current_Channel);
											//sprintf((char *)LCD_Lower, (const char*)"Slave Rsp: %d", Count);

//////												BannerProcessEvents((char*)LCD_Upper, (char*)LCD_Lower);
											#endif
											while(!CONSOLE_IsGetReady())
											{
											if(!Sync)
												{
												//c("Master and Slave out of Sync\r\n");
												c("\rSync...........");
												FHSS_Sync_Start_Master();
												FHSS_Sync_Detect = TRUE;
												Sync = TRUE;
												//c("Sync Completed\r\n");
												}

											else
												{
													if(FHSS_Hop)
															{
																FHSS_FreqHop();
																FHSS_Hop = FALSE;

															}
													if(FHSS_Sync_Detect)
														{
															Sync = FALSE;
															//Wait for 8ms and Transmit Dialog Command
															FHSS_Sync_Timer_Init(0x061A);
															while(!FHSS_Sync_Timer_Expired());

															TxPacket[0] = 0xFE;
															TxPacket[1] = DIALOG_COMMAND;
															TxPacket[2] = NULL;			//The remaining two bytest of information can be used for sending Master Hop infomation
															TxPacket[3] = NULL;
															Send_Packet(4);			//Transmit Dialog Frame to Slave
																					//Wait for Response
															SetRFMode(RF_RECEIVER);

															while(!FHSS_Sync_Timer_Expired())
																{
																	if(IRQ1_Received)
																		ReceiveFrame();
																	if(hasPacket)
																		{
																			hasPacket = FALSE;
																			if(RxPacket[0] == 0xFE)
																				{
																					if(RxPacket[1] == RESPONSE_COMMAND)
																						{
																							Sync = TRUE;
																							//FHSS_Timer_Stop();
																							FHSS_Timer_Init(0x124F);
																							Count++;

																							{
																							//c("Response Command Received\r\n");
																									//Response time for the LCD too high - Loosing Synchronization with the Slave
																							//sprintf((char*)LCD_Upper, (const char*)"Master Hop: %d", Current_Channel);
																							//sprintf((char *)LCD_Lower, (const char*)"Slave Rsp: %d", Count);
																							//BannerProcessEvents(LCD_Upper, LCD_Lower);
																							c("\rHop Channel: "); PrintDigit(Current_Channel); c(" ");
																							}
																							break;
																						}
																				}
																		}
																};

															SetRFMode(RF_STANDBY);
															FHSS_Sync_Timer_Stop();
															FHSS_Sync_Detect = FALSE;
														}

												}
											}
										}
										FHSS_Master_Enabled = FALSE;
										FHSS_Enable_902 = FALSE;
										FHSS_Enable_863 = FALSE;
										FHSS_Exit();
										c("\r\n");
										break;
							case 'b':
							case 'B':
										FHSS_Slave_Enabled = TRUE;
										if( !(FHSS_Enable_902 | FHSS_Enable_863))
											FHSS_Enable_902 = TRUE;
										FHSS_Init();

										c("Configured the Node as FHSS Slave\r\n");
										{
											BOOL Sync = FALSE;
											#if defined(__PIC24F__)
////												char LCD_Upper[16] = "Slave        ";
////												char LCD_Lower[16] = "             ";
											//BYTE Count = 0;
											//Explorer 16 - LCD Update taking more than 300ms
											//sprintf((char*)LCD_Upper, (const char*)"Master Hop: %d", Current_Channel);
											//sprintf((char *)LCD_Lower, (const char*)"Slave Rsp: %d", Count);

////												BannerProcessEvents((char*)LCD_Upper, (char*)LCD_Lower);
											#endif
											while(!CONSOLE_IsGetReady())
											{
											if(!Sync)
												{
													//c("Slave out of Sync\r\n");
													SetRFMode(RF_RECEIVER);
													FHSS_Sync_Start_Slave();
													Sync = TRUE;
													//c("Slave waiting for Sync with Master\r\n");
												}
											else
												{

													if(FHSS_Hop)
															{
																FHSS_FreqHop();
																FHSS_Hop = FALSE;
															}
													if (IRQ1_Received)
														ReceiveFrame();
													if(hasPacket)
														{
															hasPacket = FALSE;
															if(RxPacket[0] == 0xFE)
																{
																	if(RxPacket[1] == SYNC_COMMAND)
																		{
																			Sync = FALSE;
																			//FHSS_Timer_Stop();
																		}
																	else if(RxPacket[1] == DIALOG_COMMAND)
																		{
																			//Received Dialog Frame
																			//Send Response
																			TxPacket[0] = 0xFE;
																			TxPacket[1] = RESPONSE_COMMAND;
																			TxPacket[2] = NULL;					//The remaining two bytes of information can be used to update slave status information
																			TxPacket[3] = NULL;
																			Send_Packet(4);						//Send the Slave status reports to Master Device
																			//FHSS_Timer_Stop();
																			FHSS_Timer_Init(0x124F);
																			CONSOLE_PutString((char*)"\rHop Channel: "); PrintDigit(Current_Channel); c(" ");	//There will be less sync loss if we remove these demo prints..
																			SetRFMode(RF_RECEIVER);
																		}
																}
														}
												}
											}
										}
										FHSS_Slave_Enabled = FALSE;
										FHSS_Enable_902 = FALSE;
										FHSS_Enable_863 = FALSE;
										FHSS_Exit();
										c("\r\n");
										break;
							HOTKEYS
							default:	c("Re-Program\r\n");
										goto FHSSDemoStart;

						}

					input = CONSOLE_Get( );
					ConsolePut(input);
					break;
		HOTKEYS
		default:
					c("Re-Program\r\n");
					goto MainMenu;
		}
		goto MainMenu;
}


/*********************************************************************
 * Function:        void PrintHeader(char *)
 *
 * PreCondition:    Console needs to be initialized (ConsoleInit())
 *
 * Input:           String that needs to be printer as Header
 *
 * Output:          None
 *
 * Side Effects:    Prints the String on the Console along with the header notation
 *
 * Overview:        This function prints the header
 *
 * Note:            None
 ********************************************************************/

void PrintHeader(char *app)
{

    BYTE i;
    BYTE stringlen = 0;

    while( (i = app[stringlen++]) );
    stringlen--;
	//calculate the string len for centering the string *app

    CONSOLE_PutString((char*)"\r\n---------------------------------------------------------------\r\n");
    CONSOLE_PutString((char*)"           Microchip MRF89XA Radio Utility Driver Program         \r\n");
    CONSOLE_PutString((char*)"                             Version: ");
    CONSOLE_PutString((char*)VERSION);
    CONSOLE_PutString((char*)"                         \r\n");
	CONSOLE_PutString((char*)"---------------------------------------------------------------\r\n");
	//print the header

    CONSOLE_PutString(app);

    CONSOLE_PutString((char *)":\r\n");
	for (i=0; i<stringlen; i++)
	{
	CONSOLE_PutString((char *)"-");
	}
	nl();
	//print the end line and move cursor over to new line; can even use nl() function

}

/*********************************************************************
 * Function:        void PrintSubHeader(char *)
 *
 * PreCondition:    Console needs to be initialized (ConsoleInit())
 *
 * Input:           String (subheader)
 *
 * Output:          Prints the subheader on console
 *
 * Side Effects:    None
 *
 * Overview:        This function prints the char input in subheader format
 *
 * Note:            None
 ********************************************************************/

void PrintSubHeader(char *app)
{

BYTE i;
BYTE stringlen = 0;
	while( (i = app[stringlen++]) );
	stringlen--;
	CONSOLE_PutString(app);

    CONSOLE_PutString((char *)":\r\n");
	for (i=0; i<stringlen; i++)
	{
	CONSOLE_PutString((char *)"-");
	}
	nl();
	//print the end line and move cursor over to new line

}


/*********************************************************************
 * Function:        void PrintStatus(void)
 *
 * PreCondition:    Console and MRF89XA device needs to be initialized
					(ConsoleInit(), MRF89XAInit())
 *
 * Input:           None
 *
 * Output:          Prints MRF89XA register settings on the console
					Output includes frequency, output power, data rate,
					bandwidth, frequency deviation, IF Gain
 *
 * Side Effects:    None
 *
 * Overview:        This function prints MRF89XA Status on the console
					(hyperterminal)
 *
 * Note:            None
 ********************************************************************/
void PrintStatus()
{

BYTE readback;
WORD readbackw;
PrintSubHeader((char *)"System Status");
CONSOLE_PutString((char*)"\r\n");

if(Modulation_Type)
	{
	c("MRF89XA register settings in FSK mode - \r\n");
	}
else
	{
	c("MRF89XA register settings in OOK Mode - \r\n");
	}
											//Frequency Details
readback = RegisterRead(REG_MCPARAM0);
readback = (readback & 0x18);
if(readback == FREQBAND_902)
	c("Frequency Band = 902-915 MHz")
else if (readback == FREQBAND_915)
	c("Frequency Band = 915-928 MHz")
else
	c("Frequency Band = 950-960 or 863-870 MHz")
	c("\t")
											//IF Gain
readback = RegisterRead(REG_MCPARAM1);
readback = (readback & 0x03);
if(readback == IFGAIN_0)
	c("IF Gain = 0 dB")
else if(readback == IFGAIN_45)
	c("IF Gain = -4.5 dB")
else if (readback == IFGAIN_9)
	c("IF Gain = -9 dB")
else
	c("IF Gain = -13.5 dB")
	c("\t")
											//Bit Rate
readback = RegisterRead(REG_BITRATE);
readback = (200/(readback+1));
	c("Bit Rate = ")
	PrintDigit(readback);
	c("kbps\r\n")
											//Frequency Deviation
readback = RegisterRead(REG_FREGDEV);
readbackw = (400/(readback+1));
	c("Frequency Deviation = ")
	PrintDigitW(readbackw);
	c("KHz\t")
											//Bandwidth
readback = RegisterRead(REG_RXPARAM0);
readback = (readback & 0x0F);
readbackw = 200 * (1+readback)/8;
	c("Bandwidth = ")
	PrintDigitW(readbackw);
	c("KHz\t")
readback = RegisterRead(REG_TXPARAM);		//TX
readback = ((readback & 0xF0)>>4);
readbackw = (200 * (1+readback)/8);
	c("TX Bandwidth = ")
	PrintDigitW(readbackw);
	c("KHz\r\n")
											//Tx Power
readback = RegisterRead(REG_TXPARAM);
readback = (readback & 0x0E);
if(readback == TXPOWER_13)
	c("TX Power = 13 dB")
else if(readback == TXPOWER_10)
	c("TX Power = 10 dB")
else if(readback == TXPOWER_7)
	c("TX Power = 7 dB")
else if(readback == TXPOWER_4)
	c("TX Power = 4 dB")
else if(readback == TXPOWER_1)
	c("TX Power = 1 dB")
else if(readback == TXPOWER_2)
	c("TX Power = -2 dB")
else if(readback == TXPOWER_5)
	c("TX Power = -5 dB")
else if(readback == TXPOWER_8)
	c("TX Power = -8 dB")
	c("\r\n")
c(">>")

}

/*********************************************************************
 * Function:        void PrintMiniStatus(BYTE mode)
 *
 * PreCondition:    MRF89XA and Console needs to be initialized
 					Call ConsoleInit(), MRF89XAInit()
 *
 * Input:           None
 *
 * Output:          Prints the mode (TX, RX or CW) of operation and the
					valid register settings
 *
 *
 * Side Effects:    None
 *
 * Overview:        Prints the current mode the chip is configured in
 *
 *
 * Note:            None
 ********************************************************************/
void PrintMiniStatus(BYTE mode)
{
BYTE readback;
WORD readbackw;
//PrintSubHeader((char *)"System Status");
CONSOLE_PutString((char*)"\r\n");
											//Frequency Details
readback = RegisterRead(REG_MCPARAM0);
readback = (readback & 0x18);
if(readback == FREQBAND_902)
	c("Frequency Band = 902-915 MHz")
else if (readback == FREQBAND_915)
	c("Frequency Band = 915-928 MHz")
else
	c("Frequency Band = 950-960 or 863-870 MHz")
	c("\t")
if((mode == PR_TX_RX_MODE) || (mode == PR_RX_MODE))
{											//IF Gain
readback = RegisterRead(REG_MCPARAM1);
readback = (readback & 0x03);
if(readback == IFGAIN_0)
	c("IF Gain = 0 dB")
else if(readback == IFGAIN_45)
	c("IF Gain = -4.5 dB")
else if (readback == IFGAIN_9)
	c("IF Gain = -9 dB")
else
	c("IF Gain = -13.5 dB")
	c("\t")
}
{											//Bit Rate
readback = RegisterRead(REG_BITRATE);
readback = (200/(readback+1));
	c("Bit Rate = ")
	PrintDigit(readback);
	c("kbps\t")
}											//Frequency Deviation
readback = RegisterRead(REG_FREGDEV);
readback = (400/(readback+1));
	c("Frequency Deviation = ")
	PrintDigit(readback);
	c("KHz\t")
											//Bandwidth
if((mode == PR_RX_MODE) || (mode == PR_TX_RX_MODE))
{
readback = RegisterRead(REG_RXPARAM0);
readback = (readback & 0x0F);
readbackw = ((200 * (1+readback)/8));
	c("Receiver Bandwidth = ")
	PrintDigitW(readbackw);
	c("KHz\t")
}

if((mode == PR_TX_MODE) || (mode == PR_TX_RX_MODE))
{
readback = RegisterRead(REG_TXPARAM);
readback = ((readback & 0xF0)>>4);
readbackw = (200 * (1+readback)/8);
	c("TX Bandwidth = ")
	PrintDigitW(readbackw);
	c("KHz\t")
}

if(( mode == PR_TX_MODE)  || (mode == PR_TX_RX_MODE))
{											//Tx Power
readback = RegisterRead(REG_TXPARAM);
readback = (readback & 0x0F);
if(readback == TXPOWER_13)
	c("TX Power = 13 dBm")
else if(readback == TXPOWER_10)
	c("TX Power = 10 dBm")
else if(readback == TXPOWER_7)
	c("TX Power = 7 dBm")
else if(readback == TXPOWER_4)
	c("TX Power = 4 dBm")
else if(readback == TXPOWER_1)
	c("TX Power = 1 dBm")
else if(readback == TXPOWER_2)
	c("TX Power = -2 dBm")
else if(readback == TXPOWER_5)
	c("TX Power = -5 dBm")
else if(readback == TXPOWER_8)
	c("TX Power = -8 dBm")
}
c("\r\n")
c(">>")
}
#endif

#if 0

void Setup(void)
{

	BYTE input;
	SetRFMode(RF_STANDBY);
										//Program the chip to standby mode before changing from FSK to OOK or OOK to FSK
	PrintHeader((char*)"Setup");
	c("The following three steps are one time configuration settings for the transceiver.If additional configuration settings are needed, please use \"Configure MRF89XA\" menu from Main Menu\r\n\r\n");
Setup_Phase0:
	c("Select type of Modulation\r\n\r\n");
	c(" 1. FSK\r\n");
	c(" 2. OOK\r\n");
	c(">>");
	while(!CONSOLE_IsGetReady()){};
	input = CONSOLE_Get( );
	ConsolePut(input);
	switch(input)
	{
	case '1':		//FSK Modulation
				Modulation_Type = TRUE;
				break;

	case '2':		//OOK Modulation
				Modulation_Type = FALSE;
				break;

	default:	c("\r\nRe-Program\r\n");
					goto Setup_Phase0;
	}
	if(Modulation_Type)
		{
			//Program FSK Modulation parameters
			input = RegisterRead(REG_MCPARAM1);
			input = ((input & 0x3F) | MODSEL_FSK);
    	    RegisterSet(REG_MCPARAM1, input);

         }
	else
		{
			//Program OOK Modulation parameters
			input = RegisterRead(REG_MCPARAM1);
			input = ((input & 0x27) | MODSEL_OOK | OOKTHRESHTYPE_PEAK);
			RegisterSet(REG_MCPARAM1, input);

		}
Setup_Phase1:
	c("\r\n");
	if(Modulation_Type)
	{
	c("Select Frequency of operation for the transceiver\r\n\r\n");
	c(" 1. Frequency Band = 902-915 MHz, Center Frequency = 903 MHz\r\n");
	c(" 2. Frequency Band = 915-928 MHz, Center Frequency = 916 MHz\r\n");
	c(" 3. Frequency Band = 950-960 MHz, Center Frequency = 956 MHz\r\n");
	c(" 4. Frequency Band = 863-870 MHz, Center Frequency = 868 MHz\r\n");
	c(">>");
	}
	else
	{
	c("Select Frequency of operation for the transceiver\r\n\r\n");
	c(" 1. Frequency Band = 902-915 MHz, Center Frequency = 902.9 MHz\r\n");
	c(" 2. Frequency Band = 915-928 MHz, Center Frequency = 915.9 MHz\r\n");
	c(" 3. Frequency Band = 950-960 MHz, Center Frequency = 955.9 MHz\r\n");
	c(" 4. Frequency Band = 863-870 MHz, Center Frequency = 867.9 MHz\r\n");
	c(">>");
	}
	while(!CONSOLE_IsGetReady()){};
	input = CONSOLE_Get( );
	ConsolePut(input);
	switch(input)
	{
		case '1':	FREQ_BAND = FREQ_BAND_RESET = FREQBAND_902;
					RVALUE = RVALUE_RESET = 119;
					PVALUE = PVALUE_RESET = 99;
					SVALUE = SVALUE_RESET = 25;
					//Center Frequency setting = 903 MHz
					break;
		case '2': 	FREQ_BAND = FREQ_BAND_RESET = FREQBAND_915;
					RVALUE = RVALUE_RESET = 119;
					PVALUE = PVALUE_RESET = 100;
					SVALUE = SVALUE_RESET =	50;
					//Center Frequency setting = 916 MHz
					break;
		case '3':	FREQ_BAND = FREQ_BAND_RESET = FREQBAND_950;
					RVALUE = RVALUE_RESET = 125;
					PVALUE = PVALUE_RESET = 110;
					SVALUE = SVALUE_RESET = 40;
					//Center Frequency setting = 956 MHz
					break;
		case '4':	FREQ_BAND = FREQ_BAND_RESET = FREQBAND_950;
					RVALUE = RVALUE_RESET = 125;
					PVALUE = PVALUE_RESET = 100;
					SVALUE = SVALUE_RESET = 20;
					//Center Frequency setting = 868 MHz
					break;
		default:	c("\r\nRe-Program\r\n");
					goto Setup_Phase1;
	}

	RegisterSet(REG_MCPARAM0, ((InitConfigRegs[REG_MCPARAM0]&0xE7)|FREQ_BAND));
	if(FREQ_BAND == FREQBAND_950)
	{
		BYTE readback = RegisterRead(REG_MCPARAM0);
		readback = (readback & 0xF8);
		RegisterSet(REG_MCPARAM0, readback);
	}
	RegisterSet(REG_R1, RVALUE);
	RegisterSet(REG_P1, PVALUE);
	RegisterSet(REG_S1, SVALUE);

Setup_Phase2:
	if(Modulation_Type)
	{
	c("\r\n");
	c("Choose from one of the following optimal FSK settings for the transceiver\r\n\r\n");
	c(" 1. DataRate = 2 kbps, BW = 50 KHz, Frequency Deviation = 33 KHz\r\n");
	c(" 2. DataRate = 5 kbps, BW = 50 KHz, Frequency Deviation = 33 KHz\r\n");
	c(" 3. DataRate = 10 kbps, BW = 50 KHz, Frequency Deviation = 33 KHz\r\n");
	c(" 4. DataRate = 20 kbps, BW = 75 KHz, Frequency Deviation = 40 KHz\r\n");
	c(" 5. DataRate = 40 kbps, BW = 150 KHz, Frequency Deviation = 80 KHz\r\n");
	c(" 6. DataRate = 50 kbps, BW = 175 KHz, Frequency Deviation = 100 KHz\r\n");
	c(" 7. DataRate = 66.7 kbps, BW = 250 KHz, Frequency Deviation = 133 KHz\r\n");
	c(" 8. DataRate = 100 kbps, BW = 400 KHz, Frequency Deviation = 200 KHz\r\n");
	c(" 9. DataRate = 200 kbps, BW = 400 KHz, Frequency Deviation = 200 KHz\r\n");
	c(">>");
	}
	else
	{
	c("\r\n");
	c("Choose from one of the following optimal OOK settings for the transceiver\r\n\r\n");
	c(" 1. DataRate = 1.56 kbps, BW = 125 KHz\r\n");
	c(" 2. DataRate = 2.41 kbps, BW = 125 KHz\r\n");
	c(" 3. DataRate = 4.76 kbps, BW = 125 KHz\r\n");
	c(" 4. DataRate = 8 kbps, BW = 125 KHz\r\n");
	c(" 5. DataRate = 9.52 kbps, BW = 125 KHz\r\n");
	c(" 6. DataRate = 12.5 kbps, BW = 150 KHz\r\n");
	c(" 7. DataRate = 16.67 kbps, BW = 150 KHz\r\n");
	c(">>");
	}

	while(!CONSOLE_IsGetReady());
	input = CONSOLE_Get( );
	ConsolePut(input);
	if(Modulation_Type)
	{
			switch(input)
			{
				case '1': 	Rate_C = BITRATE_2; Rate_R = 2;
							FREQ_BW = RXFC_FOPLUS50;
							TX_FSK = FREGDEV_33;
							PFILTER_SETTING = PASSIVEFILT_157;
							break;
				case '2':	Rate_C = BITRATE_5; Rate_R = 5;
							FREQ_BW = RXFC_FOPLUS50;
							TX_FSK = FREGDEV_33;
							PFILTER_SETTING = PASSIVEFILT_157;
							break;
				case '3':	Rate_C = BITRATE_10; Rate_R = 10;
							FREQ_BW = RXFC_FOPLUS50;
							TX_FSK = FREGDEV_33;
							PFILTER_SETTING = PASSIVEFILT_157;
							break;
				case '4': 	Rate_C = BITRATE_20; Rate_R = 20;
							FREQ_BW = RXFC_FOPLUS75;
							TX_FSK = FREGDEV_40;
							PFILTER_SETTING = PASSIVEFILT_234;
							break;
				case '5':	Rate_C = BITRATE_40; Rate_R = 40;
							FREQ_BW = RXFC_FOPLUS150;
							TX_FSK = FREGDEV_80;
							PFILTER_SETTING = PASSIVEFILT_414;
							break;
				case '6':	Rate_C = BITRATE_50; Rate_R = 50;
							FREQ_BW = RXFC_FOPLUS175;
							TX_FSK = FREGDEV_100;
							PFILTER_SETTING = PASSIVEFILT_514;
							break;
				case '7': 	Rate_C = BITRATE_66; Rate_R = 66;
							FREQ_BW = RXFC_FOPLUS250;
							TX_FSK = FREGDEV_133;
							PFILTER_SETTING = PASSIVEFILT_676;
							break;
				case '8': 	Rate_C = BITRATE_100; Rate_R = 100;
							FREQ_BW = RXFC_FOPLUS400;
							TX_FSK = FREGDEV_200;
							PFILTER_SETTING = PASSIVEFILT_987;
							break;
				case '9':	Rate_C = BITRATE_200; Rate_R = 200;
							FREQ_BW = RXFC_FOPLUS400;
							TX_FSK = FREGDEV_200;
							PFILTER_SETTING = PASSIVEFILT_987;
							break;
				default:	c("\r\nRe-Program\r\n");
							goto Setup_Phase2;
			}
			RegisterSet(REG_BITRATE, Rate_C);
			RegisterSet(REG_RXPARAM0, (FREQ_BW | PFILTER_SETTING));
			RegisterSet(REG_FREGDEV, TX_FSK);
	}
	else
	{
		switch(input)
			{
				case '1': 	Rate_C = BITRATE_1_56; Rate_R = 0x8000;
							FREQ_BW = RXFC_FOPLUS25;
							TX_FSK = FREGDEV_100;
							PFILTER_SETTING = PASSIVEFILT_378;
							break;
				case '2':	Rate_C = BITRATE_2_41; Rate_R = 0x8000;
							FREQ_BW = RXFC_FOPLUS25;
							TX_FSK = FREGDEV_100;
							PFILTER_SETTING = PASSIVEFILT_378;
							break;
				case '3':	Rate_C = BITRATE_4_76; Rate_R = 0x8000;
							FREQ_BW = RXFC_FOPLUS25;
							TX_FSK = FREGDEV_100;
							PFILTER_SETTING = PASSIVEFILT_378;
							break;
				case '4': 	Rate_C = BITRATE_8; Rate_R = 8;
							FREQ_BW = RXFC_FOPLUS25;
							TX_FSK = FREGDEV_100;
							PFILTER_SETTING = PASSIVEFILT_378;
							break;
				case '5':	Rate_C = BITRATE_9_52; Rate_R = 0x8000;
							FREQ_BW = RXFC_FOPLUS25;
							TX_FSK = FREGDEV_100;
							PFILTER_SETTING = PASSIVEFILT_378;
							break;
				case '6':	Rate_C = BITRATE_12_5; Rate_R = 0x8000;
							FREQ_BW = RXFC_FOPLUS50;
							TX_FSK = FREGDEV_100;
							PFILTER_SETTING = PASSIVEFILT_458;
							break;
				case '7': 	Rate_C = BITRATE_16_67; Rate_R = 0x8000;
							FREQ_BW = RXFC_FOPLUS50;
							TX_FSK = FREGDEV_100;
							PFILTER_SETTING = PASSIVEFILT_458;
							break;
		}
			RegisterSet(REG_BITRATE, Rate_C);
			RegisterSet(REG_RXPARAM0, (FREQ_BW | PFILTER_SETTING));
			RegisterSet(REG_FREGDEV, TX_FSK);
	}
	//Perform frequency synthesization
	input = RegisterRead(REG_MCPARAM0);
	RegisterSet(REG_MCPARAM0, (input & 0x1F) | RF_SYNTHESIZER);
	RF_Mode = RF_SYNTHESIZER;

	//clear PLL_LOCK flag so we can see it restore on the new frequency
	input = RegisterRead(REG_IRQPARAM1);
	RegisterSet(REG_IRQPARAM1, (input | 0x02));

	SetRFMode(RF_RECEIVER);
	//transceiver will be set to FSK or OOK mode
	//Include delay and put the chip back to standby mode
	{
	WORD i;
	for(i=0;i<5000;i++)
	{
		Nop();
	}
	}
	SetRFMode(RF_STANDBY);

}

#endif
