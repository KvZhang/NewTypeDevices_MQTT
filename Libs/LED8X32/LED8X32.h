#ifndef LED8X32_h
#define LED8X32_h

#include "Arduino.h"

#define SYS_DIS	0x00			   	
#define SYS_EN	0x01			   	
#define LED_OFF	0x02			  	
#define LED_ON	0x03			   	
#define BLINK_OFF	0x08		   	
#define	BLINK_ON	0X09		   	
#define	SLAVE_MODE	0X10		   	
#define	RC_MASTER_MODE	0X18	   	
#define	EXT_CLK_MASTER_MODE	0X1C   	
#define COM_OPTION	0X20		   	
#define PWM_DUTY	0XA0			

#define CS 11  
#define RD 10  
#define WR 12  
#define DA 13
extern uint8_t zimo_alphabet[59][8];
//#define REVERSE (1)
class LED8X32{
public:
	LED8X32(uint8_t cs,uint8_t wr,uint8_t data);
	virtual ~LED8X32();

	void HT1632C_Init(void);
	void HT1632C_clr(void);
	void HT1632C_Writer_CMD(unsigned char cmd);
	void HT1632C_Writer(unsigned char Data,unsigned char cnt);
	void HT1632C_Writer_AllDATA(unsigned char Addr,unsigned char p[],unsigned char cnt);
	void HT1632C_show_string(uint8_t *string, uint8_t lenth, bool repeat=true, uint16_t ms=500);
	void HT1632C_show_handle();
private:
	uint8_t cs_pin;
	uint8_t wr_pin;
	uint8_t	data_pin;
	uint8_t ptr;
	uint8_t *string;
	bool repeat_type;
	uint8_t length;
	uint16_t time_interval;
	uint32_t start_moment;
};
#endif
