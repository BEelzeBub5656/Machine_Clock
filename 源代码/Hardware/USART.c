#include "stm32f10x.h"                  // Device header


void My_USART_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_Initstr;
	GPIO_Initstr.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Initstr.GPIO_Pin = GPIO_Pin_9;
	GPIO_Initstr.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA,&GPIO_Initstr);
	
	GPIO_Initstr.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Initstr.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA,&GPIO_Initstr);
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	
	USART_InitTypeDef USART_Initstr;
	USART_Initstr.USART_BaudRate = 9600;
	USART_Initstr.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_Initstr.USART_WordLength = USART_WordLength_8b;
	USART_Initstr.USART_StopBits = USART_StopBits_1;
	USART_Initstr.USART_Parity = USART_Parity_No;
	
	USART_Init(USART1,&USART_Initstr);
	
	USART_Cmd(USART1,ENABLE);
}

void My_USART_SendByte(uint8_t Byte)
{
	USART_SendData(USART1,Byte);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
}

void My_USART_SendArray(uint8_t *Array,uint16_t Size)
{
	uint16_t i;
	for(i=0;i<Size;i++)
	{
		My_USART_SendByte(Array[i]);
	}
}

void My_USART_SendString(char *String)
{
	uint8_t i;
	for(i=0;String[i]!='\0';i++)
	{
		My_USART_SendByte(String[i]);
	}
}


