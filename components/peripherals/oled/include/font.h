/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef _OLED_FONT_H
#define _OELD_FONT_H
//常用ASCII表
//偏移量32
//ASCII字符集: !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~
//PC2LCD2002取模方式设置：阴码+逐列式+顺向+C51格式
//总共：3个字符集（12*12、16*16和24*24），用户可以自行新增其他分辨率的字符集。
//每个字符所占用的字节数为:(size/8+((size%8)?1:0))*(size/2),其中size:是字库生成时的点阵大小(12/16/24...)

//12*12 ASCII字符集点阵
static const unsigned char asc2_1206[95][12]={
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*" ",0*/
{0x00,0x00,0x00,0x00,0x3F,0x40,0x00,0x00,0x00,0x00,0x00,0x00},/*"!",1*/
{0x00,0x00,0x30,0x00,0x40,0x00,0x30,0x00,0x40,0x00,0x00,0x00},/*""",2*/
{0x09,0x00,0x0B,0xC0,0x3D,0x00,0x0B,0xC0,0x3D,0x00,0x09,0x00},/*"#",3*/
{0x18,0xC0,0x24,0x40,0x7F,0xE0,0x22,0x40,0x31,0x80,0x00,0x00},/*"$",4*/
{0x18,0x00,0x24,0xC0,0x1B,0x00,0x0D,0x80,0x32,0x40,0x01,0x80},/*"%",5*/
{0x03,0x80,0x1C,0x40,0x27,0x40,0x1C,0x80,0x07,0x40,0x00,0x40},/*"&",6*/
{0x10,0x00,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"'",7*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0x80,0x20,0x40,0x40,0x20},/*"(",8*/
{0x00,0x00,0x40,0x20,0x20,0x40,0x1F,0x80,0x00,0x00,0x00,0x00},/*")",9*/
{0x09,0x00,0x06,0x00,0x1F,0x80,0x06,0x00,0x09,0x00,0x00,0x00},/*"*",10*/
{0x04,0x00,0x04,0x00,0x3F,0x80,0x04,0x00,0x04,0x00,0x00,0x00},/*"+",11*/
{0x00,0x10,0x00,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*",",12*/
{0x04,0x00,0x04,0x00,0x04,0x00,0x04,0x00,0x04,0x00,0x00,0x00},/*"-",13*/
{0x00,0x00,0x00,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*".",14*/
{0x00,0x20,0x01,0xC0,0x06,0x00,0x38,0x00,0x40,0x00,0x00,0x00},/*"/",15*/
{0x1F,0x80,0x20,0x40,0x20,0x40,0x20,0x40,0x1F,0x80,0x00,0x00},/*"0",16*/
{0x00,0x00,0x10,0x40,0x3F,0xC0,0x00,0x40,0x00,0x00,0x00,0x00},/*"1",17*/
{0x18,0xC0,0x21,0x40,0x22,0x40,0x24,0x40,0x18,0x40,0x00,0x00},/*"2",18*/
{0x10,0x80,0x20,0x40,0x24,0x40,0x24,0x40,0x1B,0x80,0x00,0x00},/*"3",19*/
{0x02,0x00,0x0D,0x00,0x11,0x00,0x3F,0xC0,0x01,0x40,0x00,0x00},/*"4",20*/
{0x3C,0x80,0x24,0x40,0x24,0x40,0x24,0x40,0x23,0x80,0x00,0x00},/*"5",21*/
{0x1F,0x80,0x24,0x40,0x24,0x40,0x34,0x40,0x03,0x80,0x00,0x00},/*"6",22*/
{0x30,0x00,0x20,0x00,0x27,0xC0,0x38,0x00,0x20,0x00,0x00,0x00},/*"7",23*/
{0x1B,0x80,0x24,0x40,0x24,0x40,0x24,0x40,0x1B,0x80,0x00,0x00},/*"8",24*/
{0x1C,0x00,0x22,0xC0,0x22,0x40,0x22,0x40,0x1F,0x80,0x00,0x00},/*"9",25*/
{0x00,0x00,0x00,0x00,0x08,0x40,0x00,0x00,0x00,0x00,0x00,0x00},/*":",26*/
{0x00,0x00,0x00,0x00,0x04,0x60,0x00,0x00,0x00,0x00,0x00,0x00},/*";",27*/
{0x00,0x00,0x04,0x00,0x0A,0x00,0x11,0x00,0x20,0x80,0x40,0x40},/*"<",28*/
{0x09,0x00,0x09,0x00,0x09,0x00,0x09,0x00,0x09,0x00,0x00,0x00},/*"=",29*/
{0x00,0x00,0x40,0x40,0x20,0x80,0x11,0x00,0x0A,0x00,0x04,0x00},/*">",30*/
{0x18,0x00,0x20,0x00,0x23,0x40,0x24,0x00,0x18,0x00,0x00,0x00},/*"?",31*/
{0x1F,0x80,0x20,0x40,0x27,0x40,0x29,0x40,0x1F,0x40,0x00,0x00},/*"@",32*/
{0x00,0x40,0x07,0xC0,0x39,0x00,0x0F,0x00,0x01,0xC0,0x00,0x40},/*"A",33*/
{0x20,0x40,0x3F,0xC0,0x24,0x40,0x24,0x40,0x1B,0x80,0x00,0x00},/*"B",34*/
{0x1F,0x80,0x20,0x40,0x20,0x40,0x20,0x40,0x30,0x80,0x00,0x00},/*"C",35*/
{0x20,0x40,0x3F,0xC0,0x20,0x40,0x20,0x40,0x1F,0x80,0x00,0x00},/*"D",36*/
{0x20,0x40,0x3F,0xC0,0x24,0x40,0x2E,0x40,0x30,0xC0,0x00,0x00},/*"E",37*/
{0x20,0x40,0x3F,0xC0,0x24,0x40,0x2E,0x00,0x30,0x00,0x00,0x00},/*"F",38*/
{0x0F,0x00,0x10,0x80,0x20,0x40,0x22,0x40,0x33,0x80,0x02,0x00},/*"G",39*/
{0x20,0x40,0x3F,0xC0,0x04,0x00,0x04,0x00,0x3F,0xC0,0x20,0x40},/*"H",40*/
{0x20,0x40,0x20,0x40,0x3F,0xC0,0x20,0x40,0x20,0x40,0x00,0x00},/*"I",41*/
{0x00,0x60,0x20,0x20,0x20,0x20,0x3F,0xC0,0x20,0x00,0x20,0x00},/*"J",42*/
{0x20,0x40,0x3F,0xC0,0x24,0x40,0x0B,0x00,0x30,0xC0,0x20,0x40},/*"K",43*/
{0x20,0x40,0x3F,0xC0,0x20,0x40,0x00,0x40,0x00,0x40,0x00,0xC0},/*"L",44*/
{0x3F,0xC0,0x3C,0x00,0x03,0xC0,0x3C,0x00,0x3F,0xC0,0x00,0x00},/*"M",45*/
{0x20,0x40,0x3F,0xC0,0x0C,0x40,0x23,0x00,0x3F,0xC0,0x20,0x00},/*"N",46*/
{0x1F,0x80,0x20,0x40,0x20,0x40,0x20,0x40,0x1F,0x80,0x00,0x00},/*"O",47*/
{0x20,0x40,0x3F,0xC0,0x24,0x40,0x24,0x00,0x18,0x00,0x00,0x00},/*"P",48*/
{0x1F,0x80,0x21,0x40,0x21,0x40,0x20,0xE0,0x1F,0xA0,0x00,0x00},/*"Q",49*/
{0x20,0x40,0x3F,0xC0,0x24,0x40,0x26,0x00,0x19,0xC0,0x00,0x40},/*"R",50*/
{0x18,0xC0,0x24,0x40,0x24,0x40,0x22,0x40,0x31,0x80,0x00,0x00},/*"S",51*/
{0x30,0x00,0x20,0x40,0x3F,0xC0,0x20,0x40,0x30,0x00,0x00,0x00},/*"T",52*/
{0x20,0x00,0x3F,0x80,0x00,0x40,0x00,0x40,0x3F,0x80,0x20,0x00},/*"U",53*/
{0x20,0x00,0x3E,0x00,0x01,0xC0,0x07,0x00,0x38,0x00,0x20,0x00},/*"V",54*/
{0x38,0x00,0x07,0xC0,0x3C,0x00,0x07,0xC0,0x38,0x00,0x00,0x00},/*"W",55*/
{0x20,0x40,0x39,0xC0,0x06,0x00,0x39,0xC0,0x20,0x40,0x00,0x00},/*"X",56*/
{0x20,0x00,0x38,0x40,0x07,0xC0,0x38,0x40,0x20,0x00,0x00,0x00},/*"Y",57*/
{0x30,0x40,0x21,0xC0,0x26,0x40,0x38,0x40,0x20,0xC0,0x00,0x00},/*"Z",58*/
{0x00,0x00,0x00,0x00,0x7F,0xE0,0x40,0x20,0x40,0x20,0x00,0x00},/*"[",59*/
{0x00,0x00,0x70,0x00,0x0C,0x00,0x03,0x80,0x00,0x40,0x00,0x00},/*"\",60*/
{0x00,0x00,0x40,0x20,0x40,0x20,0x7F,0xE0,0x00,0x00,0x00,0x00},/*"]",61*/
{0x00,0x00,0x20,0x00,0x40,0x00,0x20,0x00,0x00,0x00,0x00,0x00},/*"^",62*/
{0x00,0x10,0x00,0x10,0x00,0x10,0x00,0x10,0x00,0x10,0x00,0x10},/*"_",63*/
{0x00,0x00,0x00,0x00,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"`",64*/
{0x00,0x00,0x02,0x80,0x05,0x40,0x05,0x40,0x03,0xC0,0x00,0x40},/*"a",65*/
{0x20,0x00,0x3F,0xC0,0x04,0x40,0x04,0x40,0x03,0x80,0x00,0x00},/*"b",66*/
{0x00,0x00,0x03,0x80,0x04,0x40,0x04,0x40,0x06,0x40,0x00,0x00},/*"c",67*/
{0x00,0x00,0x03,0x80,0x04,0x40,0x24,0x40,0x3F,0xC0,0x00,0x40},/*"d",68*/
{0x00,0x00,0x03,0x80,0x05,0x40,0x05,0x40,0x03,0x40,0x00,0x00},/*"e",69*/
{0x00,0x00,0x04,0x40,0x1F,0xC0,0x24,0x40,0x24,0x40,0x20,0x00},/*"f",70*/
{0x00,0x00,0x02,0xE0,0x05,0x50,0x05,0x50,0x06,0x50,0x04,0x20},/*"g",71*/
{0x20,0x40,0x3F,0xC0,0x04,0x40,0x04,0x00,0x03,0xC0,0x00,0x40},/*"h",72*/
{0x00,0x00,0x04,0x40,0x27,0xC0,0x00,0x40,0x00,0x00,0x00,0x00},/*"i",73*/
{0x00,0x10,0x00,0x10,0x04,0x10,0x27,0xE0,0x00,0x00,0x00,0x00},/*"j",74*/
{0x20,0x40,0x3F,0xC0,0x01,0x40,0x07,0x00,0x04,0xC0,0x04,0x40},/*"k",75*/
{0x20,0x40,0x20,0x40,0x3F,0xC0,0x00,0x40,0x00,0x40,0x00,0x00},/*"l",76*/
{0x07,0xC0,0x04,0x00,0x07,0xC0,0x04,0x00,0x03,0xC0,0x00,0x00},/*"m",77*/
{0x04,0x40,0x07,0xC0,0x04,0x40,0x04,0x00,0x03,0xC0,0x00,0x40},/*"n",78*/
{0x00,0x00,0x03,0x80,0x04,0x40,0x04,0x40,0x03,0x80,0x00,0x00},/*"o",79*/
{0x04,0x10,0x07,0xF0,0x04,0x50,0x04,0x40,0x03,0x80,0x00,0x00},/*"p",80*/
{0x00,0x00,0x03,0x80,0x04,0x40,0x04,0x50,0x07,0xF0,0x00,0x10},/*"q",81*/
{0x04,0x40,0x07,0xC0,0x02,0x40,0x04,0x00,0x04,0x00,0x00,0x00},/*"r",82*/
{0x00,0x00,0x06,0x40,0x05,0x40,0x05,0x40,0x04,0xC0,0x00,0x00},/*"s",83*/
{0x00,0x00,0x04,0x00,0x1F,0x80,0x04,0x40,0x00,0x40,0x00,0x00},/*"t",84*/
{0x04,0x00,0x07,0x80,0x00,0x40,0x04,0x40,0x07,0xC0,0x00,0x40},/*"u",85*/
{0x04,0x00,0x07,0x00,0x04,0xC0,0x01,0x80,0x06,0x00,0x04,0x00},/*"v",86*/
{0x06,0x00,0x01,0xC0,0x07,0x00,0x01,0xC0,0x06,0x00,0x00,0x00},/*"w",87*/
{0x04,0x40,0x06,0xC0,0x01,0x00,0x06,0xC0,0x04,0x40,0x00,0x00},/*"x",88*/
{0x04,0x10,0x07,0x10,0x04,0xE0,0x01,0x80,0x06,0x00,0x04,0x00},/*"y",89*/
{0x00,0x00,0x04,0x40,0x05,0xC0,0x06,0x40,0x04,0x40,0x00,0x00},/*"z",90*/
{0x00,0x00,0x00,0x00,0x04,0x00,0x7B,0xE0,0x40,0x20,0x00,0x00},/*"{",91*/
{0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xF0,0x00,0x00,0x00,0x00},/*"|",92*/
{0x00,0x00,0x40,0x20,0x7B,0xE0,0x04,0x00,0x00,0x00,0x00,0x00},/*"}",93*/
{0x40,0x00,0x80,0x00,0x40,0x00,0x20,0x00,0x20,0x00,0x40,0x00},/*"~",94*/
};
//16*16 ASCII字符集点阵
static const unsigned char asc2_1608[95][16]={
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*" ",0*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xCC,0x00,0x0C,0x00,0x00,0x00,0x00,0x00,0x00},/*"!",1*/
{0x00,0x00,0x08,0x00,0x30,0x00,0x60,0x00,0x08,0x00,0x30,0x00,0x60,0x00,0x00,0x00},/*""",2*/
{0x02,0x20,0x03,0xFC,0x1E,0x20,0x02,0x20,0x03,0xFC,0x1E,0x20,0x02,0x20,0x00,0x00},/*"#",3*/
{0x00,0x00,0x0E,0x18,0x11,0x04,0x3F,0xFF,0x10,0x84,0x0C,0x78,0x00,0x00,0x00,0x00},/*"$",4*/
{0x0F,0x00,0x10,0x84,0x0F,0x38,0x00,0xC0,0x07,0x78,0x18,0x84,0x00,0x78,0x00,0x00},/*"%",5*/
{0x00,0x78,0x0F,0x84,0x10,0xC4,0x11,0x24,0x0E,0x98,0x00,0xE4,0x00,0x84,0x00,0x08},/*"&",6*/
{0x08,0x00,0x68,0x00,0x70,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"'",7*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xE0,0x18,0x18,0x20,0x04,0x40,0x02,0x00,0x00},/*"(",8*/
{0x00,0x00,0x40,0x02,0x20,0x04,0x18,0x18,0x07,0xE0,0x00,0x00,0x00,0x00,0x00,0x00},/*")",9*/
{0x02,0x40,0x02,0x40,0x01,0x80,0x0F,0xF0,0x01,0x80,0x02,0x40,0x02,0x40,0x00,0x00},/*"*",10*/
{0x00,0x80,0x00,0x80,0x00,0x80,0x0F,0xF8,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x00},/*"+",11*/
{0x00,0x01,0x00,0x0D,0x00,0x0E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*",",12*/
{0x00,0x00,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80},/*"-",13*/
{0x00,0x00,0x00,0x0C,0x00,0x0C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*".",14*/
{0x00,0x00,0x00,0x06,0x00,0x18,0x00,0x60,0x01,0x80,0x06,0x00,0x18,0x00,0x20,0x00},/*"/",15*/
{0x00,0x00,0x07,0xF0,0x08,0x08,0x10,0x04,0x10,0x04,0x08,0x08,0x07,0xF0,0x00,0x00},/*"0",16*/
{0x00,0x00,0x08,0x04,0x08,0x04,0x1F,0xFC,0x00,0x04,0x00,0x04,0x00,0x00,0x00,0x00},/*"1",17*/
{0x00,0x00,0x0E,0x0C,0x10,0x14,0x10,0x24,0x10,0x44,0x11,0x84,0x0E,0x0C,0x00,0x00},/*"2",18*/
{0x00,0x00,0x0C,0x18,0x10,0x04,0x11,0x04,0x11,0x04,0x12,0x88,0x0C,0x70,0x00,0x00},/*"3",19*/
{0x00,0x00,0x00,0xE0,0x03,0x20,0x04,0x24,0x08,0x24,0x1F,0xFC,0x00,0x24,0x00,0x00},/*"4",20*/
{0x00,0x00,0x1F,0x98,0x10,0x84,0x11,0x04,0x11,0x04,0x10,0x88,0x10,0x70,0x00,0x00},/*"5",21*/
{0x00,0x00,0x07,0xF0,0x08,0x88,0x11,0x04,0x11,0x04,0x18,0x88,0x00,0x70,0x00,0x00},/*"6",22*/
{0x00,0x00,0x1C,0x00,0x10,0x00,0x10,0xFC,0x13,0x00,0x1C,0x00,0x10,0x00,0x00,0x00},/*"7",23*/
{0x00,0x00,0x0E,0x38,0x11,0x44,0x10,0x84,0x10,0x84,0x11,0x44,0x0E,0x38,0x00,0x00},/*"8",24*/
{0x00,0x00,0x07,0x00,0x08,0x8C,0x10,0x44,0x10,0x44,0x08,0x88,0x07,0xF0,0x00,0x00},/*"9",25*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x0C,0x03,0x0C,0x00,0x00,0x00,0x00,0x00,0x00},/*":",26*/
{0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*";",27*/
{0x00,0x00,0x00,0x80,0x01,0x40,0x02,0x20,0x04,0x10,0x08,0x08,0x10,0x04,0x00,0x00},/*"<",28*/
{0x02,0x20,0x02,0x20,0x02,0x20,0x02,0x20,0x02,0x20,0x02,0x20,0x02,0x20,0x00,0x00},/*"=",29*/
{0x00,0x00,0x10,0x04,0x08,0x08,0x04,0x10,0x02,0x20,0x01,0x40,0x00,0x80,0x00,0x00},/*">",30*/
{0x00,0x00,0x0E,0x00,0x12,0x00,0x10,0x0C,0x10,0x6C,0x10,0x80,0x0F,0x00,0x00,0x00},/*"?",31*/
{0x03,0xE0,0x0C,0x18,0x13,0xE4,0x14,0x24,0x17,0xC4,0x08,0x28,0x07,0xD0,0x00,0x00},/*"@",32*/
{0x00,0x04,0x00,0x3C,0x03,0xC4,0x1C,0x40,0x07,0x40,0x00,0xE4,0x00,0x1C,0x00,0x04},/*"A",33*/
{0x10,0x04,0x1F,0xFC,0x11,0x04,0x11,0x04,0x11,0x04,0x0E,0x88,0x00,0x70,0x00,0x00},/*"B",34*/
{0x03,0xE0,0x0C,0x18,0x10,0x04,0x10,0x04,0x10,0x04,0x10,0x08,0x1C,0x10,0x00,0x00},/*"C",35*/
{0x10,0x04,0x1F,0xFC,0x10,0x04,0x10,0x04,0x10,0x04,0x08,0x08,0x07,0xF0,0x00,0x00},/*"D",36*/
{0x10,0x04,0x1F,0xFC,0x11,0x04,0x11,0x04,0x17,0xC4,0x10,0x04,0x08,0x18,0x00,0x00},/*"E",37*/
{0x10,0x04,0x1F,0xFC,0x11,0x04,0x11,0x00,0x17,0xC0,0x10,0x00,0x08,0x00,0x00,0x00},/*"F",38*/
{0x03,0xE0,0x0C,0x18,0x10,0x04,0x10,0x04,0x10,0x44,0x1C,0x78,0x00,0x40,0x00,0x00},/*"G",39*/
{0x10,0x04,0x1F,0xFC,0x10,0x84,0x00,0x80,0x00,0x80,0x10,0x84,0x1F,0xFC,0x10,0x04},/*"H",40*/
{0x00,0x00,0x10,0x04,0x10,0x04,0x1F,0xFC,0x10,0x04,0x10,0x04,0x00,0x00,0x00,0x00},/*"I",41*/
{0x00,0x03,0x00,0x01,0x10,0x01,0x10,0x01,0x1F,0xFE,0x10,0x00,0x10,0x00,0x00,0x00},/*"J",42*/
{0x10,0x04,0x1F,0xFC,0x11,0x04,0x03,0x80,0x14,0x64,0x18,0x1C,0x10,0x04,0x00,0x00},/*"K",43*/
{0x10,0x04,0x1F,0xFC,0x10,0x04,0x00,0x04,0x00,0x04,0x00,0x04,0x00,0x0C,0x00,0x00},/*"L",44*/
{0x10,0x04,0x1F,0xFC,0x1F,0x00,0x00,0xFC,0x1F,0x00,0x1F,0xFC,0x10,0x04,0x00,0x00},/*"M",45*/
{0x10,0x04,0x1F,0xFC,0x0C,0x04,0x03,0x00,0x00,0xE0,0x10,0x18,0x1F,0xFC,0x10,0x00},/*"N",46*/
{0x07,0xF0,0x08,0x08,0x10,0x04,0x10,0x04,0x10,0x04,0x08,0x08,0x07,0xF0,0x00,0x00},/*"O",47*/
{0x10,0x04,0x1F,0xFC,0x10,0x84,0x10,0x80,0x10,0x80,0x10,0x80,0x0F,0x00,0x00,0x00},/*"P",48*/
{0x07,0xF0,0x08,0x18,0x10,0x24,0x10,0x24,0x10,0x1C,0x08,0x0A,0x07,0xF2,0x00,0x00},/*"Q",49*/
{0x10,0x04,0x1F,0xFC,0x11,0x04,0x11,0x00,0x11,0xC0,0x11,0x30,0x0E,0x0C,0x00,0x04},/*"R",50*/
{0x00,0x00,0x0E,0x1C,0x11,0x04,0x10,0x84,0x10,0x84,0x10,0x44,0x1C,0x38,0x00,0x00},/*"S",51*/
{0x18,0x00,0x10,0x00,0x10,0x04,0x1F,0xFC,0x10,0x04,0x10,0x00,0x18,0x00,0x00,0x00},/*"T",52*/
{0x10,0x00,0x1F,0xF8,0x10,0x04,0x00,0x04,0x00,0x04,0x10,0x04,0x1F,0xF8,0x10,0x00},/*"U",53*/
{0x10,0x00,0x1E,0x00,0x11,0xE0,0x00,0x1C,0x00,0x70,0x13,0x80,0x1C,0x00,0x10,0x00},/*"V",54*/
{0x1F,0xC0,0x10,0x3C,0x00,0xE0,0x1F,0x00,0x00,0xE0,0x10,0x3C,0x1F,0xC0,0x00,0x00},/*"W",55*/
{0x10,0x04,0x18,0x0C,0x16,0x34,0x01,0xC0,0x01,0xC0,0x16,0x34,0x18,0x0C,0x10,0x04},/*"X",56*/
{0x10,0x00,0x1C,0x00,0x13,0x04,0x00,0xFC,0x13,0x04,0x1C,0x00,0x10,0x00,0x00,0x00},/*"Y",57*/
{0x08,0x04,0x10,0x1C,0x10,0x64,0x10,0x84,0x13,0x04,0x1C,0x04,0x10,0x18,0x00,0x00},/*"Z",58*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0xFE,0x40,0x02,0x40,0x02,0x40,0x02,0x00,0x00},/*"[",59*/
{0x00,0x00,0x30,0x00,0x0C,0x00,0x03,0x80,0x00,0x60,0x00,0x1C,0x00,0x03,0x00,0x00},/*"\",60*/
{0x00,0x00,0x40,0x02,0x40,0x02,0x40,0x02,0x7F,0xFE,0x00,0x00,0x00,0x00,0x00,0x00},/*"]",61*/
{0x00,0x00,0x00,0x00,0x20,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x20,0x00,0x00,0x00},/*"^",62*/
{0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01},/*"_",63*/
{0x00,0x00,0x40,0x00,0x40,0x00,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"`",64*/
{0x00,0x00,0x00,0x98,0x01,0x24,0x01,0x44,0x01,0x44,0x01,0x44,0x00,0xFC,0x00,0x04},/*"a",65*/
{0x10,0x00,0x1F,0xFC,0x00,0x88,0x01,0x04,0x01,0x04,0x00,0x88,0x00,0x70,0x00,0x00},/*"b",66*/
{0x00,0x00,0x00,0x70,0x00,0x88,0x01,0x04,0x01,0x04,0x01,0x04,0x00,0x88,0x00,0x00},/*"c",67*/
{0x00,0x00,0x00,0x70,0x00,0x88,0x01,0x04,0x01,0x04,0x11,0x08,0x1F,0xFC,0x00,0x04},/*"d",68*/
{0x00,0x00,0x00,0xF8,0x01,0x44,0x01,0x44,0x01,0x44,0x01,0x44,0x00,0xC8,0x00,0x00},/*"e",69*/
{0x00,0x00,0x01,0x04,0x01,0x04,0x0F,0xFC,0x11,0x04,0x11,0x04,0x11,0x00,0x18,0x00},/*"f",70*/
{0x00,0x00,0x00,0xD6,0x01,0x29,0x01,0x29,0x01,0x29,0x01,0xC9,0x01,0x06,0x00,0x00},/*"g",71*/
{0x10,0x04,0x1F,0xFC,0x00,0x84,0x01,0x00,0x01,0x00,0x01,0x04,0x00,0xFC,0x00,0x04},/*"h",72*/
{0x00,0x00,0x01,0x04,0x19,0x04,0x19,0xFC,0x00,0x04,0x00,0x04,0x00,0x00,0x00,0x00},/*"i",73*/
{0x00,0x00,0x00,0x03,0x00,0x01,0x01,0x01,0x19,0x01,0x19,0xFE,0x00,0x00,0x00,0x00},/*"j",74*/
{0x10,0x04,0x1F,0xFC,0x00,0x24,0x00,0x40,0x01,0xB4,0x01,0x0C,0x01,0x04,0x00,0x00},/*"k",75*/
{0x00,0x00,0x10,0x04,0x10,0x04,0x1F,0xFC,0x00,0x04,0x00,0x04,0x00,0x00,0x00,0x00},/*"l",76*/
{0x01,0x04,0x01,0xFC,0x01,0x04,0x01,0x00,0x01,0xFC,0x01,0x04,0x01,0x00,0x00,0xFC},/*"m",77*/
{0x01,0x04,0x01,0xFC,0x00,0x84,0x01,0x00,0x01,0x00,0x01,0x04,0x00,0xFC,0x00,0x04},/*"n",78*/
{0x00,0x00,0x00,0xF8,0x01,0x04,0x01,0x04,0x01,0x04,0x01,0x04,0x00,0xF8,0x00,0x00},/*"o",79*/
{0x01,0x01,0x01,0xFF,0x00,0x85,0x01,0x04,0x01,0x04,0x00,0x88,0x00,0x70,0x00,0x00},/*"p",80*/
{0x00,0x00,0x00,0x70,0x00,0x88,0x01,0x04,0x01,0x04,0x01,0x05,0x01,0xFF,0x00,0x01},/*"q",81*/
{0x01,0x04,0x01,0x04,0x01,0xFC,0x00,0x84,0x01,0x04,0x01,0x00,0x01,0x80,0x00,0x00},/*"r",82*/
{0x00,0x00,0x00,0xCC,0x01,0x24,0x01,0x24,0x01,0x24,0x01,0x24,0x01,0x98,0x00,0x00},/*"s",83*/
{0x00,0x00,0x01,0x00,0x01,0x00,0x07,0xF8,0x01,0x04,0x01,0x04,0x00,0x00,0x00,0x00},/*"t",84*/
{0x01,0x00,0x01,0xF8,0x00,0x04,0x00,0x04,0x00,0x04,0x01,0x08,0x01,0xFC,0x00,0x04},/*"u",85*/
{0x01,0x00,0x01,0x80,0x01,0x70,0x00,0x0C,0x00,0x10,0x01,0x60,0x01,0x80,0x01,0x00},/*"v",86*/
{0x01,0xF0,0x01,0x0C,0x00,0x30,0x01,0xC0,0x00,0x30,0x01,0x0C,0x01,0xF0,0x01,0x00},/*"w",87*/
{0x00,0x00,0x01,0x04,0x01,0x8C,0x00,0x74,0x01,0x70,0x01,0x8C,0x01,0x04,0x00,0x00},/*"x",88*/
{0x01,0x01,0x01,0x81,0x01,0x71,0x00,0x0E,0x00,0x18,0x01,0x60,0x01,0x80,0x01,0x00},/*"y",89*/
{0x00,0x00,0x01,0x84,0x01,0x0C,0x01,0x34,0x01,0x44,0x01,0x84,0x01,0x0C,0x00,0x00},/*"z",90*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x3E,0xFC,0x40,0x02,0x40,0x02},/*"{",91*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00},/*"|",92*/
{0x00,0x00,0x40,0x02,0x40,0x02,0x3E,0xFC,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"}",93*/
{0x00,0x00,0x60,0x00,0x80,0x00,0x80,0x00,0x40,0x00,0x40,0x00,0x20,0x00,0x20,0x00},/*"~",94*/
};
//24*24 ASICII字符集点阵
static const unsigned char asc2_2412[95][36]={
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*" ",0*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0x80,0x38,0x0F,0xFE,0x38,0x0F,0x80,0x38,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"!",1*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x06,0x00,0x00,0x0C,0x00,0x00,0x38,0x00,0x00,0x31,0x00,0x00,0x06,0x00,0x00,0x0C,0x00,0x00,0x38,0x00,0x00,0x30,0x00,0x00,0x00,0x00,0x00},/*""",2*/
{0x00,0x00,0x00,0x00,0x61,0x80,0x00,0x67,0xF8,0x07,0xF9,0x80,0x00,0x61,0x80,0x00,0x61,0x80,0x00,0x61,0x80,0x00,0x61,0x80,0x00,0x67,0xF8,0x07,0xF9,0x80,0x00,0x61,0x80,0x00,0x00,0x00},/*"#",3*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xC0,0xE0,0x03,0xE0,0xF0,0x06,0x30,0x08,0x04,0x18,0x08,0x1F,0xFF,0xFE,0x04,0x0E,0x08,0x07,0x87,0xF0,0x03,0x81,0xE0,0x00,0x00,0x00,0x00,0x00,0x00},/*"$",4*/
{0x01,0xF0,0x00,0x06,0x0C,0x00,0x04,0x04,0x08,0x06,0x0C,0x70,0x01,0xF9,0xC0,0x00,0x0E,0x00,0x00,0x3B,0xE0,0x00,0xEC,0x18,0x07,0x08,0x08,0x04,0x0C,0x18,0x00,0x03,0xE0,0x00,0x00,0x00},/*"%",5*/
{0x00,0x01,0xE0,0x00,0x07,0xF0,0x03,0xF8,0x18,0x04,0x1C,0x08,0x04,0x17,0x08,0x07,0xE1,0xD0,0x03,0xC0,0xE0,0x00,0x23,0xB0,0x00,0x3C,0x08,0x00,0x20,0x08,0x00,0x00,0x10,0x00,0x00,0x00},/*"&",6*/
{0x00,0x00,0x00,0x01,0x00,0x00,0x31,0x00,0x00,0x32,0x00,0x00,0x1C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"'",7*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0x00,0x01,0xFF,0xC0,0x07,0x80,0xF0,0x0C,0x00,0x18,0x10,0x00,0x04,0x20,0x00,0x02,0x00,0x00,0x00},/*"(",8*/
{0x00,0x00,0x00,0x20,0x00,0x02,0x10,0x00,0x04,0x0C,0x00,0x18,0x07,0x80,0xF0,0x01,0xFF,0xC0,0x00,0x7F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*")",9*/
{0x00,0x00,0x00,0x00,0x42,0x00,0x00,0x66,0x00,0x00,0x66,0x00,0x00,0x3C,0x00,0x00,0x18,0x00,0x03,0xFF,0xC0,0x00,0x18,0x00,0x00,0x3C,0x00,0x00,0x66,0x00,0x00,0x66,0x00,0x00,0x42,0x00},/*"*",10*/
{0x00,0x00,0x00,0x00,0x08,0x00,0x00,0x08,0x00,0x00,0x08,0x00,0x00,0x08,0x00,0x00,0x08,0x00,0x01,0xFF,0xC0,0x00,0x08,0x00,0x00,0x08,0x00,0x00,0x08,0x00,0x00,0x08,0x00,0x00,0x08,0x00},/*"+",11*/
{0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x31,0x00,0x00,0x32,0x00,0x00,0x1C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*",",12*/
{0x00,0x00,0x00,0x00,0x08,0x00,0x00,0x08,0x00,0x00,0x08,0x00,0x00,0x08,0x00,0x00,0x08,0x00,0x00,0x08,0x00,0x00,0x08,0x00,0x00,0x08,0x00,0x00,0x08,0x00,0x00,0x08,0x00,0x00,0x00,0x00},/*"-",13*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x38,0x00,0x00,0x38,0x00,0x00,0x38,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*".",14*/
{0x00,0x00,0x00,0x00,0x00,0x06,0x00,0x00,0x1C,0x00,0x00,0x70,0x00,0x01,0x80,0x00,0x0E,0x00,0x00,0x38,0x00,0x00,0xC0,0x00,0x07,0x00,0x00,0x1C,0x00,0x00,0x30,0x00,0x00,0x00,0x00,0x00},/*"/",15*/
{0x00,0x00,0x00,0x00,0x7F,0x80,0x01,0xFF,0xE0,0x03,0x80,0x70,0x06,0x00,0x18,0x04,0x00,0x08,0x04,0x00,0x08,0x06,0x00,0x18,0x03,0x80,0x70,0x01,0xFF,0xE0,0x00,0x7F,0x80,0x00,0x00,0x00},/*"0",16*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x08,0x01,0x00,0x08,0x01,0x00,0x08,0x03,0xFF,0xF8,0x07,0xFF,0xF8,0x00,0x00,0x08,0x00,0x00,0x08,0x00,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00},/*"1",17*/
{0x00,0x00,0x00,0x01,0xC0,0x38,0x02,0xC0,0x58,0x04,0x00,0x98,0x04,0x01,0x18,0x04,0x02,0x18,0x04,0x04,0x18,0x06,0x1C,0x18,0x03,0xF8,0x18,0x01,0xE0,0xF8,0x00,0x00,0x00,0x00,0x00,0x00},/*"2",18*/
{0x00,0x00,0x00,0x01,0xC0,0xE0,0x03,0xC0,0xF0,0x04,0x00,0x08,0x04,0x08,0x08,0x04,0x08,0x08,0x06,0x18,0x08,0x03,0xF4,0x18,0x01,0xE7,0xF0,0x00,0x01,0xE0,0x00,0x00,0x00,0x00,0x00,0x00},/*"3",19*/
{0x00,0x00,0x00,0x00,0x03,0x00,0x00,0x0D,0x00,0x00,0x11,0x00,0x00,0x61,0x00,0x00,0x81,0x08,0x03,0x01,0x08,0x07,0xFF,0xF8,0x0F,0xFF,0xF8,0x00,0x01,0x08,0x00,0x01,0x08,0x00,0x00,0x00},/*"4",20*/
{0x00,0x00,0x00,0x00,0x00,0xE0,0x07,0xFC,0xD0,0x06,0x08,0x08,0x06,0x10,0x08,0x06,0x10,0x08,0x06,0x10,0x08,0x06,0x18,0x38,0x06,0x0F,0xF0,0x06,0x07,0xC0,0x00,0x00,0x00,0x00,0x00,0x00},/*"5",21*/
{0x00,0x00,0x00,0x00,0x3F,0x80,0x01,0xFF,0xE0,0x03,0x84,0x30,0x02,0x08,0x18,0x04,0x10,0x08,0x04,0x10,0x08,0x04,0x10,0x08,0x07,0x18,0x10,0x03,0x0F,0xF0,0x00,0x07,0xC0,0x00,0x00,0x00},/*"6",22*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xC0,0x00,0x07,0x00,0x00,0x06,0x00,0x00,0x06,0x00,0xF8,0x06,0x07,0xF8,0x06,0x18,0x00,0x06,0xE0,0x00,0x07,0x00,0x00,0x06,0x00,0x00,0x00,0x00,0x00},/*"7",23*/
{0x00,0x00,0x00,0x01,0xE1,0xE0,0x03,0xF7,0xF0,0x06,0x34,0x10,0x04,0x18,0x08,0x04,0x18,0x08,0x04,0x0C,0x08,0x04,0x0C,0x08,0x06,0x16,0x18,0x03,0xF3,0xF0,0x01,0xC1,0xE0,0x00,0x00,0x00},/*"8",24*/
{0x00,0x00,0x00,0x00,0xF8,0x00,0x03,0xFC,0x30,0x03,0x06,0x38,0x04,0x02,0x08,0x04,0x02,0x08,0x04,0x02,0x08,0x04,0x04,0x10,0x03,0x08,0xF0,0x01,0xFF,0xC0,0x00,0x7F,0x00,0x00,0x00,0x00},/*"9",25*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x70,0x38,0x00,0x70,0x38,0x00,0x70,0x38,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*":",26*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x1A,0x00,0x30,0x1C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*";",27*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x00,0x14,0x00,0x00,0x22,0x00,0x00,0x41,0x00,0x00,0x80,0x80,0x01,0x00,0x40,0x02,0x00,0x20,0x04,0x00,0x10,0x08,0x00,0x08,0x00,0x00,0x00},/*"<",28*/
{0x00,0x00,0x00,0x00,0x21,0x00,0x00,0x21,0x00,0x00,0x21,0x00,0x00,0x21,0x00,0x00,0x21,0x00,0x00,0x21,0x00,0x00,0x21,0x00,0x00,0x21,0x00,0x00,0x21,0x00,0x00,0x21,0x00,0x00,0x00,0x00},/*"=",29*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x08,0x04,0x00,0x10,0x02,0x00,0x20,0x01,0x00,0x40,0x00,0x80,0x80,0x00,0x41,0x00,0x00,0x22,0x00,0x00,0x14,0x00,0x00,0x08,0x00,0x00,0x00,0x00},/*">",30*/
{0x00,0x00,0x00,0x03,0xC0,0x00,0x04,0xC0,0x00,0x04,0x00,0x00,0x08,0x00,0x38,0x08,0x0F,0x38,0x08,0x08,0x38,0x08,0x10,0x00,0x0C,0x30,0x00,0x07,0xE0,0x00,0x03,0xC0,0x00,0x00,0x00,0x00},/*"?",31*/
{0x00,0x00,0x00,0x00,0x3F,0x80,0x00,0xFF,0xE0,0x03,0x80,0x70,0x02,0x0F,0x10,0x06,0x70,0x88,0x04,0xC0,0x88,0x04,0x83,0x08,0x04,0x7F,0x88,0x02,0xC0,0x90,0x03,0x01,0x20,0x00,0xFE,0x40},/*"@",32*/
{0x00,0x00,0x08,0x00,0x00,0x18,0x00,0x01,0xF8,0x00,0x3E,0x08,0x01,0xC2,0x00,0x07,0x02,0x00,0x07,0xE2,0x00,0x00,0xFE,0x00,0x00,0x1F,0xC8,0x00,0x01,0xF8,0x00,0x00,0x38,0x00,0x00,0x08},/*"A",33*/
{0x04,0x00,0x08,0x07,0xFF,0xF8,0x07,0xFF,0xF8,0x04,0x08,0x08,0x04,0x08,0x08,0x04,0x08,0x08,0x04,0x08,0x08,0x06,0x18,0x08,0x03,0xF4,0x18,0x01,0xE7,0xF0,0x00,0x01,0xE0,0x00,0x00,0x00},/*"B",34*/
{0x00,0x00,0x00,0x00,0x3F,0x80,0x01,0xFF,0xE0,0x03,0x80,0x70,0x02,0x00,0x18,0x04,0x00,0x08,0x04,0x00,0x08,0x04,0x00,0x08,0x04,0x00,0x10,0x06,0x00,0x20,0x07,0x80,0xC0,0x00,0x00,0x00},/*"C",35*/
{0x04,0x00,0x08,0x07,0xFF,0xF8,0x07,0xFF,0xF8,0x04,0x00,0x08,0x04,0x00,0x08,0x04,0x00,0x08,0x04,0x00,0x18,0x02,0x00,0x10,0x03,0x80,0x70,0x01,0xFF,0xE0,0x00,0x7F,0x80,0x00,0x00,0x00},/*"D",36*/
{0x04,0x00,0x08,0x07,0xFF,0xF8,0x07,0xFF,0xF8,0x04,0x08,0x08,0x04,0x08,0x08,0x04,0x08,0x08,0x04,0x08,0x08,0x04,0x3E,0x08,0x04,0x00,0x08,0x06,0x00,0x18,0x01,0x00,0x60,0x00,0x00,0x00},/*"E",37*/
{0x04,0x00,0x08,0x07,0xFF,0xF8,0x07,0xFF,0xF8,0x04,0x08,0x08,0x04,0x08,0x00,0x04,0x08,0x00,0x04,0x08,0x00,0x04,0x3E,0x00,0x06,0x00,0x00,0x06,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00},/*"F",38*/
{0x00,0x00,0x00,0x00,0x3F,0x80,0x01,0xFF,0xE0,0x03,0x80,0x70,0x06,0x00,0x18,0x04,0x00,0x08,0x04,0x02,0x08,0x04,0x02,0x08,0x02,0x03,0xF0,0x07,0x83,0xF0,0x00,0x02,0x00,0x00,0x02,0x00},/*"G",39*/
{0x04,0x00,0x08,0x07,0xFF,0xF8,0x07,0xFF,0xF8,0x04,0x08,0x08,0x00,0x08,0x00,0x00,0x08,0x00,0x00,0x08,0x00,0x00,0x08,0x00,0x04,0x08,0x08,0x07,0xFF,0xF8,0x07,0xFF,0xF8,0x04,0x00,0x08},/*"H",40*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x08,0x04,0x00,0x08,0x04,0x00,0x08,0x07,0xFF,0xF8,0x07,0xFF,0xF8,0x04,0x00,0x08,0x04,0x00,0x08,0x04,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00},/*"I",41*/
{0x00,0x00,0x00,0x00,0x00,0x06,0x00,0x00,0x07,0x00,0x00,0x01,0x04,0x00,0x01,0x04,0x00,0x01,0x04,0x00,0x03,0x07,0xFF,0xFE,0x07,0xFF,0xFC,0x04,0x00,0x00,0x04,0x00,0x00,0x04,0x00,0x00},/*"J",42*/
{0x04,0x00,0x08,0x07,0xFF,0xF8,0x07,0xFF,0xF8,0x04,0x0C,0x08,0x00,0x18,0x00,0x00,0x3E,0x00,0x04,0xC7,0x80,0x05,0x03,0xC8,0x06,0x00,0xF8,0x04,0x00,0x38,0x04,0x00,0x18,0x00,0x00,0x08},/*"K",43*/
{0x04,0x00,0x08,0x07,0xFF,0xF8,0x07,0xFF,0xF8,0x04,0x00,0x08,0x00,0x00,0x08,0x00,0x00,0x08,0x00,0x00,0x08,0x00,0x00,0x08,0x00,0x00,0x08,0x00,0x00,0x18,0x00,0x00,0x60,0x00,0x00,0x00},/*"L",44*/
{0x04,0x00,0x08,0x07,0xFF,0xF8,0x07,0x80,0x08,0x07,0xFC,0x00,0x00,0x7F,0xC0,0x00,0x03,0xF8,0x00,0x07,0xC0,0x00,0x78,0x00,0x07,0x80,0x08,0x07,0xFF,0xF8,0x07,0xFF,0xF8,0x04,0x00,0x08},/*"M",45*/
{0x04,0x00,0x08,0x07,0xFF,0xF8,0x07,0x00,0x08,0x03,0xC0,0x00,0x00,0xE0,0x00,0x00,0x38,0x00,0x00,0x1E,0x00,0x00,0x07,0x00,0x00,0x01,0xC0,0x04,0x00,0xF0,0x07,0xFF,0xF8,0x04,0x00,0x00},/*"N",46*/
{0x00,0x00,0x00,0x00,0x7F,0x80,0x01,0xFF,0xE0,0x03,0x80,0x70,0x06,0x00,0x18,0x04,0x00,0x08,0x04,0x00,0x08,0x06,0x00,0x18,0x03,0x00,0x30,0x01,0xFF,0xE0,0x00,0x7F,0x80,0x00,0x00,0x00},/*"O",47*/
{0x04,0x00,0x08,0x07,0xFF,0xF8,0x07,0xFF,0xF8,0x04,0x04,0x08,0x04,0x04,0x00,0x04,0x04,0x00,0x04,0x04,0x00,0x04,0x04,0x00,0x06,0x0C,0x00,0x03,0xF8,0x00,0x01,0xF0,0x00,0x00,0x00,0x00},/*"P",48*/
{0x00,0x00,0x00,0x00,0x7F,0x80,0x01,0xFF,0xE0,0x03,0x80,0x70,0x06,0x00,0x88,0x04,0x00,0x88,0x04,0x00,0xC8,0x06,0x00,0x3C,0x03,0x00,0x3E,0x01,0xFF,0xE6,0x00,0x7F,0x84,0x00,0x00,0x00},/*"Q",49*/
{0x04,0x00,0x08,0x07,0xFF,0xF8,0x07,0xFF,0xF8,0x04,0x08,0x08,0x04,0x08,0x00,0x04,0x0C,0x00,0x04,0x0F,0x00,0x04,0x0B,0xC0,0x06,0x10,0xF0,0x03,0xF0,0x38,0x01,0xE0,0x08,0x00,0x00,0x08},/*"R",50*/
{0x00,0x00,0x00,0x01,0xE0,0xF8,0x03,0xF0,0x30,0x06,0x30,0x10,0x04,0x18,0x08,0x04,0x18,0x08,0x04,0x0C,0x08,0x04,0x0C,0x08,0x02,0x06,0x18,0x02,0x07,0xF0,0x07,0x81,0xE0,0x00,0x00,0x00},/*"S",51*/
{0x01,0x80,0x00,0x06,0x00,0x00,0x04,0x00,0x00,0x04,0x00,0x00,0x04,0x00,0x08,0x07,0xFF,0xF8,0x07,0xFF,0xF8,0x04,0x00,0x08,0x04,0x00,0x00,0x04,0x00,0x00,0x06,0x00,0x00,0x01,0x80,0x00},/*"T",52*/
{0x04,0x00,0x00,0x07,0xFF,0xE0,0x07,0xFF,0xF0,0x04,0x00,0x18,0x00,0x00,0x08,0x00,0x00,0x08,0x00,0x00,0x08,0x00,0x00,0x08,0x00,0x00,0x08,0x04,0x00,0x10,0x07,0xFF,0xE0,0x04,0x00,0x00},/*"U",53*/
{0x04,0x00,0x00,0x06,0x00,0x00,0x07,0xE0,0x00,0x07,0xFE,0x00,0x04,0x1F,0xE0,0x00,0x01,0xF8,0x00,0x00,0x38,0x00,0x01,0xE0,0x04,0x3E,0x00,0x07,0xC0,0x00,0x06,0x00,0x00,0x04,0x00,0x00},/*"V",54*/
{0x04,0x00,0x00,0x07,0xE0,0x00,0x07,0xFF,0xC0,0x04,0x1F,0xF8,0x00,0x07,0xC0,0x07,0xF8,0x00,0x07,0xFF,0x80,0x04,0x3F,0xF8,0x00,0x07,0xC0,0x04,0xF8,0x00,0x07,0x00,0x00,0x04,0x00,0x00},/*"W",55*/
{0x00,0x00,0x00,0x04,0x00,0x08,0x06,0x00,0x18,0x07,0xC0,0x78,0x05,0xF1,0xC8,0x00,0x3E,0x00,0x00,0x1F,0x80,0x04,0x63,0xE8,0x07,0x80,0xF8,0x06,0x00,0x18,0x04,0x00,0x08,0x00,0x00,0x00},/*"X",56*/
{0x04,0x00,0x00,0x06,0x00,0x00,0x07,0x80,0x00,0x07,0xE0,0x08,0x04,0x7C,0x08,0x00,0x1F,0xF8,0x00,0x07,0xF8,0x00,0x18,0x08,0x04,0xE0,0x08,0x07,0x00,0x00,0x06,0x00,0x00,0x04,0x00,0x00},/*"Y",57*/
{0x00,0x00,0x00,0x01,0x00,0x08,0x06,0x00,0x38,0x04,0x00,0xF8,0x04,0x03,0xE8,0x04,0x0F,0x08,0x04,0x7C,0x08,0x05,0xF0,0x08,0x07,0xC0,0x08,0x07,0x00,0x18,0x04,0x00,0x60,0x00,0x00,0x00},/*"Z",58*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0xFF,0xFE,0x20,0x00,0x02,0x20,0x00,0x02,0x20,0x00,0x02,0x20,0x00,0x02,0x20,0x00,0x02,0x00,0x00,0x00},/*"[",59*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x00,0x07,0x00,0x00,0x00,0xC0,0x00,0x00,0x38,0x00,0x00,0x06,0x00,0x00,0x01,0xC0,0x00,0x00,0x30,0x00,0x00,0x0E,0x00,0x00,0x01,0x00,0x00,0x00},/*"\",60*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x00,0x02,0x20,0x00,0x02,0x20,0x00,0x02,0x20,0x00,0x02,0x20,0x00,0x02,0x3F,0xFF,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"]",61*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x00,0x10,0x00,0x00,0x30,0x00,0x00,0x20,0x00,0x00,0x30,0x00,0x00,0x10,0x00,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"^",62*/
{0x00,0x00,0x01,0x00,0x00,0x01,0x00,0x00,0x01,0x00,0x00,0x01,0x00,0x00,0x01,0x00,0x00,0x01,0x00,0x00,0x01,0x00,0x00,0x01,0x00,0x00,0x01,0x00,0x00,0x01,0x00,0x00,0x01,0x00,0x00,0x01},/*"_",63*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x00,0x00,0x20,0x00,0x00,0x10,0x00,0x00,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"`",64*/
{0x00,0x00,0x00,0x00,0x00,0xF0,0x00,0x19,0xF8,0x00,0x1B,0x18,0x00,0x22,0x08,0x00,0x26,0x08,0x00,0x24,0x08,0x00,0x24,0x10,0x00,0x3F,0xF8,0x00,0x1F,0xF8,0x00,0x00,0x08,0x00,0x00,0x18},/*"a",65*/
{0x00,0x00,0x00,0x04,0x00,0x00,0x07,0xFF,0xF8,0x0F,0xFF,0xF0,0x00,0x18,0x18,0x00,0x10,0x08,0x00,0x20,0x08,0x00,0x20,0x08,0x00,0x30,0x18,0x00,0x1F,0xF0,0x00,0x0F,0xC0,0x00,0x00,0x00},/*"b",66*/
{0x00,0x00,0x00,0x00,0x07,0xC0,0x00,0x1F,0xF0,0x00,0x18,0x30,0x00,0x20,0x08,0x00,0x20,0x08,0x00,0x20,0x08,0x00,0x3C,0x08,0x00,0x1C,0x10,0x00,0x00,0x60,0x00,0x00,0x00,0x00,0x00,0x00},/*"c",67*/
{0x00,0x00,0x00,0x00,0x07,0xC0,0x00,0x1F,0xF0,0x00,0x38,0x18,0x00,0x20,0x08,0x00,0x20,0x08,0x00,0x20,0x08,0x04,0x10,0x10,0x07,0xFF,0xF8,0x0F,0xFF,0xF0,0x00,0x00,0x10,0x00,0x00,0x00},/*"d",68*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xC0,0x00,0x1F,0xF0,0x00,0x12,0x30,0x00,0x22,0x18,0x00,0x22,0x08,0x00,0x22,0x08,0x00,0x32,0x08,0x00,0x1E,0x10,0x00,0x0E,0x20,0x00,0x00,0x00},/*"e",69*/
{0x00,0x00,0x00,0x00,0x20,0x00,0x00,0x20,0x08,0x00,0x20,0x08,0x01,0xFF,0xF8,0x03,0xFF,0xF8,0x06,0x20,0x08,0x04,0x20,0x08,0x04,0x20,0x08,0x07,0x20,0x00,0x03,0x00,0x00,0x00,0x00,0x00},/*"f",70*/
{0x00,0x00,0x00,0x00,0x00,0x0E,0x00,0x0E,0x6E,0x00,0x1F,0xF3,0x00,0x31,0xB1,0x00,0x20,0xB1,0x00,0x20,0xB1,0x00,0x31,0x91,0x00,0x1F,0x13,0x00,0x2E,0x1E,0x00,0x20,0x0E,0x00,0x30,0x00},/*"g",71*/
{0x00,0x00,0x00,0x04,0x00,0x08,0x07,0xFF,0xF8,0x0F,0xFF,0xF8,0x00,0x10,0x08,0x00,0x20,0x00,0x00,0x20,0x00,0x00,0x20,0x08,0x00,0x3F,0xF8,0x00,0x1F,0xF8,0x00,0x00,0x08,0x00,0x00,0x00},/*"h",72*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x08,0x00,0x20,0x08,0x00,0x20,0x08,0x06,0x3F,0xF8,0x06,0x3F,0xF8,0x00,0x00,0x08,0x00,0x00,0x08,0x00,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00},/*"i",73*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x00,0x00,0x03,0x00,0x20,0x01,0x00,0x20,0x01,0x00,0x20,0x03,0x06,0x3F,0xFE,0x06,0x3F,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"j",74*/
{0x00,0x00,0x00,0x04,0x00,0x08,0x07,0xFF,0xF8,0x0F,0xFF,0xF8,0x00,0x01,0x88,0x00,0x03,0x00,0x00,0x2F,0xC0,0x00,0x38,0xF8,0x00,0x20,0x38,0x00,0x20,0x08,0x00,0x00,0x08,0x00,0x00,0x00},/*"k",75*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x08,0x04,0x00,0x08,0x04,0x00,0x08,0x07,0xFF,0xF8,0x0F,0xFF,0xF8,0x00,0x00,0x08,0x00,0x00,0x08,0x00,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00},/*"l",76*/
{0x00,0x20,0x08,0x00,0x3F,0xF8,0x00,0x3F,0xF8,0x00,0x10,0x08,0x00,0x20,0x00,0x00,0x3F,0xF8,0x00,0x3F,0xF8,0x00,0x10,0x08,0x00,0x20,0x00,0x00,0x3F,0xF8,0x00,0x3F,0xF8,0x00,0x00,0x08},/*"m",77*/
{0x00,0x00,0x00,0x00,0x20,0x08,0x00,0x3F,0xF8,0x00,0x3F,0xF8,0x00,0x10,0x08,0x00,0x10,0x00,0x00,0x20,0x00,0x00,0x20,0x08,0x00,0x3F,0xF8,0x00,0x1F,0xF8,0x00,0x00,0x08,0x00,0x00,0x00},/*"n",78*/
{0x00,0x00,0x00,0x00,0x07,0xC0,0x00,0x0F,0xF0,0x00,0x18,0x30,0x00,0x30,0x08,0x00,0x20,0x08,0x00,0x20,0x08,0x00,0x30,0x08,0x00,0x18,0x30,0x00,0x0F,0xF0,0x00,0x07,0xC0,0x00,0x00,0x00},/*"o",79*/
{0x00,0x00,0x00,0x00,0x20,0x01,0x00,0x3F,0xFF,0x00,0x3F,0xFF,0x00,0x10,0x11,0x00,0x20,0x09,0x00,0x20,0x08,0x00,0x20,0x08,0x00,0x30,0x38,0x00,0x1F,0xF0,0x00,0x0F,0xC0,0x00,0x00,0x00},/*"p",80*/
{0x00,0x00,0x00,0x00,0x07,0xC0,0x00,0x1F,0xF0,0x00,0x38,0x18,0x00,0x20,0x08,0x00,0x20,0x08,0x00,0x20,0x09,0x00,0x10,0x11,0x00,0x1F,0xFF,0x00,0x3F,0xFF,0x00,0x00,0x01,0x00,0x00,0x00},/*"q",81*/
{0x00,0x20,0x08,0x00,0x20,0x08,0x00,0x20,0x08,0x00,0x3F,0xF8,0x00,0x3F,0xF8,0x00,0x08,0x08,0x00,0x10,0x08,0x00,0x20,0x08,0x00,0x20,0x00,0x00,0x30,0x00,0x00,0x30,0x00,0x00,0x00,0x00},/*"r",82*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0C,0x78,0x00,0x1E,0x18,0x00,0x33,0x08,0x00,0x23,0x08,0x00,0x21,0x08,0x00,0x21,0x88,0x00,0x21,0x98,0x00,0x30,0xF0,0x00,0x38,0x60,0x00,0x00,0x00},/*"s",83*/
{0x00,0x00,0x00,0x00,0x20,0x00,0x00,0x20,0x00,0x00,0x20,0x00,0x00,0xFF,0xF0,0x03,0xFF,0xF8,0x00,0x20,0x08,0x00,0x20,0x08,0x00,0x20,0x08,0x00,0x00,0x30,0x00,0x00,0x00,0x00,0x00,0x00},/*"t",84*/
{0x00,0x00,0x00,0x00,0x20,0x00,0x00,0x3F,0xF0,0x00,0x7F,0xF8,0x00,0x00,0x18,0x00,0x00,0x08,0x00,0x00,0x08,0x00,0x20,0x10,0x00,0x3F,0xF8,0x00,0x7F,0xF0,0x00,0x00,0x10,0x00,0x00,0x00},/*"u",85*/
{0x00,0x00,0x00,0x00,0x20,0x00,0x00,0x30,0x00,0x00,0x3C,0x00,0x00,0x3F,0x80,0x00,0x23,0xF0,0x00,0x00,0x78,0x00,0x00,0x70,0x00,0x23,0x80,0x00,0x3C,0x00,0x00,0x30,0x00,0x00,0x20,0x00},/*"v",86*/
{0x00,0x20,0x00,0x00,0x3C,0x00,0x00,0x3F,0xE0,0x00,0x23,0xF8,0x00,0x00,0xE0,0x00,0x27,0x00,0x00,0x3E,0x00,0x00,0x3F,0xE0,0x00,0x21,0xF8,0x00,0x01,0xE0,0x00,0x3E,0x00,0x00,0x20,0x00},/*"w",87*/
{0x00,0x00,0x00,0x00,0x20,0x08,0x00,0x20,0x08,0x00,0x38,0x38,0x00,0x3E,0x68,0x00,0x27,0x80,0x00,0x03,0xC8,0x00,0x2C,0xF8,0x00,0x38,0x38,0x00,0x20,0x18,0x00,0x20,0x08,0x00,0x00,0x00},/*"x",88*/
{0x00,0x00,0x00,0x00,0x20,0x00,0x00,0x30,0x03,0x00,0x3C,0x01,0x00,0x3F,0x83,0x00,0x23,0xEC,0x00,0x00,0x70,0x00,0x23,0x80,0x00,0x3C,0x00,0x00,0x20,0x00,0x00,0x20,0x00,0x00,0x00,0x00},/*"y",89*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x38,0x08,0x00,0x20,0x38,0x00,0x20,0xF8,0x00,0x23,0xE8,0x00,0x2F,0x88,0x00,0x3E,0x08,0x00,0x38,0x08,0x00,0x20,0x18,0x00,0x00,0x70,0x00,0x00,0x00},/*"z",90*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x00,0x14,0x00,0x1F,0xF7,0xFC,0x30,0x00,0x06,0x20,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00},/*"{",91*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"|",92*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x00,0x02,0x30,0x00,0x06,0x1F,0xF7,0xFC,0x00,0x14,0x00,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"}",93*/
{0x00,0x00,0x00,0x18,0x00,0x00,0x60,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x20,0x00,0x00,0x10,0x00,0x00,0x08,0x00,0x00,0x04,0x00,0x00,0x04,0x00,0x00,0x0C,0x00,0x00,0x10,0x00,0x00},/*"~",94*/
};
#endif