/*
 * LinkMatrix.cpp
 *
 *  Created on: 2017Äê12ÔÂ27ÈÕ
 *      Author: KenZhang
 */

#include "LinkMatrix.h"

LinkMatrix::LinkMatrix(const uint8_t* linePtr,const uint8_t* columnPtr,uint8_t lineNum,uint8_t columnNum)
{
	// TODO Auto-generated constructor stub
	this->columnNumber=columnNum;
	this->lineNumber=lineNum;
	for(uint8_t i=0;i<lineNum;i++)
	{
		this->linePin[i]=*(linePtr+i);
	}
	for(uint8_t i=0;i<columnNum;i++)
	{
		this->columnPin[i]=*(columnPtr+i);
	}
	this->answer=0;
}
void LinkMatrix::begin()
{
	for(uint8_t i=0;i<this->columnNumber;i++)
	{
		pinMode(this->columnPin[i],INPUT_PULLUP);
	}
	for(uint8_t i=0;i<this->lineNumber;i++)
	{
		pinMode(this->linePin[i],INPUT_PULLUP);
	}
}
void LinkMatrix::handler()
{
	static uint16_t lastValue=0;
	uint16_t temp=this->readPair();
	if(temp==lastValue)
	{
		this->answer=temp;
	}
	else
	{
		this->answer=0;
		lastValue=temp;
	}
}
uint16_t LinkMatrix::getPair(void)
{
	return this->answer;
}
uint16_t LinkMatrix::readPair()
{
	uint8_t hi_byte=0;
	uint8_t lo_byte=0;
	for(uint8_t i=0;i<this->lineNumber;i++)
	{
		pinMode(this->linePin[i],OUTPUT);
		digitalWrite(this->linePin[i],0);
		for(uint8_t j=0;j<this->columnNumber;j++)
		{
			if(digitalRead(this->columnPin[j])==0)
			{
				if(hi_byte==0)
				{
					hi_byte=i*this->columnNumber+j+1;
				}
				else
				{
					lo_byte=i*this->columnNumber+j+1;
					break;
				}
			}
		}
		pinMode(this->linePin[i],INPUT_PULLUP);
	}
	return (hi_byte<<8)+lo_byte;
}
LinkMatrix::~LinkMatrix()
{
	// TODO Auto-generated destructor stub
}

