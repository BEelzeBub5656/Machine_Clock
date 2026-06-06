#ifndef __UART_H
#define __UART_H


void My_USART_Init(void);//串口初始化
void My_USART_SendByte(uint8_t Byte);//发送一个字节
void My_USART_SendArray(uint8_t *Array,uint16_t Size);//发送数组
void My_USART_SendString(char *String);//发送字符串
uint8_t My_USART_ReceiveByte(void);//接收一个字节

#endif

