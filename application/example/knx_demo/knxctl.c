/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>

#include <aos/kernel.h>
#include "knx/uknx.h"
#include "knx/uknx_ll.h"


#define EXPECT_EQ(a, b) do{ if(a!=b) return -1;} while(0)

static UKNX_U8 frame[256];

//#define IPT_SERVER "10.0.0.48"
#define IPT_SERVER "192.168.1.109"

//static UKNX_U8 layer = UKNX_LL_IP_TUNNEL;
static UKNX_U8 layer = UKNX_LL_NCN5120;
//static UKNX_U8 layer = UKNX_LL_IP_ROUTER;
//static UKNX_U8 layer = UKNX_LL_E981;

static volatile int connected = 0, quit = 0, knx_t_connected = 0;
static volatile unsigned int busytime = 0;

static void data_proc( UKNX_DATA_EVENT *pd )
{
    printf( "data event %2X:%2X to %2X:%2X, type %2X\n", pd->src_h, pd->src_l,
            pd->dest_h, pd->dest_l, pd->type );
    uknx_dump_buffer( pd->data, pd->len );

    if( pd->type == DATA_EVENT_GROUP ) {
        UKNX_BUF dpt = pd->data;

    } else
        if( pd->type == DATA_EVENT_PROP ) {

        } else
            if( pd->type == DATA_EVENT_SYSTEM ) {
                UKNX_BUF npdu = pd->data;
            }
}

static UKNX_RET event_cb( UKNX_EVENT evt, void *info )
{
    printf( "event_cb: event %s, info %p\n", uknx_event_name( evt ), info );
    switch( evt ) {
        case UKNX_EVT_CONNECTED:
            connected = 1;
            break;
        case UKNX_EVT_DISCONNECTED:
            connected = 0;
            break;
        case UKNX_EVT_DATA_IND: {
            //Process Input Data
            UKNX_DATA_EVENT *pData = ( UKNX_DATA_EVENT * )info;
            data_proc( pData );
            break;
        }
        case UKNX_EVT_BUSY_IN:
            busytime = ( unsigned int )info;
            break;
        case UKNX_EVT_BUSY_OUT:
            busytime = 0;
            break;

        /* Connection status for T_Connect_Req. */
        case UKNX_EVT_KNX_T_DISCONNECTED:
            knx_t_connected = 0;
            break;
        default:
            break;
    }

    return 0;
}

void knx_t_connect( UKNX_U8 dest_h, UKNX_U8 dest_l )
{
    UKNX_LL_FRAME *pb = ( UKNX_LL_FRAME * )frame;
    if( !knx_t_connected ) {
        T_Connect_Req( pb, dest_h, dest_l );
        knx_t_connected = 1;
        aos_msleep( 300 );
        //A_DeviceDescriptor_Read_Req(pb, 0x12, 0x23);
        UKNX_U8 key[4] = {0xFF, 0xFF, 0xFF, 0xFF};
        A_Authorize_Req( pb, 0x12, 0x23, key );
        //aos_msleep(500);
    }
}

int application_start( int argc, char *argv[] )
{
    int count = 0;

    uknx_notify_callback( event_cb );
    if( layer & UKNX_LL_IP_TUNNEL ) {
        uknx_ll_config( UKNX_CONF_IPT_SERVER, IPT_SERVER );
        uknx_ll_config( UKNX_CONF_IPT_SERVER_PORT, ( void * )3671 );
        uknx_ll_config( UKNX_CONF_IPT_LOCAL_ADDR, 0 );
        uknx_ll_config( UKNX_CONF_IPT_LOCAL_PORT, 0 );
    }

    if( layer & ( UKNX_LL_NCN5120 | UKNX_LL_E981 ) ) {
        uknx_ll_config( UKNX_CONF_TTY_DEV, "2" );
    }


    UKNX_RET ret = uknx_init();
    EXPECT_EQ( 0, ret );


    uknx_ll_init( layer );
    ret = uknx_ll_start();
    EXPECT_EQ( 0, ret );

    int test = 0, loop = 0;
    aos_msleep( 150 );
    UKNX_LL_FRAME *pb = ( UKNX_LL_FRAME * )frame;

    while( 1 ) {
        if( busytime ) {
            aos_msleep( busytime >> 10 );
        } else {
            aos_msleep( 300 );
        }
        ret = uknx_proc();

        //When engine busy time, no then A_XXX API call but uknx_proc
        if( busytime ) {
            continue;
        }

        if( loop > 2 && !test && connected ) {
            test = 1;
            //A_IndividualAddress_Read_Req(pb);
            //A_SystemNetworkParameter_Read(pb, 0, 0, 0, 1, 0xef);
            //A_GroupValue_Read_Req(pb, 0x1, 0x2, KNX_OBJ_PRIO_NORMAL);

            //UKNX_U8 data = 5;
            //A_GroupValue_Write_Req(pb, 0x12, 0x3, KNX_OBJ_PRIO_NORMAL, &data, sizeof(data));

            //A_IndividualAddress_Write_Req(pb, 0x12, 0x23);

            //A_DeviceDescriptor_Read_Req(pb, 0x12, 0x23);

            if( argc > 3 && argv[1][0] == 'r' ) {

                knx_t_connect( 0x12, 0x23 );

                UKNX_U16 memref = ( UKNX_U16 )atoi( argv[2] );
                UKNX_U8 memlen = ( UKNX_U8 )atoi( argv[3] );
                printf( "MEM Read: %2X %2X\n", memref, memlen );
                A_Memory_Read_Req( pb, 0x12, 0x23, memref, memlen );

                A_Memory_Read_Req( pb, 0x12, 0x23, memref, memlen );
                continue;
            }

            if( argc > 3 && argv[1][0] == 'w' ) {

                knx_t_connect( 0x12, 0x23 );

                UKNX_U16 memref = ( UKNX_U16 )atoi( argv[2] );
                UKNX_U8 ch = ( UKNX_U8 )atoi( argv[3] );
                printf( "MEM Write 1 Byte: %2X %2X\n", memref, ch );
                A_Memory_Write_Req( pb, 0x12, 0x23, memref, &ch, 1 );
                continue;
            }

            int i = 1;
            while( ++i < argc ) {
                A_PropertyValue_Read_Req( pb, 0x12, 0x23,
                                          ( UKNX_U8 )atoi( argv[1] ), ( UKNX_U8 )atoi( argv[i] ), 1, 1 );
            }
            //A_PropertyDescription_Read_Req(pb, 0x12, 0x23, 1, 1, 1);
        }

        loop++;

    };

    if( connected ) {
        uknx_ll_stop();
    }

    return 0;
}
