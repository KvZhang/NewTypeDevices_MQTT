/*
 * LaputaMp3.h
 *
 *  Created on: 2015Äê4ÔÂ14ÈÕ
 *      Author: ZhangYu
 */

#ifndef LAPUTAMP3_H_
#define LAPUTAMP3_H_
#include"Arduino.h"

typedef enum
{
	PLAY_ONCE=0x03,
	SET_VOLUMN=0x06,
	PLAY_REPEAT=0x08,
	STOP_PLAY=0x0E,
}MP3_CMD;

typedef enum
{
	MP3_cmd_play_once=0x03,
	MP3_cmd_set_volumn=0x06,
	MP3_cmd_play_repeat=0x08,
	MP3_cmd_reset=0x0C,
	MP3_cmd_play_pause=0x0E,
	MP3_cmd_play_stop=0x16,
	MP3_cmd_play_forder_song=0x0F,
	MP3_cmd_play_insert_with_forder=0x25,
}MP3_CMD_EX;

typedef enum
{
	MP3_status_idle,
	MP3_status_busy,
}MP3_STATUS;

class LaputaMp3
{
public:
	LaputaMp3(uint8_t pin=7);
	void write(uint8_t cmd,uint8_t dataL,uint8_t dataH=0);
	void init(uint8_t vol=25);
	bool read_status();
	virtual ~LaputaMp3();
private:
	uint8_t busyPin;
	uint32_t last_send_moment;
	unsigned char txBuffer[10];
};

#endif /* LAPUTAMP3_H_ */
