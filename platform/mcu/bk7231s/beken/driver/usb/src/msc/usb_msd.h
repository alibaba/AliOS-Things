#ifndef _USB_MSD_H_
#define _USB_MSD_H_

#define CFG_ENABLE_SYC_OP                1

/*******************************************************************************
* Function Declarations
*******************************************************************************/
extern int usb_sw_init(void);
extern int usb_sw_uninit(void);

#ifdef CFG_ENABLE_SYC_OP
extern uint32_t MUSB_HfiRead_sync( uint32_t first_block, uint32_t block_num, uint8_t *dest);
extern uint32_t MUSB_HfiWrite_sync( uint32_t first_block, uint32_t block_num, uint8_t *dest);
#endif // CFG_ENABLE_SYC_OP

#endif // _USB_MSD_H_

// EOF
