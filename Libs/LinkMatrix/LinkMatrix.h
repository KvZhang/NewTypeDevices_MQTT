/*
 * LinkMatrix.h
 *
 *  Created on: 2017Äê12ÔÂ27ÈÕ
 *      Author: KenZhang
 */

#ifndef LINKMATRIX_H_
#define LINKMATRIX_H_

#include "Arduino.h"
#define MATRIX_SIZE (5)//Max 5*5 key matrix

class LinkMatrix
{
public:
	LinkMatrix(const uint8_t* linePtr,const uint8_t* columnPtr,uint8_t lineNum,uint8_t columnNum);
	void begin();
	uint16_t getPair(void);//Hi-byte is the little number and Lo-byte is the big number, read then clear
	uint16_t readPair();//Hi-byte is the little number and Lo-byte is the big number, read then clear
	void handler();
	virtual ~LinkMatrix();
private:
	uint8_t lineNumber;
	uint8_t columnNumber;
	uint8_t linePin[MATRIX_SIZE];
	uint8_t columnPin[MATRIX_SIZE];
	uint16_t answer;
};

#endif /* LINKMATRIX_H_ */
