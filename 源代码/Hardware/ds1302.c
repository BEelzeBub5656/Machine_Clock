#include "ds1302.h"
#include "Delay.h"

struct TIMEData TimeData;
u8 read_time[7];

void ds1302_gpio_init()//CE,SCLK端口初始化
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; //PA.11  CE
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.11
	GPIO_ResetBits(GPIOA,GPIO_Pin_11); 
	 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //PA.12  SCLK
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.12
	GPIO_ResetBits(GPIOA,GPIO_Pin_12); 
}
 
void ds1032_DATAOUT_init()//配置双向I/O端口为输出态
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //PA.8  DATA
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化
	GPIO_ResetBits(GPIOA,GPIO_Pin_8);
}
 
void ds1032_DATAINPUT_init()//配置双向I/O端口为输入态
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 ;//PC.10 DATA
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIO
}
 
 
void ds1302_write_onebyte(u8 data)//向DS1302发送一字节数据
{
	ds1032_DATAOUT_init();
	u8 count=0;
	SCLK_L;
	for(count=0;count<8;count++)
		{	SCLK_L;
			if(data&0x01)
			{DATA_H;}
			else{DATA_L;}//先准备好数据再发送
			SCLK_H;//拉高时钟线，发送数据
			data>>=1;
		}
}
 
void ds1302_wirte_rig(u8 address,u8 data)//向指定寄存器地址发送数据
{
	u8 temp1=address;
	u8 temp2=data;
	CE_L;SCLK_L;Delay_us(1);
	CE_H;Delay_us(2);
	ds1302_write_onebyte(temp1);
	ds1302_write_onebyte(temp2);
	CE_L;SCLK_L;Delay_us(2);
}
 
u8 ds1302_read_rig(u8 address)//从指定地址读取一字节数据
{
	u8 temp3=address;
	u8 count=0;
	u8 return_data=0x00;
	CE_L;SCLK_L;Delay_us(3);
	CE_H;Delay_us(3);
	ds1302_write_onebyte(temp3);
	ds1032_DATAINPUT_init();//配置I/O口为输入
	Delay_us(2);
	for(count=0;count<8;count++)
	{
		Delay_us(2);//使电平持续一段时间
		return_data>>=1;
		SCLK_H;Delay_us(4);//使高电平持续一段时间
		SCLK_L;Delay_us(14);//延时14us后再去读取电压，更加准确
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8))
		{return_data=return_data|0x80;}
	}
	Delay_us(2);
	CE_L;DATA_L;
	return return_data;
}



//第一次烧录时取消注释，设置初始时间，然后打开如下注释再次进行烧录，从而实现断电不丢失数据
void ds1032_init()
{
	ds1302_wirte_rig(0x8e,0x00);//关闭写保护
//	ds1302_wirte_rig(0x80,0x02);//seconds02秒
//	ds1302_wirte_rig(0x82,0x16);//minutes5分
//	ds1302_wirte_rig(0x84,0x19);//hours9时
//	ds1302_wirte_rig(0x86,0x29);//date12日
//	ds1302_wirte_rig(0x88,0x05);//months10月
//	ds1302_wirte_rig(0x8a,0x01);//days星期日
	ds1302_wirte_rig(0x8c,0x26);//year2025年
	ds1302_wirte_rig(0x8e,0x80);//开启写保护
}

void ds1032_read_time()
{
	read_time[0]=ds1302_read_rig(0x81);//读秒
	read_time[1]=ds1302_read_rig(0x83);//读分
	read_time[2]=ds1302_read_rig(0x85);//读时
	read_time[3]=ds1302_read_rig(0x87);//读日
	read_time[4]=ds1302_read_rig(0x89);//读月
	read_time[5]=ds1302_read_rig(0x8B);//读星期
	read_time[6]=ds1302_read_rig(0x8D);//读年
}
 
void ds1032_read_realTime()
{
	ds1032_read_time();  //BCD码转换为10进制
	TimeData.second=(read_time[0]>>4)*10+(read_time[0]&0x0f);
	TimeData.minute=((read_time[1]>>4)&(0x07))*10+(read_time[1]&0x0f);
	TimeData.hour=(read_time[2]>>4)*10+(read_time[2]&0x0f);
	TimeData.day=(read_time[3]>>4)*10+(read_time[3]&0x0f);
	TimeData.month=(read_time[4]>>4)*10+(read_time[4]&0x0f);
	TimeData.week=read_time[5];
	TimeData.year=(read_time[6]>>4)*10+(read_time[6]&0x0f)+2000;
}


