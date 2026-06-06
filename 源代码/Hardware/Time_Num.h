#ifndef __TIME_NUM_H
#define __TIME_NUM_H

//表盘显示函数			locate_x:选择十位或个位		num_x:选择显示数字或表情
void PCA9685_Set_Time_Num_H(uint8_t locate_H,uint8_t num_H);//显示时位
void PCA9685_Set_Time_Num_M(uint8_t locate_M,uint8_t num_M);//显示分位
void PCA9685_Display_Time(uint8_t h1, uint8_t h2, uint8_t m1, uint8_t m2);//四位同步显示

//	emo:0---显示分隔符		1:---关闭分隔符	2:---休息		3:---表情
void PCA9685_emoji(uint8_t emo);

#endif

