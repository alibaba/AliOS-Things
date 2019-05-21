/******************************************************************
 *                                                                *
 *        Copyright Mentor Graphics Corporation 2004              *
 *                                                                *
 *                All Rights Reserved.                            *
 *                                                                *
 *    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION *
 *  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS   *
 *  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                    *
 *                                                                *
 ******************************************************************/

/*********************************************************************
 ************************Device Descriptors*************************
 ********************************************************************/

/* Device descriptor for full speed Hub   */
uint8_t MGC_aFSpeedHubTestDeviceDscr[] =
{
    0x12,       /* bLength 18 byte  */
    0x01,       /* Descriptor Type Device 0x01  */
    0x00, 0x02, /* bcdUSB 0x0200 */
    0x09,       /* bDevice Class HUB 0x09 */
    0x00,       /* bDevice SubClass */
    0x00,       /* bDevice Protocol */
    0x40,       /* maximum packet size 64 byte  */
    0x09, 0x04, /* Vendor Id 0x0409 */
    0x59, 0x00, /* Product Id 0x0059  */
    0x00, 0x01, /* Device release number 0x0100      */
    0x00,       /* iManufacturer */
    0x00,       /* iProduct */
    0x00,       /* iSerialNumber */
    0x01        /* bNumConfiguration  */
};


/* Device descriptor for High Speed Hub with Single TT arangement.
 * bDeviceProtocol should be 0x01 and bInterfaceProtocol should be
 * 0x00.
 */
uint8_t MGC_aHSpeedSttHubTestDeviceDscr[] =
{
    0x12,       /* bLength 18 byte  */
    0x01,       /* Descriptor Type Device 0x01  */
    0x00, 0x02, /* bcdUSB 0x0200 */
    0x09,       /* bDevice Class HUB 0x09 */
    0x00,       /* bDevice SubClass */
    0x01,       /* bDevice Protocol */
    0x40,       /* maximum packet size 64 byte  */
    0x09, 0x04, /* Vendor Id 0x0409 */
    0x59, 0x00, /* Product Id 0x0059  */
    0x00, 0x01, /* Device release number 0x0100      */
    0x00,       /* iManufacturer */
    0x00,       /* iProduct */
    0x00,       /* iSerialNumber */
    0x01        /* bNumConfiguration  */
};


/* Device descriptor for High Speed Hub with multiple TT arangement.
 * For multiple TT bDeviceProtocol should be 0x02,
 * first interface has bInterfaceProtocol 0x01 and second will get 0x02
 */
uint8_t MGC_aHSpeedMttHubTestDeviceDscr[] =
{
    0x12,       /* bLength 18 byte  */
    0x01,       /* Descriptor Type Device 0x01  */
    0x00, 0x02, /* bcdUSB 0x0200 */
    0x09,       /* bDevice Class HUB 0x09 */
    0x00,       /* bDevice SubClass */
    0x02,       /* bDevice Protocol */
    0x40,       /* maximum packet size 64 byte  */
    0x09, 0x04, /* Vendor Id 0x0409 */
    0x59, 0x00, /* Product Id 0x0059  */
    0x00, 0x01, /* Device release number 0x0100      */
    0x00,       /* iManufacturer */
    0x00,       /* iProduct */
    0x00,       /* iSerialNumber */
    0x01        /* bNumConfiguration  */
};

/*********************************************************************
 ************************Device Descriptors End***********************
 *********************************************************************/



/*********************************************************************
 ************************Configuration Descriptors Start*************
 *********************************************************************/

/* Full speed Hub Configuration descriptor.
 * 1 interface
 * 1 alternate setting.
 * 1 Interrupt In Endpoint.
 * Local power supply
 */
uint8_t MGC_aFSHubTestConfig1s[] =
{
    /* Configuration descriptor   */
    0x09,       /* bLength 9 byte  */
    0x02,       /* Descriptor Type Config 0x02  */
    0x19, 0x00, /* wTotalLength  25 byte i.e. 0x0019  */
    0x01,       /* Number of interface  */
    0x01,       /* Configuration value. */
    0x00,       /* iConfiguration   */
    0xC0,       /* bmAttributes: Self power no remote wakeup  */
    0x32,       /* bMaxPower   */

    /*  Interface descriptor  */
    0x09,       /* bLength 9 byte  */
    0x04,       /* Descriptor Type Interface */
    0x00,       /* bInterfaceNumber 0x00*/
    0x00,       /* bAlternateSettingNumber 0x00*/
    0x01,       /* Number of endpoint 0x01  */
    0x09,       /* bInterface Class  HUB Class */
    0x00,       /* bInterface SubClass */
    0x00,       /* bInterface Protocol */
    0x00,       /* iInterface    */

    /* Endpoint descriptor  */
    0x07,       /* Descriptor Length */
    0x05,       /* Descriptor Type    */
    0x81,       /* Direction IN and Endpoint Number 0x01 */
    0x03,       /* Interrupt Endpoint, No synchronization, DataEndpoint. */
    0x01, 0x00, /* wMaxPacketSize 0x0001 byte */
    0xFF        /* Poling Interval */
};



/* Full speed Hub Configuration descriptor.
 * 1 interface
 * 1 alternate setting.
 * 1 Interrupt IN Endpoint.
 * 1 Bulk Endpoint
 * Local power supply
 */
uint8_t MGC_aFSHubTestConfig2s[] =
{
    /* Configuration descriptor   */
    0x09,       /* bLength 9 byte  */
    0x02,       /* Descriptor Type Config 0x02  */
    0x20, 0x00, /* wTotalLength  25 byte i.e. 0x0019  */
    0x01,       /* Number of interface  */
    0x01,       /* Configuration value. */
    0x00,       /* iConfiguration   */
    0xC0,       /* bmAttributes: Self power no remote wakeup  */
    0x32,       /* bMaxPower   */

    /*  Interface descriptor  */
    0x09,       /* bLength 9 byte  */
    0x04,       /* Descriptor Type Interface */
    0x00,       /* bInterfaceNumber 0x00*/
    0x00,       /* bAlternateSettingNumber 0x00*/
    0x02,       /* Number of endpoint 0x01  */
    0x09,       /* bInterface Class  HUB Class */
    0x00,       /* bInterface SubClass */
    0x00,       /* bInterface Protocol */
    0x00,       /* iInterface    */

    /* Interrupt In Endpoint descriptor  */
    0x07,       /* Descriptor Length */
    0x05,       /* Descriptor Type    */
    0x81,       /* Direction IN and Endpoint Number 0x01 */
    0x03,       /* Interrupt Endpoint, No synchronization, DataEndpoint. */
    0x01, 0x00, /* wMaxPacketSize 0x0001 byte */
    0xFF,        /* Poling Interval */

    /* Endpoint Descriptor  : Bulk-In */
    0x07,                                   /* bLength              */
    0x05,                                   /* ENDPOINT             */
    0x82,                                   /* bEndpointAddress     */
    0x02,                                   /* bmAttributes         */
    0x40, 0x00,                             /* wMaxPacketSize       */
    0x00                                    /* bInterval            */
};


/* Full speed Hub Configuration descriptor.
 * 1 interface
 * 1 alternate setting.
 * 1 Bulk Endpoint
 * 1 Interrupt IN Endpoint.
 * Local power supply
 */
uint8_t MGC_aFSHubTestConfig3s[] =
{
    /* Configuration descriptor   */
    0x09,       /* bLength 9 byte  */
    0x02,       /* Descriptor Type Config 0x02  */
    0x20, 0x00, /* wTotalLength  25 byte i.e. 0x0019  */
    0x01,       /* Number of interface  */
    0x01,       /* Configuration value. */
    0x00,       /* iConfiguration   */
    0xC0,       /* bmAttributes: Self power no remote wakeup  */
    0x32,       /* bMaxPower   */

    /*  Interface descriptor  */
    0x09,       /* bLength 9 byte  */
    0x04,       /* Descriptor Type Interface */
    0x00,       /* bInterfaceNumber 0x00*/
    0x00,       /* bAlternateSettingNumber 0x00*/
    0x02,       /* Number of endpoint 0x01  */
    0x09,       /* bInterface Class  HUB Class */
    0x00,       /* bInterface SubClass */
    0x00,       /* bInterface Protocol */
    0x00,       /* iInterface    */

    /* Endpoint Descriptor  : Bulk-In */
    0x07,                                   /* bLength              */
    0x05,                                   /* ENDPOINT             */
    0x82,                                   /* bEndpointAddress     */
    0x02,                                   /* bmAttributes         */
    0x40, 0x00,                             /* wMaxPacketSize       */
    0x00,                                   /* bInterval            */

    /* Interrupt In Endpoint descriptor  */
    0x07,       /* Descriptor Length */
    0x05,       /* Descriptor Type    */
    0x81,       /* Direction IN and Endpoint Number 0x01 */
    0x03,       /* Interrupt Endpoint, No synchronization, DataEndpoint. */
    0x01, 0x00, /* wMaxPacketSize 0x0001 byte */
    0xFF        /* Poling Interval */

};



/* Full speed Hub Configuration descriptor.
 * 1 interface
 * 1 alternate setting.
 * 1 Interrupt OUT Endpoint.
 * Local power supply
 */
uint8_t MGC_aFSHubTestConfig1e[] =
{
    /* Configuration descriptor   */
    0x09,       /* bLength 9 byte  */
    0x02,       /* Descriptor Type Config 0x02  */
    0x19, 0x00, /* wTotalLength  25 byte i.e. 0x0019  */
    0x01,       /* Number of interface  */
    0x01,       /* Configuration value. */
    0x00,       /* iConfiguration   */
    0xC0,       /* bmAttributes: Self power no remote wakeup  */
    0x32,       /* bMaxPower   */

    /*  Interface descriptor  */
    0x09,       /* bLength 9 byte  */
    0x04,       /* Descriptor Type Interface */
    0x00,       /* bInterfaceNumber 0x00*/
    0x00,       /* bAlternateSettingNumber 0x00*/
    0x01,       /* Number of endpoint 0x01  */
    0x09,       /* bInterface Class  HUB Class */
    0x00,       /* bInterface SubClass */
    0x00,       /* bInterface Protocol */
    0x00,       /* iInterface    */

    /* Interrupt Out Endpoint descriptor  */
    0x07,       /* Descriptor Length */
    0x05,       /* Descriptor Type    */
    0x01,       /* Direction Out and Endpoint Number 0x01 */
    0x03,       /* Interrupt Endpoint, No synchronization, DataEndpoint. */
    0x01, 0x00, /* wMaxPacketSize 0x0001 byte */
    0xFF        /* Poling Interval */

};


/* Full speed Hub Configuration descriptor.
 * 1 interface
 * 1 alternate setting.
 * 1 Interrupt OUT Endpoint.
 * 1 Bulk In Endpoint.
 * Local power supply
 */
uint8_t MGC_aFSHubTestConfig2e[] =
{
    /* Configuration descriptor   */
    0x09,       /* bLength 9 byte  */
    0x02,       /* Descriptor Type Config 0x02  */
    0x20, 0x00, /* wTotalLength  25 byte i.e. 0x0019  */
    0x01,       /* Number of interface  */
    0x01,       /* Configuration value. */
    0x00,       /* iConfiguration   */
    0xC0,       /* bmAttributes: Self power no remote wakeup  */
    0x32,       /* bMaxPower   */

    /*  Interface descriptor  */
    0x09,       /* bLength 9 byte  */
    0x04,       /* Descriptor Type Interface */
    0x00,       /* bInterfaceNumber 0x00*/
    0x00,       /* bAlternateSettingNumber 0x00*/
    0x02,       /* Number of endpoint 0x01  */
    0x09,       /* bInterface Class  HUB Class */
    0x00,       /* bInterface SubClass */
    0x00,       /* bInterface Protocol */
    0x00,       /* iInterface    */

    /* Interrupt Out Endpoint descriptor  */
    0x07,       /* Descriptor Length */
    0x05,       /* Descriptor Type    */
    0x01,       /* Direction Out and Endpoint Number 0x01 */
    0x03,       /* Interrupt Endpoint, No synchronization, DataEndpoint. */
    0x01, 0x00, /* wMaxPacketSize 0x0001 byte */
    0xFF,        /* Poling Interval */

    /* Endpoint Descriptor  : Bulk-In */
    0x07,                                   /* bLength              */
    0x05,                                   /* ENDPOINT             */
    0x82,                                   /* bEndpointAddress     */
    0x02,                                   /* bmAttributes         */
    0x40, 0x00,                             /* wMaxPacketSize       */
    0x00                                    /* bInterval            */
};


/* Full speed Hub Configuration descriptor.
 * 1 interface
 * 1 alternate setting. No endpoint present
 * Local power supply
 */
uint8_t MGC_aFSHubTestConfig3e[] =
{
    /* Configuration descriptor   */
    0x09,       /* bLength 9 byte  */
    0x02,       /* Descriptor Type Config 0x02  */
    0x12, 0x00, /* wTotalLength  25 byte i.e. 0x0019  */
    0x01,       /* Number of interface  */
    0x01,       /* Configuration value. */
    0x00,       /* iConfiguration   */
    0xC0,       /* bmAttributes: Self power no remote wakeup  */
    0x32,       /* bMaxPower   */

    /*  Interface descriptor  */
    0x09,       /* bLength 9 byte  */
    0x04,       /* Descriptor Type Interface */
    0x00,       /* bInterfaceNumber 0x00*/
    0x00,       /* bAlternateSettingNumber 0x00*/
    0x00,       /* Number of endpoint 0x01  */
    0x09,       /* bInterface Class  HUB Class */
    0x00,       /* bInterface SubClass */
    0x00,       /* bInterface Protocol */
    0x00       /* iInterface    */

};





/* High speed Hub Configuration descriptor.
 * 1 interface
 * 1 alternate setting. No endpoint present
 * Local power supply
 */
uint8_t MGC_aHSHubTestConfig1s[] =
{

    /* Configuration descriptor   */
    0x09,       /* bLength 9 byte  */
    0x02,       /* Descriptor Type Config 0x02  */
    0x19, 0x00, /* wTotalLength  25 byte i.e. 0x0019  */
    0x01,       /* Number of interface  */
    0x01,       /* Configuration value. */
    0x00,       /* iConfiguration   */
    0xC0,       /* bmAttributes: Self power no remote wakeup  */
    0x32,       /* bMaxPower   */

    /*  Interface descriptor  */
    0x09,       /* bLength 9 byte  */
    0x04,       /* Descriptor Type Interface */
    0x00,       /* bInterfaceNumber 0x00*/
    0x00,       /* bAlternateSettingNumber 0x00*/
    0x01,       /* Number of endpoint 0x01  */
    0x09,       /* bInterface Class  HUB Class */
    0x00,       /* bInterface SubClass */
    0x00,       /* bInterface Protocol */
    0x00,       /* iInterface    */

    /* Endpoint descriptor  */
    0x07,       /* Descriptor Length */
    0x05,       /* Descriptor Type    */
    0x81,       /* Direction IN and Endpoint Number 0x01 */
    0x03,       /* Interrupt Endpoint, No synchronization, DataEndpoint. */
    0x01, 0x00, /* wMaxPacketSize 0x0001 byte */
    0xFF        /* Poling Interval */
};


/*********************************************************************
 ************************Configuration Descriptors End*************
 *********************************************************************/



/*********************************************************************
 ************************Hub Descriptor Start*************************
 *********************************************************************/

/* Hub descriptor with
 * 1. Ganged Power switching
 * 2. Global Over current protection
 * 3. Number of ports 1.
 */
uint8_t MGC_aHubTestHubDscr1s[] =
{
    0x09,       /* bLength  */
    0x29,       /* Descriptor type Hub 0x29 */
    0x01,       /* bNumber of ports    0x01 */
    0xA0, 0x00, /* wHubCharecteristic 0x00A0 i.e. Ganged power switching,
                   Not a compound device, Global over current protection,
                  , TT require 16 FS bit times, Port indicator supported */
    0x32,       /* powerOnGoodTime is 50*2 ms */
    0x64,       /* Maximum current require by hub controller */
    0x00,       /* Device removable */
    0xFF        /* Port power control mask */
};


/* Hub descriptor with
 * 1. Ganged Power switching
 * 2. Global Over current protection
 * 3. Number of ports 7.
 */
uint8_t MGC_aHubTestHubDscr2s[] =
{
    0x09,       /* bLength  */
    0x29,       /* Descriptor type Hub 0x29 */
    0x07,       /* bNumber of ports    0x07 */
    0xA0, 0x00, /* wHubCharecteristic 0x00A0 i.e. Ganged power switching,
                   Not a compound device, Global over current protection,
                  , TT require 16 FS bit times, Port indicator supported */
    0x32,       /* powerOnGoodTime is 50*2 ms */
    0x64,       /* Maximum current require by hub controller */
    0x00,       /* Device removable */
    0xFF        /* Port power control mask */
};


/* Hub descriptor with
 * 1. Ganged Power switching
 * 2. Global Over current protection
 * 3. Number of ports 11.
 */
uint8_t MGC_aHubTestHubDscr3s[] =
{
    0x09,       /* bLength  */
    0x29,       /* Descriptor type Hub 0x29 */
    0x0B,       /* bNumber of ports    11 */
    0xA0, 0x00, /* wHubCharecteristic 0x00A0 i.e. Ganged power switching,
                   Not a compound device, Global over current protection,
                   , TT require 16 FS bit times, Port indicator supported */
    0x32,       /* powerOnGoodTime is 50*2 ms */
    0x64,       /* Maximum current require by hub controller */
    0x00,       /* Device removable */
    0xFF        /* Port power control mask */
};


/* Hub descriptor with
 * 1. Ganged Power switching
 * 2. Global Over current protection
 * 3. Number of ports 16.
 */
uint8_t MGC_aHubTestHubDscr4s[] =
{
    0x09,       /* bLength  */
    0x29,       /* Descriptor type Hub 0x29 */
    0x10,       /* bNumber of ports    16 */
    0xA0, 0x00, /* wHubCharecteristic 0x00A0 i.e. Ganged power switching,
                   Not a compound device, Global over current protection,
                   , TT require 16 FS bit times, Port indicator supported */
    0x32,       /* powerOnGoodTime is 50*2 ms */
    0x64,       /* Maximum current require by hub controller */
    0x00,       /* Device removable */
    0xFF        /* Port power control mask */
};


/* Hub descriptor with
 * 1. Ganged Power switching
 * 2. Global Over current protection
 * 3. Number of ports 255.
 */
uint8_t MGC_aHubTestHubDscr5s[] =
{
    0x09,       /* bLength  */
    0x29,       /* Descriptor type Hub 0x29 */
    0xFF,       /* bNumber of ports  255 */
    0xA0, 0x00, /* wHubCharecteristic 0x00A0 i.e. Ganged power switching,
                   Not a compound device, Global over current protection,
                   , TT require 16 FS bit times, Port indicator supported */
    0x32,       /* powerOnGoodTime is 50*2 ms */
    0x64,       /* Maximum current require by hub controller */
    0x00,       /* Device removable */
    0xFF        /* Port power control mask */
};


/* Hub descriptor with
 * 1. Ganged Power switching
 * 2. Global Over current protection
 * 3. Number of ports 0x00.
 */
uint8_t MGC_aHubTestHubDscr1e[] =
{
    0x09,       /* bLength  */
    0x29,       /* Descriptor type Hub 0x29 */
    0x00,       /* bNumber of ports  0x00 */
    0xA0, 0x00, /* wHubCharecteristic 0x00A0 i.e. Ganged power switching,
                   Not a compound device, Global over current protection,
                   , TT require 16 FS bit times, Port indicator supported */
    0x32,       /* powerOnGoodTime is 50*2 ms */
    0x64,       /* Maximum current require by hub controller */
    0x00,       /* Device removable */
    0xFF        /* Port power control mask */
};




/* Hub descriptor with
 * 1. Port Power switching
 * 2. Port based Over current protection.
 * 3. Number of ports 1.
 */
uint8_t MGC_aHubTestHubDscr6s[] =
{
    0x09,       /* bLength  */
    0x29,       /* Descriptor type Hub 0x29 */
    0x01,       /* bNumber of ports    0x01 */
    0xA9, 0x00, /* wHubCharecteristic 0x00A0 i.e. Ganged power switching,
                   Not a compound device, Global over current protection,
                  , TT require 16 FS bit times, Port indicator supported */
    0x32,       /* powerOnGoodTime is 50*2 ms */
    0x64,       /* Maximum current require by hub controller */
    0x00,       /* Device removable */
    0xFF        /* Port power control mask */
};


/* Hub descriptor with
 * 1. Port Power switching
 * 2. Port based Over current protection.
 * 3. Number of ports 7.
 */
uint8_t MGC_aHubTestHubDscr7s[] =
{
    0x09,       /* bLength  */
    0x29,       /* Descriptor type Hub 0x29 */
    0x07,       /* bNumber of ports    0x07 */
    0xA9, 0x00, /* wHubCharecteristic 0x00A0 i.e. Ganged power switching,
                   Not a compound device, Global over current protection,
                  , TT require 16 FS bit times, Port indicator supported */
    0x32,       /* powerOnGoodTime is 50*2 ms */
    0x64,       /* Maximum current require by hub controller */
    0x00,       /* Device removable */
    0xFF        /* Port power control mask */
};


/* Hub descriptor with
 * 1. Port Power switching
 * 2. Port based Over current protection.
 * 3. Number of ports 11.
 */
uint8_t MGC_aHubTestHubDscr8s[] =
{
    0x09,       /* bLength  */
    0x29,       /* Descriptor type Hub 0x29 */
    0x0B,       /* bNumber of ports    11 */
    0xA9, 0x00, /* wHubCharecteristic 0x00A0 i.e. Ganged power switching,
                   Not a compound device, Global over current protection,
                   , TT require 16 FS bit times, Port indicator supported */
    0x32,       /* powerOnGoodTime is 50*2 ms */
    0x64,       /* Maximum current require by hub controller */
    0x00,       /* Device removable */
    0xFF        /* Port power control mask */
};


/* Hub descriptor with
 * 1. Port Power switching
 * 2. Port based Over current protection.
 * 3. Number of ports 16.
 */
uint8_t MGC_aHubTestHubDscr9s[] =
{
    0x09,       /* bLength  */
    0x29,       /* Descriptor type Hub 0x29 */
    0x10,       /* bNumber of ports    16 */
    0xA9, 0x00, /* wHubCharecteristic 0x00A0 i.e. Ganged power switching,
                   Not a compound device, Global over current protection,
                   , TT require 16 FS bit times, Port indicator supported */
    0x32,       /* powerOnGoodTime is 50*2 ms */
    0x64,       /* Maximum current require by hub controller */
    0x00,       /* Device removable */
    0xFF        /* Port power control mask */
};


/* Hub descriptor with
 * 1. Port Power switching
 * 2. Port based Over current protection.
 * 3. Number of ports 255.
 */
uint8_t MGC_aHubTestHubDscr10s[] =
{
    0x09,       /* bLength  */
    0x29,       /* Descriptor type Hub 0x29 */
    0xFF,       /* bNumber of ports  255 */
    0xA9, 0x00, /* wHubCharecteristic 0x00A0 i.e. Ganged power switching,
                   Not a compound device, Global over current protection,
                   , TT require 16 FS bit times, Port indicator supported */
    0x32,       /* powerOnGoodTime is 50*2 ms */
    0x64,       /* Maximum current require by hub controller */
    0x00,       /* Device removable */
    0xFF        /* Port power control mask */
};


/* Hub descriptor with
 * 1. Port Power switching
 * 2. Port based Over current protection.
 * 3. Number of ports 0x00.
 */
uint8_t MGC_aHubTestHubDscr2e[] =
{
    0x09,       /* bLength  */
    0x29,       /* Descriptor type Hub 0x29 */
    0x00,       /* bNumber of ports  0x00 */
    0xA9, 0x00, /* wHubCharecteristic 0x00A0 i.e. Ganged power switching,
                   Not a compound device, Global over current protection,
                   , TT require 16 FS bit times, Port indicator supported */
    0x32,       /* powerOnGoodTime is 50*2 ms */
    0x64,       /* Maximum current require by hub controller */
    0x00,       /* Device removable */
    0xFF        /* Port power control mask */
};
/*********************************************************************
 ************************Hub Descriptor End***************************
 *********************************************************************/


/*********************************************************************
 ************************Interrupt IRP DATA   ************************
 *********************************************************************/

/* Change in Hub status there is no change in port status. */
uint8_t MGC_aIntrIrpDataHubNoChangeInStatus[] = { 0x00, 0x00, 0x00, 0x00};

/* Change in Hub status there is no change in port status. */
uint8_t MGC_aIntrIrpDataHubStatusChange[] = { 0x01, 0x00, 0x00, 0x00};

/* Only PortStatus is changed There is no change in Hub Status.*/
uint8_t MGC_aIntrIrpDataFirstPortsStatusChange[] = { 0x02, 0x00, 0x00, 0x00};

/* Change in Hub status as well as port's status */
uint8_t MGC_aIntrIrpData1[] = { 0x03, 0x00, 0x00, 0x00};

/* Only PortStatus is changed There is no change in Hub Status.*/
uint8_t MGC_aIntrIrpDataSecondPortsStatusChange[] = { 0x04, 0x00, 0x00, 0x00};

/* Only PortStatus is changed There is no change in Hub Status.*/
uint8_t MGC_aIntrIrpDataThirdPortsStatusChange[] = { 0x08, 0x00, 0x00, 0x00};
/* Only PortStatus is changed There is no change in Hub Status.*/
uint8_t MGC_aIntrIrpDataForthPortsStatusChange[] = { 0x10, 0x00, 0x00, 0x00};
/* Only PortStatus is changed There is no change in Hub Status.*/
uint8_t MGC_aIntrIrpDataFifthPortsStatusChange[] = { 0x20, 0x00, 0x00, 0x00};

/*********************************************************************
 ************************Hub Status and Status Change*****************
 *********************************************************************/

/* StatusChange: No change in local Power Suply as well as Over current.
 * Status: Local Power Supply Good. No over current Exist.
 */
uint8_t MGC_aHubStatusNotChanged[] = { 0x00, 0x00, 0x00, 0x00};

/* StatusChange: change in local Power Supply but No change in Over current.
 * Status: Local Power Supply Lost. No over current Exist.
 */
uint8_t MGC_aHubStatusLpsChange[] = { 0x01, 0x00, 0x01, 0x00};


/* StatusChange: No change in local Power Suply. Change in Over current.
 * Status: Local Power Supply Good. Over current Exist.
 */
uint8_t MGC_aHubStatusOcBitChangeOcExist[] = { 0x02, 0x00, 0x02, 0x00};

/* OverCurrent condition is changed but over current condition does not
 * exists.
 */
uint8_t MGC_aHubStatusOcBitChangeOcNotExist[] = { 0x00, 0x00, 0x02, 0x00};


/* StatusChange: Change in local Power Suply. Change in Over current.
 * Status: Local Power Supply Lost. Over current Exist.
 */
uint8_t MGC_aHubStatusLpsOcChange[] = { 0x03, 0x00, 0x03, 0x00};


/*********************************************************************
 ************************Port Status and Status Change*****************
 *********************************************************************/


/* Port Status Change: Connect State changed and Device is connected.
 */
uint8_t MGC_aHubPortStatusDeviceConnect[] = { 0x01, 0x00, 0x01, 0x00};

/* Port Status Change: Connect State changed and Device is Disconnected.
 */
uint8_t MGC_aHubPortStatusDeviceDisconnect[] = { 0x00, 0x00, 0x01, 0x00};

uint8_t MGC_aHubPortStatusResetSuspendPortEnable[] = { 0x16, 0x00, 0x16, 0x00};

uint8_t MGC_aHubPortStatusOcBitChangeOcExists[] = { 0x08, 0x00, 0x08, 0x00};
uint8_t MGC_aHubPortStatusOcBitChangeOcNotExists[] = { 0x00, 0x00, 0x08, 0x00};

uint8_t MGC_aHubPortStatus4[] = { 0x00, 0x00, 0x00, 0x00};

uint8_t MGC_aHubPortStatusResCompPortEnaDevConn[] = { 0x03, 0x00, 0x10, 0x00};
uint8_t MGC_aHubPortStatusResCompPortEnaDevDisConn[] = { 0x02, 0x00, 0x02, 0x00};
uint8_t MGC_aHubPortStatusResCompPortDisableDevConn[] = { 0x01, 0x00, 0x00, 0x00};


uint8_t MGC_aHubPortStatusDebounceDeviceConnected[]    = { 0x01, 0x00, 0x00, 0x00};
uint8_t MGC_aHubPortStatusDebounceDeviceDisConnected[] = { 0x00, 0x00, 0x00, 0x00};


uint8_t MGC_aHubPortStatusDeviceDisconnectOcExists[] = { 0x08, 0x00, 0x09, 0x00};


