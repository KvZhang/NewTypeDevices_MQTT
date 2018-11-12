#include "LED8X32.h"



uint8_t zimo_alphabet[][8]={
		{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*" ",0*/
		{0x08,0x08,0x08,0x08,0x08,0x00,0x00,0x08},/*"!",1*/
		{0x14,0x14,0x28,0x00,0x00,0x00,0x00,0x00},/*""",2*/
		{0x28,0x28,0x7C,0x28,0x28,0x7C,0x28,0x28},/*"#",3*/
		{0x38,0x54,0x50,0x30,0x18,0x14,0x54,0x38},/*"$",4*/
		{0x24,0x54,0x58,0x54,0x3A,0x1A,0x2A,0x24},/*"%",5*/
		{0x10,0x28,0x28,0x36,0x54,0x54,0x4A,0x34},/*"&",6*/
		{0x10,0x10,0x20,0x00,0x00,0x00,0x00,0x00},/*"'",7*/
		{0x00,0x08,0x10,0x10,0x10,0x10,0x10,0x08},/*"(",8*/
		{0x20,0x20,0x10,0x10,0x10,0x10,0x20,0x20},/*")",9*/
		{0x00,0x00,0x10,0x38,0x10,0x28,0x00,0x00},/*"*",10*/
		{0x00,0x00,0x10,0x10,0x7C,0x10,0x10,0x00},/*"+",11*/
		{0x00,0x00,0x00,0x00,0x20,0x40,0x00,0x00},/*",",12*/
		{0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0x00},/*"-",13*/
		{0x00,0x00,0x00,0x00,0x20,0x00,0x00,0x00},/*".",14*/
		{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"/",15*/
		{0x38,0x44,0x44,0x44,0x44,0x44,0x44,0x38},/*"0",16*/
		{0x10,0x30,0x10,0x10,0x10,0x10,0x10,0x38},/*"1",17*/
		{0x38,0x44,0x44,0x08,0x10,0x20,0x40,0x7C},/*"2",18*/
		{0x38,0x44,0x04,0x18,0x04,0x04,0x44,0x38},/*"3",19*/
		{0x08,0x18,0x18,0x28,0x48,0x7C,0x08,0x1C},/*"4",20*/
		{0x7C,0x40,0x40,0x78,0x44,0x04,0x44,0x38},/*"5",21*/
		{0x18,0x24,0x40,0x58,0x64,0x44,0x44,0x38},/*"6",22*/
		{0x3C,0x04,0x08,0x08,0x10,0x10,0x10,0x10},/*"7",23*/
		{0x38,0x44,0x44,0x38,0x44,0x44,0x44,0x38},/*"8",24*/
		{0x38,0x44,0x44,0x4C,0x34,0x04,0x48,0x30},/*"9",25*/
		{0x00,0x00,0x00,0x20,0x00,0x00,0x20,0x00},/*":",26*/
		{0x00,0x00,0x00,0x20,0x00,0x00,0x20,0x40},/*";",27*/
		{0x00,0x00,0x06,0x38,0xC0,0x38,0x06,0x00},/*"<",28*/
		{0x00,0x00,0x00,0x7E,0x00,0x7E,0x00,0x00},/*"=",29*/
		{0x00,0x00,0x60,0x1C,0x03,0x1C,0x60,0x00},/*">",30*/
		{0x00,0x30,0x48,0x08,0x30,0x00,0x20,0x00},/*"?",31*/
		{0x00,0x3C,0x5E,0xA5,0xA5,0xA5,0x7E,0x3C},/*"@",32*/
		{0x10,0x10,0x18,0x28,0x28,0x3C,0x24,0x66},/*"A",33*/
		{0x78,0x24,0x24,0x38,0x24,0x24,0x24,0x78},/*"B",34*/
		{0x3C,0x44,0x40,0x40,0x40,0x40,0x44,0x38},/*"C",35*/
		{0x78,0x24,0x24,0x24,0x24,0x24,0x24,0x78},/*"D",36*/
		{0x7C,0x24,0x28,0x38,0x28,0x20,0x24,0x7C},/*"E",37*/
		{0x7C,0x24,0x28,0x38,0x28,0x20,0x20,0x70},/*"F",38*/
		{0x1C,0x24,0x40,0x40,0x4E,0x44,0x24,0x18},/*"G",39*/
		{0x66,0x24,0x24,0x3C,0x24,0x24,0x24,0x66},/*"H",40*/
		{0x7C,0x10,0x10,0x10,0x10,0x10,0x10,0x7C},/*"I",41*/
		{0x3E,0x08,0x08,0x08,0x08,0x08,0x08,0x08},/*"J",42*/
		{0x76,0x24,0x28,0x30,0x28,0x24,0x24,0x76},/*"K",43*/
		{0x70,0x20,0x20,0x20,0x20,0x20,0x22,0x7E},/*"L",44*/
		{0xEE,0x6C,0x6C,0x6C,0x54,0x54,0x54,0xD6},/*"M",45*/
		{0x6E,0x24,0x34,0x34,0x2C,0x2C,0x24,0x74},/*"N",46*/
		{0x38,0x44,0x44,0x44,0x44,0x44,0x44,0x38},/*"O",47*/
		{0x78,0x24,0x24,0x38,0x20,0x20,0x20,0x70},/*"P",48*/
		{0x38,0x44,0x44,0x44,0x44,0x74,0x4C,0x38},/*"Q",49*/
		{0x78,0x24,0x24,0x38,0x28,0x24,0x24,0x76},/*"R",50*/
		{0x3C,0x44,0x40,0x30,0x08,0x04,0x44,0x78},/*"S",51*/
		{0x7C,0x54,0x10,0x10,0x10,0x10,0x10,0x38},/*"T",52*/
		{0x66,0x24,0x24,0x24,0x24,0x24,0x24,0x18},/*"U",53*/
		{0x66,0x24,0x24,0x28,0x28,0x18,0x10,0x10},/*"V",54*/
		{0x54,0x54,0x54,0x54,0x38,0x28,0x28,0x28},/*"W",55*/
		{0x6C,0x28,0x28,0x10,0x10,0x28,0x28,0x6C},/*"X",56*/
		{0x6C,0x28,0x28,0x28,0x10,0x10,0x10,0x38},/*"Y",57*/
		{0x7C,0x48,0x08,0x10,0x10,0x20,0x24,0x7C},/*"Z",58*/
};

LED8X32::LED8X32(uint8_t cs,uint8_t  wr, uint8_t data)
{
	this->cs_pin=cs;
	this->data_pin=data;
	this->wr_pin=wr;
	this->time_interval=500;
	this->start_moment=0;
	this->ptr=0;
	this->string=NULL;
	this->length=0;
	this->repeat_type=true;
}

void LED8X32::HT1632C_Writer(unsigned char Data,unsigned char cnt)	  
{
	unsigned char i;
	for(i=0;i<cnt;i++)
	{
  		digitalWrite(this->wr_pin, LOW);
		if(Data&0x80)
		digitalWrite(this->data_pin, HIGH);
		else
		digitalWrite(this->data_pin, LOW);
		Data<<=1;
		digitalWrite(this->wr_pin, HIGH);
	}
}
void LED8X32::HT1632C_Writer_CMD(unsigned char cmd)//
{
	digitalWrite(this->cs_pin, LOW);
	LED8X32::HT1632C_Writer(0x80,3);		//
	LED8X32::HT1632C_Writer(cmd,9);		//
	digitalWrite(this->cs_pin, HIGH);
}
void LED8X32::HT1632C_Writer_AllDATA(unsigned char Addr,unsigned char p[],unsigned char cnt)//
{
	unsigned char i;
	digitalWrite(this->cs_pin, LOW);
	LED8X32::HT1632C_Writer(0xa0,3);
	LED8X32::HT1632C_Writer(Addr<<1,7);
#ifdef REVERSE
	p=p+7;
	for(i=0;i<cnt;i++)
		{
		LED8X32::HT1632C_Writer(*p,8);
			p--;
		}
	digitalWrite(this->cs_pin, HIGH);
#else
	for(i=0;i<cnt;i++)
		{
		LED8X32::HT1632C_Writer(*p,8);
			p++;
		}
	digitalWrite(this->cs_pin, HIGH);
#endif
}

void LED8X32::HT1632C_clr(void)
{
	unsigned char i;
	digitalWrite(this->cs_pin, LOW);
	HT1632C_Writer(0xa0,3);
	LED8X32::HT1632C_Writer(0x00,7);
	for(i=0;i<32;i++)
		LED8X32::HT1632C_Writer(0x00,8);
	digitalWrite(this->cs_pin, HIGH);

	this->string=NULL;
	this->length=0;
	this->start_moment=0;
	this->ptr=0;
}
void LED8X32::HT1632C_Init(void)		
{
	pinMode(this->cs_pin, OUTPUT);
	pinMode(this->wr_pin, OUTPUT);
//	pinMode(RD, OUTPUT);
	pinMode(this->data_pin, OUTPUT);
	digitalWrite(this->cs_pin, HIGH);
	digitalWrite(this->wr_pin, HIGH);
//	digitalWrite(RD, HIGH);
	digitalWrite(this->data_pin, HIGH);
	HT1632C_Writer_CMD(SYS_DIS);
	HT1632C_Writer_CMD(COM_OPTION);
	HT1632C_Writer_CMD(RC_MASTER_MODE);
//	HT1632C_Writer_CMD(SLAVE_MODE);
	HT1632C_Writer_CMD(SYS_EN);
	HT1632C_Writer_CMD(LED_ON);
//	HT1632C_Writer_CMD(PWM_DUTY);
	HT1632C_Writer_CMD(BLINK_OFF);
//	HT1632C_Writer_CMD(BLINK_ON);
}

void LED8X32::HT1632C_show_string(uint8_t *string, uint8_t lenth, bool repeat=true, uint16_t ms)
{
	this->string=string;
	this->length=lenth;
	this->repeat_type=repeat;
	this->time_interval=ms;
	this->start_moment=millis();
}

void LED8X32::HT1632C_show_handle()
{
if(this->string!=NULL && this->length)
	{

		if(millis()-this->start_moment>=this->time_interval)
		{
			this->start_moment=millis();
			if(this->ptr+3>=this->length)
			{
				if(this->repeat_type)
				{
					this->ptr=0;
				}
				else
				{
					this->HT1632C_clr();
				}
			}
			else
			{
				this->HT1632C_Writer_AllDATA(0 , zimo_alphabet[*(this->string+this->ptr+0)-' '], 8);
				this->HT1632C_Writer_AllDATA(16, zimo_alphabet[*(this->string+this->ptr+1)-' '], 8);
				this->HT1632C_Writer_AllDATA(32, zimo_alphabet[*(this->string+this->ptr+2)-' '], 8);
				this->HT1632C_Writer_AllDATA(48, zimo_alphabet[*(this->string+this->ptr+3)-' '], 8);
				this->ptr++;

			}

		}
	}

}
LED8X32::~LED8X32()
{

}