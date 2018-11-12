/*
 * LaputaComm.h
 *
 *  Created on: 2015年3月17日
 *      Author: ZhangYu
 *      Need SoftwareSerial.h
 *      D2---->Rx
 *      D3---->Tx
 */

#ifndef LAPUTACOMMSW_H_
#define LAPUTACOMMSW_H_

#define SW_SERIAL

#include "Arduino.h"
//#include "EEPROM.h"
//#include <avr/wdt.h>
#include"SoftwareSerial.h"

#define DEBUG
#define _USE_YEILD (1)
extern SoftwareSerial mySerial;
//extern HardwareSerial Serial;

typedef unsigned char 		Uint8;
typedef char 				Int8;
typedef unsigned int 		Uint16;
typedef int 				Int16;
typedef unsigned long		Uint32;
typedef long 				Int32;
typedef unsigned long long 	Uint64;
typedef long long 			Int64;
typedef void (*_Pfn)(void);
#define BUFFER_SIZE			(32)
//typedef enum
//{
//	STATE_IDLE,				//设备空闲
//	STATE_OP_WITHOUT_CMD,	//设备正在被操作，无需与其他设备联动
//	STATE_OP_WITH_CMD,		//设备正在被操作，需要与其他设备联动
//	STATE_DONE,				//玩家操作成功，需激活下一个设备
//	STATE_LATCH,			//玩家操作失误，设备锁定，需其他触发条件解锁
//};
typedef enum
{
	STATUS_OK,
	STATUS_WRONG_CHKSUM,
	STATUS_UNSUPPORT_CMD,
	STATUS_WRONG_DATA_LEN,
}ACK_STATUS;
// supported CMD
typedef enum
{
	/*0x00*/
	COMMON_CMD_READ_STATE,		//读取从机当前状态
	COMMON_CMD_READ_STATE_SUB_2,
	/*0xF0*/
	COMMON_CMD_GOTO_BOOTLOADER=0xF0,//jump to boot loader
	/*0xF1*/
	COMMON_CMD_INIT_ME,		//Back to initial state
	/*0xF2*/
	COMMON_CMD_RESET_ME,//call a watch dog reset, a really reset.
	/*0xF3*/
	COMMON_CMD_CHANGE_ADDR,	//更改地址，包括主机均有效
	/*0xF4*/
	COMMON_CMD_CHANGE_CHANNEL,
	/*0xF5*/
	COMMON_CMD_ACK,//0xF5
	COMMON_CMD_CHANGE_PRE_ADDR,//0xF6
	COMMON_CMD_CHANGE_NEXT_ADDR,//0xF7
	COMMON_CMD_READ_SYS_TICK,//0xF8
	COMMON_CMD_READ_SETTING,//0xF9
	COMMON_CMD_SET_USER_DEFINE,//0xFA
	COMMON_CMD_RW_USER_DATA,//0xFB,EEPROM
}_CMD_LIST;

//typedef union
//{
//	Uint8 all;
//	struct
//	{
//		unsigned char isReceiving :1;
//		unsigned char isNeedProcess :1;
//		unsigned char isWaitingForACK :1;
//		unsigned char isStopAutoReporting :1;
//		unsigned char chksumWrong :1;
//		unsigned char getMassData :1;
//		unsigned char withoutChksum :1;
//		unsigned char rsvd7 :1;
//	} bit;
//} FLAG_COMM;
typedef enum
{
	Frame_Header1,
	Frame_Header2,
	Frame_Address,
	Frame_CMD,
	Frame_Length,
	Frame_data0,
	Frame_data1,
	Frame_data2,
	Frame_data3,
	Frame_data4,
	Frame_data5,
	Frame_data6,
	Frame_data7,
	Frame_data8,
	Frame_data9,
	Frame_data10,
}FRAME_STRUCT;

typedef enum
{
	Receive_header1,
	Receive_header2,
	Receive_addr,
	Receive_cmd,
	Receive_length,
	Receive_data,
	Receive_checksum,
}RECEIVE_STATE;
typedef union
{
	Uint8 bytes[BUFFER_SIZE];
	struct
	{
		Uint8 header1;
		Uint8 header2;
		Uint8 address;
		Uint8 err_status;
		Uint8 length;
		Uint8 content[BUFFER_SIZE-5];
	}bit;
}STRUCT_TX;
typedef union
{
	Uint8 bytes[BUFFER_SIZE];
	struct
	{
		Uint8 header1;
		Uint8 header2;
		Uint8 address;
		Uint8 cmd;
		Uint8 length;
		Uint8 content[BUFFER_SIZE-5];
	}bit;
}STRUCT_RX;

typedef struct
{
	bool isBuzy;
	uint32_t sendMoment;
	uint16_t interval;
	uint8_t retry_count;
	uint8_t target_addr;
}STRUCT_ACK_NEEDS;

class LaputaComm
{
public:
	Uint8 address;
	Uint8 state;
	Uint8 state_sub_1;
	Uint8 state_sub_2;
	Uint8 rx_ptr;
	LaputaComm(Uint8 addr);
	Uint8 begin(Uint8 room_addr,Int32 BDR,Uint8 setPin=4);
	Uint8 begin(Int32 BDR);
	Uint8 rxBuffer[BUFFER_SIZE];
	STRUCT_TX txBuffer;
	//发送相关的命令
	void sendCMD();
	void sendCMD(Uint8 target_addr,bool ack_needs=false);
	//接收相关的命令
	bool receiveCMD();	//当串口缓冲区有未处理的数据时，调用此函数
	void cmd_ACK(Uint8 status);
	void cmd_ACK(Uint8 status,Uint8 Addr);
	void controlDoor(bool status=true);
	void controlMusicAndLight(uint8_t song=0xff,uint8_t light=0xff);
	bool sendAvailable();
//	bool isNeedProcessing(void);	//是否有待处理的命令？
	virtual ~LaputaComm();
private:
#ifdef _USE_YEILD
	uint8_t rev_state;
#endif
	void sendRepeat();
	volatile STRUCT_ACK_NEEDS ack_needs;
	Uint8 calc_chksum(Uint8 *ptr,Uint8 length);
	void read_handle(Uint8 data);
	void read_handle(Uint16 data);
	void read_handle(Uint32 data);
	Uint8 setPin;
	bool try_BDR(Int32 BDR,Int32 BDR_des, Uint8 ch);
//	bool cmdHandler();	//返回1时，说明需要操作
	volatile uint32_t lastReceiveMoment;	//记录接收时刻
//	FLAG_COMM flag;
};
void resetMyself();

#endif /* LAPUTACOMM_H_ */
