/*
 * LaputaLEDController.cpp
 *
 *  Created on: 2017Äê4ÔÂ3ÈÕ
 *      Author: zhangyu
 */

#include "LaputaLEDController.h"
Adafruit_NeoPixel strip(5,10);
LaputaLEDController::LaputaLEDController(uint16_t n,uint16_t p, neoPixelType t,uint8_t seg_n)
//NEO_GRB for 5V; NEO_RBG for 12V
{
	// TODO Auto-generated constructor stub
	this->dataPin=p;
	this->num=n;
	this->type=t;
	this->seg_length=seg_n;
}
bool LaputaLEDController::begin()
{
	strip.updateType(this->type);
	strip.updateLength(this->num);
	strip.setPin(this->dataPin);
	strip.begin();
	strip.clear();
	strip.show();
	if(strip.numPixels())
		return 0;
	else
		return 1;
}
void LaputaLEDController::show_pure_color(uint8_t c,uint8_t brightness)
{
	if(c>=Strip_color_total)
	{
		return;
	}
	for(uint16_t i=0; i<strip.numPixels(); i++)
	{
		strip.setPixelColor(i, color[c]);
	}
//	strip.setBrightness(brightness);
	strip.show();
}
void LaputaLEDController::set_single_pixel_color(uint8_t n,uint8_t c)
{
	strip.setPixelColor(n, color[c]);
}
void LaputaLEDController::set_single_pixel_color(uint8_t n,uint32_t c)
{
	strip.setPixelColor(n, c);
}
void LaputaLEDController::set_segment_color(uint8_t seg,uint8_t c)
{
	//segment from 1 to n
	for(uint8_t i=0;i<this->seg_length;i++)
	{
		strip.setPixelColor((seg-1)*this->seg_length+i, color[c]);
	}
}
void LaputaLEDController::show()
{
	strip.show();
}
LaputaLEDController::~LaputaLEDController()
{
	// TODO Auto-generated destructor stub
}

