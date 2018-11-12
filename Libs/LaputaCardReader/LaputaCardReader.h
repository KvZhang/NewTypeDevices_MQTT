/*
 * LaputaCardReader.h
 *
 *  Created on: 2017年3月11日
 *      Author: zhangyu
 */

#ifndef LAPUTACARDREADER_H_
#define LAPUTACARDREADER_H_

#include"MFRC522.h"

typedef enum
{
	Status_OK,
	Status_no_reader,
	Status_no_card,
	Status_UID_fail,
	Status_Authenticate_error,
	Status_read_write_fail,
	Status_card_not_move,
}Status_read_error;

typedef enum{
	READ_BLOCK=5,
	WRITE_BLOCK=6,
} _RC522_BLOCK;
//RC522 read buffer
typedef union{
	struct{
		unsigned char cardNumber;//0~255
		unsigned char cardType;
		unsigned char cardColour;
		unsigned char cardCmd;
		unsigned char totleLifes;//本卡可刷卡次数
		unsigned char activeTimes;//激活时长等级
		unsigned char room_be_finished[10];
		unsigned char rsv[4];
	}fields;
	unsigned char BYTES[20];
}_RC522_READ_BUFFER;
//RC522 write buffer
typedef union{
	struct{
		unsigned char remainLifes;
		unsigned int  gameStartMoment;
		unsigned char rsv[17];
	}fields;
	unsigned char BYTES[20];
}_RC522_WRITE_BUFFER;
//Card type
typedef enum{
	CARD_TYPE_ADMIN,
	CARD_TYPE_ROOM_1,
	CARD_TYPE_ROOM_2,
	CARD_TYPE_ROOM_3,
	CARD_TYPE_ROOM_4,
	CARD_TYPE_ROOM_5,
	CARD_TYPE_ROOM_6,
	CARD_TYPE_ROOM_7,
	CARD_TYPE_ROOM_8,
	CARD_TYPE_ROOM_9,
	CARD_TYPE_ROOM_10,
	CARD_TYPE_TOOLS,
	CARD_TYPE_PLAYER=0xF0,
}_CARD_TYPE;
//Card color
typedef enum{
	CARD_COLOR_RED,
	CARD_COLOR_YELLOW,
	CARD_COLOR_BLUE,
	CARD_COLOR_GREEN,
	CARD_COLOR_WHITE,
	CARD_COLOR_BLACK,
}_CARD_COLOUR;
//Card command
typedef enum{
	CARD_CMD_RESET_ME,
	CARD_CMD_RESET_ALL,
	CARD_CMD_OPEN_DOOR,
}_CARD_CMD;
typedef union
{
	unsigned char all;
	struct
	{
		unsigned char is_new_card_found:1;
		unsigned char card_not_removed:1;
		unsigned char cmd_found:1;
		unsigned char rsv:5;
	}bit;
}RC522_FLAG;
class LaputaCardReader {
public:
	uint8_t repeat_times;
	LaputaCardReader();
	MFRC522 *reader;
	_RC522_READ_BUFFER content;
	volatile RC522_FLAG flag;
	unsigned char readCard();
	unsigned char writeCard();
	virtual ~LaputaCardReader();

private:
	MFRC522::Uid last_uid;
	MFRC522::MIFARE_Key key;
	unsigned long last_occurred_moment;
	bool check_uid();
};
#endif /* LAPUTACARDREADER_H_ */
