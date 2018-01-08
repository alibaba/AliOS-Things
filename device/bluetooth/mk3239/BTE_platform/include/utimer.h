/*******************************************************************************
**  Name:       utimer.h
**
**  Description:
**
**  This file contains timer definitions from WIDCOMM's Universal Embedded
**  Drivers API.
**
*******************************************************************************/

#ifndef UTIMER_H
#define UTIMER_H

/*******************************************************************************
** Timer APIs
*******************************************************************************/

/**** Timer IDs ****/

#define UTIMER_ID_1               0
#define UTIMER_ID_2               1
#define UTIMER_ID_3               2
#define UTIMER_ID_4               3

#define UTIMER_NUM_TIMERS         4     /* Number of timers supported */

typedef UINT8 tUTIMER_ID;

/**** Timer types ****/

#define UTIMER_TYPE_PERIODIC      0
#define UTIMER_TYPE_ONESHOT       1

typedef UINT8 tUTIMER_TYPE;


/**** Timer time ****/

typedef UINT32 tUTIMER_TIME;


/**** Timer configuration ****/

typedef struct {
    tUTIMER_TIME period;
    tUTIMER_TYPE type;
} tUTIMER_CFG;


/**** Timer feature types ****/
#define UTIMER_FEAT_ID_1          0
#define UTIMER_FEAT_ID_2          1
#define UTIMER_FEAT_ID_3          2
#define UTIMER_FEAT_ID_4          3
#define UTIMER_FEAT_TYPE_PERIODIC 4
#define UTIMER_FEAT_TYPE_ONESHOT  5

typedef UINT8 tUTIMER_FEATURE;


/**** Callback for timer expiration ****/
typedef void (tUTIMER_CBACK)(tUTIMER_ID);


/*******************************************************************************
** Function Prototypes
*******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

UDRV_API extern void    UTIMER_Init(void *);
UDRV_API extern void    UTIMER_Start(tUTIMER_ID, tUTIMER_CFG *, tUTIMER_CBACK *);
UDRV_API extern void    UTIMER_Read(tUTIMER_ID, tUTIMER_TIME *);
UDRV_API extern void    UTIMER_Stop(tUTIMER_ID);
UDRV_API extern BOOLEAN UTIMER_Feature(tUTIMER_FEATURE);

#ifdef __cplusplus
}
#endif

#endif /* UTIMER_H */
