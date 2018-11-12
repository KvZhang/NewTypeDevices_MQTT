/*
 * LaputaMp3.cpp
 *
 *  Created on: 2015Äê4ÔÂ14ÈÕ
 *      Author: ZhangYu
 */

#include "LaputaMp3.h"

LaputaMp3::LaputaMp3(uint8_t pin)
{
	// TODO Auto-generated constructor stub
	this->txBuffer[0] = 0x7e;
	this->txBuffer[1] = 0xff;
	this->txBuffer[2] = 0x06;
	this->txBuffer[3] = 0x00;
	this->txBuffer[4] = 0x00;
	this->txBuffer[5] = 0x00;
	this->txBuffer[6] = 0x00;
	this->txBuffer[7] = 0x00;
	this->txBuffer[8] = 0x00;
	this->txBuffer[9] = 0xef;
	this->busyPin=pin;
	pinMode(this->busyPin,INPUT_PULLUP);
	this->last_send_moment=0;
}

LaputaMp3::~LaputaMp3()
{
	// TODO Auto-generated destructor stub
}
void LaputaMp3::write(uint8_t cmd, uint8_t dataL, uint8_t dataH)
{
	unsigned int checkSum = 0;
	while(millis()-this->last_send_moment<20);
	this->txBuffer[3] = cmd;
	this->txBuffer[5] = dataH;
	this->txBuffer[6] = dataL;
	for (unsigned i = 1; i <= 6; i++)
	{
		checkSum += this->txBuffer[i];
	}
	checkSum = ~checkSum + 1;
	this->txBuffer[7] = checkSum / 256;
	this->txBuffer[8] = checkSum % 256;
	Serial.write(this->txBuffer, 10);
	this->last_send_moment=millis();
}
void LaputaMp3::init(uint8_t vol)
{
	while(millis()<500);
	this->write(SET_VOLUMN, vol);
}
bool LaputaMp3::read_status()
{
	return !digitalRead(this->busyPin);
}
