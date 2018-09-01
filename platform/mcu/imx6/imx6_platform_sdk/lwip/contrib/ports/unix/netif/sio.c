/* Author: Magnus Ivarsson <magnus.ivarsson@volvo.com> */

#include "netif/sio.h" 
#include "netif/fifo.h"
#include "lwip/debug.h"
#include "lwip/def.h"
#include "lwip/sys.h"
#include "lwip/arch.h"

/* Following #undefs are here to keep compiler from issuing warnings
   about them being double defined. (They are defined in lwip/inet.h
   as well as the Unix #includes below.) */
#undef htonl
#undef ntohl
#undef htons
#undef ntohs
#undef HTONL
#undef NTOHL
#undef HTONS
#undef NTOHS

#include <stdlib.h>
#if defined(openbsd)
#include <util.h>
#endif
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/signal.h>
#include <sys/types.h>

#if PPP_SUPPORT && defined(linux)
#include <pty.h>
#endif

/*#define BAUDRATE B19200 */
/*#define BAUDRATE B57600 */
#define BAUDRATE B115200

#ifndef TRUE
#define TRUE  1
#endif
#ifndef FALSE
#define FALSE 0
#endif

/* for all of you who dont define SIO_DEBUG in debug.h */
#ifndef SIO_DEBUG
#define SIO_DEBUG 0
#endif


/*  typedef struct siostruct_t */
/*  {  */
/*  	sio_status_t *sio; */
/*  } siostruct_t; */

/** array of ((siostruct*)netif->state)->sio structs */
static sio_status_t statusar[3];

#if ! PPP_SUPPORT
/* --private-functions----------------------------------------------------------------- */
/** 
 * Signal handler for ttyXX0 to indicate bytes received 
 * one per interface is needed since we cannot send a instance number / pointer as callback argument (?)
 */
static void	signal_handler_IO_0( int status )
{
	LWIP_UNUSED_ARG(status);
	LWIP_DEBUGF(SIO_DEBUG, ("SigHand: rxSignal channel 0\n"));
	fifoPut( &statusar[0].myfifo, statusar[0].fd );
}

/**
 * Signal handler for ttyXX1 to indicate bytes received 
 * one per interface is needed since we cannot send a instance number / pointer as callback argument (?)
 */
static void signal_handler_IO_1( int status )
{
	LWIP_UNUSED_ARG(status);
	LWIP_DEBUGF(SIO_DEBUG, ("SigHand: rxSignal channel 1\n"));
	fifoPut( &statusar[1].myfifo, statusar[1].fd );
}
#endif

/**
* Initiation of serial device 
* @param device : string with the device name and path, eg. "/dev/ttyS0"
* @param netif  : netinterface struct, contains interface instance data
* @return file handle to serial dev.
*/
static int sio_init( char * device, int devnum, sio_status_t * siostat )
{
	struct termios oldtio,newtio;
#if ! PPP_SUPPORT
	struct sigaction saio;           /* definition of signal action */
#endif
	int fd;
	LWIP_UNUSED_ARG(siostat);

	/* open the device to be non-blocking (read will return immediately) */
	fd = open( device, O_RDWR | O_NOCTTY | O_NONBLOCK );
	if ( fd < 0 )
	{
		perror( device );
		exit( -1 );
	}

#if ! PPP_SUPPORT
	/* install the signal handler before making the device asynchronous */
	switch ( devnum )
	{
		case 0:
			LWIP_DEBUGF( SIO_DEBUG, ("sioinit, signal_handler_IO_0\n") );
			saio.sa_handler = signal_handler_IO_0;
			break;
		case 1:
			LWIP_DEBUGF( SIO_DEBUG, ("sioinit, signal_handler_IO_1\n") );
			saio.sa_handler = signal_handler_IO_1;
			break;
		default:
			LWIP_DEBUGF( SIO_DEBUG,("sioinit, devnum not allowed\n") );
			break;
	}

	saio.sa_flags = 0;
#if linux
	saio.sa_restorer = NULL;
#endif /* linux */
	sigaction( SIGIO,&saio,NULL );

	/* allow the process to receive SIGIO */
	fcntl( fd, F_SETOWN, getpid( ) );
	/* Make the file descriptor asynchronous (the manual page says only
	O_APPEND and O_NONBLOCK, will work with F_SETFL...) */
	fcntl( fd, F_SETFL, FASYNC );
#else
	fcntl( fd, F_SETFL, 0 );
#endif /* ! PPP_SUPPORT */

	tcgetattr( fd,&oldtio ); /* save current port settings */
	/* set new port settings */
	/* see 'man termios' for further settings */
	newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD | CRTSCTS;
	newtio.c_iflag = 0;
	newtio.c_oflag = 0;
	newtio.c_lflag = 0; /*ECHO; */
	newtio.c_cc[VMIN] = 1; /* Read 1 byte at a time, no timer */
	newtio.c_cc[VTIME] = 0;

	tcsetattr( fd,TCSANOW,&newtio );
	tcflush( fd, TCIOFLUSH );

	return fd;
}

/**
*
*/
static void sio_speed( int fd, int speed )
{
	struct termios oldtio,newtio;
	/*  int fd; */

	LWIP_DEBUGF( 1,("sio_speed: baudcode:%d  enter\n",speed ) );

	if ( fd < 0 )
	{
		LWIP_DEBUGF(SIO_DEBUG, ( "sio_speed: fd ERROR\n" ));
		exit( -1 );
	}

	tcgetattr( fd,&oldtio ); /* get current port settings */

	/* set new port settings 
	* see 'man termios' for further settings */
	newtio.c_cflag = speed | CS8 | CLOCAL | CREAD; /*§ | CRTSCTS; */
	newtio.c_iflag = 0;
	newtio.c_oflag = 0;
	newtio.c_lflag = 0; /*ECHO; */
	newtio.c_cc[VMIN] = 1; /* Read 1 byte at a time, no timer */
	newtio.c_cc[VTIME] = 0;

	tcsetattr( fd,TCSANOW,&newtio );
	tcflush( fd, TCIOFLUSH );

	LWIP_DEBUGF( SIO_DEBUG ,("sio_speed: leave\n" ));
}

/* --public-functions----------------------------------------------------------------------------- */
void sio_send( u8_t c, sio_status_t * siostat )
{
    /*	sio_status_t * siostat = ((siostruct_t*)netif->state)->sio; */

	if ( write( siostat->fd, &c, 1 ) <= 0 )
	{
		LWIP_DEBUGF( SIO_DEBUG,("sio_send: write refused\n") );
	}
}

void sio_send_string( u8_t *str, sio_status_t * siostat )
{
    /*	sio_status_t * siostat = ((siostruct_t*)netif->state)->sio; */
	int len = strlen( (const char *)str );

	if ( write( siostat->fd, str, len ) <= 0 )
	{
		LWIP_DEBUGF( SIO_DEBUG,("sio_send_string: write refused\n") );
	}
	LWIP_DEBUGF( (PPP_DEBUG | SIO_DEBUG),("sent:%s\n",str ) );
}


void sio_flush( sio_status_t * siostat )
{
	LWIP_UNUSED_ARG(siostat);
	/* not implemented in unix as it is not needed */
 	/*sio_status_t * siostat = ((siostruct_t*)netif->state)->sio; */
}


#if ! PPP_SUPPORT
/*u8_t sio_recv( struct netif * netif )*/
u8_t sio_recv( sio_status_t * siostat )
{
    /*	sio_status_t * siostat = ((siostruct_t*)netif->state)->sio; */
	return fifoGet( &(siostat->myfifo) );
}

s16_t sio_poll(sio_status_t * siostat)
{
    /*	sio_status_t * siostat = ((siostruct_t*)netif->state)->sio;*/
	return fifoGetNonBlock( &(siostat->myfifo) );
}


void sio_expect_string( u8_t *str, sio_status_t * siostat )
{
    /*	sio_status_t * siostat = ((siostruct_t*)netif->state)->sio;*/
	u8_t c;
 	int finger=0;
  
	LWIP_DEBUGF( (PPP_DEBUG | SIO_DEBUG), ("expect:%s\n",str) );
	while ( 1 )
	{
		c=fifoGet( &(siostat->myfifo) );
		LWIP_DEBUGF( (PPP_DEBUG | SIO_DEBUG), ("_%c",c) );
		if ( c==str[finger] )
		{
			finger++;
		} else if ( finger > 0 )
		{
                    /*it might fit in the beginning? */
			if ( str[0] == c )
			{
				finger = 1;
			}
		}
		if ( 0 == str[finger] ) 
                    break;	/* done, we have a match */
	}
	LWIP_DEBUGF( (PPP_DEBUG | SIO_DEBUG), ("[match]\n") );
}
#endif /* ! PPP_SUPPORT */

#if PPP_SUPPORT
u32_t sio_write(sio_status_t * siostat, u8_t *buf, u32_t size)
{
    return write( siostat->fd, buf, size );
}

u32_t sio_read(sio_status_t * siostat, u8_t *buf, u32_t size)
{
    return read( siostat->fd, buf, size );
}

void sio_read_abort(sio_status_t * siostat)
{
    printf("sio_read_abort: not yet implemented for unix\n");
}
#endif /* PPP_SUPPORT */

sio_status_t * sio_open( int devnum )
{
	char dev[20];

	/* would be nice with dynamic memory alloc */
	sio_status_t * siostate = &statusar[ devnum ];
/* 	siostruct_t * tmp; */


/* 	tmp = (siostruct_t*)(netif->state); */
/* 	tmp->sio = siostate; */

/* 	tmp = (siostruct_t*)(netif->state); */

/* 	((sio_status_t*)(tmp->sio))->fd = 0; */

#if ! PPP_SUPPORT
	fifoInit( &siostate->myfifo );
#endif /* ! PPP_SUPPORT */

	snprintf( dev, sizeof(dev), "/dev/ttyS%d", devnum );

	if ( (devnum == 1) || (devnum == 0) )
	{
		if ( ( siostate->fd = sio_init( dev, devnum, siostate ) ) == 0 )
		{
			LWIP_DEBUGF(SIO_DEBUG, ( "sio_open: ERROR opening serial device\n" ));
			abort( );
			return NULL;
		}
	} 
#if PPP_SUPPORT
	else if (devnum == 2) {
	    pid_t childpid;
	    childpid = forkpty(&siostate->fd, NULL, NULL, NULL);
	    if(childpid < 0) {
		perror("forkpty");
		exit (1);
	    }
	    if(childpid == 0) {
		execl("/usr/sbin/pppd", "pppd",
			"ms-dns", "198.168.100.7",
			"local", "crtscts",
			"debug",
#ifdef LWIP_PPP_CHAP_TEST
			"auth",
			"require-chap",
			"remotename", "lwip",
#else
			"noauth",
#endif
			"192.168.1.1:192.168.1.2",
			NULL);
		perror("execl pppd");
		exit (1);
	    } else {
		LWIP_DEBUGF(SIO_DEBUG, ( "sio_open: spawned pppd pid %d\n", childpid));
	    }

	}
#endif
	else
	{
		LWIP_DEBUGF(SIO_DEBUG, ( "sio_open: device %s (%d) is not supported\n", dev, devnum ));
		return NULL;
	}
	LWIP_DEBUGF( 1,("sio_open: dev=%s open.\n", dev ));

	return siostate;
}

/**
*
*/
void sio_change_baud( sioBaudrates baud, sio_status_t * siostat )
{
    /*	sio_status_t * siostat = ((siostruct_t*)netif->state)->sio;*/

	LWIP_DEBUGF( 1,("sio_change_baud\n" ));

	switch ( baud )
	{
		case SIO_BAUD_9600:
			sio_speed( siostat->fd, B9600 );
			break;
		case SIO_BAUD_19200:
			sio_speed( siostat->fd, B19200 );
			break;
		case SIO_BAUD_38400:
			sio_speed( siostat->fd, B38400 );
			break;
		case SIO_BAUD_57600:
			sio_speed( siostat->fd, B57600 );
			break;
		case SIO_BAUD_115200:
			sio_speed( siostat->fd, B115200 );
			break;

		default:
			LWIP_DEBUGF( 1,("sio_change_baud: Unknown baudrate, code:%d\n", baud ));
			break;
	}
}

