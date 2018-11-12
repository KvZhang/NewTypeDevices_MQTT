
/*
 * LaputaComm.cpp
 *
 *  Created on: 2015年3月17日
 *      Author: ZhangYu
 */
#define SERIAL_MAIN (1)
#include <LaputaCommHWV2.h>
#include "LaputaConfig.h"
extern LaputaConfig myConfig;
LaputaComm::LaputaComm(unsigned char addr)
{
	// TODO Auto-generated constructor stub
	address=addr;
	state=0;
	memset(rxBuffer,0,sizeof(rxBuffer));
	memset((uint8_t *)&txBuffer,0,sizeof(txBuffer));
	txBuffer.bit.header1=0xAB;
	txBuffer.bit.header2=0xCD;
	txBuffer.bit.address=addr;
	rx_ptr=0;
	setPin=4;
#ifdef _USE_YEILD
	rev_state=Receive_header1;
#endif
	lastReceiveMoment=0;
}
uint8_t LaputaComm::calc_chksum(uint8_t *ptr,uint8_t length)
{
	uint8_t checksum=0;
	for(unsigned i=0;i<length;i++)
	{
		checksum+=*(ptr+i);
	}
	return checksum;
}
bool LaputaComm::try_BDR(int32_t BDR,int32_t BDR_des, uint8_t ch)
{
	Serial.begin(BDR);
	Serial.println("AT+FU1");

	uint32_t moment=millis();
	while(millis()-moment<50);

	if(Serial.available())
	{
		uint8_t temp = Serial.read();
		if(temp =='O' && Serial.read()=='K')
		{
			Serial.print("AT+C");
			if(ch<=9)
			{
				Serial.print("00");
			}
			else
			{
				Serial.print("0");
			}
			Serial.println(ch);
			moment=millis();
			while(millis()-moment<100);
			Serial.print("AT+B");
			Serial.println(BDR_des);
			moment=millis();
			while(millis()-moment<100);
			Serial.begin(BDR_des);
			return 0;
		}
		else
		{
			return 1;
		}
	}
	return 1;
}
uint8_t LaputaComm::begin(uint8_t room_addr,int32_t BDR,uint8_t setPin)
{
	pinMode(setPin,OUTPUT);
	digitalWrite(setPin,LOW);
	uint32_t moment = millis();
	while(millis() - moment < 50); //need wait for 50 ms after pulling setPin down
	Serial.begin(BDR);
	Serial.println("AT+FU1");
	moment=millis();
	while(millis()-moment<100); // wait for about 100 ms for module to answer
	if(Serial.available())
	{
		uint8_t temp=Serial.read();
		if(temp =='O' && Serial.read()=='K')
		{
			Serial.print("AT+C");
			if(room_addr<=9)
			{
				Serial.print("00");
			}
			else 
			{
				Serial.print("0");
			}
			if(room_addr<=19)
			{
				Serial.println(room_addr);
			}
			else
			{
				Serial.println(20);
			}
			moment=millis();
			while(millis()-moment<100);
			pinMode(setPin,INPUT_PULLUP);
			return 0;
		}
		else
		{
			pinMode(setPin,INPUT_PULLUP);
			return 1;
		}
	}

	if(try_BDR(9600, BDR, room_addr))
	{
		if(try_BDR(19200, BDR, room_addr))
		{
			if(try_BDR(38400, BDR, room_addr))
			{
				if(try_BDR(57600, BDR, room_addr))
				{
					if(try_BDR(115200, BDR, room_addr))
					{
						pinMode(setPin,INPUT_PULLUP);
						return 2;
					}
					else
					{
						goto normal_return;
					}
				}
				else
				{
					goto normal_return;
				}
			}
			else
			{
				goto normal_return;
			}
		}
		else
		{
			goto normal_return;
		}
	}
	else
	{

		goto normal_return;
	}
	pinMode(setPin,INPUT_PULLUP);
	return 3;
normal_return:
	Serial.flush();
	pinMode(setPin,INPUT_PULLUP);
	return 0;
}
uint8_t LaputaComm::begin(int32_t BDR)
{
	Serial.begin(BDR);
	return 0;
}
void LaputaComm::sendCMD(uint8_t target_addr, bool isNeedACK)
{
	if(!sendAvailable())
	{
		return;
	}
	if(txBuffer.bit.length<=BUFFER_SIZE-6)
	{
		txBuffer.bit.address=target_addr;
		txBuffer.bit.content[txBuffer.bit.length]=calc_chksum((uint8_t *)&txBuffer.bit.address, txBuffer.bit.length+3);
		if(isNeedACK)
		{
			ack_needs.target_addr=target_addr;
			ack_needs.isBuzy=true;
			ack_needs.retry_count=0;
			ack_needs.sendMoment=millis();
			ack_needs.interval=(millis()&0xff)+200;
		}
		if(millis() >= (lastReceiveMoment + 10 + address*5))
		{
			//the 'BUS' is IDLE now, send it right now
			Serial.write((const char*)&txBuffer, txBuffer.bit.length+6);
		}
		else
		{
			//the 'Bus' just receive something, wait for a moment.
			ack_needs.sendMoment=lastReceiveMoment;
			ack_needs.interval = address*5;
			if(!isNeedACK)
			{
				ack_needs.target_addr=target_addr;
				ack_needs.isBuzy=true;
				ack_needs.retry_count=5;
			}
		}
	}
}
void LaputaComm::sendRepeat()
{
	Serial.write((const char*)&txBuffer, txBuffer.bit.length+6);
	ack_needs.sendMoment=millis();
	ack_needs.interval=(millis()&0xff)+200;
}
void LaputaComm::sendCMD(void)
{
	sendCMD(address);
}
bool LaputaComm::sendAvailable()
{
	return !ack_needs.isBuzy;
}

bool LaputaComm::receiveCMD()
{
	static uint8_t checksum = 0;

	//缓冲区有数据，开始接收流程
	if (Serial.available() > 0)
	{
		rxBuffer[rx_ptr]=Serial.read();

		lastReceiveMoment=millis();
		switch(rev_state)
		{
		case Receive_header1:
			if(rxBuffer[rx_ptr]==0xAB)
			{
				rx_ptr++;
				rev_state=Receive_header2;
			}
			else
			{
				checksum=0;
			}
			return 0;
		case Receive_header2:
			if(rxBuffer[rx_ptr]==0xCD)
			{
				rx_ptr++;
				rev_state=Receive_addr;
			}
			else
			{
				rx_ptr=0;
				rev_state=Receive_header1;
				checksum=0;
			}
			return 0;
		case Receive_addr:
			if(rxBuffer[rx_ptr]==address)
			{
				checksum+=rxBuffer[rx_ptr];
				rx_ptr++;
				rev_state=Receive_cmd;
			}
			else
			{
				rx_ptr=0;
				rev_state=Receive_header1;
				checksum=0;
			}
			return 0;
		case Receive_cmd:
			checksum+=rxBuffer[rx_ptr];
			rx_ptr++;
			rev_state=Receive_length;
			return 0;
		case Receive_length:
			checksum+=rxBuffer[rx_ptr];
			rx_ptr++;
			if(rxBuffer[Frame_Length])
			{
				rev_state=Receive_data;
			}
			else
			{
				rev_state=Receive_checksum;
			}
			return 0;
		case Receive_data:
			checksum+=rxBuffer[rx_ptr];
			rx_ptr++;
			if(rx_ptr==rxBuffer[Frame_Length]+5)
			{
				rev_state=Receive_checksum;
			}
			return 0;
		case Receive_checksum:
			rev_state=Receive_header1;
			if(checksum!=rxBuffer[rx_ptr])
			{
				rx_ptr=0;
				checksum=0;
				txBuffer.bit.err_status=STATUS_WRONG_CHKSUM;
				return 0;
			}
			rx_ptr=0;
			checksum=0;
			break;
		default:
			rx_ptr=0;
			rev_state=Receive_header1;
			checksum=0;
			return 0;
		}
		switch(rxBuffer[Frame_CMD])
		{
		case COMMON_CMD_READ_SYS_TICK:
			read_handle((uint16_t)(millis()/1000));
			return 0;
		case COMMON_CMD_GOTO_BOOTLOADER:
			cmd_ACK(STATUS_OK);
			{
				uint32_t moment=millis();
				while(millis()-moment<10);
			}
			asm("JMP 15872 ");
			return 0;
		case COMMON_CMD_INIT_ME:
			if(rxBuffer[Frame_Length]==0)
			{
				txBuffer.bit.err_status=COMMON_CMD_ACK;
				txBuffer.bit.length=2;
				txBuffer.bit.content[0]=myConfig.context.count>>8;
				txBuffer.bit.content[0]=myConfig.context.count&0xff;
				sendCMD(0);
			}
			else
			{
				cmd_ACK(STATUS_OK);
				if(rxBuffer[Frame_Length]==1)
				{
					myConfig.context.count=rxBuffer[Frame_data0];
				}
				else
				{
					myConfig.context.count=rxBuffer[Frame_data0]*256+rxBuffer[Frame_data1];
				}
				myConfig.save_config();
			}
			return 0;
		case COMMON_CMD_CHANGE_CHANNEL:
			myConfig.context.room_addr=rxBuffer[Frame_data0];
			myConfig.save_config();
			cmd_ACK(STATUS_OK);
			return 0;
		case COMMON_CMD_CHANGE_ADDR:
			myConfig.context.me_addr=rxBuffer[Frame_data0];
			address=myConfig.context.me_addr;
			myConfig.save_config();
			cmd_ACK(STATUS_OK);
			return 0;
		case COMMON_CMD_CHANGE_PRE_ADDR:
			myConfig.context.pre_addr=rxBuffer[Frame_data0];
			myConfig.save_config();
			cmd_ACK(STATUS_OK);
			return 0;
		case COMMON_CMD_CHANGE_NEXT_ADDR:
			myConfig.context.next_addr=rxBuffer[Frame_data0];
			myConfig.save_config();
			cmd_ACK(STATUS_OK);
			return 0;
		case COMMON_CMD_READ_SETTING:
			txBuffer.bit.length=4;
			txBuffer.bit.content[0]=myConfig.context.room_addr;
			txBuffer.bit.content[1]=myConfig.context.me_addr;
			txBuffer.bit.content[2]=myConfig.context.pre_addr;
			txBuffer.bit.content[3]=myConfig.context.next_addr;
			sendCMD(0x00);
			return 0;
		case COMMON_CMD_ACK:
			if(ack_needs.isBuzy)
			{
				ack_needs.isBuzy=false;
				ack_needs.retry_count=0;
			}
			return 1;
		case COMMON_CMD_RW_USER_DATA:
		{
			if(rxBuffer[Frame_Length]==0)
			{
				for(uint8_t i=0;i<14;i++)
				{
					txBuffer.bit.content[i] = myConfig.context.user_data[i];
				}
				txBuffer.bit.length=14;
				sendCMD(0);
			}
			else
			{
				for(uint8_t i=0;i<rxBuffer[Frame_Length];i++)
				{
					myConfig.context.user_data[i] = rxBuffer[Frame_data0 + i];
				}
				cmd_ACK(STATUS_OK);
			}
		}
			return 0;
		default:
			txBuffer.bit.err_status=STATUS_OK;
			return 1;
		}
	}
	else
	{
		//距离上一次接收已超过10ms
		if((millis() - lastReceiveMoment) > 10)
		{
			//清掉串口缓存，应该没有
			//判断接收超时
			if(rev_state!=Receive_header1)
			{
				while(Serial.read()>=0);
				rx_ptr=0;
				rev_state=Receive_header1;
				checksum=0;
			}
			//判断发送需求
			if(ack_needs.isBuzy)
			{
				if(millis() > (ack_needs.sendMoment + ack_needs.interval))
				{
					if(ack_needs.retry_count++>5)
					{
						ack_needs.retry_count=0;
						ack_needs.isBuzy=false;
					}
					else
					{
						sendRepeat();
					}
				}
			}
		}
		else
		{
		}
		return 0;
	}
}


void LaputaComm::read_handle(uint8_t data)
{
	txBuffer.bit.err_status=STATUS_OK;
	txBuffer.bit.length=1;
	txBuffer.bit.content[0]=data;
	sendCMD();
}
void LaputaComm::read_handle(uint16_t data)
{
	txBuffer.bit.err_status=STATUS_OK;
	txBuffer.bit.length=2;
	txBuffer.bit.content[0]=data&0xFF;
	txBuffer.bit.content[1]=(data>>8)&0xFF;
	sendCMD();
}
void LaputaComm::read_handle(uint32_t data)
{
	txBuffer.bit.err_status=STATUS_OK;
	txBuffer.bit.length=4;
	txBuffer.bit.content[0]=data&0xFF;
	txBuffer.bit.content[1]=(data>>8)&0xFF;
	txBuffer.bit.content[2]=(data>>16)&0xFF;
	txBuffer.bit.content[3]=(data>>24)&0xFF;
	sendCMD();
}
void LaputaComm::cmd_ACK(uint8_t status)
{
	txBuffer.bit.err_status=status;
	txBuffer.bit.length=0;
	sendCMD();
}
void LaputaComm::cmd_ACK(uint8_t status,uint8_t Addr)
{
	txBuffer.bit.err_status=status;
	txBuffer.bit.length=0;
	sendCMD(Addr);
}

void LaputaComm::controlDoor(bool status)
{
	txBuffer.bit.err_status=0x05;
	txBuffer.bit.length=0x01;
	txBuffer.bit.content[0]=status;
	sendCMD(0x01);
}

void LaputaComm::controlMusicAndLight(uint8_t song, uint8_t light)
{
	txBuffer.bit.err_status=0x05;
	txBuffer.bit.length=0x03;
	txBuffer.bit.content[0]=address;
	txBuffer.bit.content[1]=song;
	txBuffer.bit.content[2]=light;
	sendCMD(0x02);
}

LaputaComm::~LaputaComm()
{
// TODO Auto-generated destructor stub
}
