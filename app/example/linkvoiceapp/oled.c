/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include "driver/chip/hal_def.h"



void draw_text(int x,int line,int clean,const char *text)
{
    if(x<0){
		x=0;
	}else if(x>119){
		x=119;
	}

	if(line>3){
		line=3;
	}
	else if(line<0){
		line=0;
	}
	int row=line*2;
	if(clean){
    	oled_clear_screen();
	}

	oled_draw_string_1608(x, row,text);


	

}





