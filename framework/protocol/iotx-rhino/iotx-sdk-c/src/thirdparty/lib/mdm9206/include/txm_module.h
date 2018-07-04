#ifndef TXM_MODULE_H
#define TXM_MODULE_H


#define TXM_APPLICATION_REQUEST_ID_BASE                     0x10000

/* Define the Module ID, which is used to indicate a module is valid.  */

#define TXM_MODULE_ID                                       0x4D4F4455


#define VOID                                    void
typedef char                                    CHAR;
typedef unsigned char                           UCHAR;
typedef int                                     INT;
typedef unsigned int                            UINT;
typedef long                                    LONG;
typedef unsigned long                           ULONG;
typedef short                                   SHORT;
typedef unsigned short                          USHORT;

ULONG  _txm_module_system_call4(ULONG request, ULONG param_1, ULONG param_2, ULONG param_3, ULONG param_4);
ULONG  _txm_module_system_call5(ULONG request, ULONG param_1, ULONG param_2, ULONG param_3, ULONG param_4,
                                ULONG param_5);
ULONG  _txm_module_system_call6(ULONG request, ULONG param_1, ULONG param_2, ULONG param_3, ULONG param_4,
                                ULONG param_5, ULONG param_6);
ULONG  _txm_module_system_call7(ULONG request, ULONG param_1, ULONG param_2, ULONG param_3, ULONG param_4,
                                ULONG param_5, ULONG param_6, ULONG param_7);
ULONG  _txm_module_system_call8(ULONG request, ULONG param_1, ULONG param_2, ULONG param_3, ULONG param_4,
                                ULONG param_5, ULONG param_6, ULONG param_7, ULONG param_8);
ULONG  _txm_module_system_call9(ULONG request, ULONG param_1, ULONG param_2, ULONG param_3, ULONG param_4,
                                ULONG param_5, ULONG param_6, ULONG param_7, ULONG param_8, ULONG param_9);
ULONG  _txm_module_system_call10(ULONG request, ULONG param_1, ULONG param_2, ULONG param_3, ULONG param_4,
                                 ULONG param_5, ULONG param_6, ULONG param_7, ULONG param_8, ULONG param_9, ULONG param_10);
ULONG  _txm_module_system_call11(ULONG request, ULONG param_1, ULONG param_2, ULONG param_3, ULONG param_4,
                                 ULONG param_5, ULONG param_6, ULONG param_7, ULONG param_8, ULONG param_9, ULONG param_10, ULONG param_11);
ULONG  _txm_module_system_call12(ULONG request, ULONG param_1, ULONG param_2, ULONG param_3, ULONG param_4,
                                 ULONG param_5, ULONG param_6, ULONG param_7, ULONG param_8, ULONG param_9, ULONG param_10, ULONG param_11,
                                 ULONG param12);


#endif

