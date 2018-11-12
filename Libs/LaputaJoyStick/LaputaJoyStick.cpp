/*
 * LaputaJoyStick.cpp
 *
 *  Created on: 2017Äê8ÔÂ26ÈÕ
 *      Author: zhangyu
 */

#include "LaputaJoyStick.h"

LaputaJoyStick::LaputaJoyStick(uint8_t pin_up, uint8_t pin_down, uint8_t pin_left, uint8_t pin_right,
		uint8_t Led_pin_up, uint8_t Led_pin_down, uint8_t Led_pin_left, uint8_t Led_pin_right)
{
	// TODO Auto-generated constructor stub

	this->detect_pin_up=pin_up;

	this->detect_pin_down=pin_down;

	this->detect_pin_left=pin_left;

	this->detect_pin_right=pin_right;


//	if(led_pin_up==0)
//	{
//		this->flag_with_led=0;
//	}
//	else
//	{
		this->flag_with_led=1;
		this->led_pin_up=Led_pin_up;

		this->led_pin_down=Led_pin_down;

		this->led_pin_left=Led_pin_left;

		this->led_pin_right=Led_pin_right;

//	}
	this->flag_changed=0;
	this->direction=Direction_none;
}

void LaputaJoyStick::init()
{
	pinMode(this->detect_pin_up,INPUT_PULLUP);
	pinMode(this->detect_pin_down,INPUT_PULLUP);
	pinMode(this->detect_pin_left,INPUT_PULLUP);
	pinMode(this->detect_pin_right,INPUT_PULLUP);
	if(this->flag_with_led)
	{
		pinMode(this->led_pin_up,OUTPUT);
		digitalWrite(this->led_pin_up,0);
		pinMode(this->led_pin_down,OUTPUT);
		digitalWrite(this->led_pin_down,0);
		pinMode(this->led_pin_left,OUTPUT);
		digitalWrite(this->led_pin_left,0);
		pinMode(this->led_pin_right,OUTPUT);
		digitalWrite(this->led_pin_right,0);
	}
}

void LaputaJoyStick::handler()
{
	uint8_t temp=digitalRead(this->detect_pin_up) + 2*digitalRead(this->detect_pin_down)
					+ 4*digitalRead(this->detect_pin_left) + 8*digitalRead(this->detect_pin_right);

	if(temp == 0x0f)
	{
		this->flag_changed=0;
		this->direction=Direction_none;
		digitalWrite(this->led_pin_up,0);
		digitalWrite(this->led_pin_down,0);
		digitalWrite(this->led_pin_left,0);
		digitalWrite(this->led_pin_right,0);
	}
	else
	{
		if(this->direction!=Direction_none)//have detect changed
		{
			this->flag_changed=0;
			digitalWrite(this->led_pin_up,0);
			digitalWrite(this->led_pin_down,0);
			digitalWrite(this->led_pin_left,0);
			digitalWrite(this->led_pin_right,0);
		}
		else
		{
			this->flag_changed=1;
			switch(temp)
			{
			case 0b1110://up
				this->direction=Direction_up;
				digitalWrite(this->led_pin_up,1);
				break;
			case 0b1101://down
				this->direction=Direction_down;
				digitalWrite(this->led_pin_down,1);
				break;
			case 0b1011://left
				this->direction=Direction_left;
				digitalWrite(this->led_pin_left,1);
				break;
			case 0b111://right
				this->direction=Direction_right;
				digitalWrite(this->led_pin_right,1);
				break;
			default:
				break;
			}
		}
	}
}

uint8_t LaputaJoyStick::get_direction()
{
	if(this->flag_changed)
	{
		this->flag_changed=0;
		return this->direction;
	}
	else
	{
		return Direction_none;
	}
}

LaputaJoyStick::~LaputaJoyStick()
{
	// TODO Auto-generated destructor stub
}

