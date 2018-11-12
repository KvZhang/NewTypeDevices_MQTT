/*
 * LaputaComm.h
 *
 *  Created on: 2015年3月17日
 *      Author: ZhangYu
 */

#ifndef LAPUTACOMMHW_H_
#define LAPUTACOMMHW_H_

#define SW_SERIAL

#include "Arduino.h"

#define DEBUG
#define _USE_YEILD (1)
//extern HardwareSerial Serial;

typedef void (*_Pfn)(void);
#define BUFFER_SIZE			(32)

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
	uint8_t bytes[BUFFER_SIZE];
	struct
	{
		uint8_t header1;
		uint8_t header2;
		uint8_t address;
		uint8_t err_status;
		uint8_t length;
		uint8_t content[BUFFER_SIZE-5];
	}bit;
}STRUCT_TX;
typedef union
{
	uint8_t bytes[BUFFER_SIZE];
	struct
	{
		uint8_t header1;
		uint8_t header2;
		uint8_t address;
		uint8_t cmd;
		uint8_t length;
		uint8_t content[BUFFER_SIZE-5];
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
	uint8_t address;
	uint8_t state;
	uint8_t rx_ptr;
	LaputaComm(uint8_t addr);
	uint8_t begin(uint8_t room_addr,int32_t BDR,uint8_t setPin=4);
	uint8_t begin(int32_t BDR);
	uint8_t rxBuffer[BUFFER_SIZE];
	STRUCT_TX txBuffer;
	//发送相关的命令
	void sendCMD();
	void sendCMD(uint8_t target_addr,bool ack_needs=false);
	//接收相关的命令
	bool receiveCMD();	//当串口缓冲区有未处理的数据时，调用此函数
	void cmd_ACK(uint8_t status);
	void cmd_ACK(uint8_t status,uint8_t Addr);
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
	uint8_t calc_chksum(uint8_t *ptr,uint8_t length);
	void read_handle(uint8_t data);
	void read_handle(uint16_t data);
	void read_handle(uint32_t data);
	uint8_t setPin;
	bool try_BDR(int32_t BDR,int32_t BDR_des, uint8_t ch);
	volatile uint32_t lastReceiveMoment;	//记录接收时刻
};
void resetMyself();

#endif /* LAPUTACOMM_H_ */
