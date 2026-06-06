     #include "stm32f10x.h"                  // Device header
     #include "MyI2C.h"
     #include "PCA9685_H.h"
     #include "PCA9685_M.h"
     #include "Delay.h"
     
     
     /**************************************************
     
     * 项目: 基于STM32的智能机械时钟
     * 日期: 2025.10.12
     * MCU: STM32F103C8T6
     * 作者: 自由飞
     * 声明：未经允许禁止转载；若引用务必标明或@本人
    
    ***************************************************/
    
    
    // ===== 段6防碰撞：段位显示映射表 =====
    // seg_map[数字][段号] → 1=显示(90°), 0=隐藏
    static const uint8_t seg_map[10][7] = {
    	{1,1,1,1,1,1,0}, // 0
    	{0,1,1,0,0,0,0}, // 1
    	{1,1,0,1,1,0,1}, // 2
    	{1,1,1,1,0,0,1}, // 3
    	{0,1,1,0,0,1,1}, // 4
    	{1,0,1,1,0,1,1}, // 5
    	{1,0,1,1,1,1,1}, // 6
    	{1,1,1,0,0,0,0}, // 7
    	{1,1,1,1,1,1,1}, // 8
    	{1,1,1,1,0,1,1}  // 9
    };
    
    // 段6上一轮状态追踪（上电默认值匹配case 0初始显示）
    static uint8_t last_digit_H1 = 0;	// H十位上轮数字
    static uint8_t last_digit_H2 = 1;	// H个位上轮数字
    static uint8_t last_digit_M1 = 0;	// M十位上轮数字
    static uint8_t last_digit_M2 = 0;	// M个位上轮数字
    
    
    void PCA9685_Set_Time_Num_H(uint8_t locate_H,uint8_t num_H)	//时位
    {
    	uint8_t base_ch, hide_6, last_num;
    	uint8_t old_seg6_disp, new_seg6_disp;
    
    	// 确定通道基址和段6隐藏角
    	if(locate_H==1) { base_ch=0;  hide_6=0;   last_num=last_digit_H1; }
    	else            { base_ch=7;  hide_6=180; last_num=last_digit_H2; }
    
    	// 判断段6是否需要翻转
    	old_seg6_disp = seg_map[last_num][6];
    	new_seg6_disp = seg_map[num_H][6];
    
    	if (old_seg6_disp != new_seg6_disp)
    	{
    		// --- 避让：1,2,4,5向隐藏方向偏转20° ---
    		if (seg_map[last_num][1]) PCA9685_H_SetAngle(base_ch+1, 110);	// 段1: 90→110 (向180°)
    		if (seg_map[last_num][2]) PCA9685_H_SetAngle(base_ch+2, 70);		// 段2: 90→70  (向0°)
    		if (seg_map[last_num][4]) PCA9685_H_SetAngle(base_ch+4, 110);	// 段4: 90→110 (向180°)
    		if (seg_map[last_num][5]) PCA9685_H_SetAngle(base_ch+5, 70);		// 段5: 90→70  (向0°)
    		Delay_ms(60);		// 等避让20°完成（已添加延迟）
    
    		// --- 翻转段6 ---
    		PCA9685_H_SetAngle(base_ch+6, new_seg6_disp ? 90 : hide_6);
    		Delay_ms(180);		// 等段6翻转完成（已添加延迟）
    	}
    	// 后续switch正常设置所有段，会覆盖避让角为目标角
    
    	if(locate_H==1)
    	{
    		switch(num_H)
    		{
    			case 0:
    			//数字0
    			{
    				PCA9685_H_SetAngle(0,90);
    				PCA9685_H_SetAngle(1,90);
    				PCA9685_H_SetAngle(2,90);
    				PCA9685_H_SetAngle(3,90);
    				PCA9685_H_SetAngle(4,90);
    				PCA9685_H_SetAngle(5,90);
    				PCA9685_H_SetAngle(6,0);
    			}
    			break;
    			case 1:
    			{
    				PCA9685_H_SetAngle(0,180);
    				PCA9685_H_SetAngle(1,90);
    				PCA9685_H_SetAngle(2,90);
    				PCA9685_H_SetAngle(3,0);
    				PCA9685_H_SetAngle(4,180);
    				PCA9685_H_SetAngle(5,0);
    				PCA9685_H_SetAngle(6,0);
    			}
    			break;
    			case 2:
    			//数字2
    			{
    				PCA9685_H_SetAngle(0,90);
   				PCA9685_H_SetAngle(1,90);
   				PCA9685_H_SetAngle(2,0);
   				PCA9685_H_SetAngle(3,90);
   				PCA9685_H_SetAngle(4,90);
   				PCA9685_H_SetAngle(5,0);
   				PCA9685_H_SetAngle(6,90);
   			}
   			break;
   			case 3:
   			//数字3
   			{
   				PCA9685_H_SetAngle(0,90);
   				PCA9685_H_SetAngle(1,90);
   				PCA9685_H_SetAngle(2,90);
   				PCA9685_H_SetAngle(3,90);
   				PCA9685_H_SetAngle(4,180);
   				PCA9685_H_SetAngle(5,0);
   				PCA9685_H_SetAngle(6,90);
   			}
   			break;
   			case 4:
   			{
   				PCA9685_H_SetAngle(0,180);
   				PCA9685_H_SetAngle(1,90);
   				PCA9685_H_SetAngle(2,90);
   				PCA9685_H_SetAngle(3,0);
   				PCA9685_H_SetAngle(4,180);
   				PCA9685_H_SetAngle(5,90);
   				PCA9685_H_SetAngle(6,90);
   			}
   			break;
   			case 5:
   			{
   				PCA9685_H_SetAngle(0,90);
   				PCA9685_H_SetAngle(1,180);
   				PCA9685_H_SetAngle(2,90);
   				PCA9685_H_SetAngle(3,90);
   				PCA9685_H_SetAngle(4,180);
   				PCA9685_H_SetAngle(5,90);
   				PCA9685_H_SetAngle(6,90);
   			}
   			break;
   			case 6:
   			{
   				PCA9685_H_SetAngle(0,90);
   				PCA9685_H_SetAngle(1,180);
   				PCA9685_H_SetAngle(2,90);
   				PCA9685_H_SetAngle(3,90);
   				PCA9685_H_SetAngle(4,90);
   				PCA9685_H_SetAngle(5,90);
   				PCA9685_H_SetAngle(6,90);
   			}
   			break;
   			case 7:
   			{
   				PCA9685_H_SetAngle(0,90);
   				PCA9685_H_SetAngle(1,90);
   				PCA9685_H_SetAngle(2,90);
   				PCA9685_H_SetAngle(3,0);
   				PCA9685_H_SetAngle(4,180);
   				PCA9685_H_SetAngle(5,0);
   				PCA9685_H_SetAngle(6,0);
   			}
   			break;
   			case 8:
   			{
   				PCA9685_H_SetAngle(0,90);
   				PCA9685_H_SetAngle(1,90);
   				PCA9685_H_SetAngle(2,90);
   				PCA9685_H_SetAngle(3,90);
   				PCA9685_H_SetAngle(4,90);
   				PCA9685_H_SetAngle(5,90);
   				PCA9685_H_SetAngle(6,90);
   			}
   			break;
   			case 9:
   			{
   				PCA9685_H_SetAngle(0,90);
   				PCA9685_H_SetAngle(1,90);
   				PCA9685_H_SetAngle(2,90);
   				PCA9685_H_SetAngle(3,90);
   				PCA9685_H_SetAngle(4,180);
   				PCA9685_H_SetAngle(5,90);
   				PCA9685_H_SetAngle(6,90);
   			}
   			break;
   		}
   	}else
   	if(locate_H==2)
   	{
   		switch(num_H)
   		{
   			case 0:
   			//数字0
   			{
   				PCA9685_H_SetAngle(7,90);
   				PCA9685_H_SetAngle(8,90);
   				PCA9685_H_SetAngle(9,90);
   				PCA9685_H_SetAngle(10,90);
   				PCA9685_H_SetAngle(11,90);
   				PCA9685_H_SetAngle(12,90);
   				PCA9685_H_SetAngle(13,180);
   			}
   			break;
   			case 1:
   			{
   				PCA9685_H_SetAngle(7,0);
   				PCA9685_H_SetAngle(8,90);
   				PCA9685_H_SetAngle(9,90);
   				PCA9685_H_SetAngle(10,180);
   				PCA9685_H_SetAngle(11,180);
   				PCA9685_H_SetAngle(12,0);
   				PCA9685_H_SetAngle(13,180);
   			}
   			break;
   			case 2:
   			//数字2
   			{
   				PCA9685_H_SetAngle(7,90);
   				PCA9685_H_SetAngle(8,90);
   				PCA9685_H_SetAngle(9,0);
   				PCA9685_H_SetAngle(10,90);
   				PCA9685_H_SetAngle(11,90);
   				PCA9685_H_SetAngle(12,0);
   				PCA9685_H_SetAngle(13,90);
   			}
   			break;
   			case 3:
   			//数字3
   			{
   				PCA9685_H_SetAngle(7,90);
   				PCA9685_H_SetAngle(8,90);
   				PCA9685_H_SetAngle(9,90);
   				PCA9685_H_SetAngle(10,90);
   				PCA9685_H_SetAngle(11,180);
   				PCA9685_H_SetAngle(12,0);
   				PCA9685_H_SetAngle(13,90);
   			}
   			break;
   			case 4:
   			{
   				PCA9685_H_SetAngle(7,0);
   				PCA9685_H_SetAngle(8,90);
   				PCA9685_H_SetAngle(9,90);
   				PCA9685_H_SetAngle(10,180);
   				PCA9685_H_SetAngle(11,180);
   				PCA9685_H_SetAngle(12,90);
   				PCA9685_H_SetAngle(13,90);
   			}
   			break;
   			case 5:
   			{
   				PCA9685_H_SetAngle(7,90);
   				PCA9685_H_SetAngle(8,180);
   				PCA9685_H_SetAngle(9,90);
   				PCA9685_H_SetAngle(10,90);
   				PCA9685_H_SetAngle(11,180);
   				PCA9685_H_SetAngle(12,90);
   				PCA9685_H_SetAngle(13,90);
   			}
   			break;
   			case 6:
   			{
   				PCA9685_H_SetAngle(7,90);
   				PCA9685_H_SetAngle(8,180);
   				PCA9685_H_SetAngle(9,90);
   				PCA9685_H_SetAngle(10,90);
   				PCA9685_H_SetAngle(11,90);
   				PCA9685_H_SetAngle(12,90);
   				PCA9685_H_SetAngle(13,90);
   			}
   			break;
   			case 7:
   			{
   				PCA9685_H_SetAngle(7,90);
   				PCA9685_H_SetAngle(8,90);
   				PCA9685_H_SetAngle(9,90);
   				PCA9685_H_SetAngle(10,180);
   				PCA9685_H_SetAngle(11,180);
   				PCA9685_H_SetAngle(12,0);
   				PCA9685_H_SetAngle(13,180);
   			}
   			break;
   			case 8:
   			{
   				PCA9685_H_SetAngle(7,90);
   				PCA9685_H_SetAngle(8,90);
   				PCA9685_H_SetAngle(9,90);
   				PCA9685_H_SetAngle(10,90);
   				PCA9685_H_SetAngle(11,90);
   				PCA9685_H_SetAngle(12,90);
   				PCA9685_H_SetAngle(13,90);
   			}
   			break;
   			case 9:
   			{
   				PCA9685_H_SetAngle(7,90);
   				PCA9685_H_SetAngle(8,90);
   				PCA9685_H_SetAngle(9,90);
   				PCA9685_H_SetAngle(10,90);
   				PCA9685_H_SetAngle(11,180);
   				PCA9685_H_SetAngle(12,90);
   				PCA9685_H_SetAngle(13,90);
   			}
   			break;
   		}
   	}
   	
   	// 更新段6状态追踪
   	if(locate_H==1) last_digit_H1 = num_H;
   	else            last_digit_H2 = num_H;
   }
   
   void PCA9685_Set_Time_Num_M(uint8_t locate_M,uint8_t num_M)	//分位
   {
   	uint8_t base_ch, hide_6, last_num;
   	uint8_t old_seg6_disp, new_seg6_disp;
   
   	// 确定通道基址和段6隐藏角
   	if(locate_M==1) { base_ch=0;  hide_6=0;   last_num=last_digit_M1; }
   	else            { base_ch=7;  hide_6=180; last_num=last_digit_M2; }
   
   	// 判断段6是否需要翻转
   	old_seg6_disp = seg_map[last_num][6];
   	new_seg6_disp = seg_map[num_M][6];
   
   	if (old_seg6_disp != new_seg6_disp)
   	{
   		// --- 避让：1,2,4,5向隐藏方向偏转20° ---
   		if (seg_map[last_num][1]) PCA9685_M_SetAngle(base_ch+1, 110);
   		if (seg_map[last_num][2]) PCA9685_M_SetAngle(base_ch+2, 70);
   		if (seg_map[last_num][4]) PCA9685_M_SetAngle(base_ch+4, 110);
   		if (seg_map[last_num][5]) PCA9685_M_SetAngle(base_ch+5, 70);
   		Delay_ms(60);
   
   		// --- 翻转段6 ---
   		PCA9685_M_SetAngle(base_ch+6, new_seg6_disp ? 90 : hide_6);
   		Delay_ms(180);
   	}
   
   	if(locate_M==1)
   	{
   		switch(num_M)
   		{
   			case 0:
   			//数字0
   			{
   				PCA9685_M_SetAngle(0,90);
   				PCA9685_M_SetAngle(1,90);
   				PCA9685_M_SetAngle(2,90);
   				PCA9685_M_SetAngle(3,90);
   				PCA9685_M_SetAngle(4,90);
   				PCA9685_M_SetAngle(5,90);
   				PCA9685_M_SetAngle(6,0);
   			}
   			break;
   			case 1:
   			{
   				PCA9685_M_SetAngle(0,180);
   				PCA9685_M_SetAngle(1,90);
   				PCA9685_M_SetAngle(2,90);
   				PCA9685_M_SetAngle(3,0);
   				PCA9685_M_SetAngle(4,180);
   				PCA9685_M_SetAngle(5,0);
   				PCA9685_M_SetAngle(6,0);
   			}
   			break;
   			case 2:
   			//数字2
   			{
   				PCA9685_M_SetAngle(0,90);
   				PCA9685_M_SetAngle(1,90);
   				PCA9685_M_SetAngle(2,0);
   				PCA9685_M_SetAngle(3,90);
   				PCA9685_M_SetAngle(4,90);
   				PCA9685_M_SetAngle(5,0);
   				PCA9685_M_SetAngle(6,90);
   			}
   			break;
   			case 3:
   			//数字3
   			{
   				PCA9685_M_SetAngle(0,90);
   				PCA9685_M_SetAngle(1,90);
   				PCA9685_M_SetAngle(2,90);
   				PCA9685_M_SetAngle(3,90);
   				PCA9685_M_SetAngle(4,180);
   				PCA9685_M_SetAngle(5,0);
   				PCA9685_M_SetAngle(6,90);
   			}
   			break;
   			case 4:
   			{
   				PCA9685_M_SetAngle(0,180);
   				PCA9685_M_SetAngle(1,90);
   				PCA9685_M_SetAngle(2,90);
   				PCA9685_M_SetAngle(3,0);
   				PCA9685_M_SetAngle(4,180);
   				PCA9685_M_SetAngle(5,90);
   				PCA9685_M_SetAngle(6,90);
   			}
   			break;
   			case 5:
   			{
   				PCA9685_M_SetAngle(0,90);
   				PCA9685_M_SetAngle(1,180);
   				PCA9685_M_SetAngle(2,90);
   				PCA9685_M_SetAngle(3,90);
   				PCA9685_M_SetAngle(4,180);
   				PCA9685_M_SetAngle(5,90);
   				PCA9685_M_SetAngle(6,90);
   			}
   			break;
   			case 6:
   			{
   				PCA9685_M_SetAngle(0,90);
   				PCA9685_M_SetAngle(1,180);
   				PCA9685_M_SetAngle(2,90);
   				PCA9685_M_SetAngle(3,90);
   				PCA9685_M_SetAngle(4,90);
   				PCA9685_M_SetAngle(5,90);
   				PCA9685_M_SetAngle(6,90);
   			}
   			break;
   			case 7:
   			{
   				PCA9685_M_SetAngle(0,90);
   				PCA9685_M_SetAngle(1,90);
   				PCA9685_M_SetAngle(2,90);
   				PCA9685_M_SetAngle(3,0);
   				PCA9685_M_SetAngle(4,180);
   				PCA9685_M_SetAngle(5,0);
   				PCA9685_M_SetAngle(6,0);
   			}
   			break;
   			case 8:
   			{
   				PCA9685_M_SetAngle(0,90);
   				PCA9685_M_SetAngle(1,90);
   				PCA9685_M_SetAngle(2,90);
   				PCA9685_M_SetAngle(3,90);
   				PCA9685_M_SetAngle(4,90);
   				PCA9685_M_SetAngle(5,90);
   				PCA9685_M_SetAngle(6,90);
   			}
   			break;
			case 9:
			{
				PCA9685_M_SetAngle(0,90);
				PCA9685_M_SetAngle(1,90);
				PCA9685_M_SetAngle(2,90);
				PCA9685_M_SetAngle(3,90);
				PCA9685_M_SetAngle(4,180);
   				PCA9685_M_SetAngle(5,90);
   				PCA9685_M_SetAngle(6,90);
   			}
   			break;
   		}
   	}else
   	if(locate_M==2)
   	{
   		switch(num_M)
   		{
   			case 0:
   			//数字0
   			{
   				PCA9685_M_SetAngle(7,90);
   				PCA9685_M_SetAngle(8,90);
   				PCA9685_M_SetAngle(9,90);
   				PCA9685_M_SetAngle(10,90);
   				PCA9685_M_SetAngle(11,90);
   				PCA9685_M_SetAngle(12,90);
   				PCA9685_M_SetAngle(13,180);
   			}
   			break;
   			case 1:
   			{
   				PCA9685_M_SetAngle(7,0);
   				PCA9685_M_SetAngle(8,90);
   				PCA9685_M_SetAngle(9,90);
   				PCA9685_M_SetAngle(10,180);
   				PCA9685_M_SetAngle(11,180);
   				PCA9685_M_SetAngle(12,0);
   				PCA9685_M_SetAngle(13,180);
   			}
   			break;
   			case 2:
   			//数字2
   			{
   				PCA9685_M_SetAngle(7,90);
   				PCA9685_M_SetAngle(8,90);
   				PCA9685_M_SetAngle(9,0);
   				PCA9685_M_SetAngle(10,90);
   				PCA9685_M_SetAngle(11,90);
   				PCA9685_M_SetAngle(12,0);
   				PCA9685_M_SetAngle(13,90);
   			}
   			break;
   			case 3:
   			//数字3
   			{
   				PCA9685_M_SetAngle(7,90);
   				PCA9685_M_SetAngle(8,90);
   				PCA9685_M_SetAngle(9,90);
   				PCA9685_M_SetAngle(10,90);
   				PCA9685_M_SetAngle(11,180);
   				PCA9685_M_SetAngle(12,0);
   				PCA9685_M_SetAngle(13,90);
   			}
   			break;
   			case 4:
   			{
   				PCA9685_M_SetAngle(7,0);
   				PCA9685_M_SetAngle(8,90);
   				PCA9685_M_SetAngle(9,90);
   				PCA9685_M_SetAngle(10,180);
   				PCA9685_M_SetAngle(11,180);
   				PCA9685_M_SetAngle(12,90);
   				PCA9685_M_SetAngle(13,90);
   			}
   			break;
   			case 5:
   			{
   				PCA9685_M_SetAngle(7,90);
   				PCA9685_M_SetAngle(8,180);
   				PCA9685_M_SetAngle(9,90);
   				PCA9685_M_SetAngle(10,90);
   				PCA9685_M_SetAngle(11,180);
   				PCA9685_M_SetAngle(12,90);
   				PCA9685_M_SetAngle(13,90);
   			}
   			break;
   			case 6:
   			{
   				PCA9685_M_SetAngle(7,90);
   				PCA9685_M_SetAngle(8,180);
   				PCA9685_M_SetAngle(9,90);
   				PCA9685_M_SetAngle(10,90);
   				PCA9685_M_SetAngle(11,90);
   				PCA9685_M_SetAngle(12,90);
   				PCA9685_M_SetAngle(13,90);
   			}
   			break;
   			case 7:
   			{
   				PCA9685_M_SetAngle(7,90);
   				PCA9685_M_SetAngle(8,90);
   				PCA9685_M_SetAngle(9,90);
   				PCA9685_M_SetAngle(10,180);
   				PCA9685_M_SetAngle(11,180);
   				PCA9685_M_SetAngle(12,0);
   				PCA9685_M_SetAngle(13,180);
   			}
   			break;
   			case 8:
   			{
   				PCA9685_M_SetAngle(7,90);
   				PCA9685_M_SetAngle(8,90);
   				PCA9685_M_SetAngle(9,90);
   				PCA9685_M_SetAngle(10,90);
   				PCA9685_M_SetAngle(11,90);
   				PCA9685_M_SetAngle(12,90);
   				PCA9685_M_SetAngle(13,90);
   			}
   			break;
   			case 9:
   			{
   				PCA9685_M_SetAngle(7,90);
   				PCA9685_M_SetAngle(8,90);
   				PCA9685_M_SetAngle(9,90);
   				PCA9685_M_SetAngle(10,90);
   				PCA9685_M_SetAngle(11,180);
   				PCA9685_M_SetAngle(12,90);
   				PCA9685_M_SetAngle(13,90);
   			}
   			break;
   		}
   	}
   	
	// 更新段6状态追踪
	if(locate_M==1) last_digit_M1 = num_M;
	else            last_digit_M2 = num_M;
}


// ===== 四位时间同步显示：并行避让+翻转，解决串行延迟 =====
void PCA9685_Display_Time(uint8_t h1, uint8_t h2, uint8_t m1, uint8_t m2)
{
	uint8_t any_yield = 0;

	// ===== Phase 1: 四路并行避让 =====
	// H十位 (ch0-6, seg6=ch6, hide=0°)
	if (seg_map[last_digit_H1][6] != seg_map[h1][6]) {
		if (seg_map[last_digit_H1][1]) PCA9685_H_SetAngle(1, 110);
		if (seg_map[last_digit_H1][2]) PCA9685_H_SetAngle(2, 70);
		if (seg_map[last_digit_H1][4]) PCA9685_H_SetAngle(4, 110);
		if (seg_map[last_digit_H1][5]) PCA9685_H_SetAngle(5, 70);
		any_yield = 1;
	}
	// H个位 (ch7-13, seg6=ch13, hide=180°)
	if (seg_map[last_digit_H2][6] != seg_map[h2][6]) {
		if (seg_map[last_digit_H2][1]) PCA9685_H_SetAngle(8, 110);
		if (seg_map[last_digit_H2][2]) PCA9685_H_SetAngle(9, 70);
		if (seg_map[last_digit_H2][4]) PCA9685_H_SetAngle(11, 110);
		if (seg_map[last_digit_H2][5]) PCA9685_H_SetAngle(12, 70);
		any_yield = 1;
	}
	// M十位 (ch0-6, seg6=ch6, hide=0°)
	if (seg_map[last_digit_M1][6] != seg_map[m1][6]) {
		if (seg_map[last_digit_M1][1]) PCA9685_M_SetAngle(1, 110);
		if (seg_map[last_digit_M1][2]) PCA9685_M_SetAngle(2, 70);
		if (seg_map[last_digit_M1][4]) PCA9685_M_SetAngle(4, 110);
		if (seg_map[last_digit_M1][5]) PCA9685_M_SetAngle(5, 70);
		any_yield = 1;
	}
	// M个位 (ch7-13, seg6=ch13, hide=180°)
	if (seg_map[last_digit_M2][6] != seg_map[m2][6]) {
		if (seg_map[last_digit_M2][1]) PCA9685_M_SetAngle(8, 110);
		if (seg_map[last_digit_M2][2]) PCA9685_M_SetAngle(9, 70);
		if (seg_map[last_digit_M2][4]) PCA9685_M_SetAngle(11, 110);
		if (seg_map[last_digit_M2][5]) PCA9685_M_SetAngle(12, 70);
		any_yield = 1;
	}

	if (any_yield) Delay_ms(60);	// 等避让完成

	// ===== Phase 2: 四路并行翻转段6 =====
	if (seg_map[last_digit_H1][6] != seg_map[h1][6])
		PCA9685_H_SetAngle(6,  seg_map[h1][6] ? 90 : 0);
	if (seg_map[last_digit_H2][6] != seg_map[h2][6])
		PCA9685_H_SetAngle(13, seg_map[h2][6] ? 90 : 180);
	if (seg_map[last_digit_M1][6] != seg_map[m1][6])
		PCA9685_M_SetAngle(6,  seg_map[m1][6] ? 90 : 0);
	if (seg_map[last_digit_M2][6] != seg_map[m2][6])
		PCA9685_M_SetAngle(13, seg_map[m2][6] ? 90 : 180);

	if (any_yield) Delay_ms(180);	// 等段6翻转完成

	// ===== Phase 3: 提前更新追踪，然后正常设置所有段 =====
	// （追踪提前更新后，Set_Time_Num内部避让会跳过，避免重复执行）
	last_digit_H1 = h1;
	last_digit_H2 = h2;
	last_digit_M1 = m1;
	last_digit_M2 = m2;

	PCA9685_Set_Time_Num_H(1, h1);
	PCA9685_Set_Time_Num_H(2, h2);
	PCA9685_Set_Time_Num_M(1, m1);
	PCA9685_Set_Time_Num_M(2, m2);
}


//显示分隔符或表情
void PCA9685_emoji(uint8_t emo)
{
	switch(emo)
	{
		case 0://显示分隔符
		{
			PCA9685_H_SetAngle(14,90);
			PCA9685_M_SetAngle(14,90);
		}
		break;
		case 1://关闭分隔符
		{
			PCA9685_H_SetAngle(14,0);
			PCA9685_M_SetAngle(14,0);
		}
		break;
		case 2://休息模式
		{
			PCA9685_H_SetAngle(0,180);
			PCA9685_H_SetAngle(1,180);
			PCA9685_H_SetAngle(2,0);
			PCA9685_H_SetAngle(3,0);
			PCA9685_H_SetAngle(4,180);
			PCA9685_H_SetAngle(5,0);
			PCA9685_H_SetAngle(6,0);
			PCA9685_H_SetAngle(7,0);
			PCA9685_H_SetAngle(8,180);
			PCA9685_H_SetAngle(9,0);
			PCA9685_H_SetAngle(10,180);
			PCA9685_H_SetAngle(11,180);
			PCA9685_H_SetAngle(12,0);
			PCA9685_H_SetAngle(13,180);
			PCA9685_H_SetAngle(14,0);
			
			PCA9685_M_SetAngle(0,180);
			PCA9685_M_SetAngle(1,180);
			PCA9685_M_SetAngle(2,0);
			PCA9685_M_SetAngle(3,0);
			PCA9685_M_SetAngle(4,180);
			PCA9685_M_SetAngle(5,0);
			PCA9685_M_SetAngle(6,0);
			PCA9685_M_SetAngle(7,0);
			PCA9685_M_SetAngle(8,180);
			PCA9685_M_SetAngle(9,0);
			PCA9685_M_SetAngle(10,180);
			PCA9685_M_SetAngle(11,180);
			PCA9685_M_SetAngle(12,0);
			PCA9685_M_SetAngle(13,180);
			PCA9685_M_SetAngle(14,0);
		}
		break;
		case 3://表情
		{
			PCA9685_H_SetAngle(0,180);
			PCA9685_H_SetAngle(1,180);
			PCA9685_H_SetAngle(2,0);
			PCA9685_H_SetAngle(3,0);
			PCA9685_H_SetAngle(4,180);
			PCA9685_H_SetAngle(5,0);
			PCA9685_H_SetAngle(6,0);
			
			PCA9685_H_SetAngle(7,90);
			PCA9685_H_SetAngle(8,90);
			PCA9685_H_SetAngle(9,0);
			PCA9685_H_SetAngle(10,180);
			PCA9685_H_SetAngle(11,180);
			
			
			PCA9685_H_SetAngle(12,90);
			PCA9685_H_SetAngle(13,180);
			PCA9685_H_SetAngle(14,0);
			
			
			PCA9685_M_SetAngle(0,90);
			PCA9685_M_SetAngle(1,90);
			PCA9685_M_SetAngle(2,0);
			PCA9685_M_SetAngle(3,0);
			PCA9685_M_SetAngle(4,180);
			PCA9685_M_SetAngle(5,90);
			PCA9685_M_SetAngle(6,0);
			PCA9685_M_SetAngle(7,0);
			PCA9685_M_SetAngle(8,180);
			PCA9685_M_SetAngle(9,0);
			PCA9685_M_SetAngle(10,180);
			PCA9685_M_SetAngle(11,180);
			PCA9685_M_SetAngle(12,0);
			PCA9685_M_SetAngle(13,180);
			PCA9685_M_SetAngle(14,90);
		}
		break;
	}
}
