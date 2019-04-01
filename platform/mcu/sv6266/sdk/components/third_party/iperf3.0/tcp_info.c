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

/*
 * routines related to collection TCP_INFO using getsockopt()
 * 
 * Brian Tierney, ESnet  (bltierney@es.net)
 * 
 * Note that this is only really useful on Linux.
 * XXX: only standard on linux versions 2.4 and later
 #
 * FreeBSD has a limitted implementation that only includes the following:
 *   tcpi_snd_ssthresh, tcpi_snd_cwnd, tcpi_rcv_space, tcpi_rtt
 * Based on information on http://wiki.freebsd.org/8.0TODO, I dont think this will be
 * fixed before v8.1 at the earliest.
 *
 * OSX has no support.
 *
 * I think MS Windows does support TCP_INFO, but iperf3 does not currently support Windows.
 */

//#include <os_wrapper.h>

#include "iperf.h"
#include "iperf_api.h"
#include "iperf_locale.h"
//#if (IPERF3_ENABLE == 1)

/*************************************************************/
int
has_tcpinfo(void)
{
    return 0;
}

/*************************************************************/
int
has_tcpinfo_retransmits(void)
{
    return 0;
}

/*************************************************************/
void
save_tcpinfo(struct iperf_stream *sp, struct iperf_interval_results *irp)
{
}

/*************************************************************/
long
get_total_retransmits(struct iperf_interval_results *irp)
{

    return -1;
}

/*************************************************************/
/*
 * Return snd_cwnd in octets.
 */
long
get_snd_cwnd(struct iperf_interval_results *irp)
{
    return -1;
}

/*************************************************************/
/*
 * Return rtt in usec.
 */
long
get_rtt(struct iperf_interval_results *irp)
{
    return -1;
}
//#endif
