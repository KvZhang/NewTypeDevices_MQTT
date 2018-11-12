/*
 * LaputaTelephone.cpp
 *
 *  Created on: 2017��4��23��
 *      Author: zhangyu
 */

#include "LaputaTelephone.h"

LaputaTelephone::LaputaTelephone(uint8_t hold_pin,uint8_t dial_pin,uint8_t num_pin,uint8_t speaker_pin)
{
	// TODO Auto-generated constructor stub
	this->counter=0;
	this->dial_pin=dial_pin;
	this->hold_pin=hold_pin;
	this->num_pin=num_pin;
	this->speaker_pin=speaker_pin;
	memset(this->num,0,MAX_NUM_LENGTH);
	this->ptr=0;
	this->status=Status_phone_idle;
	pinMode(this->dial_pin,INPUT_PULLUP);
	pinMode(this->hold_pin,INPUT_PULLUP);
	pinMode(this->num_pin,INPUT_PULLUP);
	pinMode(this->speaker_pin,OUTPUT);
	digitalWrite(speaker_pin,LOW);
	for(uint8_t i=0;i<MAX_RIGHT_ANSWER_COUNT;i++)
	{
		this->right_num[i].ptr=NULL;
		this->right_num[i].len=0;
		this->right_num[i].isEnabled=false;
		this->right_num[i].isDialed=false;
	}
	this->force_ring_num=0;
	this->flag.all=0;
}

uint8_t LaputaTelephone::read_status(void)
{
	return this->status;
}

void LaputaTelephone:: status_idle()
{
	static uint8_t flag_played=0;
	static uint32_t hangout_count=0;

	if(this->flag.bit.force_ring && !flag_played)
	{
		myMp3.write(PLAY_REPEAT, song_ringing);
		flag_played=1;
	}
	else if(!this->flag.bit.force_ring)
	{
		myMp3.write(STOP_PLAY, 0);
		flag_played=0;
		delay(50);
	}

	if(digitalRead(this->hold_pin)==Phone_picked_up)
	{
		if(hangout_count && (millis()-hangout_count>100))
		{
			digitalWrite(speaker_pin,HIGH);
			if(this->flag.bit.force_ring)
			{
				if(this->force_ring_num)
				{
					myMp3.write(MP3_cmd_play_forder_song, this->force_ring_num,0x01);
				}
				else
				{
					myMp3.write(PLAY_REPEAT, song_buzy);
				}
			}
			else
			{
				myMp3.write(PLAY_REPEAT, song_idle);
			}
			this->status=Status_phone_pick_up;
			this->counter=millis();
			flag_played=0;
		}
		else if(hangout_count==0)
		{
			hangout_count=millis();
		}
	}
	else
	{
		hangout_count=0;
	}
}
void LaputaTelephone::status_pick_up()
{
	static uint32_t hang_out_moment=0;
	if(digitalRead(this->hold_pin)==Phone_hang_out)
	{
		if(hang_out_moment && (millis()-hang_out_moment>100))
		{
			delay(100);
			digitalWrite(speaker_pin,LOW);
			this->status=Status_phone_idle;
			myMp3.write(STOP_PLAY, 0);
			this->flag.bit.force_ring=0;
			this->flag.all=0;
			hang_out_moment=0;
		}
		else if(hang_out_moment==0)
		{
			hang_out_moment=millis();
		}

	}
	else
	{
		hang_out_moment=0;
	}

	if(!this->flag.bit.force_ring)
	{
		if(digitalRead(this->dial_pin)==0)
		{
			myMp3.write(STOP_PLAY, 0);
			this->status=Status_phone_dialing;
			this->counter=millis();
			hang_out_moment=0;
		}
	}
	else
	{
		if(this->flag.bit.answer_right)
		{
			myMp3.write(PLAY_ONCE, song_voice_right);
			this->status=Status_phone_wait_for_end;
			hang_out_moment=0;

		}
		else if(this->flag.bit.time_out)
		{
			myMp3.write(PLAY_REPEAT, song_buzy);
			this->status=Status_phone_wait_for_end;
			hang_out_moment=0;
		}
	}
}
void LaputaTelephone::status_dialing()
{
	static uint8_t high_count=0;
	static uint32_t high_moment=0;

	if(digitalRead(this->dial_pin)==1)
	{
		if(high_count)
		{
			this->num[this->ptr++]=high_count;
			high_count=0;
			high_moment=0;
			this->counter=millis();
		}
		if(millis()-this->counter>3000)//3s, dial out
		{
			myMp3.write(PLAY_REPEAT, song_dialing);
			this->status=Status_phone_dialed;
		}
	}
	else
	{
		if(digitalRead(this->num_pin)==0)
		{
			high_moment=millis();
		}
		else
		{
			if(high_moment &&(millis()-high_moment)>10)
			{
				high_count++;
				high_moment=0;
			}

		}
	}

	if(digitalRead(this->hold_pin)==Phone_hang_out)
	{
		digitalWrite(speaker_pin,LOW);
		myMp3.write(STOP_PLAY, 0);
		this->status=Status_phone_idle;
		this->flag.all=0;
	}
}
void LaputaTelephone::status_dialed()
{
	int8_t temp=0;
	if(this->counter&&millis()-this->counter>8000)//seems need to handle
	{
		temp=this->check_answer();
		if(temp > 0)
		{
			myMp3.write(MP3_cmd_play_forder_song, temp, 0x02);
		}
		else if(temp == 0)
		{
			myMp3.write(PLAY_REPEAT, song_wrong_number);
		}
		else
		{
			myMp3.write(PLAY_REPEAT, song_buzy);
		}
		this->counter=0;
	}


	if(digitalRead(this->hold_pin)==Phone_hang_out)
	{
		digitalWrite(speaker_pin,LOW);
		myMp3.write(STOP_PLAY, 0);
		this->status=Status_phone_idle;
		this->ptr=0;
		memset(this->num,0,MAX_NUM_LENGTH);
	}
}
void LaputaTelephone::status_wait_for_end()
{
	if(digitalRead(this->hold_pin)==Phone_hang_out)
	{
		digitalWrite(speaker_pin,LOW);
		myMp3.write(STOP_PLAY, 0);
		this->status=Status_phone_idle;
		this->ptr=0;
		memset(this->num,0,MAX_NUM_LENGTH);
		this->flag.all=0;
	}
}
void LaputaTelephone::handler()
{
	switch(this->status)
	{
	case Status_phone_idle:
		status_idle();
		break;
	case Status_phone_pick_up:
		status_pick_up();
		break;
	case Status_phone_dialing:
		status_dialing();
		break;
	case Status_phone_dialed:
		status_dialed();
		break;
	case Status_phone_wait_for_end:
		status_wait_for_end();
		break;
	default:
		this->status=Status_phone_idle;
		break;
	}
}
//������ȷ�����ַ���02�ļ���001~005
void LaputaTelephone::set_right_num(uint8_t *ptr,uint8_t len, uint8_t n)
{
	if(ptr!=NULL && n<MAX_RIGHT_ANSWER_COUNT)
	{
		this->right_num[n].ptr=ptr;
		this->right_num[n].len=len;
		this->right_num[n].isEnabled=true;
	}
}
void LaputaTelephone::set_right_num_status(uint8_t n, bool status)
{
	if(n<MAX_RIGHT_ANSWER_COUNT && this->right_num[n].ptr!=NULL)
	{
		this->right_num[n].isEnabled=status;
	}
}
int8_t LaputaTelephone::check_answer()
{
	uint8_t flag=1;
	for(uint8_t i=0;i<MAX_RIGHT_ANSWER_COUNT;i++)
	{
		if(this->ptr!=this->right_num[i].len)
		{
			flag=0;
		}
		else
		{
			for(uint8_t j=0; j<this->right_num[i].len;j++)
			{
				if(this->num[j]!=*(this->right_num[i].ptr+j))
				{
					flag=0;
					break;
				}
			}
		}
		if(flag)
		{
			if(this->right_num[i].isEnabled)
			{
				this->right_num[i].isDialed=true;
				return i+1;
			}
			else
			{
				return -1;
			}
		}
		else
		{
			flag=1;
		}
	}
	return 0;
}
uint8_t LaputaTelephone::get_length()
{
	if(this->status==Status_phone_dialed)
	{
		return this->ptr;
	}
	return 0;
}
//ǿ�����壬ժ���󣬲���01�ļ����num�׸裬sum=001~005
void LaputaTelephone::force_to_ring(bool flag,uint8_t num)
{
	this->flag.bit.force_ring=flag;
	this->force_ring_num = num;
}
//n=0~4
bool LaputaTelephone::read_right_num_status(uint8_t n)
{
	if( n < MAX_RIGHT_ANSWER_COUNT)
	{
		bool temp=this->right_num[n].isDialed;
		this->right_num[n].isDialed=false;
		return temp;
	}
	return false;
}
LaputaTelephone::~LaputaTelephone()
{
	// TODO Auto-generated destructor stub
}

