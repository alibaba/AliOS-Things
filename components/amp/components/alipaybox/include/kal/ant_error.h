/**
 * ant_error.h
 *
 * 基本错误码定义
 */
#ifndef __ANT_ERROR_H__
#define __ANT_ERROR_H__
#define ANT_OK                                  0
#define ANT_SUCCESS                             0
#define ANT_ERR_PARAM                           -1
#define ANT_ERR_PORT_NOT_OPEN                   -2
#define ANT_ERR_TIMER_FULL                      -3
#define ANT_ERR_INVALID_TIMER                   -4
#define ANT_ERR_FATAL                           -5
#define ANT_ERR_INVALID_OP                      -6
#define ANT_ERR_UART_BUSY                       -7    
#define ANT_ERR_INVALID_PORT                    -8    
#define ANT_ERR_NOMATCHVERSION                  -9
#define ANT_ERR_NOSUPPORTPIN                    -10
#define ANT_ERR_NOSUPPORTMODE                   -11
#define ANT_ERR_NOSUPPORTEINT                   -12
#define ANT_ERR_NOSUPPORTSET                    -13
#define ANT_ERR_NOSUPPORTGET                    -14
#define ANT_ERR_NOSUPPORTCONTROL                -15
#define ANT_ERR_PINALREADYSUBCRIBE              -16
#define ANT_ERR_BUSSUBBUSY                      -17
#define ANT_ERR_NOGPIOMODE                      -18
#define ANT_ERR_NORIGHTOPERATE                  -19
#define ANT_ERR_ALREADYUNSUBCRIBE               -20
#define ANT_ERR_FULLI2CBUS                      -21
#define ANT_ERR_NOTSUPPORTBYHANDLE              -22
#define ANT_ERR_INVALIDBUSHANDLE                -23
#define ANT_ERR_NOEXISTOBJEXT                   -24
#define ANT_ERR_OPERATEOBJEXTFAILED             -25
#define ANT_ERR_OPENOBJEXTFAILED                -26
#define ANT_ERR_WRITEOBJEXTFAILED               -27
#define ANT_ERR_READOBJEXTFAILED                -28
#define ANT_ERR_FLASHFULLOVER                   -29   
#define ANT_ERR_FLASHSPACE                      -30
#define ANT_ERR_DRIVE                           -31
#define ANT_ERR_DRIVEFULLOVER                   -32
#define ANT_ERR_INVALIDFLASHID                  -33
#define ANT_ERR_I2CHWFAILED                     -34
#define ANT_ERR_FILEFAILED                      -35
#define ANT_ERR_FILEOPENFAILED                  -36
#define ANT_ERR_FILENAMETOOLONG                 -37
#define ANT_ERR_FILEREADFAILED                  -38
#define ANT_ERR_FILEWRITEFAILED                 -39
#define ANT_ERR_FILESEEKFAILED                  -40
#define ANT_ERR_FILENOTFOUND                    -41
#define ANT_ERR_FILENOMORE                      -42
#define ANT_ERR_FILEDISKFULL                    -43
#define ANT_ERR_INVALID_BAUDRATE                -44
#define ANT_ERR_API_NO_RESPONSE                 -45
#define ANT_ERR_API_INVALID_RESPONSE            -46
#define ANT_ERR_SMS_EXCEED_LENGTH               -47
#define ANT_ERR_SMS_NOT_INIT                    -48
#define ANT_ERR_INVALID_TASK_ID                 -49   
#define ANT_ERR_NOT_IN_BASIC_MODE               -50
#define ANT_ERR_INVALID_PARAMETER               -51
#define ANT_ERR_PATHNOTFOUND                    -52
#define ANT_ERR_GET_MEM                         -53
#define ANT_ERR_GENERAL_FAILURE                 -54
#define ANT_ERR_FILE_EXISTS                     -55
#define ANT_ERR_SMS_INVALID_FORMAT              -56    
#define ANT_ERR_SMS_GET_FORMAT                  -57        
#define ANT_ERR_SMS_INVALID_STORAGE             -58        
#define ANT_ERR_SMS_SET_STORAGE                 -59            
#define ANT_ERR_SMS_SEND_AT_CMD                 -60
#define ANT_ERR_API_CMD_BUSY                    -61  
/* AUD -70 ~ -100*/
#define ANT_ERR_MED_BAD_FORMAT                  -70
#define ANT_ERR_MED_BUSY                        -71
#define ANT_ERR_MED_DISC_FULL                   -72
#define ANT_ERR_MED_OPEN_FILE_FAIL              -73
#define ANT_ERR_MED_BAD_FILE_EXTENSION          -74
#define ANT_ERR_MED_WRITE_PROTECTION            -75
#define ANT_ERR_MED_FILE_EXIST                  -76
#define ANT_ERR_MED_UNSUPPORT_FMT_IN_CALLING    -77
#define ANT_ERR_AUD_REC_STOP_FAIL               -78
#define ANT_ERR_MED_DRIVE_NOT_FOUND             -79
#define ANT_ERR_MED_NO_CARD                     -80
#define ANT_ERR_MEM_FULL                        -81
#define ANT_ERR_DTMFSTRING_TOO_LONG             -82 
#define ANT_ERR_WDTMF_PS_BUSY                   -83 
#define ANT_ERR_DTMF_BUSY                       -84
#define ANT_ERR_DTMF_NO_CALLING                 -85
/* reserve to -100 */
#define ANT_ERR_MED_UNKNOWN                     -100
/* File Append */
#define ANT_ERR_FILE_NO_CARD                    -101
#define ANT_ERR_FS_FATAL_ERR1                   -190
#define ANT_ERR_FS_FATAL_ERR2                   -191
/*reserved to -200 */
#define ANT_ERR_FILE_UNKNOWN                    -200
#define ANT_ERR_EINT_USED                       -300 
#define ANT_ERR_EINT_ALREADY_REGISTERED         -301 
#define ANT_ERR_ADC_ALREADY_REGISTERED          -302
#define ANT_ERR_ADC_NOT_REGISTERED              -303
#define ANT_ERR_ADC_SAMPLING_ALREADY            -304
#define ANT_ERR_CHANNEL_OUTRANGE                -305
#define ANT_ERR_CHANNEL_USED                    -306
#define ANT_ERR_CHANNEL_NOT_FOUND               -307
#define ANT_ERR_IIC_SLAVE_NOT_FOUND             -308
#define ANT_ERR_IIC_SAME_SLAVE_ADDRESS          -309
#define ANT_ERR_IIC_SLAVE_TOO_MANY              -310
#define ANT_ERR_IIC_NOT_IIC_CONTROLER           -311
#define ANT_ERR_FULLSPIBUS                      -312
#define ANT_ERR_SIM_NOT_INSERTED                -400
#define ANT_ERR_SIM_TYPE_ERROR                  -401 
/* SMS -500 ~ -550*/  
#define ANT_ERR_SMS_ERROR                       -500
#define ANT_ERR_SMS_NOT_INITIAL                 -501
#define ANT_ERR_SMS_NOT_READY                   -502
#define ANT_ERR_SMS_INVALID_PARAM               -503
#define ANT_ERR_SMS_OUT_OF_MEMORY               -504
#define ANT_ERR_SMS_INCORRECT_DATA_LENGTH       -505
#define ANT_ERR_SMS_PDU_SYNTEX_ERROR            -506
#define ANT_ERR_SMS_INVALID_MEM_INDEX           -507
#define ANT_ERR_SMS_CMD_CONFLICT                -508
#define ANT_ERR_SMS_MSG_EMPTY                   -509
#define ANT_ERR_SMS_INVALID_NUMBER_STRING       -510
#define ANT_ERR_SMS_INVALID_TEXT_CONTENT        -511
#define ANT_ERR_SMS_NOT_SUPPORTED               -512
#define ANT_ERR_SMS_INCORRECT_FORMAT            -513
#define ANT_ERR_SMS_STORAGE_FULL                -514
 
/*  RIL FTP -600    */
#define ANT_ERR_RIL_FTP_OPENFAIL                -600   
#define ANT_ERR_RIL_FTP_CLOSEFAIL               -601
#define ANT_ERR_RIL_FTP_SETPATHFAIL             -602
#define ANT_ERR_RIL_FTP_SETCFGFAIL              -603
#define ANT_ERR_RIL_FTP_RENAMEFAIL              -604
#define ANT_ERR_RIL_FTP_SIZEFAIL                -605    
#define ANT_ERR_RIL_FTP_DELETEFAIL              -606
#define ANT_ERR_RIL_FTP_MKDIRFAIL               -607
#define ANT_ERR_RAWFLASH_OVERRANGE              -8001   
#define ANT_ERR_RAWFLASH_UNIITIALIZED           -8002    
#define ANT_ERR_RAWFLASH_UNKNOW                 -8003    
#define ANT_ERR_RAWFLASH_INVLIDBLOCKID          -8004   
#define ANT_ERR_RAWFLASH_PARAMETER              -8005    
#define ANT_ERR_RAWFLASH_ERASEFlASH             -8006    
#define ANT_ERR_RAWFLASH_WRITEFLASH             -8007   
#define ANT_ERR_RAWFLASH_READFLASH              -8008    
#define ANT_ERR_RAWFLASH_MAXLENGATH             -8009   
/* Flash OPT End */
#define ANT_ERR_SYS_NOT_READY                   -9998
#define ANT_ERR_UNKOWN                          -9999
#define ANT_ERROR_NOT_SUPPORT                   -10000

#endif // End-of ANT_ERROR_H

