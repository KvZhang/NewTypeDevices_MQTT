/*
 * Color.cpp
 *
 *  Created on: 2017Äê7ÔÂ16ÈÕ
 *      Author: zhangyu
 */

#include "Color.h"
#include "Wire.h"

Color::Color()
{
	// TODO Auto-generated constructor stub
	this->color_result.all=0;
	this->flag_end=0;
	for(uint8_t i=0;i<17;i++)
	{
		this->read_buffer.byte[i]=0x0;
	}
	color_table[Color_red].all=41;
	color_table[Color_green].all=3*65536+48*256;
	color_table[Color_blue].all=(40*65536)+(20*256);
	color_table[Color_yellow].all=(60*256)+(115);
	color_table[Color_orange].all=(17*256)+(150);
	color_table[Color_white].all=0;
}
void Color::begin()
{
	Wire.begin();
}
uint32_t Color::read(uint8_t addr)
{
	Wire.beginTransmission(addr);
	Wire.write(Read_RGC_Real);
	Wire.endTransmission(false);
	Wire.requestFrom((unsigned char)addr,(unsigned char)3);
	while(!Wire.available());
	delay(1);
	this->color_result.RGB.R=Wire.read();
	this->color_result.RGB.G=Wire.read();
	this->color_result.RGB.B=Wire.read();
	uint8_t min_value=min(min(this->color_result.RGB.R,this->color_result.RGB.G),this->color_result.RGB.B);
	this->color_result.RGB.R-=min_value;
	this->color_result.RGB.G-=min_value;
	this->color_result.RGB.B-=min_value;

	uint16_t color_err_sq;
	uint16_t color_err_sq_min=0xFFFF;
	for(uint8_t i=Color_red;i<=Color_white;i++)
	{
		color_err_sq=sq(abs((int16_t)this->color_result.RGB.R-(int16_t)this->color_table[i].RGB.R))
				+sq(abs((int16_t)this->color_result.RGB.G-(int16_t)this->color_table[i].RGB.G))
				+sq(abs((int16_t)this->color_result.RGB.B-(int16_t)this->color_table[i].RGB.B));
		if(color_err_sq<color_err_sq_min)
		{
			color_err_sq_min=color_err_sq;
			this->color_result.RGB.L=i;
		}
	}
	return this->color_result.all;
}
void Color::read_all(uint8_t addr)
{

	Wire.beginTransmission(addr);
	Wire.write(Read_RGC_Real);
	Wire.endTransmission(false);
	Wire.requestFrom((unsigned char)addr,(unsigned char)4);
	while(!Wire.available());
	delay(10);
	for(uint8_t i=12;i<16;i++)
	{
		this->read_buffer.byte[i]=Wire.read();
	}
}
bool Color::is_desired_color(uint32_t desired_color)
{
	COLOR_STRUCT color;
	color.all=desired_color;
	return (abs(color.RGB.R-this->color_result.RGB.R)<=5 && abs(color.RGB.G-this->color_result.RGB.G)<=5
			&& abs(color.RGB.B-this->color_result.RGB.B)<=5);
}

void Color::calibration(uint8_t addr,uint8_t light, bool is_need_cali)
{
	Wire.beginTransmission(addr);
	Wire.write(reg_Config);
	Wire.write((light<<4)+is_need_cali);
	Wire.endTransmission(true);
}
Color::~Color()
{
	// TODO Auto-generated destructor stub
}

