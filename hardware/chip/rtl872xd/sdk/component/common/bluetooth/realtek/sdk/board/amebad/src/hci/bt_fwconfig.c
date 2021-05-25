



/** @brief configuration */
unsigned char rtlbt_config[] =
{
    0x55, 0xab, 0x23, 0x87,
	0x19, 0x00,
	0xD6, 0x00, 0x02, 0x8C, 0x8C, /*LE trx on delay*/  
};

/** @brief The length of configuration */
unsigned int rtlbt_config_len = sizeof(rtlbt_config);

extern unsigned char rtlbt_fw[];
const unsigned char *rltk_bt_get_normal_patch(void)
{
	return rtlbt_fw;
}
extern unsigned char rtlbt_mp_fw[];
const unsigned char *rltk_bt_get_mp_patch(void)
{
	return rtlbt_mp_fw;
}
