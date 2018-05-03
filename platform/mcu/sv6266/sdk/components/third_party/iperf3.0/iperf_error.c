/*
 * iperf, Copyright (c) 2014, The Regents of the University of
 * California, through Lawrence Berkeley National Laboratory (subject
 * to receipt of any required approvals from the U.S. Dept. of
 * Energy).  All rights reserved.
 *
 * If you have questions about your rights to use or distribute this
 * software, please contact Berkeley Lab's Technology Transfer
 * Department at TTD@lbl.gov.
 *
 * NOTICE.  This software is owned by the U.S. Department of Energy.
 * As such, the U.S. Government has been granted for itself and others
 * acting on its behalf a paid-up, nonexclusive, irrevocable,
 * worldwide license in the Software to reproduce, prepare derivative
 * works, and perform publicly and display publicly.  Beginning five
 * (5) years after the date permission to assert copyright is obtained
 * from the U.S. Department of Energy, and subject to any subsequent
 * five (5) year renewals, the U.S. Government is granted for itself
 * and others acting on its behalf a paid-up, nonexclusive,
 * irrevocable, worldwide license in the Software to reproduce,
 * prepare derivative works, distribute copies to the public, perform
 * publicly and display publicly, and to permit others to do so.
 *
 * This code is distributed under a BSD style license, see the LICENSE
 * file for complete information.
 */

//#include <os_wrapper.h>
//#include <rtos.h>
//#include <log.h>

#include "iperf.h"
#include "lwip/netdb.h"
#include "iperf_api.h"

//#if (IPERF3_ENABLE == 1)

char *
iperf_strerror(int i_errorno)
{
    static char errstr[256];
    int len, perr, herr;
    perr = herr = 0;

    len = sizeof(errstr);
    MEMSET(errstr, 0, len);

    switch (i_errorno) {
        case IENONE:
            sprintf(errstr,   "no error");
            break;
        case IESERVCLIENT:
            sprintf(errstr,   "cannot be both server and client");
            break;
        case IENOROLE:
            sprintf(errstr,   "must either be a client (-c) or server (-s)");
            break;
        case IESERVERONLY:
            sprintf(errstr,   "some option you are trying to set is server only");
            break;
        case IECLIENTONLY:
            sprintf(errstr,   "some option you are trying to set is client only");
            break;
        case IEDURATION:
            sprintf(errstr,   "test duration too long (maximum = %d seconds)", MAX_TIME);
            break;
        case IENUMSTREAMS:
            sprintf(errstr,   "number of parallel streams too large (maximum = %d)", MAX_STREAMS);
            break;
        case IEBANDWIDTH:
            sprintf(errstr,   "bandwidth too large (maximum = 1G)");  /*, MAX_BANDWIDTH*/
            break;
        case IEBLOCKSIZE:
            sprintf(errstr,   "block size invalid (maximum = %d bytes, minimum = %d bytes)", MAX_BLOCKSIZE, MIN_BLOCKSIZE);
            break;
        case IEBUFSIZE:
            sprintf(errstr,   "socket buffer size too large (maximum = %d bytes)", MAX_TCP_BUFFER);
            break;
        case IEINTERVAL:
            sprintf(errstr,   "invalid report interval (min = %g, max = %g seconds)", MIN_INTERVAL, MAX_INTERVAL);
            break;
        case IEBIND:
            sprintf(errstr,   "--bind must be specified to use --cport");
            break;
        case IEUDPBLOCKSIZE:
            sprintf(errstr,   "block size too large (maximum = %d bytes)", MAX_UDP_BLOCKSIZE);
            break;
        case IEMSS:
            sprintf(errstr,   "TCP MSS too large (maximum = %d bytes)", MAX_MSS);
            break;
        case IENOSENDFILE:
            sprintf(errstr,   "this OS does not support sendfile");
            break;
        case IEOMIT:
            sprintf(errstr,   "bogus value for --omit");
            break;
        case IEUNIMP:
            sprintf(errstr,   "an option you are trying to set is not implemented yet");
            break;
        case IEFILE:
            sprintf(errstr,   "unable to open -F file");
            perr = 1;
            break;
        case IEBURST:
            sprintf(errstr,   "invalid burst count (maximum = %d)", MAX_BURST);
            break;
        case IEENDCONDITIONS:
            sprintf(errstr,   "only one test end condition (-t, -n, -k) may be specified");
            break;
    	case IELOGFILE:
    	    sprintf(errstr,   "unable to open log file");
    	    perr = 1;
    	    break;
    	case IENOSCTP:
    	    sprintf(errstr,   "no SCTP support available");
    	    break;
        case IENEWTEST:
            sprintf(errstr,   "unable to create a new test");
            perr = 1;
            break;
        case IEINITTEST:
            sprintf(errstr,   "test initialization failed");
            perr = 1;
            break;
        case IELISTEN:
            sprintf(errstr,   "unable to start listener for connections");
            perr = 1;
            break;
        case IECONNECT:
            sprintf(errstr,   "unable to connect to server");
            perr = 1;
            break;
        case IEACCEPT:
            sprintf(errstr,   "unable to accept connection from client");
            herr = 1;
            perr = 1;
            break;
        case IESENDCOOKIE:
            sprintf(errstr,   "unable to send cookie to server");
            perr = 1;
            break;
        case IERECVCOOKIE:
            sprintf(errstr,   "unable to receive cookie at server");
            perr = 1;
            break;
        case IECTRLWRITE:
            sprintf(errstr,   "unable to write to the control socket");
            perr = 1;
            break;
        case IECTRLREAD:
            sprintf(errstr,   "unable to read from the control socket");
            perr = 1;
            break;
        case IECTRLCLOSE:
            sprintf(errstr,   "control socket has closed unexpectedly");
            break;
        case IEMESSAGE:
            sprintf(errstr,   "received an unknown control message");
            break;
        case IESENDMESSAGE:
            sprintf(errstr,   "unable to send control message");
            perr = 1;
            break;
        case IERECVMESSAGE:
            sprintf(errstr,   "unable to receive control message");
            perr = 1;
            break;
        case IESENDPARAMS:
            sprintf(errstr,   "unable to send parameters to server");
            perr = 1;
            break;
        case IERECVPARAMS:
            sprintf(errstr,   "unable to receive parameters from client");
            perr = 1;
            break;
        case IEPACKAGERESULTS:
            sprintf(errstr,   "unable to package results");
            perr = 1;
            break;
        case IESENDRESULTS:
            sprintf(errstr,   "unable to send results");
            perr = 1;
            break;
        case IERECVRESULTS:
            sprintf(errstr,   "unable to receive results");
            perr = 1;
            break;
        case IESELECT:
            sprintf(errstr,   "select failed");
            perr = 1;
            break;
        case IECLIENTTERM:
            sprintf(errstr,   "the client has terminated");
            break;
        case IESERVERTERM:
            sprintf(errstr,   "the server has terminated");
            break;
        case IEACCESSDENIED:
            sprintf(errstr,   "the server is busy running a test. try again later");
            break;
        case IESETNODELAY:
            sprintf(errstr,   "unable to set TCP/SCTP NODELAY");
            perr = 1;
            break;
        case IESETMSS:
            sprintf(errstr,   "unable to set TCP/SCTP MSS");
            perr = 1;
            break;
        case IESETBUF:
            sprintf(errstr,   "unable to set socket buffer size");
            perr = 1;
            break;
        case IESETTOS:
            sprintf(errstr,   "unable to set IP TOS");
            perr = 1;
            break;
        case IESETCOS:
            sprintf(errstr,   "unable to set IPv6 traffic class");
            perr = 1;
            break;
        case IESETFLOW:
            sprintf(errstr,   "unable to set IPv6 flow label");
            break;
        case IEREUSEADDR:
            sprintf(errstr,   "unable to reuse address on socket");
            perr = 1;
            break;
        case IENONBLOCKING:
            sprintf(errstr,   "unable to set socket to non-blocking");
            perr = 1;
            break;
        case IESETWINDOWSIZE:
            sprintf(errstr,   "unable to set socket window size");
            perr = 1;
            break;
        case IEPROTOCOL:
            sprintf(errstr,   "protocol does not exist");
            break;
        case IEAFFINITY:
            sprintf(errstr,   "unable to set CPU affinity");
            perr = 1;
            break;
    	case IEDAEMON:
    	    sprintf(errstr,   "unable to become a daemon");
    	    perr = 1;
    	    break;
        case IECREATESTREAM:
            sprintf(errstr,   "unable to create a new stream");
            herr = 1;
            perr = 1;
            break;
        case IEINITSTREAM:
            sprintf(errstr,   "unable to initialize stream");
            herr = 1;
            perr = 1;
            break;
        case IESTREAMLISTEN:
            sprintf(errstr,   "unable to start stream listener");
            perr = 1;
            break;
        case IESTREAMCONNECT:
            sprintf(errstr,   "unable to connect stream");
            herr = 1;
            perr = 1;
            break;
        case IESTREAMACCEPT:
            sprintf(errstr,   "unable to accept stream connection");
            perr = 1;
            break;
        case IESTREAMWRITE:
            sprintf(errstr,   "unable to write to stream socket");
            perr = 1;
            break;
        case IESTREAMREAD:
            sprintf(errstr,   "unable to read from stream socket");
            perr = 1;
            break;
        case IESTREAMCLOSE:
            sprintf(errstr,   "stream socket has closed unexpectedly");
            break;
        case IESTREAMID:
            sprintf(errstr,   "stream has an invalid id");
            break;
        case IENEWTIMER:
            sprintf(errstr,   "unable to create new timer");
            perr = 1;
            break;
        case IEUPDATETIMER:
            sprintf(errstr,   "unable to update timer");
            perr = 1;
            break;
        case IESETCONGESTION:
            sprintf(errstr,   "unable to set TCP_CONGESTION: " 
                                  "Supplied congestion control algorithm not supported on this host");
            break;
    	case IEPIDFILE:
            sprintf(errstr,   "unable to write PID file");
            perr = 1;
            break;
    	case IEV6ONLY:
    	    sprintf(errstr,   "Unable to set/reset IPV6_V6ONLY");
    	    perr = 1;
    	    break;
        case IEV4ONLY:
    	    sprintf(errstr,   "SUPPORT IPv4 ONLY");
    	    perr = 1;
    	    break;
        case IESETSCTPDISABLEFRAG:
            sprintf(errstr,   "unable to set SCTP_DISABLE_FRAGMENTS");
            perr = 1;
            break;
        case IESETSCTPNSTREAM:
            sprintf(errstr,   "unable to set SCTP_INIT num of SCTP streams\r\n");
            perr = 1;
            break;
    }
  
    return errstr;
}

//#endif //#if(IPERF3_ENABLE==1)


