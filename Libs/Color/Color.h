/*
 * Color.h
 *
 *  Created on: 2017Äê7ÔÂ16ÈÕ
 *      Author: zhangyu
 */

#ifndef COLOR_H_
#define COLOR_H_
#include "Arduino.h"


typedef enum
{
	Light_level_normal=7,
	Light_level_off	=10,
};
typedef enum
{
	Color_red,
	Color_green,
	Color_blue,
	Color_yellow,
	Color_orange,
	Color_white,
}DESIRE_COLOR;

typedef enum
{
	Read_RGBC_AD,//read back 8 bytes,L->H: R_H,R_L,G_H,G_L,B_H,B_L,C_H,C_L
	Read_Lux=0x08,//read back 4 bytes
	Read_RGC_Real=0x0C,//read RGB 3 bytes
	reg_Config=0x10,
}COLOR_REG;

typedef union
{
	uint32_t all;
	struct
	{
		uint8_t R;
		uint8_t G;
		uint8_t B;
		uint8_t L;
	}RGB;
}COLOR_STRUCT;



typedef union
{
	uint8_t byte[17];
	struct
	{
		uint8_t raw_R_H;
		uint8_t raw_R_L;
		uint8_t raw_G_H;
		uint8_t raw_G_L;
		uint8_t raw_B_H;
		uint8_t raw_B_L;
		uint8_t raw_clear_H;
		uint8_t raw_clear_L;
		uint8_t Lux_H;
		uint8_t Lux_L;
		uint8_t CT_H;
		uint8_t CT_L;
		uint8_t R;
		uint8_t G;
		uint8_t B;
		uint8_t color;
		uint8_t config;
	}bit;
}ALL_STRUCT;

class Color
{
public:
	void begin();
	uint32_t read(uint8_t addr=0x5A);
	ALL_STRUCT read_buffer;
	void read_all(uint8_t addr);
	bool is_convert_finished();
	bool is_desired_color(uint32_t desired_color=0x00);
	void calibration(uint8_t addr,uint8_t light=3, bool is_need_cali=true);
	Color();
	virtual ~Color();
private:
	COLOR_STRUCT color_table[6];
	COLOR_STRUCT color_result;
	uint8_t flag_end;
};

#endif /* COLOR_H_ */
