#ifndef __DD_H_
#define __DD_H_

typedef struct _dd_init_s_
{
    char *dev_name;

    void (*init)(void);
    void (*exit)(void);
} DD_INIT_S;


/*******************************************************************************
* Function Declarations
*******************************************************************************/
extern void g_dd_init(void);
extern void g_dd_exit(void);

#endif // __DD_H_
