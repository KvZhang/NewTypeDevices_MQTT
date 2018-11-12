/*
 * LaputaComm.cpp
 *
 *  Created on: 2015年3月17日
 *      Author: ZhangYu
 */
#define SERIAL_MAIN (1)
#include "LaputaCommSWV2.h"
#include "LaputaConfig.h"
SoftwareSerial mySerial(2,3);
extern LaputaConfig myConfig;
LaputaComm::LaputaComm(unsigned char addr)
{
	// TODO Auto-generated constructor stub
	this->address=addr;
	this->state=0;
	this->state_sub_1=0;
	this->state_sub_2=0;
	memset(this->rxBuffer,0,sizeof(this->rxBuffer));
	memset((Uint8 *)&this->txBuffer,0,sizeof(this->txBuffer));
	this->txBuffer.bit.header1=0xab;
	this->txBuffer.bit.header2=0xcd;
	this->txBuffer.bit.address=addr;
	this->rx_ptr=0;
	this->setPin=4;
#ifdef _USE_YEILD
	this->rev_state=Receive_header1;
#endif
	this->lastReceiveMoment=0;
}
Uint8 LaputaComm::calc_chksum(Uint8 *ptr,Uint8 length)
{
	Uint8 checksum=0;
	for(unsigned i=0;i<length;i++)
	{
		checksum+=*(ptr+i);
	}
	return checksum;
}
bool LaputaComm::try_BDR(Int32 BDR,Int32 BDR_des, Uint8 ch)
{
	mySerial.begin(BDR);
	mySerial.println("AT+FU1");

	uint32_t moment=millis();
	while(millis()-moment<200);

	if(mySerial.available())
	{
		uint8_t temp = mySerial.read();
		if(temp =='O' && mySerial.read()=='K')
		{
			mySerial.print("AT+C");
			if(ch<=9)
			{
				mySerial.print("00");
			}
			else
			{
				mySerial.print("0");
			}
			mySerial.println(ch);
			moment=millis();
			while(millis()-moment<100);
			mySerial.print("AT+B");
			mySerial.println(BDR_des);
			moment=millis();
			while(millis()-moment<100);
			mySerial.begin(BDR_des);
			return 0;
		}
		else
		{
//			Serial.print(temp);
//			while(mySerial.available())
//			{
//				Serial.print(mySerial.read());
//			}
//			Serial.println("  ");
			return 1;
		}
	}
	return 1;
}
Uint8 LaputaComm::begin(Uint8 room_addr,Int32 BDR,Uint8 setPin)
{
	pinMode(setPin,OUTPUT);
	digitalWrite(setPin,LOW);
	uint32_t moment=millis();
	while(millis()-moment<100);
	mySerial.begin(BDR);
	mySerial.println("AT+FU1");
	moment=millis();
	while(millis()-moment<100);
	if(mySerial.available())
	{
		uint8_t temp=mySerial.read();
		if(temp =='O' && mySerial.read()=='K')
		{
			mySerial.print("AT+C");
			if(room_addr<=9)
			{
				mySerial.print("00");
			}
			else 
			{
				mySerial.print("0");
			}
			if(room_addr<=19)
			{
				mySerial.println(room_addr);
			}
			else
			{
				mySerial.println(20);
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

	if(this->try_BDR(9600, BDR, room_addr))
	{
		if(this->try_BDR(19200, BDR, room_addr))
		{
			if(this->try_BDR(38400, BDR, room_addr))
			{
				if(this->try_BDR(57600, BDR, room_addr))
				{
					if(this->try_BDR(115200, BDR, room_addr))
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
	mySerial.flush();
	pinMode(setPin,INPUT_PULLUP);
	return 0;
}
Uint8 LaputaComm::begin(Int32 BDR)
{
	mySerial.begin(BDR);
	return 0;
}
void LaputaComm::sendCMD(Uint8 target_addr, bool ack_needs)
{
	if(!this->sendAvailable())
	{
		return;
	}
	if(this->txBuffer.bit.length<=BUFFER_SIZE-6)
	{
		this->txBuffer.bit.address=target_addr;
		this->txBuffer.bit.content[this->txBuffer.bit.length]=this->calc_chksum((Uint8 *)&this->txBuffer.bit.address, this->txBuffer.bit.length+3);
		if(ack_needs)
		{
			this->ack_needs.target_addr=target_addr;
			this->ack_needs.isBuzy=true;
			this->ack_needs.retry_count=0;
			this->ack_needs.sendMoment=millis();
			this->ack_needs.interval=(millis()&0xff)+200;
		}
		if(millis() >= (this->lastReceiveMoment + 10 + this->address*5))
		{
			//the 'BUS' is IDLE now, send it right now
			mySerial.write((const char*)&this->txBuffer, this->txBuffer.bit.length+6);
		}
		else
		{
			//the 'Bus' just receive something, wait for a moment.
			this->ack_needs.sendMoment=this->lastReceiveMoment;
			this->ack_needs.interval = this->address*5;
			if(!ack_needs)
			{
				this->ack_needs.target_addr=target_addr;
				this->ack_needs.isBuzy=true;
				this->ack_needs.retry_count=5;
			}
		}
	}
}
void LaputaComm::sendRepeat()
{
	mySerial.write((const char*)&this->txBuffer, this->txBuffer.bit.length+6);
	this->ack_needs.sendMoment=millis();
	this->ack_needs.interval=(millis()&0xff)+200;
}
void LaputaComm::sendCMD(void)
{
	this->sendCMD(this->address);
}
bool LaputaComm::sendAvailable()
{
	return !this->ack_needs.isBuzy;
}
#ifdef _USE_YEILD
bool LaputaComm::receiveCMD()
{
	static uint8_t checksum = 0;

	//缓冲区有数据，开始接收流程
	if (mySerial.available() > 0)
	{
		this->rxBuffer[this->rx_ptr]=mySerial.read();

		this->lastReceiveMoment=millis();
		switch(this->rev_state)
		{
		case Receive_header1:
			if(this->rxBuffer[this->rx_ptr]==0xAB)
			{
				this->rx_ptr++;
				this->rev_state=Receive_header2;
			}
			else
			{
				checksum=0;
			}
			return 0;
		case Receive_header2:
			if(this->rxBuffer[this->rx_ptr]==0xCD)
			{
				this->rx_ptr++;
				this->rev_state=Receive_addr;
			}
			else
			{
				this->rx_ptr=0;
				this->rev_state=Receive_header1;
				checksum=0;
			}
			return 0;
		case Receive_addr:
			if(this->rxBuffer[this->rx_ptr]==this->address)
			{
				checksum+=this->rxBuffer[this->rx_ptr];
				this->rx_ptr++;
				this->rev_state=Receive_cmd;
			}
			else
			{
				this->rx_ptr=0;
				this->rev_state=Receive_header1;
				checksum=0;
			}
			return 0;
		case Receive_cmd:
			checksum+=this->rxBuffer[this->rx_ptr];
			this->rx_ptr++;
			this->rev_state=Receive_length;
			return 0;
		case Receive_length:
			checksum+=this->rxBuffer[this->rx_ptr];
			this->rx_ptr++;
			if(this->rxBuffer[Frame_Length])
			{
				this->rev_state=Receive_data;
			}
			else
			{
				this->rev_state=Receive_checksum;
			}
			return 0;
		case Receive_data:
			checksum+=this->rxBuffer[this->rx_ptr];
			this->rx_ptr++;
			if(this->rx_ptr==this->rxBuffer[Frame_Length]+5)
			{
				this->rev_state=Receive_checksum;
			}
			return 0;
		case Receive_checksum:
			this->rev_state=Receive_header1;
			if(checksum!=this->rxBuffer[rx_ptr])
			{
				this->rx_ptr=0;
				checksum=0;
				this->txBuffer.bit.err_status=STATUS_WRONG_CHKSUM;
				return 0;
			}
			this->rx_ptr=0;
			checksum=0;
			break;
		default:
			this->rx_ptr=0;
			this->rev_state=Receive_header1;
			checksum=0;
			return 0;
		}
		switch(this->rxBuffer[Frame_CMD])
		{
		case COMMON_CMD_READ_SYS_TICK:
			this->read_handle(millis()/1000);
			return 0;
		case COMMON_CMD_GOTO_BOOTLOADER:
			this->cmd_ACK(STATUS_OK);
			{
				uint32_t moment=millis();
				while(millis()-moment<10);
			}
			asm("JMP 15872 ");
			return 0;
		case COMMON_CMD_INIT_ME:
			if(this->rxBuffer[Frame_Length]==0)
			{
				this->txBuffer.bit.err_status=COMMON_CMD_ACK;
				this->txBuffer.bit.length=2;
				this->txBuffer.bit.content[0]=myConfig.context.count>>8;
				this->txBuffer.bit.content[0]=myConfig.context.count&0xff;
				this->sendCMD(0);
			}
			else
			{
				this->cmd_ACK(STATUS_OK);
				if(this->rxBuffer[Frame_Length]==1)
				{
					myConfig.context.count=this->rxBuffer[Frame_data0];
				}
				else
				{
					myConfig.context.count=this->rxBuffer[Frame_data0]*256+this->rxBuffer[Frame_data1];
				}
				myConfig.save_config();
			}
			return 0;
		case COMMON_CMD_CHANGE_CHANNEL:
			myConfig.context.room_addr=this->rxBuffer[Frame_data0];
			myConfig.save_config();
			this->cmd_ACK(STATUS_OK);
			return 0;
		case COMMON_CMD_CHANGE_ADDR:
			myConfig.context.me_addr=this->rxBuffer[Frame_data0];
			this->address=myConfig.context.me_addr;
			myConfig.save_config();
			this->cmd_ACK(STATUS_OK);
			return 0;
		case COMMON_CMD_CHANGE_PRE_ADDR:
			myConfig.context.pre_addr=this->rxBuffer[Frame_data0];
			myConfig.save_config();
			this->cmd_ACK(STATUS_OK);
			return 0;
		case COMMON_CMD_CHANGE_NEXT_ADDR:
			myConfig.context.next_addr=this->rxBuffer[Frame_data0];
			myConfig.save_config();
			this->cmd_ACK(STATUS_OK);
			return 0;
		case COMMON_CMD_READ_SETTING:
			this->txBuffer.bit.length=4;
			this->txBuffer.bit.content[0]=myConfig.context.room_addr;
			this->txBuffer.bit.content[1]=myConfig.context.me_addr;
			this->txBuffer.bit.content[2]=myConfig.context.pre_addr;
			this->txBuffer.bit.content[3]=myConfig.context.next_addr;
			this->sendCMD(0x00);
			return 0;
		case COMMON_CMD_ACK:
			if(this->ack_needs.isBuzy)
			{
				this->ack_needs.isBuzy=false;
				this->ack_needs.retry_count=0;
			}
			return 1;
		case COMMON_CMD_RW_USER_DATA:
		{
			if(this->rxBuffer[Frame_Length]==0)
			{
				for(uint8_t i=0;i<14;i++)
				{
					this->txBuffer.bit.content[i] = myConfig.context.user_data[i];
				}
				this->txBuffer.bit.length=14;
				this->sendCMD(0);
			}
			else
			{
				for(uint8_t i=0;i<this->rxBuffer[Frame_Length];i++)
				{
					myConfig.context.user_data[i] = this->rxBuffer[Frame_data0 + i];
				}
				this->cmd_ACK(STATUS_OK);
			}
		}
			return 0;
		default:
			this->txBuffer.bit.err_status=STATUS_OK;
			return 1;
		}
	}
	else
	{
		//距离上一次接收已超过10ms
		if((millis()-this->lastReceiveMoment) > 10)
		{
			//清掉串口缓存，应该没有
			//判断接收超时
			if(this->rev_state!=Receive_header1)
			{
				while(mySerial.read()>=0);
				this->rx_ptr=0;
				this->rev_state=Receive_header1;
				checksum=0;
			}
			//判断发送需求
			if(this->ack_needs.isBuzy)
			{
				if(millis() > (this->ack_needs.sendMoment + this->ack_needs.interval))
				{
					if(this->ack_needs.retry_count++>5)
					{
						this->ack_needs.retry_count=0;
						this->ack_needs.isBuzy=false;
					}
					else
					{
						this->sendRepeat();
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
#else
bool LaputaComm::receiveCMD()
{
	Uint8 checksum = 0;

	//if there is sth, delay 10ms for receive all bytes
	if (mySerial.available() > 0)
	{
#ifdef DEBUG
		Serial.println("status 1");
#endif
		delay(10);
	}
	else
	{
		if(this->ack_needs.isBuzy)
		{
			if(millis()>this->ack_needs.sendMoment+this->ack_needs.interval)
			{
				if(this->ack_needs.retry_count++>5)
				{
					this->ack_needs.retry_count=0;
					this->ack_needs.isBuzy=false;
				}
				else
				{
					this->sendRepeat();
				}
			}
		}
		return 0;
	}
	//check the start byte 0xAA
	do
	{
		this->rxBuffer[0]=mySerial.read();
#ifdef DEBUG
		Serial.println(this->rxBuffer[0]);
#endif
		if(mySerial.available()==0)
		{
			return 0;
#ifdef DEBUG
		Serial.println("status 4");
#endif
		}
	}
	while(this->rxBuffer[0]!=0xAB);
#ifdef DEBUG
		Serial.println("status 2");
#endif
	//OK, we get a 0xAB, if the next byte count less than 3, must be an error info, flush it
	this->rxBuffer[1]=mySerial.read();
	if(mySerial.available()<3 || this->rxBuffer[1]!=0xCD)
	{
		mySerial.flush();
		return 0;
	}
#ifdef DEBUG
		Serial.println("status 3");
#endif
	//long than 3, save them
	this->rx_ptr=Frame_Address;
	while(mySerial.available() > 0)
	{
		this->rxBuffer[this->rx_ptr]=mySerial.read();
		this->rx_ptr++;
		if(rx_ptr>=BUFFER_SIZE)
		{
			mySerial.flush();
			break;
		}
	}
	//check the address
	if(this->rxBuffer[Frame_Address]!=this->address)
	{
		this->rx_ptr=0;
		return 0;
	}
	//more than 4 bytes, must a write CMD, need to check if we  have already finished the receiving
	if(this->rx_ptr>5)
	{
		//long than 16 bytes, an error length
		if(this->rxBuffer[Frame_Length]>0x10)
		{
			this->rx_ptr=0;
//			this->cmd_ACK(STATUS_WRONG_DATA_LEN);
			return 0;
		}
		// we did not receive all the bytes, need another 10 ms
		else if(this->rxBuffer[Frame_Length]>this->rx_ptr-6)
		{
			delay(10);
			while(mySerial.available() > 0)
			{
				this->rxBuffer[this->rx_ptr]=mySerial.read();
				this->rx_ptr++;
			}
			//still not receive all
			if(this->rxBuffer[Frame_Length]>this->rx_ptr-6)
			{
				this->rx_ptr=0;
//				this->cmd_ACK(STATUS_WRONG_DATA_LEN);
				return 0;
			}
		}
	}
	//check the checksum
	checksum=this->calc_chksum(&rxBuffer[Frame_Address], this->rx_ptr-3);
	if(checksum!=rxBuffer[this->rx_ptr-1])
	{
		this->rx_ptr=0;
//		this->cmd_ACK(STATUS_WRONG_CHKSUM);
		return 0;
	}
	switch(this->rxBuffer[Frame_CMD])
	{
	case COMMON_CMD_READ_SYS_TICK:
		this->read_handle(millis()/1000);
		return 0;
//	case COMMON_CMD_READ_STATE_SUB_1:
//		this->read_handle(this->state_sub_1);
//		return 0;
//	case COMMON_CMD_READ_STATE_SUB_2:
//		this->read_handle(this->state_sub_2);
//		return 0;
	case COMMON_CMD_GOTO_BOOTLOADER:
		this->cmd_ACK(STATUS_OK);
		delay(10);
		asm("JMP 15872 ");
		return 0;
	case COMMON_CMD_INIT_ME:
		this->cmd_ACK(STATUS_OK);
		myConfig.context.count=0;
		myConfig.save_config();
		delay(10);
		resetMyself();
		return 0;
//	case COMMON_CMD_CHANGE_ADDR:
//	case COMMON_CMD_RESET_ME:
//		this->cmd_ACK(STATUS_UNSUPPORT_CMD);
//		return 0;
	case COMMON_CMD_ACK:
		if(this->ack_needs.isBuzy)
		{
			this->ack_needs.isBuzy=false;
			this->ack_needs.retry_count=0;
		}
		break;
	default:
		this->txBuffer.bit.err_status=STATUS_OK;
		return 1;
	}
	this->txBuffer.bit.err_status=STATUS_OK;
	return 1;
}
#endif
void LaputaComm::read_handle(Uint8 data)
{
	this->txBuffer.bit.err_status=STATUS_OK;
	this->txBuffer.bit.length=1;
	this->txBuffer.bit.content[0]=data;
	this->sendCMD();
}
void LaputaComm::read_handle(Uint16 data)
{
	this->txBuffer.bit.err_status=STATUS_OK;
	this->txBuffer.bit.length=2;
	this->txBuffer.bit.content[0]=data&0xFF;
	this->txBuffer.bit.content[1]=(data>>8)&0xFF;
	this->sendCMD();
}
void LaputaComm::read_handle(Uint32 data)
{
	this->txBuffer.bit.err_status=STATUS_OK;
	this->txBuffer.bit.length=4;
	this->txBuffer.bit.content[0]=data&0xFF;
	this->txBuffer.bit.content[1]=(data>>8)&0xFF;
	this->txBuffer.bit.content[2]=(data>>16)&0xFF;
	this->txBuffer.bit.content[3]=(data>>24)&0xFF;
	this->sendCMD();
}
void LaputaComm::cmd_ACK(Uint8 status)
{
	this->txBuffer.bit.err_status=status;
	this->txBuffer.bit.length=0;
	this->sendCMD();
}
void LaputaComm::cmd_ACK(Uint8 status,Uint8 Addr)
{
	this->txBuffer.bit.err_status=status;
	this->txBuffer.bit.length=0;
	this->sendCMD(Addr);
}

void LaputaComm::controlDoor(bool status)
{
	this->txBuffer.bit.err_status=0x05;
	this->txBuffer.bit.length=0x01;
	this->txBuffer.bit.content[0]=status;
	this->sendCMD(0x01);
}

void LaputaComm::controlMusicAndLight(uint8_t song, uint8_t light)
{
	this->txBuffer.bit.err_status=0x05;
	this->txBuffer.bit.length=0x03;
	this->txBuffer.bit.content[0]=this->address;
	this->txBuffer.bit.content[1]=song;
	this->txBuffer.bit.content[2]=light;
	this->sendCMD(0x02);
}

LaputaComm::~LaputaComm()
{
// TODO Auto-generated destructor stub
}
