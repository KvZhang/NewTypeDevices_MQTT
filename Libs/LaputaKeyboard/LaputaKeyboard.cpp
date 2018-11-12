/*
 * LaputaKeyboard.cpp
 *
 *  Created on: 2017Äê9ÔÂ17ÈÕ
 *      Author: zhangyu
 */

#include "LaputaKeyboard.h"

STATUS_KEY_DEBOUNCE LaputaKeyboard::check_debounce(uint16_t currentKeyValue)
{
	static uint32_t debounce_count=0;
	static uint16_t temp_key=0;//last key value
	STATUS_KEY_DEBOUNCE status;
	if(debounce_count==0)
	{
		if(temp_key!=currentKeyValue)
		{
			debounce_count=millis();
			temp_key=currentKeyValue;
		}
		status = Status_init;
	}
	else if(millis()<debounce_count+this->debounceTime)//debounce 100ms
	{
		if(temp_key!=currentKeyValue)
		{
			temp_key=0;
			debounce_count=0;
			status = Status_error;
		}
		else
			status = Status_counting;
	}
	else
	{
		debounce_count=0;
		temp_key=0;
		status = Status_OK;
	}
	return status;
}

uint8_t LaputaKeyboard::convert_to_real_num(uint16_t currentKeyValue)
{
	uint8_t i=0;
	if(currentKeyValue==0) return 0;
	while((currentKeyValue & (1UL<<i))==0)
	{
		i++;
	}
	return i+1;
}

void LaputaKeyboard::get_unlatched_H_key(uint16_t currentKeyValue)
{


	if(this->lastPushMoment && (millis() > (this->lastPushMoment + this->overTimeLength)))
	{
		if(currentKeyValue != 0)
		{
			this->flag.bit.is_long_pushed=1;
		}
		else
		{
			this->flag.bit.is_overtime=1;
		}
		this->lastPushMoment = 0;
	}

	if(currentKeyValue != 0)
	{
		if(currentKeyValue != this->last_key_value)
		{
			this->buffer[this->ptr++]=this->convert_to_real_num(currentKeyValue);
			this->lastPushMoment = millis();
			this->last_key_value = currentKeyValue;
			if(this->ptr >= MAX_KEY_BUFFER_SIZE)
			{
				this->ptr = 0;
			}
		}
	}
	else
	{
		this->last_key_value = 0;
	}
}

void LaputaKeyboard::get_latched_H_key(uint16_t currentKeyValue)
{
	if(currentKeyValue)
	{
		if(millis()>this->lastPushMoment+this->overTimeLength)
		{
			this->flag.bit.is_overtime=1;
		}
	}
	else
	{
		this->flush();
		return;
	}

	if(currentKeyValue!=this->last_key_value)
	{
		this->buffer[this->ptr++]=currentKeyValue;
		this->lastPushMoment=millis();
		this->last_key_value=currentKeyValue;
		if(this->ptr>=MAX_KEY_BUFFER_SIZE)
		{
			this->ptr=0;
		}
	}
}
void LaputaKeyboard::flush()
{
	this->ptr=0;
	this->read_ptr=0;
	this->flag.all=0;
	this->lastPushMoment=0;
}
LaputaKeyboard::LaputaKeyboard(uint8_t inputPin[], uint8_t num, uint8_t type, uint16_t ms) {
	// TODO Auto-generated constructor stub
	this->read_ptr=0;
	this->ptr=0;
	this->inputPin=inputPin;
	this->pin_num=num;
	this->type=type;
	this->flag.all=0;
	this->overTimeLength=ms;
	this->lastPushMoment=0;
	this->last_key_value=0;
	this->keyValueMask=0;
	this->debounceTime = 100;
}

void LaputaKeyboard::begin(bool pullup_need)
{
	if(pullup_need)
	{
		this->debounceTime=100;
		for(uint8_t i=0;i<this->pin_num;i++)
		{
			pinMode(*(this->inputPin+i),INPUT_PULLUP);
			this->keyValueMask+=1<<i;
		}
	}
	else
	{
		this->debounceTime=50;
		for(uint8_t i=0;i<this->pin_num;i++)
		{
			pinMode(*(this->inputPin+i),INPUT);
			this->keyValueMask+=1<<i;
		}
	}
}
uint16_t LaputaKeyboard::get_current_key()
{
	uint16_t temp=0;
	for(uint8_t i=0;i<this->pin_num;i++)
	{
		temp+=digitalRead(*(this->inputPin+i))<<i;
	}
	return temp;
}
uint8_t LaputaKeyboard::available()
{
	return ((uint16_t)(MAX_KEY_BUFFER_SIZE+this->ptr-this->read_ptr)%MAX_KEY_BUFFER_SIZE);
}
uint16_t LaputaKeyboard::read()
{
	if(this->read_ptr == this->ptr)
	{
		return 0;
	}
	else
	{
		uint8_t temp=this->buffer[this->read_ptr];
		this->read_ptr=(this->read_ptr + 1) % MAX_KEY_BUFFER_SIZE;
		return temp;
	}
}
uint16_t LaputaKeyboard::peek()
{
	if(this->read_ptr != this->ptr)
	{
		return this->buffer[(this->ptr+MAX_KEY_BUFFER_SIZE-1) % MAX_KEY_BUFFER_SIZE];
	}
	else
	{
		return 0;
	}
}
void LaputaKeyboard::handler()
{
	uint16_t tempKey=this->get_current_key();
	if(this->check_debounce(tempKey)==Status_OK)
	{
		switch(this->type)
		{
		case KEY_type_latch_H:
			this->get_latched_H_key(tempKey);
			break;
		case KEY_type_unlatch_H:
			this->get_unlatched_H_key(tempKey);
			break;
		case KEY_type_unlatch_L:
			this->get_unlatched_H_key((~(tempKey))&this->keyValueMask);
			break;
		default:
			get_latched_H_key(tempKey);
			break;
		}
	}
//	else
//	{
//		if(this->lastPushMoment && (millis() > (this->lastPushMoment + this->overTimeLength)))
//		{
//			this->flag.bit.is_overtime=1;
//			this->lastPushMoment = 0;
//		}
//	}
}

LaputaKeyboard::~LaputaKeyboard() {
	// TODO Auto-generated destructor stub
}

