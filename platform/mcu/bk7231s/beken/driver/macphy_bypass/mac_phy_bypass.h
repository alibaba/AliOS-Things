#ifndef _MAC_PHY_BYPASSH_
#define _MAC_PHY_BYPASS_H_

#define MPB_ADDR_BASE                (0x01060000)

typedef union
{
	struct {
		VU32 bypass_en:1;
		VU32 resv0:3;
		VU32 mode:2;
		VU32 resv1:2;
		VU32 single_txen:1;
		VU32 resv2:23;		
	}bits;
	
	VU32 value;
}MPB_REG0x0_TypeDef;

typedef union
{
	struct {
		VU32 pre_rxdelay:20;
		VU32 resv2:12;		
	}bits;
	
	VU32 value;
}MPB_REG0x1_TypeDef;

typedef union
{
	struct {
		VU32 rxdelay:20;
		VU32 resv2:12;		
	}bits;
	
	VU32 value;
}MPB_REG0x2_TypeDef;

typedef union
{
	struct {
		VU32 pre_txdelay:20;
		VU32 resv2:12;		
	}bits;
	
	VU32 value;
}MPB_REG0x3_TypeDef;

typedef union
{
	struct {
		VU32 txdelay:20;
		VU32 resv2:12;		
	}bits;
	
	VU32 value;
}MPB_REG0x4_TypeDef;

typedef union
{
	struct {
		VU32 rxstat:20;
		VU32 resv2:12;		
	}bits;
	
	VU32 value;
}MPB_REG0x8_TypeDef;

typedef union
{
	struct {
		VU32 txstat:20;
		VU32 resv2:12;		
	}bits;
	
	VU32 value;
}MPB_REG0x9_TypeDef;

typedef union
{
	struct {
		VU32 rxerrstat:20;
		VU32 resv2:12;		
	}bits;
	
	VU32 value;
}MPB_REG0xA_TypeDef;

typedef union
{
	struct {
		VU32 txerrstat:20;
		VU32 resv2:12;		
	}bits;
	
	VU32 value;
}MPB_REG0xB_TypeDef;


typedef union
{
	struct {
		VU32 txvector0:8;
		VU32 resv1:24;		
	}bits;
	
	VU32 value;
}MPB_REG0x80_TypeDef;

typedef union
{
	struct {
		VU32 txvector1:8;
		VU32 resv1:24;		
	}bits;
	
	VU32 value;
}MPB_REG0x81_TypeDef;

typedef union
{
	struct {
		VU32 txvector2:8;
		VU32 resv1:24;		
	}bits;
	
	VU32 value;
}MPB_REG0x82_TypeDef;

typedef union
{
	struct {
		VU32 txvector3:8;
		VU32 resv1:24;		
	}bits;
	
	VU32 value;
}MPB_REG0x83_TypeDef;

typedef union
{
	struct {
		VU32 txvector4:8;
		VU32 resv1:24;		
	}bits;
	
	VU32 value;
}MPB_REG0x84_TypeDef;

typedef union
{
	struct {
		VU32 txvector5:8;
		VU32 resv1:24;		
	}bits;
	
	VU32 value;
}MPB_REG0x85_TypeDef;

typedef union
{
	struct {
		VU32 txvector6:8;
		VU32 resv1:24;		
	}bits;
	
	VU32 value;
}MPB_REG0x86_TypeDef;

typedef union
{
	struct {
		VU32 txvector7:8;
		VU32 resv1:24;		
	}bits;
	
	VU32 value;
}MPB_REG0x87_TypeDef;

typedef union
{
	struct {
		VU32 txvector8:8;
		VU32 resv1:24;		
	}bits;
	
	VU32 value;
}MPB_REG0x88_TypeDef;

typedef union
{
	struct {
		VU32 txvector9:8;
		VU32 resv1:24;		
	}bits;
	
	VU32 value;
}MPB_REG0x89_TypeDef;

typedef union
{
	struct {
		VU32 txvector10:8;
		VU32 resv1:24;		
	}bits;
	
	VU32 value;
}MPB_REG0x8A_TypeDef;

typedef union
{
	struct {
		VU32 txvector11:8;
		VU32 resv1:24;		
	}bits;
	
	VU32 value;
}MPB_REG0x8B_TypeDef;

typedef union
{
	struct {
		VU32 txvector12:8;
		VU32 resv1:24;		
	}bits;
	
	VU32 value;
}MPB_REG0x8C_TypeDef;

typedef union
{
	struct {
		VU32 txvector13:8;
		VU32 resv1:24;		
	}bits;
	
	VU32 value;
}MPB_REG0x8D_TypeDef;

typedef union
{
	struct {
		VU32 txvector14:8;
		VU32 resv1:24;		
	}bits;
	
	VU32 value;
}MPB_REG0x8E_TypeDef;

typedef union
{
	struct {
		VU32 txvector15:8;
		VU32 resv1:24;		
	}bits;
	
	VU32 value;
}MPB_REG0x8F_TypeDef;

struct MPB_TypeDef {
    volatile MPB_REG0x0_TypeDef  *r0;
    volatile MPB_REG0x1_TypeDef  *r1;
    volatile MPB_REG0x2_TypeDef  *r2;
    volatile MPB_REG0x3_TypeDef  *r3;
    volatile MPB_REG0x4_TypeDef  *r4;
    volatile MPB_REG0x8_TypeDef  *r8;
    volatile MPB_REG0x9_TypeDef  *r9;
    volatile MPB_REG0xA_TypeDef  *r10;    
    volatile MPB_REG0xB_TypeDef  *r11;
    
    volatile MPB_REG0x80_TypeDef  *r128;
    volatile MPB_REG0x81_TypeDef  *r129;
    volatile MPB_REG0x82_TypeDef  *r130;
    volatile MPB_REG0x83_TypeDef  *r131;
    volatile MPB_REG0x84_TypeDef  *r132;
    volatile MPB_REG0x85_TypeDef  *r133;
    volatile MPB_REG0x86_TypeDef  *r134;
    volatile MPB_REG0x87_TypeDef  *r135;
    volatile MPB_REG0x88_TypeDef  *r136;
    volatile MPB_REG0x89_TypeDef  *r137;
    volatile MPB_REG0x8A_TypeDef  *r138;
    volatile MPB_REG0x8B_TypeDef  *r139;
    volatile MPB_REG0x8C_TypeDef  *r140;
    volatile MPB_REG0x8D_TypeDef  *r141;
    volatile MPB_REG0x8E_TypeDef  *r142;
    volatile MPB_REG0x8F_TypeDef  *r143;
};

extern UINT32 mpb_ctrl(UINT32 cmd, void *param);
#endif // _MAC_PHY_BYPASS_H_

// eof
