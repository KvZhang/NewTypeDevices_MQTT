/*
 * LaputaJoyStick.h
 *
 *  Created on: 2017Äê8ÔÂ26ÈÕ
 *      Author: zhangyu
 */

#ifndef LAPUTAJOYSTICK_H_
#define LAPUTAJOYSTICK_H_

#include"Arduino.h"

typedef enum
{
	Direction_none,
	Direction_up,
	Direction_down,
	Direction_left,
	Direction_right,
}STICK_DIRECTION;

class LaputaJoyStick
{
public:
	LaputaJoyStick(uint8_t pin_up,uint8_t pin_down,uint8_t pin_left,uint8_t pin_right,
			uint8_t Led_pin_up, uint8_t Led_pin_down, uint8_t Led_pin_left, uint8_t Led_pin_right);
	void init();
	void handler();//need to be put in a 100ms loop
	uint8_t get_direction();
	virtual ~LaputaJoyStick();
private:
	uint8_t flag_with_led;
	uint8_t flag_changed;
	uint8_t detect_pin_up;
	uint8_t detect_pin_down;
	uint8_t detect_pin_left;
	uint8_t detect_pin_right;
	uint8_t led_pin_up;
	uint8_t led_pin_down;
	uint8_t led_pin_left;
	uint8_t led_pin_right;
	uint8_t direction;
};

#endif /* LAPUTAJOYSTICK_H_ */
