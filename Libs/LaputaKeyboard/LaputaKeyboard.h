/*
 * LaputaKeyboard.h
 *
 *  Created on: 2017Äê9ÔÂ17ÈÕ
 *      Author: zhangyu
 */

#ifndef LAPUTAKEYBOARD_H_
#define LAPUTAKEYBOARD_H_

#include "Arduino.h"

typedef enum
{
	KEY_type_unlatch_H,//high is active
	KEY_type_unlatch_L,//low is active
	KEY_type_latch_H,
	KEY_type_latch_L,
}KEY_TYPE;

typedef enum
{
	Status_OK,
	Status_init,
	Status_counting,
	Status_error,
}STATUS_KEY_DEBOUNCE;

typedef union{
	unsigned char all;
	struct{
		unsigned char is_inputting:1;
		unsigned char is_overtime:1;
		unsigned char is_long_pushed:1;
		unsigned char bit3:1;
		unsigned char bit4:1;
		unsigned char bit5:1;
		unsigned char bit6:1;
		unsigned char bit7:1;
	}bit;
}KEY_FLAG;

#define MAX_KEY_BUFFER_SIZE	(20)

class LaputaKeyboard {
public:
	KEY_FLAG flag;
	uint16_t buffer[MAX_KEY_BUFFER_SIZE];
	LaputaKeyboard(uint8_t inputPin[], uint8_t num, uint8_t type=(uint8_t)KEY_type_unlatch_H, uint16_t ms=3000);
	void begin(bool pullup_need=true);
	uint16_t get_current_key();//get direct value,b0~b7
	uint8_t available();
	uint16_t read();
	uint16_t peek();
	void flush();
	void handler();
	virtual ~LaputaKeyboard();
private:
	uint8_t ptr;
	uint8_t read_ptr;
	uint8_t type;
	uint8_t* inputPin;
	uint8_t pin_num;
	uint32_t lastPushMoment;//ms
	uint16_t overTimeLength;//ms
	uint16_t last_key_value;
	uint16_t keyValueMask;
	uint8_t debounceTime;
	STATUS_KEY_DEBOUNCE check_debounce(uint16_t currentKeyValue);
	void get_latched_H_key(uint16_t currentKeyValue);
	void get_unlatched_H_key(uint16_t currentKeyValue);
	uint8_t convert_to_real_num(uint16_t currentKeyValue);
};

#endif /* LAPUTAKEYBOARD_H_ */
